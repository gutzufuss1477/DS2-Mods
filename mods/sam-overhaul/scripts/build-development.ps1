param()
$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$version=(Get-Content -LiteralPath (Join-Path $root 'VERSION.txt') -Raw).Trim()
if($version -notmatch '^\d+\.\d+\.\d+-dev\.\d+$'){throw 'This script stages development builds only.'}
$logs=Join-Path $root 'build\validation'
New-Item -ItemType Directory -Path $logs -Force | Out-Null
& (Join-Path $PSScriptRoot 'build-msvc.cmd') 2>&1 | Tee-Object -FilePath (Join-Path $logs 'build.txt')
if($LASTEXITCODE -ne 0){throw 'Combined Sam Overhaul build failed.'}
& (Join-Path $root 'tests\build-run-autodrive-cpu.cmd') 2>&1 | Tee-Object -FilePath (Join-Path $logs 'autodrive.txt')
if($LASTEXITCODE -ne 0){throw 'Autodrive regression failed.'}
& (Join-Path $root 'tests\build-run-footprints.cmd') 2>&1 | Tee-Object -FilePath (Join-Path $logs 'footprints.txt')
if($LASTEXITCODE -ne 0){throw 'Footprint integration regression failed.'}
python (Join-Path $PSScriptRoot 'validate-development.py') 2>&1 | Tee-Object -FilePath (Join-Path $logs 'binary.txt')
if($LASTEXITCODE -ne 0){throw 'Binary or configuration validation failed.'}
$name="DS2_Sam_Overhaul_v$version"
$stage=Join-Path $root "development\$name"
$allowed=@("$name.asi",'ds2_sam_overhaul.ini','README.md','README_DE.md','CHANGELOG.md','THIRD_PARTY_NOTICES.md','LICENSE_MINHOOK.txt','DEVELOPMENT_STATUS.txt','SHA256SUMS.txt')
if(Test-Path $stage){
    $unexpected=Get-ChildItem $stage -Force | Where-Object {$_.PSIsContainer -or $_.Name -notin $allowed}
    if($unexpected){throw 'Unexpected content in development staging directory; nothing removed.'}
}
New-Item -ItemType Directory -Path $stage -Force | Out-Null
Copy-Item -LiteralPath (Join-Path $root "build\public\$name.asi") -Destination $stage -Force
foreach($file in @('ds2_sam_overhaul.ini','README.md','README_DE.md','CHANGELOG.md','THIRD_PARTY_NOTICES.md')){
    Copy-Item -LiteralPath (Join-Path $root $file) -Destination $stage -Force
}
Copy-Item -LiteralPath (Join-Path $root 'vendor\minhook\LICENSE.txt') -Destination (Join-Path $stage 'LICENSE_MINHOOK.txt') -Force
@"
Sam Overhaul $version - Git development build only.
No Nexus release, GitHub release, release archive, release tag, or game installation is performed by this script.
The footprint core was gameplay-confirmed separately; the combined Sam Overhaul gameplay regression remains pending.
Default: [Footprints] HideFootprints=0. Set to 1 in ds2_sam_overhaul.ini and restart to enable.
Remove the old Sam Overhaul ASI and ds2_footprint_native_probe.asi before manually installing this combined ASI.
Original public v1.0.0 release files and Mod Suite distribution metadata remain untouched.
"@ | Set-Content -LiteralPath (Join-Path $stage 'DEVELOPMENT_STATUS.txt') -Encoding UTF8
$hashes=@()
Get-ChildItem $stage -File | Where-Object Name -ne 'SHA256SUMS.txt' | Sort-Object Name | ForEach-Object {
    $hashes+=((Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash+'  '+$_.Name)
}
$hashes | Set-Content -LiteralPath (Join-Path $stage 'SHA256SUMS.txt') -Encoding ASCII
Write-Output ('DEVELOPMENT_BUILD='+$stage)
Write-Output ('ASI_SHA256='+(Get-FileHash (Join-Path $stage "$name.asi") -Algorithm SHA256).Hash)
Write-Output 'ALL_CHECKS_PASSED; NO_RELEASE_CREATED; GAME_FILES_UNCHANGED'