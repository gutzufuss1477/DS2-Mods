[CmdletBinding()]
param([string]$OutputDirectory='release/test16-single-asi')
$ErrorActionPreference='Stop'
Push-Location $PSScriptRoot
try {
 $cc='C:\Program Files\LLVM\bin\clang-cl.exe'
 $ld='C:\Program Files\LLVM\bin\lld-link.exe'
 New-Item -ItemType Directory -Force -Path build/api,$OutputDirectory | Out-Null
 & 'D:\DS2-Asset-Lab\tools\geometry-venv\Scripts\python.exe' tools/embed_rendered_icon.py
 if ($LASTEXITCODE) {throw 'Icon payload validation failed'}
 & 'D:\DS2-Asset-Lab\tools\geometry-venv\Scripts\python.exe' tools/embed_portable_payloads.py
 if ($LASTEXITCODE) {throw 'Embedded asset validation failed'}
 & $ld /lib /machine:x64 /def:src/kernel32.def /out:build/api/kernel32.lib
 if ($LASTEXITCODE) { throw 'Imports failed' }
 foreach ($source in @('src/addon.cpp','src/icon_gpu.cpp','build/api/embedded_payload.cpp')) {
  $object='build/api/'+[IO.Path]::GetFileNameWithoutExtension($source)+'.obj'
  & $cc /nologo /std:c++17 /O2 /GS- /GR- /EHs-c- /Zl /W4 /Isrc /c $source "/Fo$object"
  if ($LASTEXITCODE) { throw "Compilation failed: $source" }
 }
 & $ld /dll /entry:DllMain /machine:x64 /nodefaultlib /dynamicbase /nxcompat /opt:ref /opt:icf "/out:$OutputDirectory/ds2_atlas_chiral_addon.asi" /implib:build/api/addon.lib build/api/addon.obj build/api/icon_gpu.obj build/api/embedded_payload.obj build/api/kernel32.lib
 if ($LASTEXITCODE) { throw 'Link failed' }
 Get-FileHash "$OutputDirectory/ds2_atlas_chiral_addon.asi"
} finally { Pop-Location }
