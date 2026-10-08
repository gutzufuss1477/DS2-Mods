# Technical Notes - Sam Overhaul v1.1.0-dev.1

Target executable:
- Steam DS2.exe 1.10.89.0
- PE timestamp: 0x6A3DAE46
- SizeOfImage: 0x0B292000
- SHA-256: BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B

Key validated systems:
- DSPlayerEquipmentManage visibility routing for carried cargo.
- Backpack outline/bind-effect suppression for hidden backpack cargo.
- Spare-shoe visibility path while preserving worn shoes.
- Monorail final blocker bypass only; earlier CanJumpDown state checks stay native.
- Zipline detach action gate while retaining native input/animation.
- Landing classifier/animation path with temporary native-state substitution and restoration.
- Vehicle Autodrive readiness timer with configurable seconds.
- DSVehicleWeaponPartsResource streaming patcher for IDs 118/119, 163, 165 and 169.
- Chiral Cannon Weapon ID 163 -> Ammo ID 275 -> DSAmmoParameter +0x80 charge value.
  Vanilla 3.0 seconds; default release value 0.75 seconds.

## Footprint integration (development)

The archived v1.0.0 release excluded the earlier unsuccessful experiments. The new integration uses the subsequently gameplay-confirmed resource-filtered stamp append at RVA `0x02250210`, with the same three resource UUIDs, vtable validation, guarded reads and twelve-argument x64 forwarding boundary.

`src/footprints/footprint_core.h` is copied unchanged from the confirmed probe. Its UTF-8/LF normalized SHA-256 is `e054c990d66126e04bc0b21068d4f1877e4adffb4b73580cc029cd2c1cd16e63`.

`[Footprints] HideFootprints` is explicitly opt-in; missing, empty, invalid and truncated settings do not enable the filter. When off, no footprint hook or executable hash operation is installed/performed. When on, the exact executable hash and function prologue must match. A loaded standalone probe or an already-patched prologue causes refusal, without turning off the existing base-mod features.

The native append is skipped only for verified footprint resources. Ownership/allocation, removal lists, tracking state and save serialization are not rewritten. The option includes normal and scanner-highlighted footprints. Earlier rendered footprints are not forcibly deleted mid-session; start/reload with the option active to suppress their reconstruction.

The integration uses the existing pinned ASI, worker, INI and log. There is no second module or worker. Per-resource diagnostic dumps and the probe's repeating logs are not written by this build. Startup status is logged, plus at most one first-hit and one unknown-context message during the existing Autodrive wait loop.

MinHook 1.3.4 is statically linked from vendored, unmodified upstream source, with its license retained. The combined binary preserves the no-CRT build and imports only KERNEL32.dll and bcrypt.dll. The footprint translation unit explicitly disables C++ exception unwinding (`/EHs-c-`) and uses no throwing runtime operations. The original code's build flags and gameplay patch bodies remain unchanged.

The mutex name remains compatible with v1.0.0 to prevent concurrent Sam Overhaul workers. Install only one Sam Overhaul ASI and remove the standalone footprint probe before using the combined build.

Automated results and the outstanding combined-game test are recorded in `FOOTPRINTS_VALIDATION.md`.
