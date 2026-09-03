# DS2 Climbing Power Gloves Range v1.0.0

Climbing Power Gloves Range increases the magnetic cargo pickup range of both **Climbing Power Gloves** tiers in **DEATH STRANDING 2: ON THE BEACH** on PC.

- Level 1: native `8 m`, configurable default `30 m`
- Level 2: native `10 m`, configurable default `50 m`

Both tiers have separate settings, so Level 2 can retain a meaningful range advantage. The mod is intended to reduce the need to use the Sticky Gun or Sticky Cannon for ordinary remote cargo pickup; it does not reproduce their targeting, rope, or other special functions.

Version 1.0.0 has been tested in game. With Level 2 equipped, cargo can be collected from a long distance; switching to Level 1 immediately reduces that range; removing the gloves restores the normal close-range pickup. Save/load and area-transition persistence remain useful regression checks.

For the German documentation, see [README_DE.md](README_DE.md).

## Requirements

- Steam PC version `DS2.exe 1.10.89.0`
- EXE SHA-256: `BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B`
- A compatible external 64-bit ASI loader

Other game builds are intentionally rejected. After a game update, wait for an explicitly compatible mod release before enabling it again.

## Installation

Extract these two files from the release archive into the game directory next to `DS2.exe`:

- `ds2_climbing_gloves_range.asi`
- `ds2_climbing_gloves_range.ini`

Fully restart the game after installing the mod or changing the INI.

To uninstall, remove both files and restart the game. The mod does not modify save files, `DS2.exe`, or game archives on disk.

## Configuration

The included defaults are:

```ini
[ClimbingGlovesRange]
Enabled=1
Level1RangeMeters=30
Level2RangeMeters=50
DebugLog=0
```

| Setting | Accepted value | Description |
|---|---:|---|
| `Enabled` | `0` or `1` | Enables or disables the range patch. |
| `Level1RangeMeters` | `8` to `100` | Pickup range for Level 1 gloves. |
| `Level2RangeMeters` | `10` to `100` | Pickup range for Level 2 gloves. Must not be lower than Level 1. |
| `DebugLog` | `0` or `1` | Writes additional diagnostics to `ds2_climbing_gloves_range.log`. |

Setting both ranges to the same number gives both tiers the same reach. Invalid values cause a safe abort and are reported in the log. Critical errors are logged even when `DebugLog=0`.

## Scope

The mod resolves the exact `DSItemParameter` resources for the two glove tiers and changes only their range field:

| Item | Item ID | Resource level | Native range field |
|---|---:|---:|---:|
| Climbing Power Glove Lv1 | 53 | 0 | `Params[6] = 8.0` |
| Climbing Power Glove Lv2 | 54 | 1 | `Params[6] = 10.0` |

The global hand-pickup range, other gloves, pickup speed, throw values, and `Params[7]` are not changed. Native visibility, target selection, cargo, obstruction, and state checks continue to apply. The Sticky Gun and Sticky Cannon are not modified.

The resources are resolved without a code hook through the canonical global `DSItemSystem` pointer and its validated item array. Factory and RTTI metadata are used only for static type and layout validation.

## Compatibility and fail-safe behavior

Before writing, the mod validates the executable build, `DSItemSystem` initialization and lookup, VTables, type metadata, and both resource identities. Unknown values or values already changed by another mod are treated as conflicts and are not overwritten.

Another mod that patches the same two glove parameters may therefore be incompatible. On an unsupported or modified executable, the mod remains inactive and logs the reason.

## Offline target validation

From this mod directory, run:

```powershell
python .\tools\validate_target.py "C:\Program Files (x86)\Steam\steamapps\common\DEATH STRANDING 2 - ON THE BEACH\DS2.exe"
```

The validator checks the hash, PE metadata, relevant signatures, VTables, and RVAs. Only a complete `OK` result identifies the supported target.

## Testing status

Confirmed in game for v1.0.0:

- Level 2 provides the longest pickup range.
- Switching to Level 1 reduces the pickup range immediately.
- Removing the gloves restores the normal close-range pickup.

Recommended regression coverage still includes save/load, area transitions, travel or rest, vehicle pickup, multiple cargo pieces, height differences, and obstacles. See [TEST_PLAN.md](TEST_PLAN.md) for the full checklist.
