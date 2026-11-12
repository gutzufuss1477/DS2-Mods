[CmdletBinding()]
param()
$ErrorActionPreference='Stop'
Set-StrictMode -Version Latest
$release=Join-Path $PSScriptRoot 'release'
if(Test-Path -LiteralPath (Join-Path $PSScriptRoot 'docs/README.txt')){
 foreach($readme in @('README.txt','README_DE.txt')){Copy-Item -LiteralPath (Join-Path $PSScriptRoot ('docs/'+$readme)) -Destination (Join-Path $release $readme)}
}
Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'install.ps1') -Destination (Join-Path $release 'install.ps1')
$names=@('ds2_all_containers_small.asi','README.txt','README_DE.txt','install.ps1')
$files=[ordered]@{}
foreach($name in $names){$files[$name]=(Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $release $name)).Hash.ToLowerInvariant()}
$log=Join-Path $PSScriptRoot 'build/tests-1.0.0.log'
if(-not (Test-Path -LiteralPath $log)){throw 'Run and capture the tests first.'}
$testText=Get-Content -Raw -LiteralPath $log
if($testText -notmatch 'Ran 17 tests' -or $testText -notmatch '(?m)^OK\s*$'){throw 'Latest test log does not show passing tests.'}
if((Get-Item -LiteralPath $log).LastWriteTimeUtc -lt (Get-Item -LiteralPath (Join-Path $release 'ds2_all_containers_small.asi')).LastWriteTimeUtc){throw 'Build changed after tests. Run tests again.'}
$manifest=[ordered]@{
 name='All Containers Small';version='1.0.0';gameVersion='1.10.89.0'
 gameSha256='bf3d1c665545930bc850d8f5df486f7395885bb729d4fd408fdb03390de0765b'
 inGameVerification=[ordered]@{date='2026-11-12';runtimeBuild='0.1.1';releaseRuntimeChange='version label only';confirmed=@('startup','cargo collection: S models and size','inventory: S size and unchanged weights','small cases on backpack');notConfirmed=@('all equipment deployment paths','long sessions','save/reload with repacked cargo')}
 nativeTestsPassed=17;resourceDefinitionsChecked=2058
 sizeAndModels='S, canonical small case with matching appearance; functional entities preserved'
 preserved='Cart/EnemyCart, captured animals, BB pods/prototype, bodies, vehicles, Coffinboard, GhostMech guns, special scripted entity, NonBaggage, Human/Coffinboard volumes'
 fixes='0.1.0 startup crash at DS2+0x11CE82F: preserve DSHoveringCartLowComponent through original entity selection; ignore helper processes before opening log'
 hookRvas=@('0xB6DA86','0x11DCB00','0x11DCDC0');files=$files
}
$manifest | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $release 'manifest.json') -Encoding utf8
$dist=Join-Path $PSScriptRoot 'dist';New-Item -ItemType Directory -Force -Path $dist | Out-Null
$zip=Join-Path $dist 'DS2_All_Containers_Small_v1.0.0.zip'
$paths=@($names+@('manifest.json') | ForEach-Object {Join-Path $release $_})
Compress-Archive -LiteralPath $paths -DestinationPath $zip -Force
Get-FileHash -Algorithm SHA256 -LiteralPath $zip
