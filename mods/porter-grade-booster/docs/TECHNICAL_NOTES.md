# Technical notes

## Runtime strategy

The released ASI uses the exact native detour implementation that was validated in-game.

It dynamically scans for the confirmed 64-bit stat-mutator function using an AOB signature and installs
a small x64 trampoline only when the signature resolves uniquely.

The hook then applies strict filters:

1. The call must originate from the confirmed delivery Porter Grade caller.
2. The stat ID must be in the range 0..4.
3. The original delivery gain must be positive.
4. The category must be enabled in the INI.

Only the `R8` add amount is changed. The original DS2 stat mutator then continues normally.

## Modes

Multiplier:
`boosted = round(original * Multiplier)`

Additive:
`boosted = original + ExtraPoints`

Both:
`boosted = round(original * Multiplier) + ExtraPoints`

The final value is clamped to the cap supplied by DS2.

## Why zero categories stay zero

The hook deliberately ignores calls with an original amount of zero.
This preserves DS2's category assignment logic and prevents unrelated categories from gaining progress.

## Independence from Lost Cargo Likes Booster

Porter Grade Booster operates on the Porter Grade 64-bit stat-add path.
Lost Cargo Likes Booster operates on Facility Likes / Connection progression.

They do not need a shared hook or common core and are intentionally distributed as separate ASI files.
