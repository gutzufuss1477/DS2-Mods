$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$name = 'DS2_Chiral_Bandwidth_Costs_v1.0.0'
$releaseRoot = Join-Path $root 'release'
New-Item -ItemType Directory -Force -Path $releaseRoot | Out-Null

$stage = Join-Path $releaseRoot $name
$zip = Join-Path $releaseRoot "$name.zip"
$hashFile = "$zip.sha256.txt"
$fullReleaseRoot = [System.IO.Path]::GetFullPath($releaseRoot).TrimEnd('\') + '\'
$fullStage = [System.IO.Path]::GetFullPath($stage)
$fullZip = [System.IO.Path]::GetFullPath($zip)
$fullHashFile = [System.IO.Path]::GetFullPath($hashFile)

foreach ($target in @($fullStage, $fullZip, $fullHashFile)) {
    if (!$target.StartsWith($fullReleaseRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
        throw "Refusing to recreate release path outside release root: $target"
    }
}

if (Test-Path -LiteralPath $fullStage) {
    Remove-Item -LiteralPath $fullStage -Recurse -Force
}
if (Test-Path -LiteralPath $fullZip) {
    Remove-Item -LiteralPath $fullZip -Force
}
if (Test-Path -LiteralPath $fullHashFile) {
    Remove-Item -LiteralPath $fullHashFile -Force
}

# Invalidate any older public artifact before rebuilding, so a failed build/test
# cannot leave a stale ZIP and matching sidecar looking current.
& (Join-Path $root 'scripts\test-synthetic.ps1')

New-Item -ItemType Directory -Force -Path $fullStage | Out-Null

$runtimeFiles = @(
    'ds2_chiral_bandwidth_costs.asi',
    'ds2_chiral_bandwidth_costs.ini'
)
foreach ($file in $runtimeFiles) {
    $source = Join-Path $root "build\public\$file"
    if (!(Test-Path -LiteralPath $source)) { throw "Runtime file missing after tests: $source" }
    Copy-Item -LiteralPath $source -Destination (Join-Path $fullStage $file) -Force
}

Add-Type -AssemblyName System.IO.Compression
Add-Type -AssemblyName System.IO.Compression.FileSystem
$fixedTimestamp = [DateTimeOffset]::new(2026, 8, 29, 0, 0, 0, [TimeSpan]::Zero)
$zipStream = [System.IO.File]::Open(
    $fullZip,
    [System.IO.FileMode]::CreateNew,
    [System.IO.FileAccess]::ReadWrite,
    [System.IO.FileShare]::None
)
try {
    $zipArchive = [System.IO.Compression.ZipArchive]::new(
        $zipStream,
        [System.IO.Compression.ZipArchiveMode]::Create,
        $false
    )
    try {
        foreach ($file in $runtimeFiles) {
            $entry = $zipArchive.CreateEntry($file, [System.IO.Compression.CompressionLevel]::Optimal)
            $entry.LastWriteTime = $fixedTimestamp
            $input = [System.IO.File]::OpenRead((Join-Path $fullStage $file))
            $output = $entry.Open()
            try {
                $input.CopyTo($output)
            } finally {
                $output.Dispose()
                $input.Dispose()
            }
        }
    } finally {
        $zipArchive.Dispose()
    }
} finally {
    $zipStream.Dispose()
}

$archive = [System.IO.Compression.ZipFile]::OpenRead($fullZip)
try {
    $entries = @($archive.Entries | ForEach-Object { $_.FullName })
    if ($entries.Count -ne $runtimeFiles.Count -or
        (Compare-Object -ReferenceObject $runtimeFiles -DifferenceObject $entries)) {
        throw "Package content check failed: $($entries -join ', ')"
    }
} finally {
    $archive.Dispose()
}

$zipHash = (Get-FileHash -LiteralPath $fullZip -Algorithm SHA256).Hash.ToUpperInvariant()
Set-Content -LiteralPath $fullHashFile -Value "$zipHash  $name.zip" -Encoding ascii

Write-Host "Packaged exactly two runtime files: $fullZip"
Write-Host "SHA-256: $zipHash"
