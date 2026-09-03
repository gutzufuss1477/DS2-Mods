$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
& (Join-Path $root 'scripts\test-synthetic.ps1')

$name = 'DS2_Climbing_Power_Gloves_Range_v1.0.0'
$releaseRoot = Join-Path $root 'release'
$stage = Join-Path $releaseRoot $name
$fullReleaseRoot = [System.IO.Path]::GetFullPath($releaseRoot).TrimEnd('\') + '\'
$fullStage = [System.IO.Path]::GetFullPath($stage)
if (!$fullStage.StartsWith($fullReleaseRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
    throw "Refusing to recreate staging directory outside release root: $fullStage"
}
if (Test-Path -LiteralPath $fullStage) {
    Remove-Item -LiteralPath $fullStage -Recurse -Force
}
New-Item -ItemType Directory -Force -Path $stage | Out-Null

$runtimeFiles = @(
    'ds2_climbing_gloves_range.asi',
    'ds2_climbing_gloves_range.ini'
)
$fixedTimestamp = [DateTime]::new(
    2026, 9, 3, 0, 0, 0, [DateTimeKind]::Utc
)
foreach ($file in $runtimeFiles) {
    $built = Join-Path (Join-Path $root 'build\public') $file
    if (!(Test-Path -LiteralPath $built)) { throw "Built runtime file missing: $built" }
    $staged = Join-Path $stage $file
    Copy-Item -LiteralPath $built -Destination $staged -Force
    (Get-Item -LiteralPath $staged).LastWriteTimeUtc = $fixedTimestamp
}

$zip = Join-Path $releaseRoot "$name.zip"
if (Test-Path -LiteralPath $zip) { Remove-Item -LiteralPath $zip -Force }
Compress-Archive -Path (Join-Path $stage '*') -DestinationPath $zip -CompressionLevel Optimal

Add-Type -AssemblyName System.IO.Compression.FileSystem
$archive = [System.IO.Compression.ZipFile]::OpenRead($zip)
try {
    $entries = @($archive.Entries | ForEach-Object { $_.FullName })
    if ($entries.Count -ne $runtimeFiles.Count -or
        (Compare-Object -ReferenceObject $runtimeFiles -DifferenceObject $entries)) {
        throw "Package content check failed: $($entries -join ', ')"
    }
} finally {
    $archive.Dispose()
}

$hash = (Get-FileHash -LiteralPath $zip -Algorithm SHA256).Hash.ToUpperInvariant()
$hashFile = "$zip.sha256.txt"
"$hash  $([System.IO.Path]::GetFileName($zip))" |
    Set-Content -LiteralPath $hashFile -Encoding ascii
$asiHash = (Get-FileHash -LiteralPath (Join-Path $stage 'ds2_climbing_gloves_range.asi') -Algorithm SHA256).Hash.ToUpperInvariant()
$iniHash = (Get-FileHash -LiteralPath (Join-Path $stage 'ds2_climbing_gloves_range.ini') -Algorithm SHA256).Hash.ToUpperInvariant()
@(
    "$asiHash  build/public/ds2_climbing_gloves_range.asi"
    "$iniHash  build/public/ds2_climbing_gloves_range.ini"
    "$hash  release/$name.zip"
) | Set-Content -LiteralPath (Join-Path $root 'SHA256SUMS.txt') -Encoding ascii
Write-Host "Packaged exactly two runtime files: $zip"
Write-Host "Package SHA-256: $hash"
Write-Host "Checksum file: $hashFile"
Write-Host "Updated checksums: $(Join-Path $root 'SHA256SUMS.txt')"
