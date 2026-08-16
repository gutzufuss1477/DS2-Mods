# DS2 Tri-Cruiser Cargo Capacity v1.1.0

High-capacity Tri-Cruiser mod for **DEATH STRANDING 2: ON THE BEACH**.

## Capacity

- Target cargo footprint capacity: **320 units total**
- Native footprint sizes are preserved: S=1, M=2, L=4, XL=6
- Mixed S/M/L/XL cargo can be loaded in arbitrary order
- **v1.1.0 keeps the same 320-unit capacity with 0, 1 or 2 side accessories installed**
- Tested with Battery Units and the Anti-Gravity Unit / side accessory system
- Native cargo ownership, unloading, driving and save/load behavior is preserved

Extra cargo can overlap in the cargo screen or be invisible/overlap on the physical bike. This is intentional. The mod prioritizes stable logical cargo capacity over visual placement.

When the Tri-Cruiser is heavily loaded, the vehicle customization menu may prevent changing side components until cargo is unloaded. The mod does not alter this native garage behavior.

## Installation / update from v1.0.0

1. Install a working 64-bit ASI loader (for example Ultimate ASI Loader by ThirteenAG).
2. **Delete/replace the old `DS2_TriCruiser_CargoCapacity_v1.0.0.asi`. Do not keep both versions installed.**
3. Copy `DS2_TriCruiser_CargoCapacity_v1.1.0.asi` into the same game/ASI-loader directory.
4. Start the game normally.

To uninstall, remove the ASI file.

## Compatibility / baseline

Built and validated for **DS2.exe v1.10.89.0**.

The mod checks the expected original bytes at every patched location before installing. If the executable does not match the supported baseline, it fails closed instead of applying partial patches.

## Validation completed

The development builds were tested with:

- freely mixed S/M/L/XL cargo
- the full 320-unit target
- menu close/reopen and driving
- complete unload/reload cycles
- save/load cycles
- cargo while mounting with backpack cargo
- one and two mounted side accessories
- Battery Units and Anti-Gravity side equipment

## Technical summary

The release only contains the proven capacity/accessory mechanism:

- regular cargo mode: 160 footprint units per native bike side = 320 total
- native one-side accessory mode: full 320-unit cargo limit on the active virtual cargo path
- exact dual-accessory state is routed into that proven native one-side path
- cyclic reuse of native per-size slot-key banks
- virtual Tri-Cruiser UI item maximum 13 -> 320 for the relevant capacity calculation
- real accessory state is never written or removed
- no NULL-transform guards
- no ownership or relation manipulation
- no fake success/result overrides
- no runtime diagnostics, status writer or telemetry
