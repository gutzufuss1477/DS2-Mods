param([switch]$Tests)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$llvm = if ($env:LLVM_BIN) { $env:LLVM_BIN } else { 'C:\Program Files\LLVM\bin' }
$clang = Join-Path $llvm 'clang-cl.exe'
$link = Join-Path $llvm 'lld-link.exe'
$out = Join-Path $root 'build'
New-Item -ItemType Directory -Force -Path $out | Out-Null
& $link /lib /machine:x64 "/def:$root\src\kernel32.def" "/out:$out\kernel32.lib"
if ($LASTEXITCODE) { throw 'Import library build failed' }
$source = if ($Tests) { "$root\tests\runtime_tests.cpp" } else { "$root\src\apas.cpp" }
$name = if ($Tests) { 'apas_tests' } else { 'ds2_apas_memory_costs' }
$extension = if ($Tests) { 'dll' } else { 'asi' }
& $clang --target=x86_64-pc-windows-msvc /nologo /c /O2 /GS- /GR- /EHs-c- /Zl /Oi /W4 /WX /clang:-fno-builtin /clang:-mcx16 /clang:-Wno-unused-function "/I$root\src" /TP "/Fo$out\$name.obj" $source
if ($LASTEXITCODE) { throw 'Compile failed' }
& $link /dll /entry:DllMain /nodefaultlib /machine:x64 /subsystem:windows /Brepro "/out:$out\$name.$extension" "$out\$name.obj" "$out\kernel32.lib"
if ($LASTEXITCODE) { throw 'Link failed' }
Get-FileHash -Algorithm SHA256 -LiteralPath "$out\$name.$extension" | Format-List
