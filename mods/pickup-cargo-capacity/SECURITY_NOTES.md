# Security and runtime behaviour - v1.0.2

PickupCargoCapacity.asi is an in-process native ASI plugin. It is intended for DS2.exe and performs no networking, registry writes, cross-process memory access or telemetry.

## Runtime sequence

1. Read PickupCargoCapacity.ini and locate the DS2.exe module base.
2. Compare the four original pack-area instruction sequences, then apply the same coordinated capacity patches as v1.0.1. A mismatch prevents the capacity patch.
3. If enabled and capacity > 160, separately validate the two original instructions used by the Pickup's rear cargo indicator. One hook affects the five visual light stages; the other affects the float fed into the vehicle model's display material.
4. Reserve a local 4 KiB code island with VirtualAlloc, close enough for two verified x64 rel32 jumps. The code island is written once, its instruction cache is flushed and its protection changed to read/execute.
5. Patch the two display-only instruction locations using VirtualProtect, with rollback attempts if any write fails. The code island is retained while referenced by a live hook and reclaimed when the process exits.
6. Write a PickupCargoCapacity_STATUS.txt with the capacity status and independent REAR_INDICATORS status.

The temporary display count is calculated as min(10, ceil(occupied_areas * 10 / usable_areas)), for usable_areas between 10 and 30. The two detours do not modify the authoritative cargo counters, cargo placement data, inventory ownership or save-game format. No allocations or external calls occur per UI update.

Optional display patch initialization failures are reported by REAR_INDICATORS. They do not disable the already installed main capacity patch.

## Windows API and PE hardening

The LLVM fallback imports the expected KERNEL32.dll APIs, including VirtualAlloc, VirtualFree, VirtualProtect, FlushInstructionCache, CreateThread, GetModuleHandleW, GetModuleFileNameW, GetPrivateProfileIntW, CreateFileW, WriteFile and CloseHandle. The recommended MSVC build uses a conventional DLL entry point and static runtime, with additional standard runtime imports.

ASLR, DEP/NX and high-entropy ASLR are enabled; MSVC builds additionally enable Control Flow Guard. Builds are unsigned unless separately signed by a trusted code-signing certificate.

Do not turn off antivirus protection. If a file is incorrectly detected, submit it and its corresponding source/build information to the antivirus vendor.
