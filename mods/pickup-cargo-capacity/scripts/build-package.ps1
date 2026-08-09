[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'

$modRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$buildRoot = [IO.Path]::GetFullPath((Join-Path $modRoot 'build'))
$asiPath = Join-Path $buildRoot 'PickupCargoCapacity.asi'
$packageName = 'DS2_Pickup_Cargo_Capacity_v1.0.1'
$packageDir = [IO.Path]::GetFullPath((Join-Path $buildRoot $packageName))
$zipPath = [IO.Path]::GetFullPath((Join-Path $buildRoot ($packageName + '.zip')))

if (-not (Test-Path -LiteralPath $asiPath)) {
    throw "Build output not found: $asiPath"
}
if (-not $packageDir.StartsWith($buildRoot, [StringComparison]::OrdinalIgnoreCase)) {
    throw 'Package directory escaped the build directory.'
}

if (Test-Path -LiteralPath $packageDir) {
    Remove-Item -LiteralPath $packageDir -Recurse -Force
}
if (Test-Path -LiteralPath $zipPath) {
    Remove-Item -LiteralPath $zipPath -Force
}

New-Item -ItemType Directory -Path $packageDir | Out-Null
Copy-Item -LiteralPath $asiPath -Destination (Join-Path $packageDir 'PickupCargoCapacity.asi')
Copy-Item -LiteralPath (Join-Path $modRoot 'config\PickupCargoCapacity.ini') -Destination $packageDir
Copy-Item -LiteralPath (Join-Path $modRoot 'README.md') -Destination (Join-Path $packageDir 'README_EN.md')
Copy-Item -LiteralPath (Join-Path $modRoot 'SECURITY_NOTES.md') -Destination $packageDir
Copy-Item -LiteralPath (Join-Path $modRoot 'CHANGELOG.md') -Destination $packageDir
Copy-Item -LiteralPath (Join-Path $modRoot 'VALIDATION.md') -Destination $packageDir

$fixedTimestamp = [DateTime]'2026-08-09T00:00:00Z'
Get-ChildItem -LiteralPath $packageDir -File | ForEach-Object {
    $_.LastWriteTimeUtc = $fixedTimestamp
}

$hashFiles = Get-ChildItem -LiteralPath $packageDir -File | Sort-Object Name
$hashLines = foreach ($file in $hashFiles) {
    $hash = (Get-FileHash -LiteralPath $file.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
    "$hash  $($file.Name)"
}
Set-Content -LiteralPath (Join-Path $packageDir 'SHA256SUMS.txt') -Value $hashLines -Encoding ascii
(Get-Item -LiteralPath (Join-Path $packageDir 'SHA256SUMS.txt')).LastWriteTimeUtc = $fixedTimestamp

Compress-Archive -LiteralPath $packageDir -DestinationPath $zipPath -CompressionLevel Optimal

$zipHash = (Get-FileHash -LiteralPath $zipPath -Algorithm SHA256).Hash
Write-Host "Built: $zipPath"
Write-Host "SHA-256: $zipHash"
