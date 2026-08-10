# Validation

## Confirmed data path

The analysis identified the additive 64-bit stat mutator used by delivery Porter Grade progression.

Confirmed Porter Grade stat IDs:

- 0 = Porter
- 1 = Combat
- 2 = Stealth
- 3 = Servicemanship
- 4 = Bridge Link

For a repeatable Standard Order, the mutator received exactly the same values shown by the result screen:

- Porter: 276
- Combat: 200
- Stealth: no call / 0
- Servicemanship: 126
- Bridge Link: 110

All non-zero category additions came from the same confirmed delivery caller.

## x2 validation

With a 2.0 multiplier:

- Porter: 276 -> 552
- Combat: 200 -> 400
- Servicemanship: 126 -> 252
- Bridge Link: 110 -> 220
- Stealth remained 0

The in-game Porter Grade result screen matched these values exactly and DS2 processed the resulting
category/Porter Class progression normally.

## x10 validation

With a 10.0 multiplier:

- Porter: 276 -> 2760
- Combat: 200 -> 2000
- Servicemanship: 126 -> 1260
- Bridge Link: 110 -> 1100
- Stealth remained 0

Multiple level-ups were processed normally.

## Parallel compatibility validation

Lost Cargo Likes Booster and Porter Grade Booster were loaded together.

A Standard Order and Lost Cargo were submitted in a single delivery sequence.

Observed:

- Lost Cargo Facility Likes / connection progression completed.
- The result advanced normally to the Porter Grade screen.
- Porter Grade values were boosted normally.
- No conflict was observed between the two ASI mods.

This validates the intended design: both mods stay separate and may be installed together.
