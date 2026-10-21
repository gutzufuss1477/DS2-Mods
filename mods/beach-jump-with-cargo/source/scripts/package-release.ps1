param(
    [string]$LlvmBin = "$env:ProgramFiles\LLVM\bin",
    [string]$ResourceCompiler = "",
    [string]$Python = "python",
    [string]$GameExe = ""
)
$ErrorActionPreference = 'Stop'
if (-not $GameExe) {
    $pf86 = ${env:ProgramFiles(x86)}
    if (-not $pf86) { $pf86 = 'C:\Program Files (x86)' }
    $GameExe = Join-Path $pf86 'Steam\steamapps\common\DEATH STRANDING 2 - ON THE BEACH\DS2.exe'
}
$root = Split-Path -Parent $PSScriptRoot

& (Join-Path $PSScriptRoot 'build.ps1') -LlvmBin $LlvmBin -ResourceCompiler $ResourceCompiler
$binary = Join-Path $root 'build\ds2_beach_jump_with_cargo.asi'
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
try {
    $env:DS2_EXE = $GameExe
    & $Python (Join-Path $root 'tests\verify_release.py')
    if ($LASTEXITCODE -ne 0) { throw 'Release verification failed.' }
    & $Python (Join-Path $PSScriptRoot 'package_release.py')
    if ($LASTEXITCODE -ne 0) { throw 'Packaging failed.' }
} finally {
    $env:DS2_EXE = $savedGame
}
