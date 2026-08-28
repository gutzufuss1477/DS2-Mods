$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
& (Join-Path $root 'scripts\build-llvm.ps1')

$name = 'DS2_Construction_Anywhere_v1.0.0'
$releaseRoot = Join-Path $root 'release'
$stage = Join-Path $releaseRoot $name
$fullReleaseRoot = [System.IO.Path]::GetFullPath($releaseRoot).TrimEnd('\') + '\'
$fullStage = [System.IO.Path]::GetFullPath($stage)
if (!$fullStage.StartsWith($fullReleaseRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
    throw "Refusing to recreate staging directory outside release root: $fullStage"
}
if (Test-Path -LiteralPath $fullStage) {
    Remove-Item -LiteralPath $fullStage -Recurse -Force
}
New-Item -ItemType Directory -Force -Path $stage | Out-Null

Copy-Item -LiteralPath (Join-Path $root 'build\public\ds2_construction_anywhere.asi') -Destination $stage -Force
Copy-Item -LiteralPath (Join-Path $root 'build\public\ds2_construction_anywhere.ini') -Destination $stage -Force

$zip = Join-Path $releaseRoot "$name.zip"
if (Test-Path -LiteralPath $zip) { Remove-Item -LiteralPath $zip -Force }
Compress-Archive -Path (Join-Path $stage '*') -DestinationPath $zip -CompressionLevel Optimal
$legacyHash = "$zip.sha256.txt"
if (Test-Path -LiteralPath $legacyHash) { Remove-Item -LiteralPath $legacyHash -Force }

Write-Host "Packaged: $zip"
