# Changelog

## 1.1.0-dev.1 — unreleased development build

- Integrate the in-game-confirmed footprint filter into the existing Sam Overhaul ASI.
- Add optional `[Footprints] HideFootprints=0/1` to `ds2_sam_overhaul.ini`; default and missing-key fallback are 0.
- Hide normal and scan-highlighted footprints, including their reconstruction when loading saved worlds.
- Preserve the tested resource matching and complete native argument forwarding; unrelated decals pass through.
- Add executable SHA-256/prologue guards, standalone-probe conflict detection and limited diagnostics in the existing Sam Overhaul log.
- Preserve all 33 previous INI defaults and the existing cargo, movement, Autodrive and truck-weapon code paths.
- Add footprint configuration/ABI/concurrency, combined-ASI loading and binary-integrity tests; retain the Autodrive regression suite.
- Git development build only: no Nexus release, release tag, Mod Suite package update or change to the v1.0.0 release archive.
- Combined in-game regression remains pending; the footprint core was gameplay-validated separately.

## 1.0.0 - 2026-09-30

First public release of Sam Overhaul.

- Added configurable hiding for shoulder cargo, hip cargo, backpack cargo and spare shoes.
- Keeps currently worn shoes visible and leaves inventory, weight and cargo ownership native.
- Added monorail dismount beyond the final vanilla safety/height blocker while preserving earlier state checks.
- Added wider zipline dismount availability using the native input and exit animation.
- Added qualifying landing rolls with backpack/cargo while preserving native landing thresholds and fall-damage logic.
- Added configurable Autodrive activation time with a tested 2.0-second default.
- Added independent Heavy Machine Gun tuning: range, aim speed, lock-on, burst duration and cooldown.
- Added independent Mortar tuning: range, aim speed, lock-on, recovery and cooldown.
- Added independent Chiral Cannon tuning: range, long-range profile, aim speed, lock-on, recovery, cooldown and charge time.
- Chiral Cannon default charge reduced from 3.0 seconds to 0.75 seconds after live A/B testing.
- Added independent Missile Launcher tuning: range, aim speed, lock-on, recovery and cooldown.
- Removed unsuccessful footprint/worn-path experiments from the public release.
- Added strict target-build/context validation and local startup/configuration logging.
- Final build passes the existing 105,704-check Autodrive regression suite.
