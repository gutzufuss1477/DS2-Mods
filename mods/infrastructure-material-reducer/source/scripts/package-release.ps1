param(
  [switch]$UseSourceBuild
)
# Keep -UseSourceBuild for compatibility; source build is always mandatory now.
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$version = [IO.File]::ReadAllText((Join-Path $root 'VERSION.txt')).Trim()
if ($version -notmatch '^[0-9]+\.[0-9]+\.[0-9]+$') { throw 'Invalid VERSION.txt' }
$name = "DS2_Infrastructure_One_Unit_v$version"
$sourceText = [IO.File]::ReadAllText((Join-Path $root 'src\infrastructure_one_unit.cpp'))
if (-not $sourceText.Contains('#define MOD_VERSION "' + $version + '"')) {
  throw "Source MOD_VERSION disagrees with $version"
}
$iniFile = Join-Path $root 'config\ds2_infrastructure_one_unit.ini'
$ini = [IO.File]::ReadAllText($iniFile)
if (-not $ini.Contains('MineProductionCrystalCost=0')) {
  throw 'Public INI must keep the mining option disabled by default'
}
if (-not $ini.Contains('DebugLog=0')) { throw 'DebugLog must default to 0' }
Write-Host "Running offline release checks for v$version..."
& py -3 (Join-Path $root 'tests\test_policy.py')
if ($LASTEXITCODE -ne 0) { throw 'Base infrastructure regression failed' }
& py -3 (Join-Path $root 'tests\test_mine_cost_scaling.py')
if ($LASTEXITCODE -ne 0) { throw 'Mine price scaling regression failed' }
& (Join-Path $root 'scripts\build-llvm.ps1')
if ($LASTEXITCODE -ne 0) { throw 'LLVM source build failed' }
$asi = Join-Path $root 'build\public\ds2_infrastructure_one_unit.asi'
if (!(Test-Path $asi)) { throw 'Compiled ASI missing' }
$asiHash = (Get-FileHash $asi -Algorithm SHA256).Hash.ToUpperInvariant()
# The release/reference binary is exactly the compiled public source binary.
$reference = Join-Path $root 'reference\ds2_infrastructure_one_unit.asi'
Copy-Item -LiteralPath $asi -Destination $reference -Force
$referenceHash = (Get-FileHash $reference -Algorithm SHA256).Hash.ToUpperInvariant()
if ($referenceHash -ne $asiHash) { throw 'Reference ASI hash differs from release build' }
("$asiHash  ds2_infrastructure_one_unit.asi") | Set-Content -LiteralPath (Join-Path $root 'reference\SHA256.txt') -Encoding ascii
$releaseRoot = Join-Path $root "release\$name"
$zip = Join-Path $root "release\$name.zip"
# Never delete/overwrite earlier release folders (v1.0.0 etc.).
if (Test-Path $releaseRoot) { Remove-Item -LiteralPath $releaseRoot -Recurse -Force }
if (Test-Path $zip) { Remove-Item -LiteralPath $zip -Force }
New-Item -ItemType Directory -Path $releaseRoot -Force | Out-Null
Copy-Item -LiteralPath $asi -Destination (Join-Path $releaseRoot 'ds2_infrastructure_one_unit.asi') -Force
Copy-Item -LiteralPath $iniFile -Destination (Join-Path $releaseRoot 'ds2_infrastructure_one_unit.ini') -Force
$readme = @"
Infrastructure One Unit v$version
Update to the existing mod (same ASI and INI filenames).

Install:
1. Exit Death Stranding 2.
2. Copy ds2_infrastructure_one_unit.asi next to DS2.exe, replacing the old ASI.
3. Existing installation: KEEP your configured INI and ADD the key
   MineProductionCrystalCost=0 under [InfrastructureOneUnit].
   Fresh install: copy the included ds2_infrastructure_one_unit.ini.
4. Restart DS2.

Existing infrastructure feature:
RemainingUnits=1 reduces the missing construction/upgrade materials per category.
Roads, monorails and mine builds/upgrades keep their individual INI switches.

NEW mine extraction feature:
MineProductionCrystalCost=0   vanilla price (default/disabled)
MineProductionCrystalCost=1   1 chiral crystal per full extraction batch
MineProductionCrystalCost=10  10 crystals per batch
MineProductionCrystalCost=100 100 crystals per batch
The feature never increases the normal game's price. Native material output
and the player's normal crystal deduction are retained.
There is no zero-cost option: a positive step is required for safe mining.

Validated in game for value 1 at different mines, normal material yield and
matching 1/1 UI. Other positive amounts passed offline tests but are not
individually verified in-game.

Requirements: Steam PC DS2.exe v1.10.89.0 and a compatible x64 ASI loader.
ASI loader not included. Other game versions/launchers are not confirmed.
Back up saved games before first use. Construction progress may persist.
"@
[IO.File]::WriteAllText((Join-Path $releaseRoot 'README.txt'),$readme,[Text.Encoding]::ASCII)
Copy-Item -LiteralPath (Join-Path $root 'NEXUS_CHANGELOG.txt') -Destination (Join-Path $releaseRoot 'CHANGELOG.txt') -Force
$files = Get-ChildItem -LiteralPath $releaseRoot -File | Sort-Object Name
$hashLines = foreach ($file in $files) {
  $h=(Get-FileHash $file.FullName -Algorithm SHA256).Hash.ToUpperInvariant()
  "$h  $($file.Name)"
}
$hashLines | Set-Content -LiteralPath (Join-Path $releaseRoot 'SHA256.txt') -Encoding ascii
Compress-Archive -Path (Join-Path $releaseRoot '*') -DestinationPath $zip -CompressionLevel Optimal -Force
$zipHash=(Get-FileHash $zip -Algorithm SHA256).Hash.ToUpperInvariant()
("$zipHash  $name.zip") | Set-Content -LiteralPath "$zip.sha256.txt" -Encoding ascii
Write-Host "Release ZIP: $zip"
Write-Host "Release SHA-256: $zipHash"
Write-Host "Release ASI SHA-256: $asiHash"
