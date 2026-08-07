# Changelog

## v1.0.1
- Fixed Lost Cargo with different base Like rewards not receiving the configured bonus.
- Removed the hard-coded `RBX == 0x22` (34 Likes) requirement.
- Lost Cargo reward components of 2 or more can now receive the configured bonus on the validated writer/caller path.
- Kept `RBX == 1` excluded because it is the known regular-order signature from v1.0.0 validation.
- Preserved the DS2 build guard, caller guard, `R8 == 0`, record validation, overflow checks and duplicate guard.

## v1.0.0
- Initial release.
