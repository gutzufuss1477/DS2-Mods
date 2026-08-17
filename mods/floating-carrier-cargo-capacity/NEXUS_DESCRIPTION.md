# Floating Carrier Cargo Capacity v1.0.0

**Short description**  
Greatly increases cargo and weight capacity for both Level 1 and Level 2 Floating Carriers while keeping the game's native visible stacking.

## Description

Raises the Floating Carrier's cargo footprint from 36 to 240 units and increases both Level 1 and Level 2 weight limits to 5000 kg. Mixed S/M/L/XL cargo is supported and the game continues to place visible cargo using its native vertical stacking logic.

## Installation instructions

1. Install a working 64-bit ASI loader.
2. Remove any older Floating Carrier test/RC ASI files.
3. Copy `DS2_FloatingCarrier_CargoCapacity_v1.0.0.asi` into your DS2 ASI mod directory.
4. Start the game normally.

To uninstall, remove the ASI file. Before uninstalling, it is recommended to reduce the carrier to native cargo/weight limits.

## Main features

- 240 total cargo footprint units instead of 36.
- Native cargo sizes preserved: S=1, M=2, L=4, XL=6.
- Supports mixed cargo.
- Level 1 weight limit: 300 kg -> 5000 kg.
- Level 2 weight limit: 900 kg -> 5000 kg.
- Native visible vertical cargo stacking is preserved.
- No hotkeys, config file or Cheat Engine required.

## Requirements

- DEATH STRANDING 2: ON THE BEACH, tested with DS2.exe v1.10.89.0.
- A working 64-bit ASI loader.

## Compatibility

Designed to coexist with the Pickup Cargo Capacity and Tri-Cruiser Cargo Capacity mods because they use separate vehicle-specific capacity paths.

## Known limitation

The visible cargo tower continues to grow upward. Extremely tall stacks can collide with ceilings, terminal roofs or other world geometry. This is a physical placement limitation, not a logical cargo-limit failure.

## Shout outs

Thanks to the DS2 modding community and everyone who helped test the large mixed-cargo, overload and save-state behaviour during development.
