$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
$compiler = if ($env:MSVC_BIN) {
    Join-Path $env:MSVC_BIN 'cl.exe'
} else {
    & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -find 'VC\Tools\MSVC\**\bin\Hostx64\x64\cl.exe' | Select-Object -First 1
}
if (!$compiler -or !(Test-Path -LiteralPath $compiler)) { throw 'MSVC x64 compiler missing' }
$bin = Split-Path -Parent $compiler
$out = Join-Path $root 'build\public'
New-Item -ItemType Directory -Force -Path $out | Out-Null
& (Join-Path $bin 'lib.exe') /nologo /machine:x64 "/def:$root\src\kernel32.def" "/out:$out\kernel32.lib"
if ($LASTEXITCODE) { throw 'kernel32 import library creation failed' }
& $compiler /nologo /c /O2 /Ob0 /GS- /GR- /EHs-c- /Zl /Oi- /W4 /WX "/I$root\src" /TP "/Fo$out\climbing_gloves_range.obj" "$root\src\climbing_gloves_range.cpp"
if ($LASTEXITCODE) { throw 'compile failed' }
& (Join-Path $bin 'link.exe') /nologo /dll /entry:DllMain /nodefaultlib /machine:x64 /subsystem:windows /Brepro "/out:$out\ds2_climbing_gloves_range.asi" "$out\climbing_gloves_range.obj" "$out\kernel32.lib"
if ($LASTEXITCODE) { throw 'link failed' }
Copy-Item -LiteralPath "$root\config\ds2_climbing_gloves_range.ini" -Destination $out -Force
Write-Host "Built: $out\ds2_climbing_gloves_range.asi"
Get-FileHash -LiteralPath "$out\ds2_climbing_gloves_range.asi" -Algorithm SHA256
