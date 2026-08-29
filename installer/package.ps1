[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$AppRoot,

    [Parameter(Mandatory = $true)]
    [string]$ZipPath
)

$ErrorActionPreference = 'Stop'
$appRootFull = [System.IO.Path]::GetFullPath($AppRoot).TrimEnd('\')
$zipPathFull = [System.IO.Path]::GetFullPath($ZipPath)
$zipDirectory = [System.IO.Path]::GetDirectoryName($zipPathFull)
$stagingPath = [System.IO.Path]::Combine(
    $zipDirectory,
    '.' + [System.IO.Path]::GetFileName($zipPathFull) + '.staging-' + [System.Diagnostics.Process]::GetCurrentProcess().Id)

if (-not (Test-Path -LiteralPath $appRootFull -PathType Container)) {
    throw "Application directory does not exist: $appRootFull"
}
if ((Get-Item -LiteralPath $appRootFull -Force).Attributes -band [System.IO.FileAttributes]::ReparsePoint) {
    throw "Application directory must not be a reparse point: $appRootFull"
}
if ([System.IO.File]::Exists($zipPathFull) -or [System.IO.File]::Exists($stagingPath)) {
    throw "ZIP output or staging path already exists."
}

Add-Type -AssemblyName System.IO.Compression
Add-Type -AssemblyName System.IO.Compression.FileSystem
$zipStream = $null
$archive = $null
try {
    $zipStream = [System.IO.FileStream]::new($stagingPath, [System.IO.FileMode]::CreateNew, [System.IO.FileAccess]::Write, [System.IO.FileShare]::None)
    $archive = [System.IO.Compression.ZipArchive]::new($zipStream, [System.IO.Compression.ZipArchiveMode]::Create, $false)
    $fixedTimestamp = [System.DateTimeOffset]::new(2026, 1, 1, 0, 0, 0, [System.TimeSpan]::Zero)
    foreach ($file in (Get-ChildItem -LiteralPath $appRootFull -Recurse -File | Sort-Object FullName)) {
        $relative = $file.FullName.Substring($appRootFull.Length).TrimStart('\').Replace('\', '/')
        $entry = $archive.CreateEntry("DS2-Mod-Suite/$relative", [System.IO.Compression.CompressionLevel]::NoCompression)
        $entry.LastWriteTime = $fixedTimestamp
        $entryStream = $entry.Open()
        $sourceStream = [System.IO.FileStream]::new($file.FullName, [System.IO.FileMode]::Open, [System.IO.FileAccess]::Read, [System.IO.FileShare]::Read)
        try {
            $sourceStream.CopyTo($entryStream)
        }
        finally {
            $sourceStream.Dispose()
            $entryStream.Dispose()
        }
    }
    $archive.Dispose()
    $archive = $null
    $zipStream.Dispose()
    $zipStream = $null
    [System.IO.File]::Move($stagingPath, $zipPathFull)
}
catch {
    if ($archive) { $archive.Dispose() }
    if ($zipStream) { $zipStream.Dispose() }
    if ([System.IO.File]::Exists($stagingPath)) {
        [System.IO.File]::Delete($stagingPath)
    }
    throw
}
