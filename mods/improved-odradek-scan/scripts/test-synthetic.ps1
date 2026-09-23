[CmdletBinding()]
param(
    [string]$TargetExe = $env:DS2_EXE,
    [string]$PythonExe = 'python',
    [switch]$SkipMappedImageTest
)

$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
if (![Environment]::Is64BitProcess) { throw 'Run tests in a 64-bit PowerShell process.' }
if (!$SkipMappedImageTest) {
    if (!$TargetExe) {
        $TargetExe = Join-Path ${env:ProgramFiles(x86)} 'Steam\steamapps\common\DEATH STRANDING 2 - ON THE BEACH\DS2.exe'
    }
    $TargetExe = (Resolve-Path -LiteralPath $TargetExe -ErrorAction Stop).Path
    # Validate the exact build before mapping or changing any in-memory hook slots.
    & $PythonExe (Join-Path $root 'tools\validate_target.py') $TargetExe
    if ($LASTEXITCODE) { throw 'offline DS2 target validation failed' }
}
& (Join-Path $root 'scripts\build-llvm.ps1')

$llvm = if ($env:LLVM_BIN) {
    $env:LLVM_BIN
} elseif ($env:LLVM_HOME -and (Test-Path -LiteralPath (Join-Path $env:LLVM_HOME 'bin\clang-cl.exe'))) {
    Join-Path $env:LLVM_HOME 'bin'
} elseif ($env:LLVM_HOME) {
    $env:LLVM_HOME
} else {
    'C:\Program Files\LLVM\bin'
}
$clang = Join-Path $llvm 'clang-cl.exe'
$link = Join-Path $llvm 'lld-link.exe'
$out = Join-Path $root 'build\tests'
New-Item -ItemType Directory -Force -Path $out | Out-Null

$object = Join-Path $out 'synthetic_runtime_test.obj'
& $clang --target=x86_64-pc-windows-msvc /nologo /c /O2 /Ob0 /GS- /GR- /EHs-c- /Zl /Oi /W4 /WX /clang:-fno-builtin /clang:-mcx16 "/I$root\src" /TP "/Fo$object" "$root\tests\synthetic_runtime_test.cpp"
if ($LASTEXITCODE) { throw 'synthetic test compile failed' }

$dll = Join-Path $out 'synthetic_runtime_test.dll'
& $link /dll /entry:DllMain /nodefaultlib /machine:x64 /subsystem:windows /Brepro "/out:$dll" $object (Join-Path $root 'build\public\kernel32.lib')
if ($LASTEXITCODE) { throw 'synthetic test link failed' }
Copy-Item -LiteralPath (Join-Path $root 'config\ds2_odradek_scan.ini') -Destination (Join-Path $out 'ds2_odradek_scan.ini') -Force

Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;

