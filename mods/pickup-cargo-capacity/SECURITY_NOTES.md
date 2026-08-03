# Security and runtime behaviour

`PickupCargoCapacity.asi` is an in-process native plugin intended to be loaded by an ASI loader together with `DS2.exe`.

## Runtime actions

The plugin:

1. reads `PickupCargoCapacity.ini`;
2. obtains the base address of the current executable with `GetModuleHandleW(0)`;
3. validates the exact original bytes at four known code locations;
4. temporarily changes protection for those exact bytes with `VirtualProtect`;
5. writes four coordinated capacity constants;
6. restores the original page protection and flushes the instruction cache;
7. writes `PickupCargoCapacity_STATUS.txt`.

If any expected original byte does not match, no game code is modified and the status file reports `STATE=ERROR`.

## Imported Windows APIs

The source imports only these `KERNEL32.dll` functions:

- `DisableThreadLibraryCalls`
- `CreateThread`
- `CloseHandle`
- `GetModuleHandleW`
- `GetModuleFileNameW`
- `GetPrivateProfileIntW`
- `VirtualProtect`
- `FlushInstructionCache`
- `GetCurrentProcess`
- `CreateFileW`
- `WriteFile`

There is no networking, registry access, shell execution, remote-process access, process injection, downloading, telemetry or persistence mechanism.

`CreateThread` avoids performing patch work directly under the Windows loader lock. `VirtualProtect` and `FlushInstructionCache` are required for the documented in-process byte patch.
