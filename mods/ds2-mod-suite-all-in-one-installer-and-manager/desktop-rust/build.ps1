param(
    [Parameter(Mandatory = $true)][string]$PayloadRoot,
    [Parameter(Mandatory = $true)][string]$OutputRoot,
    [string]$Python = 'python',
    [string]$Catalog
)
$ErrorActionPreference = 'Stop'
Set-Location $PSScriptRoot
$payloadPath = (Resolve-Path -LiteralPath $PayloadRoot).Path
$buildRoot = [System.IO.Path]::GetFullPath($OutputRoot)
if ($buildRoot -eq [System.IO.Path]::GetPathRoot($buildRoot)) { throw 'Choose a build subfolder, not a drive root.' }
if (Test-Path -LiteralPath (Join-Path $buildRoot 'packages')) { throw 'Choose a fresh output folder; existing releases are never overwritten.' }
New-Item -ItemType Directory -Path $buildRoot -Force | Out-Null
$env:CARGO_TARGET_DIR = Join-Path $buildRoot 'target'
$catalogArgs = @()
if ($Catalog) { $catalogArgs = @('--catalog', (Resolve-Path -LiteralPath $Catalog).Path) }
& $Python prepare.py --payload-root $payloadPath @catalogArgs
if ($LASTEXITCODE -ne 0) { throw 'Data generation failed.' }
& npm.cmd ci --ignore-scripts
if ($LASTEXITCODE -ne 0) { throw 'npm dependency installation failed.' }
& cargo test --manifest-path src-tauri\Cargo.toml --locked -- --test-threads=1
if ($LASTEXITCODE -ne 0) { throw 'Rust regression tests failed.' }
& npm.cmd run build
if ($LASTEXITCODE -ne 0) { throw 'Release build failed.' }
& cargo metadata --manifest-path src-tauri\Cargo.toml --format-version 1 --locked --filter-platform x86_64-pc-windows-msvc | Set-Content -LiteralPath build-dependencies.json -Encoding UTF8
if ($LASTEXITCODE -ne 0) { throw 'Dependency inventory failed.' }
& $Python package.py --payload-root $payloadPath --exe (Join-Path $env:CARGO_TARGET_DIR 'release\ds2-mod-suite.exe') --out (Join-Path $buildRoot 'packages')
if ($LASTEXITCODE -ne 0) { throw 'Packaging failed.' }
