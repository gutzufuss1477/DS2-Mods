# Build

The project is a freestanding x64 Windows ASI/DLL with no CRT dependency.

The release binary was built with Clang/LLD targeting the Microsoft x64 ABI.

Example build flow:

```bat
clang++ -target x86_64-pc-windows-msvc -O2 -ffreestanding -fno-exceptions -fno-rtti -fno-stack-protector -c src\ds2_apas_memory_costs.cpp -o ds2_apas_memory_costs.obj
lld-link /lib /machine:x64 /def:kernel32.def /out:kernel32.lib
lld-link /machine:x64 /dll /entry:DllMain /nodefaultlib /subsystem:windows /out:ds2_apas_memory_costs.asi ds2_apas_memory_costs.obj kernel32.lib
```

`kernel32.def` only needs the imports referenced by the source:

```text
CloseHandle
CreateFileW
CreateMutexW
CreateThread
DisableThreadLibraryCalls
GetLastError
GetModuleFileNameW
GetModuleHandleW
GetPrivateProfileStringW
Sleep
VirtualProtect
VirtualQuery
WriteFile
```

No third-party runtime library is required by the ASI itself.
