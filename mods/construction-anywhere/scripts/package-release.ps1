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
$stageDocs = Join-Path $stage 'docs'
New-Item -ItemType Directory -Force -Path $stageDocs | Out-Null

Copy-Item -LiteralPath (Join-Path $root 'build\public\ds2_construction_anywhere.asi') -Destination $stage -Force
Copy-Item -LiteralPath (Join-Path $root 'build\public\ds2_construction_anywhere.ini') -Destination $stage -Force
Copy-Item -LiteralPath (Join-Path $root 'README_DE.md') -Destination (Join-Path $stage 'README.txt') -Force
Copy-Item -LiteralPath (Join-Path $root 'README.md') -Destination (Join-Path $stage 'README_EN.md') -Force
Copy-Item -LiteralPath (Join-Path $root 'CHANGELOG.md') -Destination (Join-Path $stage 'CHANGELOG.txt') -Force
Copy-Item -LiteralPath (Join-Path $root 'SECURITY_NOTES.md') -Destination $stage -Force
Copy-Item -LiteralPath (Join-Path $root 'docs\TEST_PLAN.md') -Destination $stageDocs -Force
Copy-Item -LiteralPath (Join-Path $root 'docs\VALIDATION.md') -Destination $stageDocs -Force
Copy-Item -LiteralPath (Join-Path $root 'docs\TECHNICAL_NOTES.md') -Destination $stageDocs -Force

$asiHash = (Get-FileHash -LiteralPath (Join-Path $stage 'ds2_construction_anywhere.asi') -Algorithm SHA256).Hash.ToUpperInvariant()
@(
    "$asiHash  ds2_construction_anywhere.asi"
    "$((Get-FileHash -LiteralPath (Join-Path $stage 'ds2_construction_anywhere.ini') -Algorithm SHA256).Hash.ToUpperInvariant())  ds2_construction_anywhere.ini"
) | Set-Content -LiteralPath (Join-Path $stage 'SHA256.txt') -Encoding ascii

$zip = Join-Path $releaseRoot "$name.zip"
if (Test-Path -LiteralPath $zip) { Remove-Item -LiteralPath $zip -Force }
Compress-Archive -Path (Join-Path $stage '*') -DestinationPath $zip -CompressionLevel Optimal
$zipHash = (Get-FileHash -LiteralPath $zip -Algorithm SHA256).Hash.ToUpperInvariant()
"$zipHash  $name.zip" | Set-Content -LiteralPath "$zip.sha256.txt" -Encoding ascii

Write-Host "Packaged: $zip"
Write-Host "SHA-256: $zipHash"
