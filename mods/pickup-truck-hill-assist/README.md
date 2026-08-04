# Pickup Truck Hill Assist v1.2.0

Native ASI mod for **DEATH STRANDING 2: ON THE BEACH**.

The mod increases the slope-dependent friction of the pickup truck by patching
validated streamed vehicle-physics resources. It uses an event-driven listener
and does not perform a broad process-memory scan.

## Validated target

- Platform: Steam
- Game build: `1.10.89.0`
- `DS2.exe` SHA-256:
  `BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B`
- Validated release ASI SHA-256:
  `99F1ECF32CDF175D07675803A7FAF1083B0264BFAEEED7A85BA012290B27E4C6`

The game offsets, signatures and resource layouts are build-specific.
Compatibility with later game builds must be revalidated.

## Contents

- `src/PickupTruckHillAssist.cpp` — complete native source code
- `BUILD_RELEASE.cmd` — Visual Studio command-line build
- `BUILD.md` — build requirements and flags
- `config/PickupTruckHillAssist.ini.example` — validated settings example
- `UPDATE_CHECKLIST.md` — procedure for later DS2 updates
- `README_SOURCE.md` — original source-release information
- `PERMISSIONS.txt` — permissions
- `THIRD_PARTY_NOTICES.txt` — external loader and ownership notices

## Build

Open an **x64 Native Tools Command Prompt for Visual Studio 2022** and run:

`BUILD_RELEASE.cmd`

Output:

`build\PickupTruckHillAssist.asi`

The external x64 ASI loader is not included.

## Installation

1. Copy `PickupTruckHillAssist.asi` into the DS2 directory used by the ASI loader.
2. Copy `config\PickupTruckHillAssist.ini.example` beside the ASI.
3. Rename it to `PickupTruckHillAssist.ini`.
4. Start the game and inspect `PickupTruckHillAssist.log`.

## Validated configuration

- Grip multiplier: 4.00
- Minimum friction: 1.75
- Assist start slope: 6 degrees
- Full assist slope: 22 degrees
- Maximum friction: 8.00
- Stop after changed entries: 14
- Targeted streaming window: groups 4300 to 4800
