# Changelog

## v1.1.1
- Prevented an unmatched Connection event from leaving the bonus permanently armed and blocking later Lost Cargo deliveries.
- Replaced stale or conflicting pending transactions when a new eligible reward is detected.
- Replaced the broad `RBX > 1` classification with confirmation that consecutive reward components belong to the same Reward entry and Cargo source; this excludes the traced Standard Order path.
- Relaxed Connection synchronization to tolerate later internal Like components while still adding exactly the configured bonus.
- Always clears a matched pending transaction, including failed validation/write attempts, so one unusual delivery cannot disable later bonuses.

## v1.1.0
- Fixed the configured Lost Cargo Like bonus not producing equivalent Facility star/connection progress.
- The same bonus is now synchronized into the matched Facility Connection Points.
- Changed bonus application to once per detected Lost Cargo reward transaction, preventing multiple bonuses from the internal reward-component sequence.
- Preserved variable base Like support introduced after v1.0.0.
- The mod does not force a star level; DS2 continues to evaluate its own connection thresholds.

## v1.0.1
- Fixed Lost Cargo with different base Like rewards not receiving the configured bonus.
- Removed the hard-coded `RBX == 0x22` (34 Likes) requirement.
- Kept the known regular-order `RBX == 1` signature excluded.

## v1.0.0
- Initial release.
