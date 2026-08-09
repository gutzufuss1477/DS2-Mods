# Release validation — v1.0.1

Validation date: 2026-08-09

## Build

- Compiler: clang-cl 22.1.8, target `x86_64-pc-windows-msvc`
- Linker: lld-link
- Architecture: x64 native DLL/ASI
- Build is deterministic: two consecutive builds produced the same ASI SHA-256.
- Package is deterministic: two consecutive package runs produced the same ZIP SHA-256.

## Binary properties

- File description: `DS2 Off-road Pickup Cargo Capacity Expansion`
- File version: `1.0.1.0`
- Product name: `DS2 Pickup Cargo Capacity`
- ASLR: enabled
- DEP/NX compatibility: enabled
- High-entropy ASLR: enabled
- Imports: the documented eleven `KERNEL32.dll` functions only

## Hashes

- `PickupCargoCapacity.asi`: `6b3fa29f9b671d93e43db7001389f59f9c83095bcf612f26b2ee6b4d5e1c4692`

The final package hash is printed by `scripts/build-package.ps1` because it includes this validation document and the package manifest.

## Antivirus scan

Microsoft Defender Antivirus reported no threats for the ASI and release ZIP with security intelligence version `1.457.77.0` dated 2026-08-09.

This result documents the tested definitions and cannot guarantee the behaviour of future antivirus signatures or other vendors.

## In-game validation

The v1.0.1 ASI was tested in *Death Stranding 2: On the Beach* with `CapacityUnits=480`.

The generated status file reported:

```text
STATE=READY
DETAIL=All four validated pickup pack-limit patches are active
CONFIGURED_CAPACITY_UNITS=480
EFFECTIVE_CAPACITY_UNITS=480
PACK_AREA_COUNT=31
INDEX_LIMIT_BYTES=124
```

The pickup accepted cargo up to the expected 300% display value. The percentage is consistent with the unchanged UI reference of 160 units: `480 / 160 = 300%`.
