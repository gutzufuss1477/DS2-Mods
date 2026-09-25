# Build with a local Windows LLVM installation. No game files are changed.
[CmdletBinding()]
param([string]$LlvmBin = 'C:\Program Files\LLVM\bin')
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
    New-Item -ItemType Directory -Path 'build', 'release' -Force | Out-Null
    & $linker '/lib' '/machine:x64' '/def:src/kernel32.def' '/out:build/kernel32.lib'
    if ($LASTEXITCODE -ne 0) { throw "Import library build failed: $LASTEXITCODE" }
    & $compiler '--target=x86_64-pc-windows-msvc' '/nologo' '/std:c++17' '/O2' '/GS-' '/GR-' '/EHs-c-' '/Zl' '/c' 'src/mod.cpp' '/Fobuild/mod.obj'
    if ($LASTEXITCODE -ne 0) { throw "Compilation failed: $LASTEXITCODE" }
    & $compiler '--target=x86_64-pc-windows-msvc' '/nologo' '/c' 'src/freecrafting_thunks.s' '/Fobuild/freecrafting_thunks.obj'
    if ($LASTEXITCODE -ne 0) { throw "Assembly failed: $LASTEXITCODE" }
    & $compiler '--target=x86_64-pc-windows-msvc' '/nologo' '/c' 'src/special_boot_thunk.s' '/Fobuild/special_boot_thunk.obj'
    if ($LASTEXITCODE -ne 0) { throw "Special boot assembly failed: $LASTEXITCODE" }
    & $compiler '--target=x86_64-pc-windows-msvc' '/nologo' '/c' 'src/durability_thunks.s' '/Fobuild/durability_thunks.obj'
    if ($LASTEXITCODE -ne 0) { throw "Durability assembly failed: $LASTEXITCODE" }
    & $linker '/dll' '/entry:DllMain' '/machine:x64' '/nodefaultlib' '/dynamicbase' '/nxcompat' '/opt:ref' '/opt:icf' '/out:release/ds2_crafting_unlocks.asi' '/implib:build/ds2_crafting_unlocks.lib' 'build/mod.obj' 'build/freecrafting_thunks.obj' 'build/special_boot_thunk.obj' 'build/durability_thunks.obj' 'build/kernel32.lib'
    if ($LASTEXITCODE -ne 0) { throw "Link failed: $LASTEXITCODE" }
    Get-FileHash -Algorithm SHA256 -LiteralPath 'release/ds2_crafting_unlocks.asi'
} finally {
    Pop-Location
}
