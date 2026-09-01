# Changelog

## v1.0.0

- Promoted the acknowledged native level state machine to stable after gameplay
  confirmation.
- Confirmed ordinary construction promotion, including Transponder/Fast Travel
  Station and Cargo Catapult, with one native `1 -> Max (3)` upgrade.
- Confirmed visible Safe House/Bunker and normal bridge construction followed
  by exactly two native upgrades: `1 -> Completion (2) -> Max (4)`.
- Made detailed foundation `DIAG point=0/1` output opt-in through
  `DebugLog=1`; the production default remains `DebugLog=0`.

## TEST v0.1.5

- Kept foundation tracking across equal-current native `SetLevel(1)` refreshes.
- Kept the candidate marker across the exact lifecycle-2 material normalization
  to level 1.
- Added bounded entry and post-native-Update foundation diagnostics.

## TEST v0.1.4

- Matched the native foundation timer rule, allowing build-pending after the
  native `0.1 s` dwell while retaining all other gates.

## TEST v0.1.3

- Replaced the integer ladder with direct native milestone requests:
  `1 -> Max` for ordinary constructions and
  `1 -> Completion -> Max` for foundation constructions.

## TEST v0.1.2

- Serialized native level requests behind complete acknowledgement to prevent
  overlapping construction transactions.

## TEST v0.1.1 — withdrawn

- Attempted a synchronous integer-level ladder.
- Withdrawn because asynchronous native acknowledgement could leave an
  invisible object and a stuck construction transaction.
