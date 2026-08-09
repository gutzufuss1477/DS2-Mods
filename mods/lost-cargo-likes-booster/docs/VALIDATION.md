# Validation

Validated target: Steam PC DS2 build 1.10.89.0, timestamp `0x6A3DAE46`, image size `0x0B292000`.

## v1.0.0 baseline

- The original tested normal Lost Cargo reward contained internal Like components `34 + 17 + 12` and normally awarded `+63`.
- A regular-order comparison had `RBX=1`.

## Variable base rewards

- A later Lost Cargo case used different internal base components and demonstrated that a fixed `RBX == 0x22` guard was too narrow.
- The Like-side guard was therefore generalized to `RBX > 1` on the same validated writer/caller path, while keeping the known regular-order `RBX=1` case excluded.

## v1.1.0 Connection synchronization

The diagnostic probe demonstrated that increasing the visible Facility Like total alone did not equivalently increase Facility star progression.

A validated Connection-Sync test then observed:

- Lost Cargo vanilla reward total for the transaction: `128` Likes.
- Connection Points before synchronization: `4677`.
- Configured `ExtraLikes`: `5000`.
- Connection Points after synchronization: `9677`.
- Visible Facility Likes after synchronization: `9677`.
- Connection level before the update: `4`.
- Next observed Connection threshold: `6200`.
- In-game result: the facility progressed to 5 stars.

The test also confirmed the structural correlation between the Like record and Connection record: `LikeRecord == ConnectionRecord + 0x18`.

v1.1.0 therefore synchronizes the configured bonus only when the matching Like/Connection records and expected totals agree. It does not directly force a star level.

## Production binary

The release binary is PE32+ x86-64, exports `DllMain`, and imports only KERNEL32.dll functions required by the hook/configuration implementation.

## v1.1.1 candidate regression tests

The source candidate adds recovery for stale or conflicting pending reward state. Before release, validate all of the following in game:

Diagnostic comparison observed a Standard Order using separate Reward entries for components `276 + 200`, while the compared Lost Cargo transaction reused the same Reward entry and Cargo source for components `25 + 25 + 12`. The candidate now requires this repeated Reward/Cargo correlation before arming the bonus.

1. Return at least 10 Lost Cargo deliveries without reloading and confirm every delivery receives exactly one bonus.
2. Alternate Standard Orders and Lost Cargo deliveries and confirm Standard Orders neither receive the bonus nor disable it for the next Lost Cargo delivery.
3. Confirm Connection/star progress receives the configured bonus for every tested Lost Cargo delivery, including multi-component and fragile-marked rewards.
4. Exercise two facilities in succession to confirm a changed Facility record cannot leave the previous transaction armed.
5. Reload a save with and without a pending reward screen and confirm the next Lost Cargo delivery behaves normally.

This section is a required test plan, not a claim of completed in-game validation.
