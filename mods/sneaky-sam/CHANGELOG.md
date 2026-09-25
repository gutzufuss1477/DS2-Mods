# Changelog

## v1.0.1

- Rebuilt with Microsoft Visual C++ and the Windows SDK to avoid a Nexus archive-scanner false positive.
- Added a standard Windows version resource.
- Added explicit ASI/version exports.
- Relay memory now transitions from RW to RX instead of being allocated RWX.
- Gameplay and visibility behaviour are unchanged from v1.0.0.
- Nexus archive scan accepted the rebuilt package.

## v1.0.0

- Initial public release.
- Hides cargo carried on both shoulders and both hips.
- Keeps cargo inventory, weight, condition and slot state unchanged.
- Cargo becomes visible normally again when moved to the backpack or another non-target slot.
- Uses the game's native baggage visibility path.
- Validated on DS2.exe v1.10.89.0.
