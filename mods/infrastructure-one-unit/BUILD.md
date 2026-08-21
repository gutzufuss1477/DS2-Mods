# Building

## Windows / LLVM

Requirements:

- LLVM with `clang-cl.exe` and `lld-link.exe`
- PowerShell

Run:

```powershell
.\scripts\build-llvm.ps1
```

The output is written to:

```text
build\public\
```

## Linux cross-build used for repository validation

The source also builds with LLVM's Windows target and the supplied minimal import library definition:

```bash
lld-link /lib /machine:x64 /def:src/kernel32.def /out:kernel32.lib
clang++ --target=x86_64-pc-windows-msvc -c -O2 -fno-exceptions -fno-rtti \
  -fno-stack-protector -fno-builtin -ffreestanding -I src \
  -o infrastructure_one_unit.obj src/infrastructure_one_unit.cpp
lld-link /dll /entry:DllMain /nodefaultlib /machine:x64 /subsystem:windows /Brepro \
  /out:ds2_infrastructure_one_unit.asi infrastructure_one_unit.obj kernel32.lib
```

The release binary under `reference/` is the exact in-game validated binary. It differs from the final validated testfix only in the embedded release label.
