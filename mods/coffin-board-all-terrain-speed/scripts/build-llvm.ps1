$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$llvm = if ($env:LLVM_BIN) {
    $env:LLVM_BIN
} elseif ($env:LLVM_HOME -and (Test-Path -LiteralPath (Join-Path $env:LLVM_HOME 'bin\clang-cl.exe'))) {
    Join-Path $env:LLVM_HOME 'bin'
} elseif ($env:LLVM_HOME) {
    $env:LLVM_HOME
} else {
    'C:\Program Files\LLVM\bin'
}
$clang = Join-Path $llvm 'clang-cl.exe'
$link = Join-Path $llvm 'lld-link.exe'

foreach ($tool in @($clang, $link)) {
    if (!(Test-Path -LiteralPath $tool)) { throw "LLVM tool missing: $tool" }
}

$source = Join-Path $root 'src\coffin_board_all_terrain_speed.cpp'
$definitions = Join-Path $root 'src\kernel32.def'
$configuration = Join-Path $root 'config\ds2_coffin_board_all_terrain_speed.ini'
foreach ($inputFile in @($source, $definitions, $configuration)) {
    if (!(Test-Path -LiteralPath $inputFile)) { throw "Build input missing: $inputFile" }
}

$out = Join-Path $root 'build\public'
New-Item -ItemType Directory -Force -Path $out | Out-Null

$kernel32 = Join-Path $out 'kernel32.lib'
& $link /lib /machine:x64 "/def:$definitions" "/out:$kernel32"
if ($LASTEXITCODE) { throw 'kernel32 import library creation failed' }

$object = Join-Path $out 'coffin_board_all_terrain_speed.obj'
& $clang --target=x86_64-pc-windows-msvc /nologo /c /O2 /Ob0 /GS- /GR- /EHs-c- /Zl /Oi /W4 /WX /clang:-fno-builtin /clang:-mcx16 "/I$root\src" /TP "/Fo$object" $source
if ($LASTEXITCODE) { throw 'compile failed' }

$asi = Join-Path $out 'ds2_coffin_board_all_terrain_speed.asi'
& $link /dll /entry:DllMain /nodefaultlib /machine:x64 /subsystem:windows /Brepro "/out:$asi" $object $kernel32
if ($LASTEXITCODE) { throw 'link failed' }

Copy-Item -LiteralPath $configuration -Destination (Join-Path $out 'ds2_coffin_board_all_terrain_speed.ini') -Force
$hash = (Get-FileHash -LiteralPath $asi -Algorithm SHA256).Hash.ToUpperInvariant()
Write-Host "Built: $asi"
Write-Host "SHA-256: $hash"
