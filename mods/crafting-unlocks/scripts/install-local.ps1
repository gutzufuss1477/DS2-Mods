[CmdletBinding()]
param([string]$GameDir='C:\Program Files (x86)\Steam\steamapps\common\DEATH STRANDING 2 - ON THE BEACH')
$ErrorActionPreference='Stop'
Set-StrictMode -Version Latest
if (Get-Process DS2 -ErrorAction SilentlyContinue) { throw 'Close DS2 before migrating the installed ASIs. No process was stopped.' }
$modRoot=Split-Path -Parent $PSScriptRoot
$gameRoot=(Resolve-Path -LiteralPath $GameDir).Path
if ((Get-FileHash -LiteralPath (Join-Path $gameRoot 'DS2.exe')).Hash -ne 'BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B') { throw 'Unsupported DS2.exe; nothing installed.' }
$stamp=[DateTime]::UtcNow.ToString('yyyyMMdd-HHmmss-fff')
$backup=Join-Path $modRoot ('build/install-backups/'+$stamp)
New-Item -ItemType Directory -Path $backup -Force | Out-Null
$asi=Join-Path $gameRoot 'ds2_crafting_unlocks.asi'
$ini=Join-Path $gameRoot 'ds2_crafting_unlocks.ini'
$legacy=Join-Path $gameRoot 'ds2_overpowered_equipment.asi'
$retired=$legacy+'.disabled-crafting-1.6.0-'+$stamp
$hadAsi=Test-Path -LiteralPath $asi
$hadIni=Test-Path -LiteralPath $ini
$hadLegacy=Test-Path -LiteralPath $legacy
foreach($name in @('ds2_crafting_unlocks.asi','ds2_crafting_unlocks.ini','ds2_overpowered_equipment.asi')) {
 $file=Join-Path $gameRoot $name
 if(Test-Path -LiteralPath $file){Copy-Item -LiteralPath $file -Destination (Join-Path $backup $name)}
}
$configSource=if($hadIni){$ini}else{Join-Path $modRoot 'release/ds2_crafting_unlocks.ini'}
$config=[IO.File]::ReadAllText($configSource)
if($config -match '(?im)^\[AtlasEquipment\]') {
 $config=[regex]::Replace($config,'(?ims)(^\[AtlasEquipment\]\s*\r?\n(?:(?!^\[).)*?^Enabled\s*=\s*)[01]',{param($m) $m.Groups[1].Value+'1'})
}else{$config+="`r`n[AtlasEquipment]`r`nEnabled=1`r`n"}
[IO.File]::WriteAllText((Join-Path $backup 'installed.ini'),$config,[Text.UTF8Encoding]::new($false))
$source=Join-Path $modRoot 'release/ds2_crafting_unlocks.asi'
try {
 if($hadLegacy){Move-Item -LiteralPath $legacy -Destination $retired}
 Copy-Item -LiteralPath $source -Destination $asi
 Copy-Item -LiteralPath (Join-Path $backup 'installed.ini') -Destination $ini
 if((Get-FileHash -LiteralPath $source).Hash -ne (Get-FileHash -LiteralPath $asi).Hash){throw 'Installed ASI hash mismatch'}
 if((Get-FileHash -LiteralPath (Join-Path $backup 'installed.ini')).Hash -ne (Get-FileHash -LiteralPath $ini).Hash){throw 'Installed INI hash mismatch'}
}catch{
 if($hadAsi){Copy-Item -LiteralPath (Join-Path $backup 'ds2_crafting_unlocks.asi') -Destination $asi}else{if(Test-Path -LiteralPath $asi){Remove-Item -LiteralPath $asi}}
 if($hadIni){Copy-Item -LiteralPath (Join-Path $backup 'ds2_crafting_unlocks.ini') -Destination $ini}else{if(Test-Path -LiteralPath $ini){Remove-Item -LiteralPath $ini}}
 if($hadLegacy -and (Test-Path -LiteralPath $retired)){Move-Item -LiteralPath $retired -Destination $legacy}
 throw
}
[ordered]@{version='1.6.0';game=$gameRoot;backup=$backup;atlasEnabled=1;retiredStandalone=$retired;asiSha256=(Get-FileHash -LiteralPath $asi).Hash;installedIniSha256=(Get-FileHash -LiteralPath $ini).Hash} | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $modRoot 'validation/install-1.6.0.json') -Encoding utf8
Write-Output "Crafting & Equipment Overhaul 1.6.0 installed with ATLAS enabled. Previous files: $backup"
