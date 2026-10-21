$ErrorActionPreference = 'Stop'

$Root = Split-Path -Parent $PSScriptRoot
$Reference = Join-Path $Root 'reference\DS2VehicleCore_v1.1.0.asi'
$Expected = '78F4D90B6473F9163E65A227AE6444D516133ABAC34C9E6575BD2B13C3CB60B6'

if (-not (Test-Path -LiteralPath $Reference)) {
    throw "Reference ASI not found: $Reference"
}

$Actual = (Get-FileHash -LiteralPath $Reference -Algorithm SHA256).Hash

Write-Host "Expected: $Expected"
Write-Host "Actual:   $Actual"

if ($Actual -ne $Expected) {
    throw 'Reference ASI hash mismatch.'
}

Write-Host 'Reference ASI hash is valid.'
