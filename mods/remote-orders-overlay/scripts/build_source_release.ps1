$ErrorActionPreference = 'Stop'

$Root = Split-Path -Parent $PSScriptRoot
$Build = Join-Path $Root 'build\source-release'
$Output = Join-Path $Root 'release'
$ImportDefs = Join-Path $Root 'tools\minimal_imports'
New-Item -ItemType Directory -Force -Path $Build, $Output | Out-Null

$ClangCommand = Get-Command clang-cl.exe -ErrorAction SilentlyContinue
$LinkCommand = Get-Command lld-link.exe -ErrorAction SilentlyContinue
$DllToolCommand = Get-Command llvm-dlltool.exe -ErrorAction SilentlyContinue
$LlvmBin = Join-Path $env:ProgramFiles 'LLVM\bin'
$Clang = if ($ClangCommand) { $ClangCommand.Source } else { Join-Path $LlvmBin 'clang-cl.exe' }
$Link = if ($LinkCommand) { $LinkCommand.Source } else { Join-Path $LlvmBin 'lld-link.exe' }
$DllTool = if ($DllToolCommand) { $DllToolCommand.Source } else { Join-Path $LlvmBin 'llvm-dlltool.exe' }
foreach ($Tool in @($Clang, $Link, $DllTool)) {
    if (-not (Test-Path $Tool)) { throw "Required LLVM tool was not found: $Tool" }
}

$CommonCompile = @('/nologo','/c','/O2','/GS-','/GR-','/EHsc-','/Zl','/utf-8','/std:c++17','/W4','/WX')
& $Clang @CommonCompile /Oi (Join-Path $Root 'src\ds2_remote_orders_overlay.cpp') "/Fo$(Join-Path $Build 'overlay.obj')"
if ($LASTEXITCODE -ne 0) { throw 'Overlay compilation failed.' }
& $Clang @CommonCompile /Oi- /clang:-fno-builtin-memcpy /clang:-fno-builtin-memset `
    (Join-Path $Root 'src\crt_strings.cpp') "/Fo$(Join-Path $Build 'crt_strings.obj')"
if ($LASTEXITCODE -ne 0) { throw 'CRT helper compilation failed.' }

$SdkRoot = Join-Path ${env:ProgramFiles(x86)} 'Windows Kits\10\Lib'
$Sdk = Get-ChildItem $SdkRoot -Directory -ErrorAction SilentlyContinue | Sort-Object Name -Descending | Where-Object {
    Test-Path (Join-Path $_.FullName 'um\x64\kernel32.lib')
} | Select-Object -First 1
if ($Sdk) {
    $LibPath = Join-Path $Sdk.FullName 'um\x64'
} else {
    & $DllTool -m i386:x86-64 -d (Join-Path $ImportDefs 'kernel32.def') -l (Join-Path $Build 'kernel32.lib')
    if ($LASTEXITCODE -ne 0) { throw 'Minimal Kernel32 import library generation failed.' }
    & $DllTool -m i386:x86-64 -d (Join-Path $ImportDefs 'ntdll.def') -l (Join-Path $Build 'ntdll.lib')
    if ($LASTEXITCODE -ne 0) { throw 'Minimal NTDLL import library generation failed.' }
    $LibPath = $Build
}

$Asi = Join-Path $Output 'DS2_Remote_Orders_Overlay.asi'
& $Link /dll /machine:x64 /subsystem:windows /entry:DllMain /nodefaultlib /dynamicbase /nxcompat /highentropyva /timestamp:0 `
    "/out:$Asi" (Join-Path $Build 'overlay.obj') (Join-Path $Build 'crt_strings.obj') `
    "/libpath:$LibPath" kernel32.lib ntdll.lib
if ($LASTEXITCODE -ne 0) { throw 'ASI link failed.' }

Write-Output $Asi
Get-FileHash -Algorithm SHA256 $Asi
