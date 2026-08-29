# Changelog

## 1.0.0 - 2026-08-29

- First public release with INI-configurable chiral-bandwidth costs for eleven construction types.
- Changed every construction label and comment in the packaged INI to English.
- Confirmed an exact custom cost of `5` in the live placement HUD for Generator and Zipline on Steam PC `DS2.exe 1.10.89.0`.
- Deliberately excluded the Transponder because it is a tool with `NeedWithinQpid=false` and therefore has no reliable bandwidth charge to tune.
- Added `Native` as the safe default for every construction.
- Added exact Steam PC `DS2.exe 1.10.89.0` build gating.
- Added canonical construction-resource discovery, type validation, conflict detection, verified writes, and rollback requirements.
- Added reproducible LLVM build, synthetic-test, target-validator, and release-package helpers.
- Added a deterministic two-file Nexus archive with a SHA-256 sidecar.
- Documented the save-backup requirement and the risk of changing costs after structures already exist.
