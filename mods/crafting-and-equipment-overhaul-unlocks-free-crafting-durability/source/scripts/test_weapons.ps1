[CmdletBinding()]
param([string]$LlvmBin='C:\Program Files\LLVM\bin')
$ErrorActionPreference='Stop'
Push-Location (Join-Path $PSScriptRoot '..')
try {
 New-Item -ItemType Directory -Force build | Out-Null
 & (Join-Path $LlvmBin 'clang-cl.exe') --target=x86_64-pc-windows-msvc /nologo /std:c++17 /O2 /GS- /GR- /EHs-c- /Zl /W4 /c tests/weapons/native.cpp /Fobuild/tests.obj
 if($LASTEXITCODE){throw 'Test compilation failed'}
 & (Join-Path $LlvmBin 'lld-link.exe') /dll /noentry /nodefaultlib /machine:x64 /dynamicbase /nxcompat /out:build/native_tests.dll /implib:build/tests.lib build/tests.obj
 if($LASTEXITCODE){throw 'Test link failed'}
 & (Join-Path $LlvmBin 'clang-cl.exe') --target=x86_64-pc-windows-msvc /nologo /std:c++17 /O2 /GS- /GR- /EHs-c- /Zl /W4 /c tests/weapons/pistol_gpu_test.cpp /Fobuild/pistol_gpu_test.obj
 if($LASTEXITCODE){throw 'GPU test compilation failed'}
 $sdk=Get-ChildItem 'C:\Program Files (x86)\Windows Kits\10\Lib' -Directory | Sort-Object Name -Descending | Select-Object -First 1
 $d3d=Join-Path $sdk.FullName 'um/x64/d3d12.lib'
 & (Join-Path $LlvmBin 'lld-link.exe') /dll /noentry /nodefaultlib /machine:x64 /out:build/pistol_gpu_test.dll build/pistol_gpu_test.obj build/kernel32.lib $d3d
 if($LASTEXITCODE){throw 'GPU test link failed'}
 python tests/weapons/test_native.py
 if($LASTEXITCODE){throw 'Native validation failed'}
} finally {Pop-Location}
