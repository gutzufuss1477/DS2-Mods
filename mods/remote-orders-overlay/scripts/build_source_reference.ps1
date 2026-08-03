$ErrorActionPreference = 'Stop'
$Root = Split-Path -Parent $PSScriptRoot
$Build = Join-Path $Root 'build\reference'
New-Item -ItemType Directory -Force -Path $Build | Out-Null

$Clang = (Get-Command clang-cl.exe -ErrorAction Stop).Source
$Link = (Get-Command lld-link.exe -ErrorAction Stop).Source
$SdkRoot = Join-Path ${env:ProgramFiles(x86)} 'Windows Kits\10\Lib'
$Sdk = Get-ChildItem $SdkRoot -Directory | Sort-Object Name -Descending | Where-Object {
    Test-Path (Join-Path $_.FullName 'um\x64\kernel32.lib')
} | Select-Object -First 1
if (-not $Sdk) { throw 'Windows SDK x64 libraries were not found.' }
$Lib = Join-Path $Sdk.FullName 'um\x64'

& $Clang /nologo /c /O2 /Oi /GS- /GR- /EHsc- /Zl /std:c++17 /W4 /WX `
    (Join-Path $Root 'src\ds2_remote_orders_overlay.cpp') /Fo$(Join-Path $Build 'overlay.obj')
& $Clang /nologo /c /O2 /Oi /GS- /GR- /EHsc- /Zl /std:c++17 /W4 /WX `
    (Join-Path $Root 'src\crt_strings.cpp') /Fo$(Join-Path $Build 'crt_strings.obj')
& $Link /dll /machine:x64 /subsystem:windows /entry:DllMain /nodefaultlib /dynamicbase /nxcompat /highentropyva `
    /out:$(Join-Path $Build 'DS2_Remote_Orders_Overlay_REFERENCE.asi') `
    (Join-Path $Build 'overlay.obj') (Join-Path $Build 'crt_strings.obj') `
    /libpath:$Lib kernel32.lib ntdll.lib

Write-Warning 'This source-compiled artifact is for review only. The public runtime-tested release is produced by build_release.ps1.'
