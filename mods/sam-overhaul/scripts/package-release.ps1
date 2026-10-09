param(
    [string]$TargetExe = ""
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
$version = (Get-Content -LiteralPath (Join-Path $root "VERSION.txt") -Raw).Trim()
if ($version -notmatch '^\d+\.\d+\.\d+$') {
    throw "Development version cannot be packaged as a public release."
}
$build = Join-Path $root "build\public"
$release = Join-Path $root "release"
$name = "DS2_Sam_Overhaul_v$version"
$folder = Join-Path $release $name
$zip = Join-Path $release "$name.zip"

# Fail closed before writing ANY public artifact. Never silently overwrite a
# previously generated package or the historical v1.0.0 release.
if ((Test-Path -LiteralPath $folder) -or (Test-Path -LiteralPath $zip)) {
    throw "Release artifacts already exist. Refusing to overwrite: $name"
}
if ($TargetExe) {
    if (!(Test-Path -LiteralPath $TargetExe)) { throw "DS2.exe was not found." }
    $expected = "BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B"
    $actual = (Get-FileHash -Algorithm SHA256 -LiteralPath $TargetExe).Hash
    if ($actual -ne $expected) {
        throw "Unsupported target DS2.exe SHA256: $actual"
    }
}

& (Join-Path $PSScriptRoot "build-msvc.cmd")
if ($LASTEXITCODE -ne 0) { throw "Release build failed with exit code $LASTEXITCODE" }

$compiled = Join-Path $build "$name.asi"
if (!(Test-Path -LiteralPath $compiled)) { throw "Compiled ASI is missing." }
$required = @("ds2_sam_overhaul.ini", "README.md", "README_DE.md",
              "CHANGELOG.md", "THIRD_PARTY_NOTICES.md")
foreach ($requiredFile in $required) {
    if (!(Test-Path -LiteralPath (Join-Path $root $requiredFile))) {
        throw "Required release source missing: $requiredFile"
    }
}
$license = Join-Path $root "vendor\minhook\LICENSE.txt"
if (!(Test-Path -LiteralPath $license)) { throw "MinHook license missing." }

New-Item -ItemType Directory -Path $folder -Force | Out-Null
Copy-Item -LiteralPath $compiled -Destination $folder
foreach ($requiredFile in $required) {
    Copy-Item -LiteralPath (Join-Path $root $requiredFile) -Destination $folder
}
Copy-Item -LiteralPath $license -Destination (Join-Path $folder "LICENSE_MINHOOK.txt")

# The files are at archive root, ready to copy the ASI and INI beside DS2.exe.
Compress-Archive -Path (Join-Path $folder "*") -DestinationPath $zip -CompressionLevel Optimal

$hashLines = @()
Get-ChildItem $folder -File | Sort-Object Name | ForEach-Object {
    $hash = (Get-FileHash -Algorithm SHA256 -LiteralPath $_.FullName).Hash
    $hashLines += "$hash  $($_.Name)"
}
$zipHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $zip).Hash
$hashLines += "$zipHash  $(Split-Path -Leaf $zip)"
$hashLines | Set-Content -LiteralPath (Join-Path $root "SHA256SUMS.txt") -Encoding ASCII

Write-Output ('NEXUS_UPLOAD_PACKAGE=' + $zip)
Write-Output ('NEXUS_PACKAGE_SHA256=' + $zipHash)
Write-Output ('RELEASE_ASI_SHA256=' + (Get-FileHash $compiled -Algorithm SHA256).Hash)
Write-Output 'PREVIOUS_V1_0_0_RELEASE_PRESERVED=True'
Write-Output 'NEXUS_NOT_UPLOADED=True'
