$ErrorActionPreference = "Stop"

$Root = Split-Path -Parent $PSScriptRoot
$Src = Join-Path $Root "src"
$Out = Join-Path $Root "reference"
New-Item -ItemType Directory -Force -Path $Out | Out-Null

$ModCpp = Join-Path $Src "mod.cpp"
$HookAsm = Join-Path $Src "hook.s"
$PebAsm = Join-Path $Src "peb.s"
$ModObj = Join-Path $Src "mod.obj"
$HookObj = Join-Path $Src "hook.obj"
$PebObj = Join-Path $Src "peb.obj"
$Asi = Join-Path $Out "SamStatsBooster.asi"

& clang++ --target=x86_64-pc-windows-msvc -c $ModCpp -o $ModObj -O2 `
  -ffreestanding -fno-builtin -fno-exceptions -fno-rtti `
  -fno-stack-protector -fno-threadsafe-statics

& clang --target=x86_64-pc-windows-msvc -c $PebAsm -o $PebObj
& clang --target=x86_64-pc-windows-msvc -c $HookAsm -o $HookObj

& lld-link /dll /entry:DllMain /nodefaultlib /machine:x64 `
  "/out:$Asi" $ModObj $PebObj $HookObj

Write-Host "Built $Asi"
