# Beach Jump with Cargo

Keep Sam's carried cargo when using Beach Jump fast travel in **DEATH STRANDING 2: ON THE BEACH**.

**Version 1.0.0** · [Download ZIP](release/DS2_Beach_Jump_with_Cargo_v1.0.0.zip) · [Deutsch](README_DE.md)

## What it does

Normally, Beach Jump fast travel processes Sam's carried baggage before the jump. This mod skips that baggage-handling step so cargo already attached to Sam stays with him through the transfer.

Confirmed in gameplay with:

- Facility to Transponder jumps.
- Transponder / fast-travel jumps in both directions.
- Hot Spring Jump travel.
- Cargo on Sam's backpack and body attachment points.

The mod does not claim to transport vehicles, Floating Carriers, cargo left on the ground, or other cargo that is not currently carried by Sam.

## Requirements

- **DEATH STRANDING 2: ON THE BEACH PC Steam 1.10.89.0**, 64-bit.
- A compatible **x64 ASI loader**, such as Ultimate ASI Loader.

The ASI loader is not included. The mod validates the supported executable and hook bytes before activating. A game update may require a mod update.

## Installation

1. Fully close DS2.
2. Install an x64 ASI loader if you do not already use one.
3. Extract these two files beside `DS2.exe`:
   - `ds2_beach_jump_with_cargo.asi`
   - `ds2_beach_jump_with_cargo.ini`
4. Start the game normally.

The ZIP contains exactly those two files.

## Configuration

```ini
[BeachJumpWithCargo]
Enabled=1
```

Use `Enabled=0` to disable the mod. Restart DS2 after changing the setting. Missing settings default to enabled; invalid values keep the hook inactive.

## How it works

The mod hooks `DSBaggageManager::HandlingBaggagesOnFastTravel(bool)` and skips that routine while enabled. The actual fast-travel, destination, animation and loading logic remain native.

The change is applied only in memory. The mod does not modify `DS2.exe`, game archives or save files on disk.

## Troubleshooting

- Check `ds2_beach_jump_with_cargo.log` beside the ASI.
- `status=ACTIVE` means the hook was installed.
- `status=UNSUPPORTED_GAME_BUILD` means the executable identity does not match the supported build.
- `status=TARGET_BYTES_REJECTED` means another mod or game update changed the same code location.
- If there is no log, verify that the x64 ASI loader is working.

Mods that patch the same fast-travel baggage handler may conflict.

## Validation

The working mechanism was confirmed in gameplay before the public release: repeated Facility/Transponder travel and Hot Spring Jump travel retained the cargo carried by Sam. Version 1.0.0 removes the private diagnostic modes and per-jump telemetry while keeping the same skip behavior.

The release pipeline also verifies the supported DS2 executable SHA-256, PE identity, exact target prologue, embedded file version and reproducible binary build. See [VALIDATION.md](docs/VALIDATION.md).

## Build

Build and package on Windows x64 with LLVM, the Windows SDK resource compiler and Python 3:

```powershell
.\scripts\build.ps1
.\scripts\package-release.ps1
```

See [TECHNICAL.md](docs/TECHNICAL.md) and [CHANGELOG.md](CHANGELOG.md).
