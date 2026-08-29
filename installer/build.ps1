[CmdletBinding()]
param(
    [switch]$SkipPackage
)

$ErrorActionPreference = 'Stop'
$installerRoot = [System.IO.Path]::GetFullPath($PSScriptRoot)
$workspaceRoot = [System.IO.Path]::GetFullPath((Join-Path $installerRoot '..'))
$distRoot = [System.IO.Path]::GetFullPath((Join-Path $installerRoot 'dist'))
$appRoot = [System.IO.Path]::GetFullPath((Join-Path $distRoot 'DS2-Mod-Suite'))
$zipPath = Join-Path $distRoot 'DS2_Mod_Suite_v1.1.0.zip'
$zipHashPath = "$zipPath.sha256.txt"

function Assert-ChildPath([string]$parent, [string]$candidate) {
    $resolvedParent = [System.IO.Path]::GetFullPath($parent).TrimEnd('\') + '\'
    $resolvedCandidate = [System.IO.Path]::GetFullPath($candidate)
    if (-not $resolvedCandidate.StartsWith($resolvedParent, [System.StringComparison]::OrdinalIgnoreCase)) {
        throw "Unsafe build path: $resolvedCandidate"
    }
}

function Assert-NotReparsePoint([string]$path) {
    if (Test-Path -LiteralPath $path) {
        $item = Get-Item -LiteralPath $path -Force
        if (($item.Attributes -band [System.IO.FileAttributes]::ReparsePoint) -ne 0) {
            throw "Refusing to use a reparse-point build path: $path"
        }
    }
}

function Find-Assembly([string]$name) {
    $assembly = Get-ChildItem -LiteralPath 'C:\Windows\Microsoft.NET\assembly' -Recurse -Filter $name -ErrorAction SilentlyContinue |
        Sort-Object { if ($_.FullName -match 'GAC_64') { 0 } elseif ($_.FullName -match 'GAC_MSIL') { 1 } else { 2 } } |
        Select-Object -First 1
    if (-not $assembly) {
        throw "Required .NET Framework assembly was not found: $name"
    }
    return $assembly.FullName
}

function New-Ds2Icon([string]$path) {
    Add-Type -AssemblyName System.Drawing
    $bitmap = [System.Drawing.Bitmap]::new(256, 256, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    $graphics = [System.Drawing.Graphics]::FromImage($bitmap)
    $shape = [System.Drawing.Drawing2D.GraphicsPath]::new()
    $font = $null
    $textBrush = $null
    $tileBrush = $null
    $pngStream = $null
    $fileStream = $null
    $writer = $null
    try {
        $graphics.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::AntiAlias
        $graphics.TextRenderingHint = [System.Drawing.Text.TextRenderingHint]::AntiAliasGridFit
        $radius = 42
        $shape.AddArc(8, 8, $radius, $radius, 180, 90)
        $shape.AddArc(206, 8, $radius, $radius, 270, 90)
        $shape.AddArc(206, 206, $radius, $radius, 0, 90)
        $shape.AddArc(8, 206, $radius, $radius, 90, 90)
        $shape.CloseFigure()
        $tileBrush = [System.Drawing.SolidBrush]::new([System.Drawing.ColorTranslator]::FromHtml('#49A9DD'))
        $graphics.FillPath($tileBrush, $shape)
        $font = [System.Drawing.Font]::new('Segoe UI', 72, [System.Drawing.FontStyle]::Bold, [System.Drawing.GraphicsUnit]::Pixel)
        $textBrush = [System.Drawing.SolidBrush]::new([System.Drawing.ColorTranslator]::FromHtml('#07131A'))
        $format = [System.Drawing.StringFormat]::new()
        $format.Alignment = [System.Drawing.StringAlignment]::Center
        $format.LineAlignment = [System.Drawing.StringAlignment]::Center
        $graphics.DrawString('DS2', $font, $textBrush, [System.Drawing.RectangleF]::new(0, -2, 256, 256), $format)
        $format.Dispose()

        $pngStream = [System.IO.MemoryStream]::new()
        $bitmap.Save($pngStream, [System.Drawing.Imaging.ImageFormat]::Png)
        $pngBytes = $pngStream.ToArray()
        $fileStream = [System.IO.FileStream]::new($path, [System.IO.FileMode]::Create, [System.IO.FileAccess]::Write, [System.IO.FileShare]::None)
        $writer = [System.IO.BinaryWriter]::new($fileStream)
        $writer.Write([uint16]0)
        $writer.Write([uint16]1)
        $writer.Write([uint16]1)
        $writer.Write([byte]0)
        $writer.Write([byte]0)
        $writer.Write([byte]0)
        $writer.Write([byte]0)
        $writer.Write([uint16]1)
        $writer.Write([uint16]32)
        $writer.Write([uint32]$pngBytes.Length)
        $writer.Write([uint32]22)
        $writer.Write($pngBytes)
    }
    finally {
        if ($writer) { $writer.Dispose() }
        elseif ($fileStream) { $fileStream.Dispose() }
        if ($pngStream) { $pngStream.Dispose() }
        if ($textBrush) { $textBrush.Dispose() }
        if ($tileBrush) { $tileBrush.Dispose() }
        if ($font) { $font.Dispose() }
        $shape.Dispose()
        $graphics.Dispose()
        $bitmap.Dispose()
    }
}

Assert-ChildPath $installerRoot $distRoot
Assert-ChildPath $distRoot $appRoot
Assert-NotReparsePoint $installerRoot
Assert-NotReparsePoint $distRoot
Assert-NotReparsePoint $appRoot
if (-not $SkipPackage) {
    foreach ($staleRelease in @($zipPath, $zipHashPath)) {
        Assert-ChildPath $distRoot $staleRelease
        if (Test-Path -LiteralPath $staleRelease) {
            Remove-Item -LiteralPath $staleRelease -Force
        }
    }
}
if (Test-Path -LiteralPath $appRoot) {
    Remove-Item -LiteralPath $appRoot -Recurse -Force
}
New-Item -ItemType Directory -Path $appRoot -Force | Out-Null

$catalogPath = Join-Path $installerRoot 'catalog.json'
$catalog = Get-Content -LiteralPath $catalogPath -Raw | ConvertFrom-Json
$settingsSchemaPath = Join-Path $installerRoot 'settings-schema.json'
$settingsSchema = Get-Content -LiteralPath $settingsSchemaPath -Raw | ConvertFrom-Json
if ($settingsSchema.schemaVersion -ne 1 -or -not $settingsSchema.mods) {
    throw 'The settings schema is invalid.'
}
$sources = Get-Content -LiteralPath (Join-Path $installerRoot 'payload-sources.json') -Raw | ConvertFrom-Json

$expectedHashes = @{}
$expectedHashes[$catalog.loader.payload.Replace('/', '\')] = $catalog.loader.sha256.ToUpperInvariant()
foreach ($mod in $catalog.mods) {
    foreach ($file in $mod.files) {
        $expectedHashes[$file.payload.Replace('/', '\')] = $file.sha256.ToUpperInvariant()
    }
}

$sourceByDestination = @{}
foreach ($entry in $sources) {
    $source = [System.IO.Path]::GetFullPath((Join-Path $installerRoot $entry.source))
    $relativeDestination = $entry.destination.Replace('/', '\')
    if (-not $expectedHashes.ContainsKey($relativeDestination)) {
        throw "Payload source has no catalog entry: $relativeDestination"
    }
    if (-not (Test-Path -LiteralPath $source -PathType Leaf)) {
        throw "Payload source is missing: $source"
    }
    $sourceHash = (Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash
    if ($sourceHash -ne $expectedHashes[$relativeDestination]) {
        throw "Payload hash mismatch for $source. Expected $($expectedHashes[$relativeDestination]), found $sourceHash"
    }

    if ($sourceByDestination.ContainsKey($relativeDestination)) {
        throw "Duplicate payload destination in payload-sources.json: $relativeDestination"
    }
    $sourceByDestination[$relativeDestination] = $source
}

$missingDestinations = @($expectedHashes.Keys | Where-Object { -not $sourceByDestination.ContainsKey($_) })
if ($sourceByDestination.Count -ne $expectedHashes.Count -or $missingDestinations.Count -gt 0) {
    throw "Catalog/source set mismatch. Missing: $($missingDestinations -join ', ')"
}

Copy-Item -LiteralPath (Join-Path $installerRoot 'README.md') -Destination (Join-Path $appRoot 'README.txt') -Force
Copy-Item -LiteralPath (Join-Path $installerRoot 'THIRD_PARTY_NOTICES.txt') -Destination (Join-Path $appRoot 'THIRD_PARTY_NOTICES.txt') -Force
Copy-Item -LiteralPath (Join-Path $installerRoot 'vendor\ultimate-asi-loader-v9.7.2\LICENSE.txt') -Destination (Join-Path $appRoot 'LICENSE_Ultimate_ASI_Loader.txt') -Force

$compiler = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\Roslyn\csc.exe'
if (-not (Test-Path -LiteralPath $compiler)) {
    throw 'Visual Studio Build Tools Roslyn compiler was not found.'
}

$references = @(
    (Find-Assembly 'PresentationFramework.dll'),
    (Find-Assembly 'PresentationCore.dll'),
    (Find-Assembly 'WindowsBase.dll'),
    (Find-Assembly 'System.Xaml.dll'),
    (Find-Assembly 'System.Runtime.Serialization.dll')
)
$outputExe = Join-Path $appRoot 'DS2ModManager.exe'
$iconPath = Join-Path $appRoot 'DS2ModSuite.ico'
New-Ds2Icon $iconPath
$sourceFiles = Get-ChildItem -LiteralPath (Join-Path $installerRoot 'src') -Filter '*.cs' -File | Sort-Object Name
$arguments = @(
    '/nologo',
    '/target:winexe',
    '/platform:x64',
    '/optimize+',
    '/debug-',
    '/deterministic+',
    '/langversion:latest',
    '/codepage:65001',
    "/win32manifest:$installerRoot\app.manifest",
    "/win32icon:$iconPath",
    "/out:$outputExe",
    "/resource:$catalogPath,DS2ModSuite.Catalog",
    "/resource:$settingsSchemaPath,DS2ModSuite.SettingsSchema"
)
$embeddedHashes = @{}
foreach ($relativeDestination in ($expectedHashes.Keys | Sort-Object)) {
    $payloadSource = $sourceByDestination[$relativeDestination]
    $payloadHash = $expectedHashes[$relativeDestination]
    if ($embeddedHashes.ContainsKey($payloadHash)) {
        continue
    }
    $embeddedHashes[$payloadHash] = $true
    $resourceName = "DS2ModSuite.Payload.$payloadHash"
    $arguments += "/resource:$payloadSource,$resourceName"
}
foreach ($reference in $references) {
    $arguments += "/reference:$reference"
}
foreach ($sourceFile in $sourceFiles) {
    $arguments += $sourceFile.FullName
}

& $compiler @arguments
if ($LASTEXITCODE -ne 0 -or -not (Test-Path -LiteralPath $outputExe)) {
    throw "C# compilation failed with exit code $LASTEXITCODE"
}
Copy-Item -LiteralPath (Join-Path $installerRoot 'DS2ModManager.exe.config') -Destination "$outputExe.config" -Force

$selfTestResult = Join-Path $appRoot 'selftest-result.txt'
$selfTestProcess = Start-Process -FilePath $outputExe -ArgumentList "--self-test --result `"$selfTestResult`"" -Wait -PassThru -WindowStyle Hidden
if ($selfTestProcess.ExitCode -ne 0 -or -not (Test-Path -LiteralPath $selfTestResult)) {
    throw "Built executable self-test failed with exit code $($selfTestProcess.ExitCode)"
}
$selfTestText = Get-Content -LiteralPath $selfTestResult -Raw
if ($selfTestText -notmatch 'SELF-TEST PASSED') {
    throw "Built executable self-test failed:`r`n$selfTestText"
}
Remove-Item -LiteralPath $selfTestResult -Force

$sumPath = Join-Path $appRoot 'SHA256SUMS.txt'
$sumLines = Get-ChildItem -LiteralPath $appRoot -Recurse -File |
    Where-Object { $_.FullName -ne $sumPath } |
    Sort-Object FullName |
    ForEach-Object {
        $relative = $_.FullName.Substring($appRoot.Length).TrimStart('\').Replace('\', '/')
        $hash = (Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash
        "$hash  $relative"
    }
[System.IO.File]::WriteAllLines($sumPath, $sumLines, [System.Text.UTF8Encoding]::new($false))

if (-not $SkipPackage) {
    $packager = Join-Path $installerRoot 'package.ps1'
    & powershell.exe -NoProfile -ExecutionPolicy Bypass -File $packager -AppRoot $appRoot -ZipPath $zipPath
    if ($LASTEXITCODE -ne 0 -or -not (Test-Path -LiteralPath $zipPath -PathType Leaf)) {
        throw "Deterministic ZIP packaging failed with exit code $LASTEXITCODE"
    }
    $zipHash = (Get-FileHash -LiteralPath $zipPath -Algorithm SHA256).Hash
    [System.IO.File]::WriteAllText($zipHashPath, "$zipHash  $([System.IO.Path]::GetFileName($zipPath))`r`n", [System.Text.UTF8Encoding]::new($false))
}

Write-Host "Built: $outputExe"
if (-not $SkipPackage) {
    Write-Host "Package: $zipPath"
}
