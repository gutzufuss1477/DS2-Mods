[CmdletBinding()]
param(
 [string]$GameDir='C:\Program Files (x86)\Steam\steamapps\common\DEATH STRANDING 2 - ON THE BEACH',
 [switch]$ValidateOnly
)
$ErrorActionPreference='Stop'
Set-StrictMode -Version Latest
if(Get-Process -Name DS2 -ErrorAction SilentlyContinue){throw 'Close DS2 before installing. No files changed.'}
$gameRoot=(Resolve-Path -LiteralPath $GameDir).Path
$payload=if(Test-Path -LiteralPath (Join-Path $PSScriptRoot 'release/manifest.json')){Join-Path $PSScriptRoot 'release'}else{$PSScriptRoot}
$manifest=Get-Content -Raw -LiteralPath (Join-Path $payload 'manifest.json') | ConvertFrom-Json
if((Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $gameRoot 'DS2.exe')).Hash -ne $manifest.gameSha256){throw 'Unsupported DS2.exe. No files changed.'}
$name='ds2_all_containers_small.asi'
$source=Join-Path $payload $name
$expected=$manifest.files.PSObject.Properties[$name].Value
if((Get-FileHash -Algorithm SHA256 -LiteralPath $source).Hash -ne $expected){throw 'Package integrity check failed.'}
$loaders=@('winmm.dll','dinput8.dll','version.dll','dsound.dll') | Where-Object {Test-Path -LiteralPath (Join-Path $gameRoot $_)}
if(-not $loaders){throw 'An existing ASI loader is required. No files changed.'}
if($ValidateOnly){Write-Output 'Validation passed: supported EXE, intact ASI, proxy DLL present. No files installed.';return}
if(Get-Process -Name DS2 -ErrorAction SilentlyContinue){throw 'DS2 started during validation. No files changed.'}
$backup=Join-Path $PSScriptRoot ('install-backups/'+[DateTime]::UtcNow.ToString('yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Force -Path $backup | Out-Null
$dest=Join-Path $gameRoot $name
$previous=Test-Path -LiteralPath $dest
if($previous){Copy-Item -LiteralPath $dest -Destination (Join-Path $backup $name)}
try {
 Copy-Item -LiteralPath $source -Destination $dest
 if((Get-FileHash -LiteralPath $dest).Hash -ne $expected){throw 'Installed hash mismatch.'}
} catch {
 if($previous){Copy-Item -LiteralPath (Join-Path $backup $name) -Destination $dest}
 elseif(Test-Path -LiteralPath $dest){Remove-Item -LiteralPath $dest}
 throw
}
[ordered]@{version=$manifest.version;game=$gameRoot;backup=$backup;asiSha256=$expected;installedAtUtc=[DateTime]::UtcNow.ToString('o')} | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $backup 'install.json') -Encoding utf8
Write-Output "All Containers Small $($manifest.version) installed. Start DS2 normally. Backup: $backup"
