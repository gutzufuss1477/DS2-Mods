$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$src = Join-Path $root 'src'
$out = Join-Path $root 'build\public'
$clang = 'C:\Program Files\LLVM\bin\clang-cl.exe'
$lld = 'C:\Program Files\LLVM\bin\lld-link.exe'
$read = 'C:\Program Files\LLVM\bin\llvm-readobj.exe'

if (Test-Path $out) { Remove-Item $out -Recurse -Force }
New-Item -ItemType Directory -Force -Path $out | Out-Null

& $lld /dll /noentry /machine:x64 "/def:$src\kernel32.def" "/out:$out\kernel32_stub.dll" "/implib:$out\kernel32.lib"
if ($LASTEXITCODE -ne 0) { throw 'Failed to create KERNEL32 import library.' }

$obj = Join-Path $out 'sneaky_sam.obj'
& $clang --target=x86_64-pc-windows-msvc /nologo /c /O2 /GS- /Gs9999999 /GR- /EHs-c- /Zl /Oi /W4 /WX /clang:-ffreestanding /clang:-fno-builtin "/Fo$obj" (Join-Path $src 'sneaky_sam.cpp')
if ($LASTEXITCODE -ne 0) { throw 'Compilation failed.' }

$asi = Join-Path $out 'DS2_SneakySam_v1.0.0.asi'
& $lld /dll /machine:x64 /entry:DllMain /nodefaultlib /timestamp:0 "/out:$asi" "/implib:$out\sneaky_sam.lib" $obj "$out\kernel32.lib"
if ($LASTEXITCODE -ne 0) { throw 'Linking failed.' }

& $read --file-headers --coff-imports $asi | Out-File (Join-Path $out 'PE_REPORT.txt') -Encoding utf8
$hash = (Get-FileHash $asi -Algorithm SHA256).Hash.ToUpper()
Set-Content (Join-Path $out 'SHA256.txt') "$hash  DS2_SneakySam_v1.0.0.asi" -Encoding ascii
Write-Output "ASI=$asi"
Write-Output "SHA256=$hash"
