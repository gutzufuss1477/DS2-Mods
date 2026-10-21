[CmdletBinding()]
param([string]$GameDir='C:\Program Files (x86)\Steam\steamapps\common\DEATH STRANDING 2 - ON THE BEACH')
$ErrorActionPreference='Stop'
Set-StrictMode -Version Latest
if(Get-Process DS2 -ErrorAction SilentlyContinue){throw 'Close DS2 before migrating the installed ASIs. No process was stopped.'}
$modRoot=Split-Path -Parent $PSScriptRoot
$gameRoot=(Resolve-Path -LiteralPath $GameDir).Path
if((Get-FileHash -LiteralPath (Join-Path $gameRoot 'DS2.exe')).Hash -ne 'BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B'){throw 'Unsupported DS2.exe; nothing installed.'}
$release=Join-Path $modRoot 'release'
$source=Join-Path $release 'ds2_crafting_unlocks.asi'
$expected=(Get-Content -LiteralPath (Join-Path $release 'SHA256SUMS.txt') | Where-Object {$_ -match '  ds2_crafting_unlocks\.asi$'}).Split(' ')[0]
if((Get-FileHash -LiteralPath $source).Hash -ne $expected){throw 'Release checksum differs; rebuild the package first.'}
$asi=Join-Path $gameRoot 'ds2_crafting_unlocks.asi'
$ini=Join-Path $gameRoot 'ds2_crafting_unlocks.ini'
$hadAsi=Test-Path -LiteralPath $asi
$hadIni=Test-Path -LiteralPath $ini
$defaults=[IO.File]::ReadAllText((Join-Path $release 'ds2_crafting_unlocks.ini'))
$config=if($hadIni){[IO.File]::ReadAllText($ini)}else{$defaults}
# Add only missing new settings; preserve explicitly configured 0/1 values.
foreach($section in @('AtlasEquipment','SuppressedWeapons')){
 $sectionPattern='(?mis)^\['+[regex]::Escape($section)+'\][^\r\n]*\r?\n(?:(?!^\[).)*'
 $defaultBlock=[regex]::Match($defaults,$sectionPattern).Value
 $current=[regex]::Match($config,$sectionPattern)
 if(-not $current.Success){$config+="`r`n"+$defaultBlock;continue}
 $missing=''
 foreach($match in [regex]::Matches($defaultBlock,'(?m)^([A-Za-z][A-Za-z0-9]*)\s*=.*$')){
  $key=$match.Groups[1].Value
  if(-not [regex]::IsMatch($current.Value,'(?mi)^\s*'+[regex]::Escape($key)+'\s*=')){$missing+=$match.Value.TrimEnd()+"`r`n"}
 }
 if($missing){$config=$config.Insert($current.Index+$current.Length,"`r`n"+$missing+"`r`n")}
}
if(Get-Process DS2 -ErrorAction SilentlyContinue){throw 'DS2 started during validation. No files changed.'}
$stamp=[DateTime]::UtcNow.ToString('yyyyMMdd-HHmmss-fff')
$backup=Join-Path $modRoot ('build/install-backups/'+$stamp)
New-Item -ItemType Directory -Path $backup -Force | Out-Null
$legacyNames=@('ds2_overpowered_equipment.asi','ds2_more_silenced_guns.asi')
foreach($name in @('ds2_crafting_unlocks.asi','ds2_crafting_unlocks.ini')+$legacyNames){
 $file=Join-Path $gameRoot $name
 if(Test-Path -LiteralPath $file){Copy-Item -LiteralPath $file -Destination (Join-Path $backup $name)}
}
[IO.File]::WriteAllText((Join-Path $backup 'installed.ini'),$config,[Text.UTF8Encoding]::new($false))
$retired=@{}
try{
 foreach($name in $legacyNames){
  $file=Join-Path $gameRoot $name
  if(Test-Path -LiteralPath $file){$destination=$file+'.disabled-crafting-1.7.0-'+$stamp;Move-Item -LiteralPath $file -Destination $destination;$retired[$file]=$destination}
 }
 Copy-Item -LiteralPath $source -Destination $asi
 Copy-Item -LiteralPath (Join-Path $backup 'installed.ini') -Destination $ini
 if((Get-FileHash -LiteralPath $source).Hash -ne (Get-FileHash -LiteralPath $asi).Hash){throw 'Installed ASI hash mismatch'}
 if((Get-FileHash -LiteralPath (Join-Path $backup 'installed.ini')).Hash -ne (Get-FileHash -LiteralPath $ini).Hash){throw 'Installed INI hash mismatch'}
}catch{
 if($hadAsi){Copy-Item -LiteralPath (Join-Path $backup 'ds2_crafting_unlocks.asi') -Destination $asi}else{if(Test-Path -LiteralPath $asi){Remove-Item -LiteralPath $asi}}
 if($hadIni){Copy-Item -LiteralPath (Join-Path $backup 'ds2_crafting_unlocks.ini') -Destination $ini}else{if(Test-Path -LiteralPath $ini){Remove-Item -LiteralPath $ini}}
 foreach($file in $retired.Keys){if(Test-Path -LiteralPath $retired[$file]){Move-Item -LiteralPath $retired[$file] -Destination $file}}
 throw
}
[ordered]@{version='1.7.0';game=$gameRoot;backup=$backup;retiredStandalone=$retired;existingSettingsPreserved=$true;asiSha256=(Get-FileHash -LiteralPath $asi).Hash;installedIniSha256=(Get-FileHash -LiteralPath $ini).Hash} | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $modRoot 'validation/install-1.7.0.json') -Encoding utf8
Write-Output "Crafting & Equipment Overhaul 1.7.0 installed. Existing settings retained; missing new options added. Backups: $backup"
