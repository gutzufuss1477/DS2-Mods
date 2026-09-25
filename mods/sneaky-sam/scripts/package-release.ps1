$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$build = Join-Path $root 'build\public\DS2_SneakySam_v1.0.1.asi'
$release = Join-Path $root 'release'
$folder = Join-Path $release 'DS2_Sneaky_Sam_v1.0.1'
$zip = Join-Path $release 'DS2_Sneaky_Sam_v1.0.1.zip'

if (!(Test-Path $build)) { throw 'Run scripts\build-msvc.cmd first.' }
if (Test-Path $folder) { Remove-Item $folder -Recurse -Force }
New-Item -ItemType Directory -Force -Path $folder | Out-Null

Copy-Item $build (Join-Path $folder 'DS2_SneakySam_v1.0.1.asi') -Force
Copy-Item (Join-Path $root 'README.md') (Join-Path $folder 'README.txt') -Force

if (Test-Path $zip) { Remove-Item $zip -Force }
Compress-Archive -Path "$folder\*" -DestinationPath $zip -CompressionLevel Optimal

$asiHash = (Get-FileHash (Join-Path $folder 'DS2_SneakySam_v1.0.1.asi') -Algorithm SHA256).Hash.ToUpper()
$zipHash = (Get-FileHash $zip -Algorithm SHA256).Hash.ToUpper()
Set-Content (Join-Path $root 'SHA256SUMS.txt') @(
    "$asiHash  DS2_SneakySam_v1.0.1.asi",
    "$zipHash  DS2_Sneaky_Sam_v1.0.1.zip"
) -Encoding ascii

Write-Output "ASI_SHA256=$asiHash"
Write-Output "ZIP_SHA256=$zipHash"
