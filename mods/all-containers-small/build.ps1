[CmdletBinding()]
param([string]$LlvmBin='C:\Program Files\LLVM\bin')
$ErrorActionPreference='Stop'
Set-StrictMode -Version Latest
Push-Location $PSScriptRoot
try {
 New-Item -ItemType Directory -Force build,release | Out-Null
 $cc=Join-Path $LlvmBin 'clang-cl.exe'; $ld=Join-Path $LlvmBin 'lld-link.exe'
 & $ld /lib /machine:x64 /def:src/kernel32.def /out:build/kernel32.lib
 if($LASTEXITCODE){throw 'Import library failed'}
 & $cc --target=x86_64-pc-windows-msvc /nologo /std:c++17 /O2 /GS- /GR- /EHs-c- /Zl /W4 /WX /c src/mod.cpp /Fobuild/mod.obj
 if($LASTEXITCODE){throw 'Compilation failed'}
 & $ld /dll /entry:DllMain /machine:x64 /nodefaultlib /dynamicbase /nxcompat /opt:ref /opt:icf /timestamp:0 /out:release/ds2_all_containers_small.asi /implib:build/mod.lib build/mod.obj build/kernel32.lib
 if($LASTEXITCODE){throw 'Link failed'}
 Get-FileHash -Algorithm SHA256 -LiteralPath release/ds2_all_containers_small.asi
} finally {Pop-Location}
