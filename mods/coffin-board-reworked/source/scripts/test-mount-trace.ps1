$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$llvm = 'C:\Program Files\LLVM\bin'
$out = Join-Path $root 'build\mount-trace-tests'
New-Item -ItemType Directory -Path $out -Force | Out-Null
$defs = (Get-Content -LiteralPath (Join-Path $root 'src\kernel32.def') -Raw) + "`nExitProcess`nGetStdHandle`n"
$defFile = Join-Path $out 'kernel32-test.def'
Set-Content -LiteralPath $defFile -Value $defs -Encoding ascii
& (Join-Path $llvm 'lld-link.exe') /lib /machine:x64 "/def:$defFile" "/out:$out\kernel32-test.lib"
if ($LASTEXITCODE) { throw 'test import library failed' }
& (Join-Path $llvm 'clang-cl.exe') --target=x86_64-pc-windows-msvc /nologo /c /O2 /Ob0 /GS- /GR- /EHs-c- /Zl /Oi /W4 /WX /clang:-fno-builtin /clang:-mcx16 /clang:-Wno-unused-variable /clang:-Wno-unused-function /clang:-Wno-unused-const-variable "/Fo$out\test.obj" (Join-Path $root 'tests\mount_trace_abi.cpp')
if ($LASTEXITCODE) { throw 'test compilation failed' }
& (Join-Path $llvm 'lld-link.exe') /entry:test_main /nodefaultlib /machine:x64 /subsystem:console /Brepro "/out:$out\mount-trace-test.exe" "$out\test.obj" "$out\kernel32-test.lib"
if ($LASTEXITCODE) { throw 'test linking failed' }
Push-Location $out
try {
    & (Join-Path $out 'mount-trace-test.exe')
    if ($LASTEXITCODE) { throw "mount trace ABI test failed: $LASTEXITCODE" }
} finally { Pop-Location }
