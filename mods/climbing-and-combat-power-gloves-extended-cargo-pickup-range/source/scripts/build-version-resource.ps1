param([Parameter(Mandatory=$true)][string]$OutputDirectory)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$sdkRoot = Join-Path ${env:ProgramFiles(x86)} 'Windows Kits\10\bin'
$resourceCompiler = if ($env:RC_EXE) { $env:RC_EXE } else {
    Get-ChildItem -LiteralPath $sdkRoot -Directory |
        Where-Object { $_.Name -match '^10\.0\.\d+\.\d+$' } |
        Sort-Object { [version]$_.Name } -Descending |
        ForEach-Object { Join-Path $_.FullName 'x64\rc.exe' } |
        Where-Object { Test-Path -LiteralPath $_ } | Select-Object -First 1
}
if (!$resourceCompiler -or !(Test-Path -LiteralPath $resourceCompiler)) {
    throw 'Windows SDK resource compiler missing; install the SDK or set RC_EXE.'
}
& $resourceCompiler /nologo /fo (Join-Path $OutputDirectory 'version.res') (Join-Path $root 'src\version.rc')
if ($LASTEXITCODE) { throw 'version resource compile failed' }
