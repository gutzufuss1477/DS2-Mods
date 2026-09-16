# Technical notes - v1.0.0

## Runtime model

The release is a one-shot data patch. It does not install a persistent code detour, INT3 hook or polling loop after the proficiency descriptor registry becomes available.

The target DS2 build is identified by:

- PE timestamp: `0x6A3DAE46`
- image size: `0x0B292000`
- known game version: `DS2.exe 1.10.89.0`

## Verified descriptor set

The runtime proficiency manager exposes 13 verified descriptors. Twelve correspond to visible Sam's Stats categories; one additional internal stealth-takedown descriptor is kept disabled in the public INI by default.

Confirmed firearm identity mapping used by the release:

- Handguns - descriptor id 8 / stat field `0x49`
- Assault Rifles - descriptor id 5 / stat field `0x43`
- Sniper Rifles - descriptor id 7 / stat field `0x47`
- Machine Guns - descriptor id 4 / stat field `0x41`
- Shotguns - descriptor id 6 / stat field `0x45`

The firearm descriptor arrays were runtime-verified against Sam's Stats. Visible rows include reload speed, reticle movement and recoil. A fourth internal row uses the same level-dependent reduction table and is scaled together with the category.

## Melee mapping

The two level-dependent Melee Combat rows were runtime-mapped to:

1. Attack Power
2. Consecutive Attacks

Vanilla Consecutive Attacks values are `3 / 3 / 4 / 4 / 5`. The release scales the count around neutral count 1 and rounds to the nearest whole attack count.

## Carrying Capacity

Only the level-varying additive row `0 / 10 / 15 / 20 / 30` is scaled. The remaining descriptor rows are constant across all five proficiency levels and are intentionally left unchanged by a per-level multiplier mod.

## Reduction clamp

Large multipliers can mathematically push factor-style reductions below zero. Example: a vanilla factor `0.5` with a `6.0x` bonus magnitude would become `-2.0` under the raw formula. The default safety policy clamps these factors at `0.0`, corresponding to a maximum displayed reduction of `-100%`.

The clamp can be disabled in the INI for deliberate experiments, but is not recommended for normal play.
