param(
    [switch]$UseSourceBuild
)

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$version = '1.0.0'
$name = "DS2_Infrastructure_One_Unit_v$version"

if ($UseSourceBuild) {
    & (Join-Path $root 'scripts\build-llvm.ps1')
    if ($LASTEXITCODE) { throw 'Build failed' }
    $asi = Join-Path $root 'build\public\ds2_infrastructure_one_unit.asi'
} else {
    $asi = Join-Path $root 'reference\ds2_infrastructure_one_unit.asi'
}

if (!(Test-Path $asi)) { throw "ASI missing: $asi" }

$releaseRoot = Join-Path $root "release\$name"
$zip = Join-Path $root "release\$name.zip"
Remove-Item $releaseRoot -Recurse -Force -ErrorAction SilentlyContinue
Remove-Item $zip -Force -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force $releaseRoot | Out-Null

Copy-Item $asi (Join-Path $releaseRoot 'ds2_infrastructure_one_unit.asi') -Force
Copy-Item (Join-Path $root 'config\ds2_infrastructure_one_unit.ini') (Join-Path $releaseRoot 'ds2_infrastructure_one_unit.ini') -Force

$readme = @'
Infrastructure One Unit v1.0.0

Install:
1. Install a compatible x64 ASI loader for DS2.
2. Copy the ASI and INI next to DS2.exe / the ASI loader.
3. Restart the game.

Supported:
- Roads: Stage 1 construction and Stage 2 upgrades
- Monorails: Stage 1 construction
- Mines: Stage 1 construction and Stage 2/3 upgrades

Configuration:
RemainingUnits=1 means one unit per still-open material category.
Higher values are supported and never increase a smaller native remainder.

Existing online/player contributions are preserved.
The game keeps native container splitting and overflow handling.

Supported DS2.exe: Steam v1.10.89.0 only.
The ASI loader is not included.

Save warning:
The game may persist the reduced infrastructure progress. Back up a save before first use.
'@
Set-Content -LiteralPath (Join-Path $releaseRoot 'README.txt') -Value $readme -Encoding ascii

$files = Get-ChildItem -LiteralPath $releaseRoot -File | Sort-Object Name
$hashLines = foreach ($file in $files) {
    $hash = (Get-FileHash $file.FullName -Algorithm SHA256).Hash.ToUpperInvariant()
    "$hash  $($file.Name)"
}
$hashLines | Set-Content -LiteralPath (Join-Path $releaseRoot 'SHA256.txt') -Encoding ascii

Compress-Archive -Path (Join-Path $releaseRoot '*') -DestinationPath $zip -CompressionLevel Optimal
$zipHash = (Get-FileHash $zip -Algorithm SHA256).Hash.ToUpperInvariant()
"$zipHash  $name.zip" | Set-Content -LiteralPath "$zip.sha256.txt" -Encoding ascii

Write-Host "Release: $zip"
Write-Host "SHA-256: $zipHash"
