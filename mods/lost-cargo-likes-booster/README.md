# Lost Cargo Likes Booster v1.1.1

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

## v1.1.1

- Prevents an unmatched reward transaction from disabling bonuses for later Lost Cargo deliveries.
- Distinguishes Lost Cargo from Standard Orders through repeated Reward-entry and Cargo-source correlation instead of a broad Like-component threshold.
- Keeps Standard Order Likes and Connection progress unchanged.
- Synchronizes the configured bonus to the matching Lost Cargo Connection Points exactly once.
- Supports consecutive deliveries at different facilities without carrying stale transaction state forward.

## Build and verification

Run `scripts/build-llvm.ps1` from PowerShell with LLVM installed. The script builds the checked-in `src/freight_likes_booster.cpp` and writes `build/public/`, including the PE report and SHA-256.

See `docs/VALIDATION.md` and `SECURITY_NOTES.md` for the validation boundary and safety notes.
