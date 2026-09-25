# DS2 Sneaky Sam v1.0.1

Cosmetic cargo-visibility mod for **DEATH STRANDING 2: ON THE BEACH**.

Sneaky Sam hides cargo attached to Sam's four body cargo slots:

- right shoulder
- left shoulder
- right hip
- left hip

The cargo remains fully carried and functional. Moving it back to the backpack or another unaffected cargo position makes it visible again through the game's normal rendering path.

## What it does not change

- cargo inventory or ownership
- weight or capacity
- cargo condition
- slot assignment
- save data
- movement or gameplay stats
- enemy or BT detection / stealth calculations

This is a visual mod only.

## Installation

1. Install a working 64-bit ASI loader.
2. Copy `DS2_SneakySam_v1.0.1.asi` into the DS2 game / ASI loader directory.
3. Start the game normally.

To uninstall, remove the ASI file.

## Compatibility

- Built and validated for **DS2.exe v1.10.89.0**.
- Supported executable SHA-256: `BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B`.
- The mod validates the expected PE baseline and exact native visibility callsite before patching. If the expected executable layout is not present, it fails closed.

## v1.0.1

v1.0.1 keeps the proven v1.0.0 gameplay hook unchanged but rebuilds the ASI using the Microsoft Visual C++ toolchain and Windows SDK.

Changes:

- standard MSVC-generated PE layout;
- Windows version resource;
- explicit `InitializeASI` and version exports;
- relay memory is allocated read/write and then changed to read/execute;
- no executable memory is allocated as RWX;
- no change to cargo or visibility behaviour.

This build was accepted by the Nexus archive scanner after the v1.0.0 binary produced a false-positive archive warning.

## Technical summary

Native player baggage visibility refresh: RVA `0xF6BF30`.

Hooked callsite: RVA `0xF6C09D`.

Original native target: RVA `0x1194520`.

Target baggage slot types:

- `4` RightArm / right shoulder
- `5` LeftArm / left shoulder
- `6` RightWaist / right hip
- `7` LeftWaist / left hip

For those four slot types only, the native visibility argument is forced to false before the original game function is called.

## Validation

Confirmed in-game on the supported executable:

- all four shoulder/hip cargo positions are hidden;
- cargo remains functional;
- moving cargo back to the backpack restores normal visibility;
- inventory, weight and gameplay state remain native.

The v1.0.1 archive and ASI were also scanned locally with Microsoft Defender with no detections.
