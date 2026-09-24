# Place next to the ASI and run after closing the game.
# Collects only this mod's log, catalogue, selection template, and configuration.
[CmdletBinding()]
param([string]$GameDirectory = $PSScriptRoot)
$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$folder = (Resolve-Path -LiteralPath $GameDirectory).Path
$names = @(
    'ds2_crafting_unlocks.log',
    'ds2_crafting_catalogue.tsv',
    'ds2_crafting_items.generated.ini',
    'ds2_crafting_unlocks.ini'
)
$files = @($names | ForEach-Object {
    $p = Join-Path $folder $_
    if (Test-Path -LiteralPath $p -PathType Leaf) { $p }
})
if (-not (Test-Path -LiteralPath (Join-Path $folder 'ds2_crafting_unlocks.log'))) {
    throw 'No mod log found. Check that the ASI and INI are next to DS2.exe and the existing ASI loader is active.'
}
$destination = Join-Path $folder ('DS2_Crafting_Test_Result_' + [guid]::NewGuid().ToString('N').Substring(0, 12) + '.zip')
Compress-Archive -LiteralPath $files -DestinationPath $destination -CompressionLevel Optimal
Write-Output $destination