public static class OdradekNativeTest {
    [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    public static extern IntPtr LoadLibraryW(string path);
    [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    public static extern IntPtr LoadLibraryExW(string path, IntPtr file, uint flags);
    [DllImport("kernel32.dll", SetLastError = true)]
    public static extern IntPtr GetProcAddress(IntPtr module, string name);
    [DllImport("kernel32.dll", SetLastError = true)]
    public static extern bool FreeLibrary(IntPtr module);
    [DllImport("kernel32.dll", SetLastError = true)]
    public static extern IntPtr VirtualAlloc(IntPtr address, UIntPtr size, uint allocationType, uint protection);
    [DllImport("kernel32.dll", SetLastError = true)]
    public static extern bool VirtualFree(IntPtr address, UIntPtr size, uint freeType);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate uint NoArgs();
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate uint WithImage(IntPtr image, uint capacity);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate uint WithMath(IntPtr image, IntPtr atan2f, IntPtr fmodf);
}
'@

$module = [OdradekNativeTest]::LoadLibraryW($dll)
if ($module -eq [IntPtr]::Zero) { throw "LoadLibrary failed: $([Runtime.InteropServices.Marshal]::GetLastWin32Error())" }
$image = [IntPtr]::Zero
$mappedImage = [IntPtr]::Zero
try {
    $settingsProc = [OdradekNativeTest]::GetProcAddress($module, 'RunPackagedSettingsTest')
    $syntheticProc = [OdradekNativeTest]::GetProcAddress($module, 'RunSyntheticOdradekTest')
    $mappedHookProc = [OdradekNativeTest]::GetProcAddress($module, 'RunMappedImageHookTest')
    if ($settingsProc -eq [IntPtr]::Zero -or $syntheticProc -eq [IntPtr]::Zero -or
        $mappedHookProc -eq [IntPtr]::Zero) { throw 'test export missing' }
    $settingsTest = [Runtime.InteropServices.Marshal]::GetDelegateForFunctionPointer(
        $settingsProc, [type][OdradekNativeTest+NoArgs]
    )
    $syntheticTest = [Runtime.InteropServices.Marshal]::GetDelegateForFunctionPointer(
        $syntheticProc, [type][OdradekNativeTest+WithImage]
    )
    $mappedHookTest = [Runtime.InteropServices.Marshal]::GetDelegateForFunctionPointer(
        $mappedHookProc, [type][OdradekNativeTest+WithImage]
    )

    $settingsResult = $settingsTest.Invoke()
    if ($settingsResult -ne 0) { throw "packaged settings test failed: $settingsResult" }

    [uint32]$capacity = 0x0B292000
    $image = [OdradekNativeTest]::VirtualAlloc([IntPtr]::Zero, [UIntPtr]$capacity, 0x3000, 0x40)
    if ($image -eq [IntPtr]::Zero) { throw "VirtualAlloc failed: $([Runtime.InteropServices.Marshal]::GetLastWin32Error())" }
    $syntheticResult = $syntheticTest.Invoke($image, $capacity)
    if ($syntheticResult -ne 0) { throw "synthetic runtime test failed: $syntheticResult" }

    $hudProc = [OdradekNativeTest]::GetProcAddress($module, 'RunScannedHudTest')
    if ($hudProc -eq [IntPtr]::Zero) { throw 'HUD candidate test export missing' }
    $hudTest = [Runtime.InteropServices.Marshal]::GetDelegateForFunctionPointer(
        $hudProc, [type][OdradekNativeTest+WithImage]
    )
    $hudResult = $hudTest.Invoke($image, $capacity)
    if ($hudResult -ne 0) { throw "HUD candidate test failed: $hudResult" }
    $distanceProc = [OdradekNativeTest]::GetProcAddress($module, 'RunScannedDistanceTest')
    if ($distanceProc -eq [IntPtr]::Zero) { throw 'HUD distance test export missing' }
    $distanceTest = [Runtime.InteropServices.Marshal]::GetDelegateForFunctionPointer(
        $distanceProc, [type][OdradekNativeTest+WithImage]
    )
    $distanceResult = $distanceTest.Invoke($image, $capacity)
    if ($distanceResult -ne 0) { throw "HUD distance test failed: $distanceResult" }
    $sensorProc = [OdradekNativeTest]::GetProcAddress($module, 'RunCargoSensorTest')
    if ($sensorProc -eq [IntPtr]::Zero) { throw 'Cargo sensor test export missing' }
    $sensorTest = [Runtime.InteropServices.Marshal]::GetDelegateForFunctionPointer(
        $sensorProc, [type][OdradekNativeTest+WithImage]
    )
    $sensorResult = $sensorTest.Invoke($image, $capacity)
    if ($sensorResult -ne 0) { throw "Cargo sensor test failed: $sensorResult" }
    Write-Host 'Cargo sensor: scoped optional cap, native veto, progression, exact boundaries: PASS'
    Write-Host 'Scoped HUD distance: untouched/dropped cargo 300m chain, exclusions, expiry, ABI and no persistent writes: PASS'
    Write-Host 'Scanned cargo HUD: range, expiry, no persistent writes, bounded pass-through: PASS'
    Write-Host 'Packaged settings and synthetic runtime tests: PASS'
    if ($SkipMappedImageTest) {
        Write-Warning 'Mapped-image hook test and offline target validation: SKIPPED (not release validation).'
    } else {
        # DONT_RESOLVE_DLL_REFERENCES: no game entry point is executed; no file is patched.
        $mappedImage = [OdradekNativeTest]::LoadLibraryExW($TargetExe, [IntPtr]::Zero, 1)
        if ($mappedImage -eq [IntPtr]::Zero) {
            throw "LoadLibraryExW failed: $([Runtime.InteropServices.Marshal]::GetLastWin32Error())"
        }
        $mappedHookResult = $mappedHookTest.Invoke($mappedImage, $capacity)
        if ($mappedHookResult -ne 0) { throw "mapped WRITECOPY hook test failed: $mappedHookResult" }
        Write-Host 'Mapped hooks/rollback, vanilla spent cargo, native 495/500 fade and 1000 range: PASS'
        Write-Host 'Visual wave: exact call sites, executable fifth-argument relay, CPU/source isolation, guards and group rollback: PASS'
        Write-Host 'Recipient meter: after-likes suffix, native reward predicate/formatter, no-likes fallback, integer refresh, TLS, ABI, isolation, bounds and three-site rollback: PASS'
        Write-Host 'Cargo meter: native UI field isolation, type/range guards, relay ABI and rollback: PASS'
        Write-Host 'Survey alignment: five variants, exact relay, foreign-state pass-through; native multipart condition: PASS'
        $mathModule = [OdradekNativeTest]::LoadLibraryW('ucrtbase.dll')
        if ($mathModule -eq [IntPtr]::Zero) { throw 'Native math test library missing' }
        try {
            $proc = [OdradekNativeTest]::GetProcAddress($module, 'RunMappedCargoSensorTest')
            if ($proc -eq [IntPtr]::Zero) { throw 'Native sensor test export missing' }
            $nativeSensorTest = [Runtime.InteropServices.Marshal]::GetDelegateForFunctionPointer(
                $proc, [type][OdradekNativeTest+WithMath]
            )
            $nativeSensorResult = $nativeSensorTest.Invoke($mappedImage,
                [OdradekNativeTest]::GetProcAddress($mathModule, 'atan2f'),
                [OdradekNativeTest]::GetProcAddress($mathModule, 'fmodf'))
            if ($nativeSensorResult -ne 0) { throw "Native sensor test failed: $nativeSensorResult" }
            Write-Host 'Actual native sensor gate: 500/700/1000 boundaries, optional cap, angle/progression: PASS'
        } finally {
            [void][OdradekNativeTest]::FreeLibrary($mathModule)
        }
    }
} finally {
    if ($mappedImage -ne [IntPtr]::Zero) { [void][OdradekNativeTest]::FreeLibrary($mappedImage) }
    if ($image -ne [IntPtr]::Zero) { [void][OdradekNativeTest]::VirtualFree($image, [UIntPtr]::Zero, 0x8000) }
    [void][OdradekNativeTest]::FreeLibrary($module)
}
