$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
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

$testSource = Join-Path $root 'tests\synthetic_runtime_test.cpp'
if (!(Test-Path -LiteralPath $testSource)) { throw "Synthetic test source missing: $testSource" }

$object = Join-Path $out 'synthetic_runtime_test.obj'
& $clang --target=x86_64-pc-windows-msvc /nologo /c /O2 /Ob0 /GS- /GR- /EHs-c- /Zl /Oi /W4 /WX /clang:-Wno-unused-function /clang:-fno-builtin /clang:-mcx16 "/I$root\src" /TP "/Fo$object" $testSource
if ($LASTEXITCODE) { throw 'synthetic test compile failed' }

$dll = Join-Path $out 'synthetic_runtime_test.dll'
& $link /dll /entry:DllMain /nodefaultlib /machine:x64 /subsystem:windows /Brepro "/out:$dll" $object (Join-Path $root 'build\public\kernel32.lib')
if ($LASTEXITCODE) { throw 'synthetic test link failed' }
Copy-Item -LiteralPath (Join-Path $root 'config\ds2_climbing_gloves_range.ini') -Destination (Join-Path $out 'ds2_climbing_gloves_range.ini') -Force

Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;

public static class ClimbingGlovesNativeTest {
    [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    public static extern IntPtr LoadLibraryW(string path);
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
}
'@

$module = [ClimbingGlovesNativeTest]::LoadLibraryW($dll)
if ($module -eq [IntPtr]::Zero) {
    throw "LoadLibrary failed: $([Runtime.InteropServices.Marshal]::GetLastWin32Error())"
}
$image = [IntPtr]::Zero
try {
    $settingsProc = [ClimbingGlovesNativeTest]::GetProcAddress($module, 'RunPackagedSettingsTest')
    $syntheticProc = [ClimbingGlovesNativeTest]::GetProcAddress($module, 'RunSyntheticClimbingGlovesTest')
    if ($settingsProc -eq [IntPtr]::Zero -or $syntheticProc -eq [IntPtr]::Zero) {
        throw 'synthetic test export missing'
    }
    $settingsTest = [Runtime.InteropServices.Marshal]::GetDelegateForFunctionPointer(
        $settingsProc, [type][ClimbingGlovesNativeTest+NoArgs]
    )
    $syntheticTest = [Runtime.InteropServices.Marshal]::GetDelegateForFunctionPointer(
        $syntheticProc, [type][ClimbingGlovesNativeTest+WithImage]
    )

    $settingsResult = $settingsTest.Invoke()
    if ($settingsResult -ne 0) { throw "packaged settings test failed: $settingsResult" }

    [uint32]$capacity = 0x0B292000
    $image = [ClimbingGlovesNativeTest]::VirtualAlloc(
        [IntPtr]::Zero, [UIntPtr]$capacity, 0x3000, 0x40
    )
    if ($image -eq [IntPtr]::Zero) {
        throw "VirtualAlloc failed: $([Runtime.InteropServices.Marshal]::GetLastWin32Error())"
    }
    $syntheticResult = $syntheticTest.Invoke($image, $capacity)
    if ($syntheticResult -ne 0) { throw "synthetic runtime test failed: $syntheticResult" }
    Write-Host 'Synthetic climbing-gloves tests: PASS'
} finally {
    if ($image -ne [IntPtr]::Zero) {
        [void][ClimbingGlovesNativeTest]::VirtualFree($image, [UIntPtr]::Zero, 0x8000)
    }
    [void][ClimbingGlovesNativeTest]::FreeLibrary($module)
}
