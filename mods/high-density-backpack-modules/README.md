# High-Density Backpack Modules v1.1.0


More backpack equipment, cleaner visuals and eight accessory slots for **DEATH STRANDING 2: ON THE BEACH**.

## Features

- Up to **30 functional modules**, each using one logical grid cell, independent of its original footprint.
- Occupied placements automatically move to a nearby free cell. At 30 modules, the native replacement dialog still applies.
- Visible modules keep their original size and are packed into a separate layout. Excess models are hidden in backpack customization, cargo management and normal gameplay; their equipment entries remain installed.
- **Eight accessory slots:** the last two rows display physical charms; the first six are invisible equipment slots. Equip only charms you have unlocked. Unlocking a charm does not equip it automatically.
- Equipped additional charms are included in effect checks. Native module-specific effect limits remain; not every charm effect or combination has been measured in game.

## Installation and updating

1. Close the game and install a compatible external 64-bit ASI loader if needed.
2. Remove previous High-Density Backpack ASI versions from every active loader folder. **Keep exactly one Backpack ASI active.** Multiple versions can cause an older build to load instead.
3. Copy `DS2_HighDensityBackpackModules_v1.1.0.asi` beside `DS2.exe`.
4. Keep any existing `DS2_HighDensityBackpackModules.charms.ini` when updating from a test build.
5. Start the game. `DS2_HighDensityBackpackModules.log` should identify **v1.1.0** and contain `status=PATCH_APPLIED`.

The download contains one ASI only; no loader, installer or configuration file is bundled. The six additional charm selections are saved automatically on leaving the customization menu to the INI beside the game executable. They are installation-wide preferences shared across saves and filtered by current unlock availability. The two visible charms use the normal game save. Additional slots start empty.

## Known limitations

- Clean module rendering in gameplay, customization and cargo management was confirmed by the user, including the reported existing-save load case. The layout uses native footprints; unusual geometry and animations may still clip.
- The module limit is 30, not unlimited. The visible subset changes with the installed equipment and available visual space.
- Seven currently unlocked charms were equipped together in the user's game test; the eighth slot is available when its charm is unlocked. All eight menu rows are covered by local tests.
- Actual charm effect magnitudes, full restart persistence of extra slots and every menu entry/exit route have not all been validated in game. See [validation](docs/VALIDATION.md).

## Compatibility

Supported executable: **DS2.exe v1.10.89.0** (PC / Steam).

SHA-256: `BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B`.

The mod checks executable metadata and instruction/data signatures before patching and remains inactive on a mismatch. It modifies the running process, not the executable on disk. Compatibility with other mods that patch the same functions is not established.

## Removal

With the mod still active, remove additional charms and reduce the modules to a layout that fits their original footprints. Apply the changes and save, then close the game and remove the ASI. The INI and log may also be removed.

## Source and verification

[Technical notes](docs/TECHNICAL_NOTES.md), [changelog](CHANGELOG.md), [German instructions](README_DE.md).

Build with `scripts/build-llvm.ps1` or `build.sh`. Run `tests/run.ps1`, then `tests/verify_baseline.py path/to/DS2.exe` and `tests/native_menu.py build/runtime_tests.dll path/to/DS2.exe`. Package with `scripts/package-release.py`.

Tests include original native menu and Cryptobiosis instructions, patch/branch checks and isolated fixtures for placement, visibility, effects and state-file handling. They do not replace an in-game test. Historical v1.0.0 artifacts are retained separately.

No separate open-source licence is granted unless explicitly stated otherwise.
