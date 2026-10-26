$ErrorActionPreference='Stop'
Push-Location $PSScriptRoot
try {
 $cc='C:\Program Files\LLVM\bin\clang-cl.exe';$ld='C:\Program Files\LLVM\bin\lld-link.exe'
 & $cc /nologo /std:c++17 /O2 /GS- /GR- /EHs-c- /Zl /W4 /Isrc /c tests/runtime.cpp /Fobuild/runtime.obj
 if($LASTEXITCODE){throw 'Runtime test compile failed'}
 & $ld /entry:mainCRTStartup /subsystem:console /machine:x64 /nodefaultlib /out:build/runtime.exe build/runtime.obj build/icon_gpu.obj build/exo_icon_gpu.obj build/payload.obj build/kernel32.lib
 if($LASTEXITCODE){throw 'Runtime test link failed'}
 & .\build\runtime.exe
 if($LASTEXITCODE){throw 'Runtime test failed'}
} finally {Pop-Location}
