# Changelog

## 1.0.0 - 2026-08-28

- Promoted the tested Steam PC `1.10.89.0` build to the first stable release.
- Confirmed firearm and grenade use inside a facility/building weapons-prohibited area.
- Retained the isolated one-byte enforcement patch, fail-closed target checks and separate native warning-presentation path.
- Clarified the validated runtime scope and remaining regression matrix.

## 0.1.0-test

- Added the first Steam PC `1.10.89.0` test build.
- Isolated and bypassed only enforcement return reason `6` derived from `DSWeaponParameter::IsApplyWeaponsProhibitedArea`.
- Preserved the game's separate warning-ring, sound and announcement systems.
- Added fail-closed build/signature checks, optional startup logging and offline validation tooling.
