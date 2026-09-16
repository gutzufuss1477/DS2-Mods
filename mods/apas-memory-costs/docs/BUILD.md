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
