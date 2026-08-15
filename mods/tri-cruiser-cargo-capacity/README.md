# DS2 Tri-Cruiser Cargo Capacity v1.0.0

Stable high-capacity Tri-Cruiser mod for **DEATH STRANDING 2: ON THE BEACH**.

## Capacity

- Target cargo footprint capacity: **320 units total**
- Native footprint sizes are preserved: S=1, M=2, L=4, XL=6
- Mixed S/M/L/XL cargo can be loaded in arbitrary order
- The game continues to use its native cargo ownership, unloading, driving and save/load lifecycle

The extra cargo can overlap in the cargo screen or be invisible on the physical bike. This is intentional. The mod prioritizes stability and logical cargo capacity over visual placement.

## Installation

1. Remove older Tri-Cruiser development/test ASIs.
2. Copy `DS2_TriCruiser_CargoCapacity_v1.0.0.asi` into the game's ASI/mod loader directory.
3. Start the game normally.

## Compatibility / baseline

Built and validated for **DS2.exe v1.10.89.0**.

The mod checks the expected original bytes at every patched location before installing. If the executable does not match the supported baseline, it fails closed instead of applying partial patches.

## Validation completed

Development builds were tested with freely mixed S/M/L/XL cargo, menu close/reopen, driving, full unload/reload, and save/load cycles. The 320-unit build reached its native capacity rejection path while remaining stable.

## Technical summary

The release only contains the proven capacity mechanism:

- 160 footprint units per native bike side
- cyclic reuse of the native per-size slot-key banks
- virtual Tri-Cruiser UI item maximum 13 -> 320 for the relevant capacity calculation
- no NULL-transform guards
- no ownership or relation manipulation
- no fake success/result overrides
- no runtime diagnostic hooks or status writer
