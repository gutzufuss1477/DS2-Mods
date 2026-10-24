# Build with a local Windows LLVM installation. No game files are changed.
[CmdletBinding()]
param([string]$LlvmBin = 'C:\Program Files\LLVM\bin', [string]$OutputDirectory = 'release')
$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$compiler = Join-Path $LlvmBin 'clang-cl.exe'
$linker = Join-Path $LlvmBin 'lld-link.exe'
if (-not (Test-Path -LiteralPath $compiler -PathType Leaf)) {
    $compiler = (Get-Command 'clang-cl.exe' -ErrorAction Stop).Source
}
if (-not (Test-Path -LiteralPath $linker -PathType Leaf)) {
    $linker = (Get-Command 'lld-link.exe' -ErrorAction Stop).Source
}
Push-Location $PSScriptRoot
try {
    python tools/check_local_inputs.py
    if ($LASTEXITCODE -ne 0) { throw 'Missing or mismatched local build inputs' }
    python tools/enforce_visual_router_v1.py
    if ($LASTEXITCODE -ne 0) { throw "Visual API contract regression" }
    # Hard compatibility gate: never build a Crafting host that breaks ATLAS/Chiral Boots Addon API V1.
    python 'tools/enforce_addon_api_v1.py'
    if ($LASTEXITCODE -ne 0) { throw 'Public shoe add-on API V1 regression: build stopped before outputs' }
    New-Item -ItemType Directory -Path 'build', $OutputDirectory -Force | Out-Null
    & $linker '/lib' '/machine:x64' '/def:src/kernel32.def' '/out:build/kernel32.lib'
    if ($LASTEXITCODE -ne 0) { throw "Import library build failed: $LASTEXITCODE" }
    & $compiler '--target=x86_64-pc-windows-msvc' '/nologo' '/std:c++17' '/O2' '/GS-' '/GR-' '/EHs-c-' '/Zl' '/c' 'src/mod.cpp' '/Fobuild/mod.obj'
    if ($LASTEXITCODE -ne 0) { throw "Compilation failed: $LASTEXITCODE" }
    & $compiler '--target=x86_64-pc-windows-msvc' '/nologo' '/std:c++17' '/O2' '/GS-' '/GR-' '/EHs-c-' '/Zl' '/W4' '/c' 'src/atlas/runtime.cpp' '/Fobuild/atlas.obj'
    if ($LASTEXITCODE -ne 0) { throw "ATLAS compilation failed: $LASTEXITCODE" }
    python tools/embed_weapon_pixels.py
    if ($LASTEXITCODE -ne 0) { throw 'Weapon texture validation failed' }
    python tools/embed_coffin_pixels.py
    if ($LASTEXITCODE -ne 0) { throw 'Private coffin albedo validation failed' }
    python tools/embed_story_menu_pixels.py
    if ($LASTEXITCODE -ne 0) { throw 'Original Story menu texture validation failed' }
    & $compiler '--target=x86_64-pc-windows-msvc' '/nologo' '/std:c++17' '/O2' '/GS-' '/GR-' '/EHs-c-' '/Zl' '/W4' '/c' 'src/coffin_menu_gpu.cpp' '/Fobuild/coffin_menu_gpu.obj'
    if ($LASTEXITCODE -ne 0) { throw 'Story menu GPU compilation failed' }
    & $compiler '--target=x86_64-pc-windows-msvc' '/nologo' '/c' 'src/coffin_menu_thunks.s' '/Fobuild/coffin_menu_thunks.obj'
    if ($LASTEXITCODE -ne 0) { throw 'Story menu adapter assembly failed' }
    & $compiler '--target=x86_64-pc-windows-msvc' '/nologo' '/std:c++17' '/O2' '/GS-' '/GR-' '/EHs-c-' '/Zl' '/W4' '/c' 'src/coffin_gpu.cpp' '/Fobuild/coffin_gpu.obj'
    if ($LASTEXITCODE -ne 0) { throw 'Private coffin GPU compilation failed' }
    & $compiler '--target=x86_64-pc-windows-msvc' '/nologo' '/std:c++17' '/O2' '/GS-' '/GR-' '/EHs-c-' '/Zl' '/W4' '/c' 'src/weapons/runtime.cpp' '/Fobuild/weapons.obj'
    if ($LASTEXITCODE -ne 0) { throw 'Weapons compilation failed' }
    & $compiler '--target=x86_64-pc-windows-msvc' '/nologo' '/std:c++17' '/O2' '/GS-' '/GR-' '/EHs-c-' '/Zl' '/W4' '/c' 'src/weapons/pistol_gpu.cpp' '/Fobuild/weapons_gpu.obj'
    if ($LASTEXITCODE -ne 0) { throw 'Weapons GPU compilation failed' }
    & $compiler '--target=x86_64-pc-windows-msvc' '/nologo' '/c' 'src/atlas/loading_thunk.s' '/Fobuild/atlas_loading_thunk.obj'
    if ($LASTEXITCODE -ne 0) { throw "ATLAS assembly failed: $LASTEXITCODE" }
    & $compiler '--target=x86_64-pc-windows-msvc' '/nologo' '/c' 'src/freecrafting_thunks.s'  '/Fobuild/freecrafting_thunks.obj'
    if ($LASTEXITCODE -ne 0) { throw "Assembly failed: $LASTEXITCODE" }
    & $compiler '--target=x86_64-pc-windows-msvc' '/nologo' '/c' 'src/special_boot_thunk.s' '/Fobuild/special_boot_thunk.obj'
    if ($LASTEXITCODE -ne 0) { throw "Special boot assembly failed: $LASTEXITCODE" }
    & $compiler '--target=x86_64-pc-windows-msvc' '/nologo' '/c' 'src/durability_thunks.s' '/Fobuild/durability_thunks.obj'
    if ($LASTEXITCODE -ne 0) { throw "Durability assembly failed: $LASTEXITCODE" }
    & $compiler '--target=x86_64-pc-windows-msvc' '/nologo' '/c' 'src/pcc_thunk.s' '/Fobuild/pcc_thunk.obj'
    if ($LASTEXITCODE -ne 0) { throw 'PCC assembly failed' }
    & $compiler '--target=x86_64-pc-windows-msvc' '/nologo' '/c' 'src/coffin_story_view_thunk.s' '/Fobuild/coffin_story_view_thunk.obj'
    if ($LASTEXITCODE -ne 0) { throw 'Story visual-only assembly failed' }
    $artifact = Join-Path $OutputDirectory 'ds2_crafting_unlocks.asi'
    & $linker '/dll' '/entry:DllMain' '/machine:x64' '/nodefaultlib' '/dynamicbase' '/nxcompat' '/opt:ref' '/opt:icf' "/out:$artifact" '/implib:build/ds2_crafting_unlocks.lib' 'build/mod.obj' 'build/atlas.obj' 'build/weapons.obj' 'build/weapons_gpu.obj' 'build/atlas_loading_thunk.obj' 'build/freecrafting_thunks.obj' 'build/special_boot_thunk.obj' 'build/durability_thunks.obj' 'build/pcc_thunk.obj' 'build/coffin_gpu.obj' 'build/coffin_story_view_thunk.obj' 'build/coffin_menu_gpu.obj' 'build/coffin_menu_thunks.obj' 'build/kernel32.lib'
    if ($LASTEXITCODE -ne 0) { throw "Link failed: $LASTEXITCODE" }
    $configuration = Join-Path $OutputDirectory 'ds2_crafting_unlocks.ini'
    if (-not (Test-Path -LiteralPath $configuration)) { Copy-Item -LiteralPath 'config/default.ini' -Destination $configuration }
    Get-FileHash -Algorithm SHA256 -LiteralPath $artifact
} finally {
    Pop-Location
}
