param(
    [string]$LlvmBin = "$env:ProgramFiles\LLVM\bin",
    [string]$ResourceCompiler = "",
    [string]$Python = 'python',
    [string]$GameExe = "${env:ProgramFiles(x86)}\Steam\steamapps\common\DEATH STRANDING 2 - ON THE BEACH\DS2.exe"
)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
& (Join-Path $PSScriptRoot 'build.ps1') -LlvmBin $LlvmBin -ResourceCompiler $ResourceCompiler
$binary = Join-Path $root 'build\ds2_jump_ramp_unlimited.asi'
$version = (Get-Content -LiteralPath (Join-Path $root 'VERSION.txt') -Raw).Trim()
$info = (Get-Item -LiteralPath $binary).VersionInfo
if ($info.FileVersion -ne $version -or $info.ProductVersion -ne $version) {
    throw 'PE version metadata does not match VERSION.txt.'
}
$firstHash = (Get-FileHash -LiteralPath $binary -Algorithm SHA256).Hash
& (Join-Path $PSScriptRoot 'build.ps1') -LlvmBin $LlvmBin -ResourceCompiler $ResourceCompiler
if ((Get-FileHash -LiteralPath $binary -Algorithm SHA256).Hash -ne $firstHash) {
    throw 'Repeated build produced a different binary.'
}
$savedGame = $env:DS2_EXE
$savedLlvm = $env:LLVM_BIN
try {
    $env:DS2_EXE = $GameExe
    $env:LLVM_BIN = $LlvmBin
    & $Python (Join-Path $root 'tests\verify_native_dispatcher.py')
    if ($LASTEXITCODE -ne 0) { throw 'Native verification failed.' }
    & $Python (Join-Path $PSScriptRoot 'package_release.py')
    if ($LASTEXITCODE -ne 0) { throw 'Packaging failed.' }
} finally {
    $env:DS2_EXE = $savedGame
    $env:LLVM_BIN = $savedLlvm
}
