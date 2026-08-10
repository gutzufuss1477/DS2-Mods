# Porter Grade Booster

Configurable Porter Grade progression booster for **DEATH STRANDING 2: ON THE BEACH**.

The mod boosts the category points that DS2 already awards when a delivery is completed.
It does **not** directly set category levels, Porter Class, or maximum rank.

## Features

- `Multiplier` mode: `original gain × multiplier`
- `Additive` mode: `original gain + fixed extra points`
- `Both` mode: combines both
- Per-category switches for:
  - Porter
  - Combat
  - Stealth
  - Servicemanship
  - Bridge Link
- Categories with a native gain of `0` remain `0`
- DS2 keeps control of level-ups, Porter Class progression and rewards

## Install

1. Install an external x64 ASI loader (not included).
2. Copy `reference/PorterGradeBooster.asi` and `config/PorterGradeBooster.ini` beside the loader in the DS2 game/mod directory.
3. Restart the game after editing the INI.

## Configuration

```ini
[PorterGradeBooster]
Enabled=1

Mode=Multiplier
Multiplier=2.0
ExtraPoints=0

Porter=1
Combat=1
Stealth=1
Service=1
BridgeLink=1
```

### Modes

- `Mode=Multiplier`: `Final = Original × Multiplier`
- `Mode=Additive`: `Final = Original + ExtraPoints`
- `Mode=Both`: `Final = (Original × Multiplier) + ExtraPoints`

The booster only acts on categories for which the delivery already grants a positive amount.

## Confirmed category mapping

| Stat ID | Category |
|---:|---|
| 0 | Porter |
| 1 | Combat |
| 2 | Stealth |
| 3 | Servicemanship |
| 4 | Bridge Link |

## Validation

Repeatable Standard Order baseline:

| Category | Vanilla | 2x | 10x |
|---|---:|---:|---:|
| Porter | +276 | +552 | +2760 |
| Combat | +200 | +400 | +2000 |
| Stealth | +0 | +0 | +0 |
| Servicemanship | +126 | +252 | +1260 |
| Bridge Link | +110 | +220 | +1100 |

DS2 correctly handled category level-ups and overall Porter Class increases.

The mod was also tested **in parallel with Lost Cargo Likes Booster** by submitting a Standard Order
and Lost Cargo together. Both Facility/Connection progression and Porter Grade progression completed normally.

See `docs/VALIDATION.md` and `docs/TECHNICAL_NOTES.md`.

## Build

Run:

```powershell
./scripts/build-llvm.ps1
```

with LLVM/Clang and `lld-link` available.

The reference binary in `reference/PorterGradeBooster.asi` is the exact binary validated in-game.

### Reference ASI SHA-256

`7ad9d021e0f7d1e61ca50c9f40d803954b49083a88c31335a18a34be8f0e2213`

## Compatibility

`PorterGradeBooster.asi` and `Lost Cargo Likes Booster` are separate mods and may be loaded simultaneously.

## Version

`v1.0.0`
