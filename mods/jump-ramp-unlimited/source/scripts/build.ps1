param(
    [string]$LlvmBin = "$env:ProgramFiles\LLVM\bin",
    [string]$ResourceCompiler = ""
)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$out = Join-Path $root 'build'
$null = New-Item -ItemType Directory -Force -Path $out
if (-not $ResourceCompiler) {
    $sdk = Join-Path ${env:ProgramFiles(x86)} 'Windows Kits\10\bin'
    $ResourceCompiler = Get-ChildItem -LiteralPath $sdk -Directory |
        Where-Object { $_.Name -match '^\d+\.\d+\.\d+\.\d+$' } |
        Sort-Object { [version]$_.Name } -Descending |
        ForEach-Object { Join-Path $_.FullName 'x64\rc.exe' } |
        Where-Object { Test-Path -LiteralPath $_ } | Select-Object -First 1
}
$clang = Join-Path $LlvmBin 'clang-cl.exe'
$linker = Join-Path $LlvmBin 'lld-link.exe'
foreach ($tool in @($clang, $linker, $ResourceCompiler)) {
    if (-not $tool -or -not (Test-Path -LiteralPath $tool)) { throw "Build tool not found: $tool" }
}
$version = (Get-Content -LiteralPath (Join-Path $root 'VERSION.txt') -Raw).Trim()
$source = Get-Content -LiteralPath (Join-Path $root 'src\jump_ramp_unlimited.cpp') -Raw
$resource = Get-Content -LiteralPath (Join-Path $root 'src\version.rc') -Raw
if (-not $source.Contains('#define MOD_VERSION "' + $version + '"') -or
    -not $resource.Contains('"ProductVersion", "' + $version + '\0"')) {
    throw 'Source and resource versions must match VERSION.txt.'
}
& $linker /lib /machine:x64 "/def:$root\src\kernel32.def" "/out:$out\kernel32.lib"
if ($LASTEXITCODE -ne 0) { throw 'Import library build failed.' }
& $clang --target=x86_64-pc-windows-msvc /nologo /c /O2 /Ob0 /GS- /GR- /EHs-c- /Zl /Oi /W4 /WX /Brepro /clang:-fno-builtin "/I$root\src" /TP "/Fo$out\jump_ramp_unlimited.obj" "$root\src\jump_ramp_unlimited.cpp"
if ($LASTEXITCODE -ne 0) { throw 'Compilation failed.' }
& $ResourceCompiler /nologo "/fo$out\version.res" "$root\src\version.rc"
if ($LASTEXITCODE -ne 0) { throw 'Version resource compilation failed.' }
& $linker /dll /entry:DllMain /nodefaultlib /machine:x64 /subsystem:windows /dynamicbase /nxcompat /Brepro "/out:$out\ds2_jump_ramp_unlimited.asi" "$out\jump_ramp_unlimited.obj" "$out\version.res" "$out\kernel32.lib"
if ($LASTEXITCODE -ne 0) { throw 'Linking failed.' }
Get-FileHash -LiteralPath "$out\ds2_jump_ramp_unlimited.asi" -Algorithm SHA256
