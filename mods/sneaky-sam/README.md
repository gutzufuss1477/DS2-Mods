# DS2 Sneaky Sam v1.0.0

Cosmetic cargo-visibility mod for **DEATH STRANDING 2: ON THE BEACH**.

## What it does

Sneaky Sam hides cargo attached to Sam's four body cargo slots:

- right shoulder
- left shoulder
- right hip
- left hip

The cargo remains fully carried and functional. Moving an item back to the backpack makes it visible again through the game's normal rendering path.

## What it does not change

- cargo inventory or ownership
- weight or capacity
- cargo condition
- slot assignment
- movement or gameplay stats
- enemy or BT detection / stealth calculations

This is a visual mod only.

## Installation

1. Install a working 64-bit ASI loader.
2. Copy `DS2_SneakySam_v1.0.0.asi` into the same game directory used by your other DS2 ASI mods.
3. Start the game normally.

To uninstall, remove the ASI file.

## Compatibility

- Built and validated for **DS2.exe v1.10.89.0**.
- Supported executable SHA-256: `BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B`.
- The mod validates the supported PE baseline and the exact native visibility callsite before patching. If the expected executable layout is not present, it fails closed.

## Technical summary

DS2 already has native baggage visibility handling inside `DSPlayerEquipmentManageComponent`.

The mod hooks the native baggage visibility call at RVA `0xF6C09D`. The original call targets RVA `0x1194520`.

For baggage entities whose native slot type is one of:

- `4` = RightArm / right shoulder
- `5` = LeftArm / left shoulder
- `6` = RightWaist / right hip
- `7` = LeftWaist / left hip

the visibility argument is forced to false before calling the original game function.

No baggage entity is removed or modified.

## Validation

The v0.1.0 test build was tested in-game on the supported executable.

Confirmed:

- all four shoulder/hip cargo slots are hidden;
- cargo remains carried and functional;
- moving cargo back to the backpack makes it visible again;
- all four target slot types were observed in the runtime test log;
- inventory, weight and gameplay state are left native.

The v1.0.0 release keeps the same proven hook and filter while removing per-slot development telemetry.

## Source

Source is retained with the development project for auditing and reproducible builds.
