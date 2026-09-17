# Build Notes

The source in `src/ds2_apas_memory_costs.cpp` is the exact source stored with the
promoted v1.0.3 performance build.

It is a small x64 Windows ASI/DLL implementation using only Win32 APIs and no
third-party runtime dependency.

Important imports used by this source:

```text
GetModuleHandleW
GetModuleFileNameW
CreateThread
CloseHandle
DisableThreadLibraryCalls
Sleep
VirtualQuery
GetPrivateProfileIntW
```

The Git release binary is intentionally the exact tested build and was not recompiled
for this handover package.

If rebuilding later, preserve a conventional PE layout because earlier ultra-minimal
builds triggered Nexus antivirus quarantine even though the mod behaviour itself was
legitimate.

## Unlock All v1.1.0 release build

The optional release additionally imports:

```text
VirtualProtect
FlushInstructionCache
GetCurrentProcess
```

because it modifies two bytes in the executable `.text` section after validating the
exact expected local signature.

For Nexus uploads, retain a conventional x64 PE layout with:

- `.text`
- `.rdata`
- `.data`
- `.pdata`
- `.reloc`

Earlier ultra-minimal ASI builds without normal unwind metadata triggered Nexus
quarantine more readily.

The v1.1.0 Nexus release was therefore compiled with unwind tables enabled and linked
as a normal relocatable x64 DLL/ASI.
