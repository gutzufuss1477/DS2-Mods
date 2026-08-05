$ErrorActionPreference = 'Stop'

$Root = Split-Path -Parent $PSScriptRoot
$Release = Join-Path $Root 'release'
$Version = 'v0.2.0'

& (Join-Path $PSScriptRoot 'build_source_release.ps1')
if ($LASTEXITCODE -ne 0) { throw 'Source release build failed.' }

$Asi = Join-Path $Release 'DS2_Remote_Orders_Overlay.asi'
$Ini = Join-Path $Release 'DS2_Remote_Orders_Overlay.ini'
$Readme = Join-Path $Release 'README_v0.2.0.txt'
$License = Join-Path $Root 'LICENSE'
$Zip = Join-Path $Release "DS2_Remote_Orders_Overlay_$Version.zip"
$Files = @($Asi, $Ini, $Readme, $License)
$FixedTimestamp = [DateTimeOffset]::new(2000, 1, 1, 0, 0, 0, [TimeSpan]::Zero)

Add-Type -AssemblyName System.IO.Compression
if (Test-Path -LiteralPath $Zip) { Remove-Item -LiteralPath $Zip -Force }
$Stream = [IO.File]::Open($Zip, [IO.FileMode]::CreateNew, [IO.FileAccess]::ReadWrite, [IO.FileShare]::None)
$Archive = [IO.Compression.ZipArchive]::new($Stream, [IO.Compression.ZipArchiveMode]::Create, $false)
try {
    foreach ($File in $Files) {
        $Entry = $Archive.CreateEntry([IO.Path]::GetFileName($File), [IO.Compression.CompressionLevel]::Optimal)
        $Entry.LastWriteTime = $FixedTimestamp
        $Input = [IO.File]::OpenRead($File)
        $Output = $Entry.Open()
        try { $Input.CopyTo($Output) } finally { $Output.Dispose(); $Input.Dispose() }
    }
} finally {
    $Archive.Dispose()
    $Stream.Dispose()
}

Get-FileHash -Algorithm SHA256 $Asi, $Zip
