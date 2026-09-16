# DS2 Proficiency Bonus Multiplier

INI-driven proficiency bonus scaling for **DEATH STRANDING 2: ON THE BEACH**.

**Release:** v1.0.0  
**Supported game build:** `DS2.exe 1.10.89.0`  
**Architecture:** x64 ASI, one-shot runtime data patch

## What it does

The mod scales the level-dependent bonus tables behind Sam's proficiency system. Multipliers are configurable independently for Level 1 through Level 5 and for each visible proficiency category.

It does not change proficiency gain, level thresholds, unlocked levels or save data.

## Supported categories

- Terrain Traversal
- Carrying Capacity
- Endurance
- KO Resistance
- Lung Capacity
- Strand Skills
- Melee Combat
- Handguns
- Assault Rifles
- Sniper Rifles
- Machine Guns
- Shotguns

`StealthTakedownInternal` is present in the INI for completeness but is disabled by default.

## Scaling rules

The runtime tables contain different value semantics, so the mod does not blindly multiply every raw float.

- Factor rows with neutral value `1.0`: `1 + (vanilla - 1) * multiplier`
- Additive rows with neutral value `0.0`: `vanilla * multiplier`
- Melee consecutive-attack count: `1 + (vanilla - 1) * multiplier`, rounded to a whole attack count
- Constant descriptor rows: intentionally unchanged
- Reduction factors: clamped at `0.0` by default to prevent negative internal factors

INI multipliers are clamped to the range `0.0` through `10.0`.

## Installation

Copy the files from `release/` into the folder from which your DS2 ASIs are loaded, normally next to `DS2.exe`.

A working ASI loader is required.

Restart the game after editing the INI. The patch is applied once after the proficiency registry initializes.

## Safety model

Before any write, the mod verifies:

- PE timestamp and image size for the supported DS2 build
- all 13 known proficiency descriptors
- exact thresholds
- exact vanilla effect arrays

If any verification fails, the patch is aborted.

Non-DS2 helper/launcher processes are ignored silently. If `DS2.exe` itself has an unsupported build identity, the mod writes an unsupported-build status to `ds2_proficiency_bonus_multiplier.log` and performs no changes.

## Build

The project intentionally avoids the CRT and links only the required Kernel32 imports. The supplied `src/build_windows.cmd` expects LLVM/Clang for Windows, either in `C:\Program Files\LLVM\bin` or available on `PATH`.

Build output: `build/DS2_ProficiencyBonusMultiplier.asi`

## Repository layout

- `src/` - C++ source, import definition and Windows build script
- `release/` - ready-to-use ASI, INI and README
- `docs/` - Nexus copy-paste description and technical notes
- `CHANGELOG.md` - release history

## Compatibility

Do not load older ProficiencyBonus development/proof/mapping ASIs together with the release build. Other mods that alter the same proficiency descriptor tables can trigger the strict validation abort.
