$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$clang='C:\Program Files\LLVM\bin\clang-cl.exe';$link='C:\Program Files\LLVM\bin\lld-link.exe';$dt='C:\Program Files\LLVM\bin\llvm-dlltool.exe';$ro='C:\Program Files\LLVM\bin\llvm-readobj.exe'
$out=Join-Path $root 'build\diagnostic';if(Test-Path $out){Remove-Item $out -Recurse -Force};New-Item -ItemType Directory $out|Out-Null
$def=Join-Path $root 'src\kernel32.def';$k32=Join-Path $out 'kernel32.lib';& $dt -m i386:x86-64 -d $def -l $k32
$common=@('--target=x86_64-pc-windows-msvc','/nologo','/c','/O2','/Ob0','/GS-','/GR-','/EHs-c-','/Zl','/Oi','/W4','/WX','/clang:-fno-builtin','/DDIAGNOSTIC_TRACE',"/I$root\src")
$obj=Join-Path $out 'freight_likes_booster.obj';& $clang @common '/TP' "/Fo$obj" (Join-Path $root 'src\freight_likes_booster.cpp');if($LASTEXITCODE){throw 'compile failed'}
$asi=Join-Path $out 'ds2_lost_cargo_likes_booster.asi';& $link /dll /entry:DllMain /nodefaultlib /machine:x64 /subsystem:windows /Brepro "/out:$asi" $obj $k32;if($LASTEXITCODE){throw 'link failed'}
& $ro --file-headers --coff-imports --coff-exports $asi|Out-File (Join-Path $out 'PE_REPORT.txt') -Encoding utf8
$hash=(Get-FileHash $asi -Algorithm SHA256).Hash.ToUpper();Set-Content (Join-Path $out 'SHA256.txt') "$hash  ds2_lost_cargo_likes_booster.asi" -Encoding ascii
Write-Output "ASI=$asi";Write-Output "SHA256=$hash"
