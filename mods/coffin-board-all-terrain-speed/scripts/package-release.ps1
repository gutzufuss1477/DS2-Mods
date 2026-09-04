$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
& (Join-Path $root 'scripts\test-synthetic.ps1')
if ($LASTEXITCODE) { throw 'Synthetic validation failed' }

$targetExe = 'C:\Program Files (x86)\Steam\steamapps\common\DEATH STRANDING 2 - ON THE BEACH\DS2.exe'
if (!(Test-Path -LiteralPath $targetExe -PathType Leaf)) {
    throw "DS2 target missing: $targetExe"
}
& python (Join-Path $root 'tools\validate_target.py') $targetExe
if ($LASTEXITCODE) { throw 'Offline DS2 target validation failed' }

$name = 'DS2_Coffin_Board_All_Terrain_Speed_v1.1.0'
$releaseRoot = Join-Path $root 'release'
$stage = Join-Path $releaseRoot $name
$zip = Join-Path $releaseRoot "$name.zip"
$checksumPath = Join-Path $root 'SHA256SUMS.txt'
$fullReleaseRoot = [System.IO.Path]::GetFullPath($releaseRoot).TrimEnd('\') + '\'
$fullStage = [System.IO.Path]::GetFullPath($stage)
$fullZip = [System.IO.Path]::GetFullPath($zip)
foreach ($target in @($fullStage, $fullZip)) {
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
New-Item -ItemType Directory -Force -Path $stage | Out-Null

$runtimeFiles = @(
    'ds2_coffin_board_all_terrain_speed.asi',
    'ds2_coffin_board_all_terrain_speed.ini'
)
$configuration = Join-Path $root 'build\public\ds2_coffin_board_all_terrain_speed.ini'
$configurationText = Get-Content -LiteralPath $configuration -Raw
$requiredComments = @(
    '; Top speed on land and water. 500 = 300 km/h normal / 400 km/h boost (range: 100-1000).',
    '; Acceleration toward the new top speed. 400 = about 4x native (range: 100-500).',
    '; 1 lets you mount and ride the Coffin Board while a Floating Carrier is attached. 0 keeps the native restriction.'
)
foreach ($comment in $requiredComments) {
    if (!$configurationText.Contains($comment)) {
        throw "Final INI is missing required English comment: $comment"
    }
}
$effectiveIniLines = @(
    Get-Content -LiteralPath $configuration | ForEach-Object { $_.Trim() } |
        Where-Object { $_ -and !$_.StartsWith(';') -and !$_.StartsWith('#') }
)
$expectedIniLines = @(
    '[CoffinBoardAllTerrainSpeed]',
    'SpeedPercent=500',
    'AccelerationPercent=400',
    'AllowFloatingCarrier=1'
)
if ($effectiveIniLines.Count -ne $expectedIniLines.Count -or
    (Compare-Object -ReferenceObject $expectedIniLines -DifferenceObject $effectiveIniLines -SyncWindow 0)) {
    throw "Final INI must contain only the section plus SpeedPercent=500, AccelerationPercent=400, and AllowFloatingCarrier=1."
}
foreach ($file in $runtimeFiles) {
    Copy-Item -LiteralPath (Join-Path $root "build\public\$file") -Destination $stage -Force
}

$stagedFiles = @(
    Get-ChildItem -LiteralPath $stage -File | Sort-Object Name |
        ForEach-Object { $_.Name }
)
$expectedStagedFiles = @($runtimeFiles | Sort-Object)
if ($stagedFiles.Count -ne $expectedStagedFiles.Count -or
    (Compare-Object -ReferenceObject $expectedStagedFiles -DifferenceObject $stagedFiles)) {
    throw "Staging content check failed: $($stagedFiles -join ', ')"
}

Add-Type -AssemblyName System.IO.Compression
Add-Type -AssemblyName System.IO.Compression.FileSystem
$fixedTimestamp = [DateTimeOffset]::new(2026, 9, 4, 12, 0, 0, [TimeSpan]::Zero)
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
            $entry = $zipArchive.CreateEntry(
                $file,
                [System.IO.Compression.CompressionLevel]::Optimal
            )
            $entry.LastWriteTime = $fixedTimestamp
            $input = [System.IO.File]::OpenRead((Join-Path $stage $file))
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

$archive = [System.IO.Compression.ZipFile]::OpenRead($zip)
try {
    $entries = @($archive.Entries | ForEach-Object { $_.FullName })
    $entriesMatch = $entries.Count -eq $runtimeFiles.Count
    if ($entriesMatch) {
        for ($index = 0; $index -lt $runtimeFiles.Count; $index++) {
            if ($entries[$index] -cne $runtimeFiles[$index]) {
                $entriesMatch = $false
                break
            }
        }
    }
    if (!$entriesMatch) {
        throw "Package content check failed: $($entries -join ', ')"
    }
} finally {
    $archive.Dispose()
}

$checksumLines = @(
    "$(Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $root 'build\public\ds2_coffin_board_all_terrain_speed.asi') | Select-Object -ExpandProperty Hash)  build/public/ds2_coffin_board_all_terrain_speed.asi",
    "$(Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $root 'build\public\ds2_coffin_board_all_terrain_speed.ini') | Select-Object -ExpandProperty Hash)  build/public/ds2_coffin_board_all_terrain_speed.ini",
    "$(Get-FileHash -Algorithm SHA256 -LiteralPath $zip | Select-Object -ExpandProperty Hash)  release/$name.zip"
)
$checksumLines | Set-Content -LiteralPath $checksumPath -Encoding ascii

Write-Host "Packaged exactly two runtime files: $zip"
Write-Host "Updated checksums: $checksumPath"
