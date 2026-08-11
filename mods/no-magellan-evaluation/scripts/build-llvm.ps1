$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$llvm = if ($env:LLVM_HOME) { $env:LLVM_HOME } else { 'C:\Program Files\LLVM\bin' }
$clang = Join-Path $llvm 'clang-cl.exe'
$link = Join-Path $llvm 'lld-link.exe'

foreach ($tool in @($clang, $link)) {
    if (!(Test-Path $tool)) { throw "LLVM tool missing: $tool" }
}

$out = Join-Path $root 'build\public'
New-Item -ItemType Directory -Force $out | Out-Null

$k32 = Join-Path $out 'kernel32.lib'
& $link /lib /machine:x64 "/def:$root\src\kernel32.def" "/out:$k32"
if ($LASTEXITCODE) { throw 'kernel32 import library creation failed' }

$obj = Join-Path $out 'no_magellan_evaluation.obj'
& $clang --target=x86_64-pc-windows-msvc /nologo /c /O2 /Ob0 /GS- /GR- /EHs-c- /Zl /Oi /W4 /WX /clang:-fno-builtin "/I$root\src" /TP "/Fo$obj" "$root\src\no_magellan_evaluation.cpp"
if ($LASTEXITCODE) { throw 'compile failed' }

$asi = Join-Path $out 'ds2_no_magellan_evaluation.asi'
& $link /dll /entry:DllMain /nodefaultlib /machine:x64 /subsystem:windows /Brepro "/out:$asi" $obj $k32
if ($LASTEXITCODE) { throw 'link failed' }

Copy-Item "$root\config\ds2_no_magellan_evaluation.ini" (Join-Path $out 'ds2_no_magellan_evaluation.ini') -Force
$hash = (Get-FileHash $asi -Algorithm SHA256).Hash.ToUpperInvariant()
$hash | Set-Content (Join-Path $out 'SHA256.txt') -Encoding ascii

Write-Host "Built: $asi"
Write-Host "SHA-256: $hash"
