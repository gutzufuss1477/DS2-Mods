param([string]$LlvmBin='C:\Program Files\LLVM\bin')
$ErrorActionPreference='Stop'
Push-Location (Join-Path $PSScriptRoot '..')
try {
 $cc=Join-Path $LlvmBin 'clang-cl.exe';$ld=Join-Path $LlvmBin 'lld-link.exe'
 & $cc --target=x86_64-pc-windows-msvc /nologo /std:c++17 /O2 /GS- /GR- /EHs-c- /Zl /W4 /c tests/atlas/native.cpp /Fobuild/atlas_tests.obj
 if($LASTEXITCODE){throw 'ATLAS test compilation failed'}
 & $cc --target=x86_64-pc-windows-msvc /nologo /c tests/atlas/thunk_fixture.s /Fobuild/atlas_fixture.obj
 if($LASTEXITCODE){throw 'ATLAS fixture assembly failed'}
 & $ld /dll /entry:DllMain /machine:x64 /nodefaultlib /dynamicbase /nxcompat /out:build/atlas_tests.dll /implib:build/atlas_tests.lib /export:TestThunk build/atlas_tests.obj build/atlas_fixture.obj build/atlas_loading_thunk.obj build/kernel32.lib
 if($LASTEXITCODE){throw 'ATLAS test linking failed'}
 python tests/atlas/test_native.py
 if($LASTEXITCODE){throw 'ATLAS native tests failed'}
} finally {Pop-Location}
