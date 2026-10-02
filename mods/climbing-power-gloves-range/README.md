# DS2 Climbing Power Gloves Range v1.1.1

Climbing Power Gloves Range extends magnetic cargo pickup in DEATH STRANDING 2: ON THE BEACH on PC.

The original feature remains unchanged:

- Climbing Power Gloves Level 1: native 8 m, configurable default 30 m
- Climbing Power Gloves Level 2: native 10 m, configurable default 50 m

Version 1.1.0 also adds optional magnetic remote cargo pickup to Combat Power Gloves.

Version 1.1.1 fixes right-side cargo being pulled toward Sam without being stowed while using a vehicle. The native catch transition was confirmed live on both sides from a truck and also from a bike on 2026-10-02. The installed v1.1.1 ASI was also confirmed after a full restart: both vehicle sides stow cargo correctly.

## Requirements

- Steam PC version DS2.exe 1.10.89.0
- EXE SHA-256: BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B
- Compatible external 64-bit ASI loader

Unsupported game builds are rejected.

## Installation

Copy ds2_climbing_gloves_range.asi and ds2_climbing_gloves_range.ini beside DS2.exe, then fully restart the game.

To uninstall, remove both files and restart. The mod does not modify save files, DS2.exe, or game archives on disk.

## Configuration

ClimbingGlovesRange:
- Enabled=1
- Level1RangeMeters=30
- Level2RangeMeters=50
- DebugLog=0

CombatGloves:
- EnableCargoPickup=1

Combat Power Gloves do not have separate range values in this release. Their native seven combat parameters remain unchanged.

## Technical scope

Climbing Power Gloves:
- Item ID 53 / Level 1: native Params[6] = 8.0
- Item ID 54 / Level 2: native Params[6] = 10.0

Combat Power Gloves:
- Item IDs 55 and 56 are validated explicitly.
- Their seven native parameters are preserved.
- Additional pickup metadata is stored in a private shadow parameter array.
- Hooks extend glove action-cost, action-availability, and vehicle pickup handling.
- Native boolean results are handled as the game's actual 8-bit AL result.
- A fresh right-hand vehicle handover stages the native cargo candidate and selects CatchBaggage state 3. Expired handovers, active actions, occupied hands, and non-vehicle states are rejected. Native code performs the actual transfer and cleanup.
- The unsuccessful right-to-left remap and forced guard/hold-breath catch experiments are not used by the fix.

Normal hand pickup, Sticky Gun, Sticky Cannon, saves, game archives, and unrelated glove behavior are not modified.

## Compatibility and fail-safe behavior

The mod validates the executable build, item-system structures, item identities, native parameter values, and exact code signatures used by the Combat extension. Unknown or conflicting values fail closed and are logged instead of being overwritten.

## Testing status

Confirmed in game for v1.1.0:

- Combat Power Gloves can remotely pick up loose cargo beyond normal hand-pickup distance.
- Executing the pickup works without crashing.
- Combat Power Gloves can still perform their normal strike/attack behavior.
- Climbing Power Gloves continue to pick up cargo remotely.
- The original 30 m / 50 m Climbing range behavior remains intact.

For German documentation, see README_DE.md.

## Build and checks

Use `scripts/package-release.ps1` with LLVM, or `scripts/package-release.ps1 -Toolchain MSVC` with Visual Studio C++ Build Tools. Both run the synthetic tests before packaging. The new catch tests cover native fallback, duplicate candidates, expired/invalid TTLs, occupied hands, non-combat equipment, on-foot pickup, and read-only memory.
