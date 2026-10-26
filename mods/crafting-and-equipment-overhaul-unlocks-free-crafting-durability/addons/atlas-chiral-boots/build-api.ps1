[CmdletBinding()]
param([string]$LlvmBin='C:\Program Files\LLVM\bin',[string]$OutputDirectory='release')
$ErrorActionPreference='Stop'
Push-Location $PSScriptRoot
try {
    python tools/check_local_inputs.py
    if ($LASTEXITCODE -ne 0) { throw 'Missing or mismatched local game-derived inputs' }
    New-Item -ItemType Directory -Path build,$OutputDirectory -Force | Out-Null
    $cc=Join-Path $LlvmBin 'clang-cl.exe'; $ld=Join-Path $LlvmBin 'lld-link.exe'
    & $ld /lib /machine:x64 /def:src/kernel32.def /out:build/kernel32.lib
    if ($LASTEXITCODE -ne 0) { throw 'Import library failed' }
    foreach($name in @('src/addon.cpp','build/api/embedded_payload.cpp','src/icon_gpu.cpp')) {
        $obj='build/'+[IO.Path]::GetFileNameWithoutExtension($name)+'.obj'
        & $cc /nologo /std:c++17 /O2 /GS- /GR- /EHs-c- /Zl /W4 /Isrc /c $name "/Fo$obj"
        if ($LASTEXITCODE -ne 0) { throw "Compile failed: $name" }
    }
    & $ld /dll /timestamp:0 /entry:DllMain /machine:x64 /nodefaultlib /dynamicbase /nxcompat /opt:ref /opt:icf "/out:$OutputDirectory/ds2_atlas_chiral_addon.asi" /implib:build/addon.lib build/addon.obj build/icon_gpu.obj build/embedded_payload.obj build/kernel32.lib
    if ($LASTEXITCODE -ne 0) { throw 'Link failed' }
    Get-FileHash -Algorithm SHA256 "$OutputDirectory/ds2_atlas_chiral_addon.asi"
} finally { Pop-Location }
