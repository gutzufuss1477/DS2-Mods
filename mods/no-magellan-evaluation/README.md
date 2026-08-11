# No Magellan Evaluation v1.0.0

Removes the Like reward penalty from DHV Magellan deliveries in **DEATH STRANDING 2: ON THE BEACH** while leaving the game's original `DHV-Magellan-Lieferung` / Magellan delivery indicator intact.

## What it does

When DS2 marks an order as transported by the DHV Magellan, the game normally selects a reduced reward multiplier for the affected order-reward component. This mod normalizes that multiplier lookup back to the same selector used by the equivalent non-Magellan delivery.

It does **not** add a fixed Like amount, multiply the final Like total, edit cargo/save records, or remove the Magellan delivery label from the results screen.

## Validated target

- Platform: Steam PC
- Game build: `1.10.89.0`
- PE timestamp: `0x6A3DAE46`
- Image size: `0x0B292000`
- `DS2.exe` SHA-256 used by the existing validated DS2 mod baseline: `BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B`
- v1.0.0 release ASI SHA-256: `3EC72DA55493471470246C39179D8F56154D149409A95FF15F200FA0D5054FE3`

The code path is build-specific. The ASI refuses to install its runtime hook if the expected build metadata or instruction signature does not match.

## Installation

1. Install an external x64 ASI loader for DS2 (not included).
2. Copy `ds2_no_magellan_evaluation.asi` and `ds2_no_magellan_evaluation.ini` into the DS2 game directory used by the ASI loader.
3. Restart the game.

Uninstall by removing both files and restarting the game.

## Configuration

```ini
[NoMagellanEvaluation]
Enabled=1
DebugLog=0
```

- `Enabled=1` enables the mod. Set it to `0` to disable the hook.
- `DebugLog=1` creates `ds2_no_magellan_evaluation.log` beside the ASI. Logging is off by default.

## Validation result

A prepared A/B test used the same standard order and equivalent delivery state:

- Normal delivery: affected order reward = **81 Likes**
- Unmodified Magellan delivery baseline: **65 Likes**
- Magellan delivery with the validated fix: **81 Likes**
- Both validated final result screens showed **381 total Likes** (`81 + 300 additional Likes`).

The Magellan delivery label remains visible by design; only the reward penalty is neutralized.

See `docs/VALIDATION.md` and `SECURITY_NOTES.md` for the technical boundary.

## Build

Run `scripts/build-llvm.ps1` from PowerShell with LLVM for Windows installed. Output is written to `build/public/`.

See `BUILD.md` for details.
