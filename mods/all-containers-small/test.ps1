param([string]$LlvmBin='C:\Program Files\LLVM\bin')
$ErrorActionPreference='Stop'
Push-Location $PSScriptRoot
try {
 $cc=Join-Path $LlvmBin 'clang-cl.exe';$ld=Join-Path $LlvmBin 'lld-link.exe'
 & $cc --target=x86_64-pc-windows-msvc /nologo /std:c++17 /O2 /GS- /GR- /EHs-c- /Zl /W4 /WX /c tests/native.cpp /Fobuild/native_tests.obj
 if($LASTEXITCODE){throw 'Test compilation failed'}
 & $ld /dll /entry:DllMain /machine:x64 /nodefaultlib /dynamicbase /nxcompat /timestamp:0 /out:build/native_tests.dll /implib:build/native_tests.lib build/native_tests.obj build/kernel32.lib
 if($LASTEXITCODE){throw 'Test linking failed'}
 python tests/test_native.py
 if($LASTEXITCODE){throw 'Tests failed'}
} finally {Pop-Location}
