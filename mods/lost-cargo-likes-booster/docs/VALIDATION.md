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

## v1.1.1 transaction classification and regression test

Diagnostic comparison observed a Standard Order using separate Reward entries for components `276 + 200`, while the compared Lost Cargo transaction reused the same Reward entry and Cargo source for components `25 + 25 + 12`. v1.1.1 requires this repeated Reward/Cargo correlation before arming the bonus.

The release candidate was tested in game with the following sequence:

1. At the first facility, a Standard Order received only its normal Likes and minimal normal star progress.
2. Lost Cargo delivered immediately afterward received the configured bonus and the corresponding larger star/Connection increase.
3. At a second facility, another Standard Order again received no configured bonus.
4. Lost Cargo delivered afterward again received the configured bonus and corresponding star/Connection progress.

This confirms Standard Orders are excluded, Lost Cargo is boosted once, Connection progress follows the bonus, and transaction state does not leak across the tested facility transition. Longer multi-delivery and save-reload regression testing remains recommended.
