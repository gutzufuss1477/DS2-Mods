param([Parameter(Mandatory=$true)][string]$HostAsi,[string]$BootsAsi='', [string]$LegacyBoots='', [string]$IncompatibleBoots='')
$ErrorActionPreference='Stop'
Push-Location $PSScriptRoot
try {
 $cc='C:\Program Files\LLVM\bin\clang-cl.exe';$ld='C:\Program Files\LLVM\bin\lld-link.exe'
 & $cc /nologo /std:c++17 /O2 /GS- /GR- /EHs-c- /Zl /W4 /Isrc /c tests/companion_harness.cpp /Fobuild/companion-harness.obj
 if($LASTEXITCODE){throw 'Harness compile failed'}
 & $ld /dll /timestamp:0 /entry:DllMain /machine:x64 /nodefaultlib /dynamicbase /nxcompat /opt:ref /opt:icf /out:build/companion-harness.dll /implib:build/companion-harness.lib build/companion-harness.obj build/icon_gpu.obj build/exo_icon_gpu.obj build/payload.obj build/kernel32.lib
 if($LASTEXITCODE){throw 'Harness link failed'}
 $argsList=@('tests/test_addon_pair.py','--host',$HostAsi)
 if($BootsAsi){$argsList+=@('--boots',$BootsAsi)}
 if($LegacyBoots){$argsList+=@('--legacy-boots',$LegacyBoots)}
 if($IncompatibleBoots){$argsList+=@('--incompatible-boots',$IncompatibleBoots)}
 python @argsList
 if($LASTEXITCODE){throw 'Combination checks failed'}
} finally {Pop-Location}
