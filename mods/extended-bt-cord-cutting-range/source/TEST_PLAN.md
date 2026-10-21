# In-Game Test Plan

Applies to `DS2 Extended BT Cord Cutting Range v1.0.0`.

## Confirmed release behavior

The following was confirmed before release:

| Configuration | Observed result |
|---|---|
| `Multiplier=5.0` | Cord-Cut becomes available substantially earlier than vanilla. |
| Save load | Normal. |
| General gameplay | Stable and responsive. |
| Original distance failure branch | Preserved. |

## 1. Baseline

1. Set `Multiplier=1.0`.
2. Fully restart the game.
3. Approach a valid Gazer in stealth and note the Cord-Cut activation distance.
4. This is the vanilla-equivalent baseline.

## 2. Default release setting

1. Set `Multiplier=5.0`.
2. Fully restart the game.
3. Approach the same type of Gazer under comparable conditions.
4. Confirm that the Cord-Cut prompt becomes available substantially earlier.

## 3. Additional multiplier checks

Test, if needed:

- `2.0`
- `3.0`
- `5.0`
- `10.0`

Large values are intentionally allowed for testing, but `5.0` is the confirmed release default.

## 4. Regression checks

- Load a save and remain in gameplay for several minutes.
- Test multiple Gazers.
- Test after area transitions.
- Test after rest/fast travel if applicable.
- Confirm no freeze, input lock, crash or persistent state issue occurs.
- Confirm normal stealth and valid-target requirements are still enforced.

## 5. Issue report checklist

Include:

- INI contents
- approximate multiplier
- whether the Cord-Cut prompt appeared
- whether the Gazer was a valid cut target
- `ds2_extended_bt_cord_cutting_range.log`
- short reproduction steps
