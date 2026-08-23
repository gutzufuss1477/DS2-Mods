# Description

High-Density Backpack Modules lets you install far more functional backpack components by reducing each module to one logical grid anchor.

The visible models keep their original size, so batteries, pouches, ammo containers, solar generators and stabilizers can overlap visually while remaining separate installed entries with their normal native effects.

When the selected anchor is already occupied, the mod searches for the nearest free anchor that also keeps the complete visible component shape inside the backpack.

## Main features

- One logical anchor per functional backpack component
- Automatic relocation from occupied anchors
- Native full-shape boundary protection
- Supports mixed batteries, grenade pouches, ammo containers, utility pouches, solar generators, stabilizers and more
- Native save/load handling retained
- Multiple battery effects confirmed to stack
- No Cheat Engine dependency
- Fail-closed version and instruction validation

## Important limitation

The backpack still uses the native 5 x 6 grid and therefore has 30 unique logical anchors. Large components have fewer usable anchors because their full visible model must remain inside the backpack.

When no valid anchor remains, the game can show its normal replacement dialog. Choose **Cancel** unless you intentionally want to replace an existing component.

Visual overlap and clipping are expected.

## Installation

1. Install a compatible 64-bit ASI loader.
2. Copy `DS2_HighDensityBackpackModules_v1.0.0.asi` into the game folder beside `DS2.exe` or into the directory used by your ASI loader.
3. Start the game.
4. Verify that `DS2_HighDensityBackpackModules.log` contains `status=PATCH_APPLIED`.

## Requirements

- Death Stranding 2 PC / Steam
- Supported game executable: `DS2.exe v1.10.89.0`
- Compatible external 64-bit ASI loader

## Uninstallation

First reduce the backpack to a layout that fits under the original component footprints, apply it and save. Then remove the ASI file.

## Shout outs

Thanks to the community member who suggested an overlappable backpack-module mod and to the players who helped validate the different placement paths.
