# Hill Assist and Speed Boost

Source code and validated release reference for **DS2 Vehicle Physics Core v1.1.0**
for **DEATH STRANDING 2: ON THE BEACH**.

This is the final combined version of:

- Pickup Truck Hill Assist
- Vehicle Speed Boost for pickup truck and Tri-Cruiser

Both modules share one event-driven streaming listener inside
`DS2VehicleCore.asi`. Each feature is enabled independently by placing its
corresponding INI file beside the ASI.

## Validated target

- Platform: Steam
- Game build: `1.10.89.0`
- Core version: `1.1.0`
- Validated ASI SHA-256:
  `78F4D90B6473F9163E65A227AE6444D516133ABAC34C9E6575BD2B13C3CB60B6`

The offsets, signatures, resource layouts and streaming window are
game-build-specific. Compatibility with later builds must be revalidated.

## Repository layout

- `src/DS2VehicleCore.cpp` — complete combined native source
- `config/PickupTruckHillAssist.ini` — final Hill Assist configuration
- `config/VehicleSpeedBoost.ini` — final normal-speed and boost configuration
- `scripts/build-llvm.cmd` — release-equivalent clang-cl/lld-link build
- `scripts/build-msvc.cmd` — Visual Studio compiler fallback build
- `reference/DS2VehicleCore_v1.1.0.asi` — exact published reference binary
- `reference/BUILD_INFO_ORIGINAL.txt` — original build information
- `RELEASE_VERIFICATION.md` — source and release provenance
- `SECURITY_NOTES.md` — runtime behaviour and imported Windows APIs
- `UPDATE_CHECKLIST.md` — validation procedure after a DS2 update

## Configuration

### Hill Assist

The validated configuration uses:

- grip multiplier: 400%
- minimum friction: 175%
- assist start: 6 degrees
- full assist: 22 degrees
- maximum friction: 800%
- expected changed entries: 14
- streaming groups: 4300–4800

### Vehicle Speed Boost

The validated release configuration uses:

- pickup normal speed: 150%
- Tri-Cruiser normal speed: 150%
- pickup boost top speed: 150%
- Tri-Cruiser boost top speed: 150%
- expected profiles: two pickup profiles and one bike profile
- streaming groups: 4300–4800

## Build

For the release-equivalent toolchain, open a developer command prompt with
LLVM tools available and run:

`scripts\build-llvm.cmd`

For a normal Visual Studio 2022 x64 Native Tools prompt, run:

`scripts\build-msvc.cmd`

Both scripts output:

`build\DS2VehicleCore.asi`

Compiler and linker metadata can change the binary hash. The reference hash is
for the exact published v1.1.0 ASI.

## Installation

1. Install an external x64 ASI loader for the game.
2. Copy `DS2VehicleCore.asi` beside the loader.
3. Copy one or both INI files beside the ASI.
4. Start the game.
5. Review `DS2VehicleCore.log`.

The ASI loader itself is not included.
