# DS2 Climbing Power Gloves Range v1.1.0

Climbing Power Gloves Range extends magnetic cargo pickup in DEATH STRANDING 2: ON THE BEACH on PC.

The original feature remains unchanged:

- Climbing Power Gloves Level 1: native 8 m, configurable default 30 m
- Climbing Power Gloves Level 2: native 10 m, configurable default 50 m

Version 1.1.0 also adds optional magnetic remote cargo pickup to Combat Power Gloves.

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
- Two narrowly scoped hooks extend glove action-cost and action-availability handling for cargo pickup action 6.
- Native boolean results are handled as the game's actual 8-bit AL result.
- The experimental state/update bypass used during development is not included.

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
