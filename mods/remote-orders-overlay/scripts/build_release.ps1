$ErrorActionPreference = 'Stop'
$Root = Split-Path -Parent $PSScriptRoot
$Output = Join-Path $Root 'dist\DS2_Remote_Orders_Overlay.asi'
New-Item -ItemType Directory -Force -Path (Split-Path -Parent $Output) | Out-Null
python (Join-Path $Root 'tools\patch_no_log.py') `
    (Join-Path $Root 'validated_baseline\DS2_Remote_Orders_Intermediate_Overlay_compact_width.asi') `
    $Output
Write-Host "Release created: $Output"
