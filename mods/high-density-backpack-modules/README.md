# DS2 High-Density Backpack Modules v1.0.0

High-capacity backpack-component mod for **DEATH STRANDING 2: ON THE BEACH**.

The mod reduces the logical grid footprint of functional backpack components to one anchor cell and automatically relocates blocked placements to a nearby free, shape-valid anchor. The original visible model size is retained, so components may overlap visually while remaining separate native entries with their normal effects.

## Features

- Functional backpack components use one logical grid anchor instead of their original multi-cell footprint.
- Automatically searches for the nearest free anchor when the selected anchor is occupied.
- Keeps the complete native component shape for backpack-boundary checks.
- Prevents large components from being stored with their visible model hanging outside the backpack.
- Preserves native component IDs, levels, effects, list entries and save/load handling.
- Supports mixed layouts containing batteries, grenade pouches, ammo containers, utility pouches, solar generators, stabilizers and other functional modules.
- Fails closed when the supported executable or validated patch sites do not match.

## Practical capacity

DS2 uses a native `5 x 6` backpack grid, giving 30 unique logical anchor cells. The mod does not virtualize multiple owners for the exact same anchor.

The practical maximum depends on the selected component mix:

- Small components have many valid anchors.
- Large batteries and stabilizers need an anchor from which their complete visible shape still fits inside the backpack.
- Once no free and shape-valid anchor remains, DS2 can show its normal replacement dialog.

This is therefore a high-density capacity mod, not a literal unlimited-slot implementation.

## Validated behaviour

Development testing confirmed:

- 16 mixed functional components installed simultaneously in one layout;
- menu close and reopen with the compact layout intact;
- save, full game restart and load with the compact layout intact;
- substantially increased battery capacity from multiple installed batteries;
- automatic relocation of blocked anchors;
- large visible models kept within the native backpack boundary.

Visual clipping and overlap between modules are expected and intentional.

## Supported target

- `DS2.exe v1.10.89.0`
- SHA-256: `BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B`
- PE timestamp: `0x6A3DAE46`
- Image size: `0x0B292000`

The ASI verifies the executable header, native reference scan, component-shape lookup, conflict branch, occupancy flag and every modified instruction before activating.

## Installation

1. Install a compatible external 64-bit ASI loader for Death Stranding 2.
2. Copy `DS2_HighDensityBackpackModules_v1.0.0.asi` into the same game/ASI-loader directory used by your other DS2 ASI mods.
3. Start the game.
4. Confirm that `DS2_HighDensityBackpackModules.log` is created and contains `status=PATCH_APPLIED`.

A ready end-user package is available at `release/DS2_High_Density_Backpack_Modules_v1.0.0.zip`.

The ASI loader itself is not included.

## Usage notes

- Place components normally in the backpack-customization menu at a facility terminal.
- When the selected anchor is occupied, the mod may move the new component to a nearby valid anchor during confirmation.
- Visible modules can overlap heavily. This does not indicate that a component was lost; use the object list to verify installed entries.
- When the native replacement dialog appears because no valid anchor remains, choose **Cancel** unless you intentionally want to replace existing equipment.
- Component classes still use their own native effect logic. Battery-capacity stacking was explicitly validated; not every possible component combination has been measured individually.

## Compatibility

The mod changes only the backpack-component placement path documented in `docs/TECHNICAL_NOTES.md`.

Another mod that patches the same replacement-collection, grid-validation/commit or reference-check call sites may conflict. On a mismatch, this mod remains inactive rather than applying an uncertain patch.

## Uninstall

Before removing the mod:

1. With the mod still installed, reduce the backpack to a layout that fits under the original component footprints.
2. Apply the layout and create a new save.
3. Remove `DS2_HighDensityBackpackModules_v1.0.0.asi`.
4. The generated log may also be deleted.

Removing the mod while a save still contains a high-density layout is not recommended.

## Build

Use either:

- `scripts/build-llvm.ps1` on Windows; or
- `build.sh` on Linux with `clang-cl` and `lld-link` available.

The release ASI is built from the included source without the default C/C++ runtime libraries. It imports only `KERNEL32.dll`.

## Reference ASI SHA-256

`C4C5CC2B525BEB88AEF264521B56C00E6616ABF6DFB0C2F6970C92379D719C1C`

## Source / licence

Source is published for transparency, auditing and reproducible build verification. No separate open-source licence is granted unless explicitly stated otherwise.

## Version

`v1.0.0`
