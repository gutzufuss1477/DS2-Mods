$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
& (Join-Path $root 'scripts\build-llvm.ps1')

$name = 'DS2_Zipline_Range_and_Speed_v1.0.0'
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

Copy-Item -LiteralPath (Join-Path $root 'build\public\ds2_zipline_range.asi') -Destination $stage -Force
Copy-Item -LiteralPath (Join-Path $root 'build\public\ds2_zipline_range.ini') -Destination $stage -Force

$zip = Join-Path $releaseRoot "$name.zip"
if (Test-Path -LiteralPath $zip) { Remove-Item -LiteralPath $zip -Force }
Compress-Archive -Path (Join-Path $stage '*') -DestinationPath $zip -CompressionLevel Optimal

Add-Type -AssemblyName System.IO.Compression.FileSystem
$archive = [System.IO.Compression.ZipFile]::OpenRead($zip)
try {
    $entries = @($archive.Entries | ForEach-Object { $_.FullName })
    $expected = @('ds2_zipline_range.asi', 'ds2_zipline_range.ini')
    if ($entries.Count -ne $expected.Count -or
        (Compare-Object -ReferenceObject $expected -DifferenceObject $entries)) {
        throw "Package content check failed: $($entries -join ', ')"
    }
} finally {
    $archive.Dispose()
}

Write-Host "Packaged exactly two runtime files: $zip"
