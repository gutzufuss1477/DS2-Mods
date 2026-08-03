# DS2 Remote Orders Overlay

A read-only world-map overlay for **DEATH STRANDING 2: ON THE BEACH**.

Hover over or select a facility on the world map to see available orders associated with that facility. The overlay is an interim solution while direct activation of the native **Accept Orders** menu from the world map remains under research.

## Current release

- Version: `v0.1.3-alpha`
- Supported game: Steam `DS2.exe v1.10.89.0`
- Supported executable SHA-256: `BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B`
- Runtime status: tested successfully on the development machine and the host gaming PC
- Logging: disabled; no Remote Orders log file is created
- Release ASI SHA-256: `696D05285C23F2D73EE27C23827A27BA347C76D79DF0A84F7DB29E164DCB695C`

## Features

- Shows up to ten available orders in a compact two-column overlay.
- Uses DS2's native MissionMenu display-data builder for localized text.
- Tracks the facility selected on the world map.
- Displays the facility number used by the game.
- Click-through and non-activating Win32 overlay.
- Read-only: does not accept, cancel, start or complete orders.
- Does not intentionally change save data, mission state or facility state.
- Compatible by design with Hill Assist and Truck/Vehicle mods because it operates on separate game systems.

## Installation

1. Close the game.
2. Install one working x64 ASI loader that loads `.asi` files beside `DS2.exe`.
3. Remove older Remote Orders test ASIs.
4. Copy `release/DS2_Remote_Orders_Overlay.asi` beside `DS2.exe`.
5. Start the game and open the world map.

Use only one ASI loader DLL. Multiple `.asi` plugins may share the same loader.

## Source and release provenance

This repository contains the recovered C++ source for the functional overlay implementation and the exact runtime-tested public binary.

The original historical compiler/linker invocation was not preserved well enough to claim a byte-identical source-only rebuild. A later source cleanup build was tested and withdrawn because it prevented the game from starting.

For that reason, the public no-log release is produced conservatively from the known-working baseline ASI by a deterministic, auditable data-only patch:

```powershell
.\scripts\build_release.ps1
```

The patch only replaces ten UTF-16 log paths with the Windows `NUL` device. It verifies that `.text`, `.data` and `.pdata` remain byte-for-byte unchanged and that the final SHA-256 matches the runtime-tested release.

A separate source compilation script is included for code review:

```powershell
.\scripts\build_source_reference.ps1
```

The resulting reference build is **not** the public release and is not claimed to have passed the in-game smoke test.

## Repository layout

- `src/` — recovered production source, adjusted to represent no-log behaviour
- `validated_baseline/` — exact working v0.1.2 baseline used for deterministic release reproduction
- `tools/patch_no_log.py` — reviewed data-only release patch
- `scripts/` — release and reference-source build scripts
- `release/` — runtime-tested no-log ASI and Nexus-ready ZIP
- `docs/` — security, verification and Nexus documentation

## Native-menu goal

The final goal remains opening the native **Accept Orders** menu directly from the world map. The current overlay has already confirmed reusable MissionSource and MissionMenu-controller paths documented in `docs/NATIVE_MENU_RESEARCH.md`.

## License

The source is published for transparency and security review. See `LICENSE`.
