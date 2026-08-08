# Lost Cargo Likes Booster v1.1.0

Adds a configurable bonus when returning Lost Cargo in **DEATH STRANDING 2: ON THE BEACH** (validated Steam PC build 1.10.89.0).

Starting with v1.1.0, the configured bonus is applied to both the visible Facility Like total and the matched Facility Connection Points. This means the extra Likes now contribute to the actual star/connection progression instead of only increasing the displayed Like counter.

## Install

1. Install an external x64 ASI loader (not included).
2. Copy `ds2_lost_cargo_likes_booster.asi` and `ds2_lost_cargo_likes_booster.ini` beside the loader in the DS2 game directory.
3. Restart the game after changing the INI. Uninstall by removing both files, then restart the game.

## Configuration

```ini
[LostCargoLikesBooster]
ExtraLikes=5000
```

`ExtraLikes` accepts 1..1000000. Missing, zero, malformed, or out-of-range values use 5000.

The same configured bonus is synchronized to the Facility Connection Points when the matching Lost Cargo reward transaction is processed. The mod does **not** directly force a facility to 5 stars; DS2 still evaluates its normal star thresholds.

## v1.1.0

- Fixed extra Lost Cargo Likes not properly contributing to star/connection progression.
- Synchronizes the configured Like bonus with the matching Facility Connection Points.
- Applies the bonus once per detected Lost Cargo reward transaction instead of once for every internal Like component.
- Keeps support for Lost Cargo with different base Like reward amounts, including the previously reported fragile-marked case.
- Does not use an "instant 5 stars" level override.

## Build and verification

Run `scripts/build-llvm.ps1` from PowerShell with LLVM installed. The script builds the checked-in `src/freight_likes_booster.cpp` and writes `build/public/`, including the PE report and SHA-256.

See `docs/VALIDATION.md` and `SECURITY_NOTES.md` for the validation boundary and safety notes.
