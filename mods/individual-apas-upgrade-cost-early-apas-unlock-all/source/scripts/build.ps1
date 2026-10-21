param([switch]$Tests, [ValidateSet('MSVC','LLVM')][string]$Toolchain = 'MSVC')
$ErrorActionPreference = 'Stop'
& (Join-Path $PSScriptRoot '../../../tools/native/build-hardened.ps1') -Mod apas -Toolchain $Toolchain -Tests:$Tests
