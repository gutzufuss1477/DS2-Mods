$ErrorActionPreference = "Stop"

clang++ --target=x86_64-pc-windows-msvc -c .\mod.cpp -o .\mod.obj -O2 `
  -ffreestanding -fno-builtin -fno-exceptions -fno-rtti `
  -fno-stack-protector -fno-threadsafe-statics

clang --target=x86_64-pc-windows-msvc -c .\peb.s -o .\peb.obj
clang --target=x86_64-pc-windows-msvc -c .\hook.s -o .\hook.obj

lld-link /dll /entry:DllMain /nodefaultlib /machine:x64 `
  /out:PorterGradeBooster.asi mod.obj peb.obj hook.obj

Write-Host "Built PorterGradeBooster.asi"
