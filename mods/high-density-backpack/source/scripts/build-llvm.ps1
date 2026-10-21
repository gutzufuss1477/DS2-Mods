param(
    [string]$ClangCl = 'C:\Program Files\LLVM\bin\clang-cl.exe',
    [string]$LldLink = 'C:\Program Files\LLVM\bin\lld-link.exe',
    [ValidateSet('Clean', 'Overlap')][string]$VisualStyle = 'Clean'
)

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$src = Join-Path $root 'src'
$out = Join-Path $root 'build'
$reference = Join-Path $root 'dist'
$suffix = if ($VisualStyle -eq 'Overlap') { '_Classic_Overlap' } else { '' }
$asi = Join-Path $reference "DS2_HighDensityBackpackModules_v1.1.0$suffix.asi"

New-Item -ItemType Directory -Path $out, $reference -Force | Out-Null
Remove-Item -LiteralPath $asi -Force -ErrorAction SilentlyContinue

& $LldLink /dll /noentry /machine:x64 "/def:$src\kernel32.def" "/out:$out\kernel32_stub.dll" "/implib:$out\kernel32.lib"
if ($LASTEXITCODE -ne 0) { throw 'Failed to create the KERNEL32 import library.' }

$compileArgs = @('--target=x86_64-pc-windows-msvc', '/nologo', '/c', '/O2', '/GS-', '/Gs9999999', '/GR-', '/EHs-c-', '/Zl', '/Oi', '/W4', '/WX',
    '/clang:-ffreestanding', '/clang:-fno-builtin', '/clang:-funwind-tables')
if ($VisualStyle -eq 'Overlap') { $compileArgs += '/DHDB_OVERLAP_VISUALS=1' }
$compileArgs += "/Fo$out\high_density_backpack_modules.obj"
$compileArgs += "$src\high_density_backpack_modules.cpp"
& $ClangCl @compileArgs
if ($LASTEXITCODE -ne 0) { throw 'Compilation failed.' }

& $ClangCl --target=x86_64-pc-windows-msvc /nologo /c "/Fo$out\charm_adapters.obj" "$src\charm_adapters.s"
if ($LASTEXITCODE -ne 0) { throw 'Assembly failed.' }

& $LldLink /dll /machine:x64 /entry:DllMain /nodefaultlib /timestamp:0 `
    "/out:$asi" "/implib:$out\high_density_backpack_modules.lib" `
    "$out\high_density_backpack_modules.obj" "$out\charm_adapters.obj" "$out\kernel32.lib"
if ($LASTEXITCODE -ne 0) { throw 'Linking failed.' }

$hash = (Get-FileHash -Algorithm SHA256 -LiteralPath $asi).Hash
Write-Host "Built: $asi"
Write-Host "SHA-256: $hash"
