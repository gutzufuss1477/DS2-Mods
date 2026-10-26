# Current offline runtime suite; build-api.ps1 must be run first.
$ErrorActionPreference='Stop'
& (Join-Path $PSScriptRoot 'test-runtime.ps1')
