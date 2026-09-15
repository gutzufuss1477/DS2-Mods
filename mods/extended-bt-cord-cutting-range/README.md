# DS2 Extended BT Cord Cutting Range v1.0.0

Extends the distance at which Sam can cut a Gazer's umbilical cord with the Blood Boomerang in **DEATH STRANDING 2: ON THE BEACH** on PC.

The mod changes the confirmed `DSPlayerCodeCutWithBoomerangState` distance value while keeping the game's original target, state, stealth and failure logic intact.

## Confirmed behavior

The vanilla internal Cord-Cut length is `4.0`.

The configured multiplier is applied to that value before the game's original helper and distance comparison:

- `1.0` = vanilla
- `2.0` = 2x vanilla
- `5.0` = 5x vanilla
- `10.0` = 10x vanilla

Version 1.0.0 was confirmed in game with `Multiplier=5.0`. The Cord-Cut prompt became available substantially earlier while the game remained stable.

## Installation

Copy these two files from the release archive into the same location used by your other DS2 ASI mods:

- `ds2_extended_bt_cord_cutting_range_v100.asi`
- `ds2_extended_bt_cord_cutting_range.ini`

Remove older development/test versions of this mod first and fully restart the game.

## Configuration

```ini
[Range]
Enabled=1
Multiplier=5.0
```

| Setting | Accepted value | Description |
|---|---:|---|
| `Enabled` | `0` or `1` | Enables or disables the range patch. |
| `Multiplier` | `1.0` to `50.0` | Multiplies the vanilla Cord-Cut distance value. |

The default `5.0` is the in-game tested value.

## Technical scope

The mod hooks the confirmed distance path in `DSPlayerCodeCutWithBoomerangState`.

The original game sequence loads `mCodeCutWithBoomerangLength`, passes it through the game's helper and compares it against the current distance. The mod multiplies only the length input before that helper.

The original distance failure branch remains intact.

The mod does **not** bypass:

- valid Gazer target checks
- stealth/state requirements
- normal Cord-Cut action logic
- original failure/control-flow logic

## Log

The mod creates:

`ds2_extended_bt_cord_cutting_range.log`

A successful load contains:

`status=patched`

## Build

Run:

```powershell
.\scripts\build-llvm.ps1
```

The build output is written to `build/public/`.

## Testing status

Confirmed before v1.0.0 release:

- Save loads normally.
- Gameplay remains responsive.
- `Multiplier=5.0` substantially increases the usable Cord-Cut distance.
- The original distance failure branch remains active.
