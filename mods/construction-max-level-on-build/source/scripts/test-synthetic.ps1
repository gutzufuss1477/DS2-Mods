$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
& (Join-Path $root 'scripts\build-llvm.ps1')
$llvm = if ($env:LLVM_BIN) { $env:LLVM_BIN } else { 'C:\Program Files\LLVM\bin' }
$clang = Join-Path $llvm 'clang-cl.exe'
$link = Join-Path $llvm 'lld-link.exe'
$out = Join-Path $root 'build\tests'
New-Item -ItemType Directory -Force -Path $out | Out-Null

$object = Join-Path $out 'synthetic_runtime_test.obj'
& $clang --target=x86_64-pc-windows-msvc /nologo /c /O2 /Ob0 /GS- /GR- /EHs-c- /Zl /Oi /W4 /WX /clang:-fno-builtin /clang:-mcx16 "/I$root\src" /TP "/Fo$object" (Join-Path $root 'tests\synthetic_runtime_test.cpp')
if ($LASTEXITCODE) { throw 'synthetic test compile failed' }

$dll = Join-Path $out 'synthetic_runtime_test.dll'
& $link /dll /entry:DllMain /nodefaultlib /machine:x64 /subsystem:windows /Brepro "/out:$dll" $object (Join-Path $root 'build\public\kernel32.lib')
if ($LASTEXITCODE) { throw 'synthetic test link failed' }

Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
public static class ConstructionNativeTest {
    [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    public static extern IntPtr LoadLibraryW(string path);
    [DllImport("kernel32.dll", SetLastError = true)]
    public static extern IntPtr GetProcAddress(IntPtr module, string name);
    [DllImport("kernel32.dll", SetLastError = true)]
    public static extern bool FreeLibrary(IntPtr module);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate uint RunTest();
}
'@

$module = [ConstructionNativeTest]::LoadLibraryW($dll)
if ($module -eq [IntPtr]::Zero) { throw "LoadLibrary failed: $([Runtime.InteropServices.Marshal]::GetLastWin32Error())" }
try {
    $address = [ConstructionNativeTest]::GetProcAddress($module, 'RunSyntheticConstructionLevelTest')
    if ($address -eq [IntPtr]::Zero) { throw 'test export missing' }
    $test = [Runtime.InteropServices.Marshal]::GetDelegateForFunctionPointer($address, [type][ConstructionNativeTest+RunTest])
    $result = $test.Invoke()
    if ($result -ne 0) { throw "synthetic construction-level test failed: $result" }
    Write-Host 'Synthetic construction-level runtime test: PASS'
} finally {
    [void][ConstructionNativeTest]::FreeLibrary($module)
}
