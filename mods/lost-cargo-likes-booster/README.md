# Lost Cargo Likes Booster

Adds a configurable number of **extra Facility Likes** once for each delivered item of lost cargo in **DEATH STRANDING 2: ON THE BEACH** (Steam PC, build 1.10.89.0 / DS2.exe timestamp `0x6A3DAE46`).

## Validated behaviour

With `ExtraLikes=5000`, a manually tested lost-cargo delivery that normally awarded `+63` awarded `+5063`. A regular-order comparison used a different validated component signature and is excluded.

## Install

1. Install an external x64 ASI loader (not included).
2. Copy `reference/freight_likes_booster_ds2_production.asi` and `config/freight_likes_booster_ds2_production.ini` beside the loader in the DS2 game directory.
3. Restart the game after editing the INI.

## Configuration

```ini
[FreightLikesBooster]
ExtraLikes=5000
```

Valid range: `1`–`1000000`. Invalid values safely use `5000`.

## Build

Run `scripts/build-llvm.ps1` in PowerShell with LLVM installed. The output is written under `build/wo049/`.

See `docs/VALIDATION.md`, `SECURITY_NOTES.md`, and `UPDATE_CHECKLIST.md`.
