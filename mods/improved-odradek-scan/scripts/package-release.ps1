[CmdletBinding()]
param(
    [string]$TargetExe = $env:DS2_EXE,
    [string]$PythonExe = 'python'
)

$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
# Packaging always requires the complete suite, including the exact target image.
& (Join-Path $root 'scripts\test-synthetic.ps1') -TargetExe $TargetExe -PythonExe $PythonExe

$name = 'Improved_Odradek_Scan_v1.0.0'
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

Copy-Item -LiteralPath (Join-Path $root 'build\public\ds2_odradek_scan.asi') -Destination $stage -Force
Copy-Item -LiteralPath (Join-Path $root 'build\public\ds2_odradek_scan.ini') -Destination $stage -Force

Copy-Item -LiteralPath (Join-Path $root 'docs\RELEASE_README.txt') -Destination (Join-Path $stage 'README.txt')
Copy-Item -LiteralPath (Join-Path $root 'docs\RELEASE_CHANGELOG.txt') -Destination (Join-Path $stage 'CHANGELOG.txt')

$zip = Join-Path $releaseRoot "$name.zip"
if (Test-Path -LiteralPath $zip) { Remove-Item -LiteralPath $zip -Force }
Compress-Archive -Path (Join-Path $stage '*') -DestinationPath $zip -CompressionLevel Optimal

Add-Type -AssemblyName System.IO.Compression.FileSystem
$archive = [System.IO.Compression.ZipFile]::OpenRead($zip)
try {
    $entries = @($archive.Entries | ForEach-Object { $_.FullName })
    $expected = @('ds2_odradek_scan.asi', 'ds2_odradek_scan.ini', 'README.txt', 'CHANGELOG.txt')
    if ($entries.Count -ne $expected.Count -or
        (Compare-Object -ReferenceObject $expected -DifferenceObject $entries)) {
        throw "Package content check failed: $($entries -join ', ')"
    }
} finally {
    $archive.Dispose()
}

$hash = (Get-FileHash -LiteralPath $zip -Algorithm SHA256).Hash.ToUpperInvariant()
Write-Host "Packaged two runtime files plus README and changelog: $zip"
Write-Host "Package SHA-256: $hash"
