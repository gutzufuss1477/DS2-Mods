# Lost Cargo Likes Booster v1.0.1

Adds configurable **Facility Likes** for each delivered item of lost cargo in DEATH STRANDING 2: ON THE BEACH (Steam PC build 1.10.89.0).

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

The plugin remains active until game exit. It has no time limit or hit limit. It is guarded to the validated DS2 build and lost-cargo writer path; the known regular-order signature is excluded.

## v1.0.1

- Fixed Lost Cargo with different base Like rewards not receiving the configured bonus.
- The booster no longer requires an exact 34-Like base component.
- Variable Lost Cargo reward components of 2 or more are accepted on the validated writer/caller path.
- The known regular-order `RBX=1` signature remains excluded.

## Build and verification

Run `scripts/build-llvm.ps1` from PowerShell with LLVM installed. The script builds the checked-in `src/freight_likes_booster.cpp` and writes `build/public/`, including the PE report and SHA-256. The public binary hash is recorded in `SHA256SUMS.txt`.

See `docs/VALIDATION.md` and `SECURITY_NOTES.md` for the validation boundary and safety notes.
