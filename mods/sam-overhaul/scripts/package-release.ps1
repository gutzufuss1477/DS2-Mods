param(
    [string]$TargetExe = ""
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
$version = (Get-Content -LiteralPath (Join-Path $root "VERSION.txt") -Raw).Trim()
if ($version -notmatch '^\d+\.\d+\.\d+$') {
    throw "Development build: release packaging is blocked. Use build-development.ps1; no release was created."
}
$build = Join-Path $root "build\public"
$release = Join-Path $root "release"
$folder = Join-Path $release "DS2_Sam_Overhaul_v$version"
$zip = Join-Path $release "DS2_Sam_Overhaul_v$version.zip"

& (Join-Path $PSScriptRoot "build-msvc.cmd")
if ($LASTEXITCODE -ne 0) { throw "Build failed with exit code $LASTEXITCODE" }

if (Test-Path $folder) { Remove-Item $folder -Recurse -Force }
New-Item -ItemType Directory -Path $folder -Force | Out-Null

Copy-Item (Join-Path $build "DS2_Sam_Overhaul_v$version.asi") $folder
Copy-Item (Join-Path $root "ds2_sam_overhaul.ini") $folder
Copy-Item (Join-Path $root "README.md") $folder
Copy-Item (Join-Path $root "CHANGELOG.md") $folder

if ($TargetExe) {
    $expected = "BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B"
    $actual = (Get-FileHash -Algorithm SHA256 -LiteralPath $TargetExe).Hash
    if ($actual -ne $expected) {
        throw "Unsupported DS2.exe SHA256: $actual"
    }
}

if (Test-Path $zip) { Remove-Item $zip -Force }
Compress-Archive -Path (Join-Path $folder "*") -DestinationPath $zip -CompressionLevel Optimal

$hashLines = @()
Get-ChildItem $folder -File | Sort-Object Name | ForEach-Object {
    $hash = (Get-FileHash -Algorithm SHA256 -LiteralPath $_.FullName).Hash
    $hashLines += "$hash  $($_.Name)"
}
$zipHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $zip).Hash
$hashLines += "$zipHash  $(Split-Path -Leaf $zip)"
$hashLines | Set-Content -LiteralPath (Join-Path $root "SHA256SUMS.txt") -Encoding ASCII

Write-Host "Release: $zip"
Write-Host "SHA256: $zipHash"
