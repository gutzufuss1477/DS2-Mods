# DS2 Remote Orders Overlay

A read-only world-map overlay for **DEATH STRANDING 2: ON THE BEACH**.

Hover over or select a facility on the world map to see available orders associated with that facility. The overlay is an interim solution while direct activation of the native **Accept Orders** menu from the world map remains under research.

## Current release

- Version: `v0.2.0`
- Supported game: Steam `DS2.exe v1.10.89.0`
- Supported executable SHA-256: `BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B`
- Runtime status: tested successfully in-game with German, French, Italian and Spanish UI configurations
- Logging: disabled; no Remote Orders log file is created
- Release ASI SHA-256: `02C3046773001DC6499B6B670C6DD0903F37F29EF5D19F1DB4A8DB1F2F183821`

## Features

- Shows up to ten available orders in a compact two-column overlay.
- Uses DS2's native MissionMenu display-data builder for localized text.
- Localizes the overlay UI in all 18 interface languages supported by the Steam release.
- Supports an INI language override when the game and Windows use different languages.
- Removes repeated native facility-name variants caused by capitalization, apostrophe and invisible-format differences.
- Tracks the facility selected on the world map.
- Displays the facility number used by the game.
- Click-through and non-activating Win32 overlay.
- Read-only: does not accept, cancel, start or complete orders.
- Does not intentionally change save data, mission state or facility state.
- Compatible by design with Hill Assist and Truck/Vehicle mods because it operates on separate game systems.

## Installation

1. Close the game.
2. Install one working x64 ASI loader that loads `.asi` files beside `DS2.exe`.
3. Remove older Remote Orders Overlay ASIs.
4. Copy `release/DS2_Remote_Orders_Overlay.asi` and `release/DS2_Remote_Orders_Overlay.ini` beside `DS2.exe`.
5. Start the game and open the world map.

Use only one ASI loader DLL. Multiple `.asi` plugins may share the same loader.

`Language=auto` uses the Windows display language. If the game uses a different language, set the matching language code in `DS2_Remote_Orders_Overlay.ini`, for example `fr`, `it`, `es-ES` or `es-419`.

## Source and release provenance

Version v0.2.0 is compiled from the reviewed C++ source and has passed in-game localization smoke tests. Build it with:

```powershell
.\scripts\build_source_release.ps1
```

The legacy v0.1.3-alpha no-log release remains reproducible from the known-working baseline ASI with:

```powershell
.\scripts\build_release.ps1
```

That legacy script only replaces ten UTF-16 log paths with the Windows `NUL` device and preserves the baseline executable code unchanged.

## Repository layout

- `src/` — v0.2.0 production source
- `validated_baseline/` — exact working v0.1.2 baseline retained for legacy reproduction
- `tools/patch_no_log.py` — reviewed data-only legacy release patch
- `scripts/` — source-release and legacy reproduction scripts
- `release/` — current ASI, language configuration and Nexus-ready ZIP
- `docs/` — security, verification and Nexus documentation

## Native-menu goal

The final goal remains opening the native **Accept Orders** menu directly from the world map. The current overlay has already confirmed reusable MissionSource and MissionMenu-controller paths documented in `docs/NATIVE_MENU_RESEARCH.md`.

## License

The source is published for transparency and security review. See `LICENSE`.
