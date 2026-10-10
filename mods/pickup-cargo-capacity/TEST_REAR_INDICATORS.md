# Rear cargo indicator - v1.0.2 verification

## Supported target

- Game: Steam DS2.exe v1.10.89.0
- SHA-256: BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B
- Config used: CapacityUnits=320, RearIndicatorScaling=1

## Technical path

The DSVehicleTruck function at RVA 0x01F935E0 derives a five-stage visual indicator from the number of occupied 16-unit pack areas. The stage instructions at 0x01F9369A and material-display instructions at 0x01F93792 are replaced with checked detours. Original item counts are never altered.

- Vanilla reference: 10 usable pack areas, 160 capacity units.
- Default expanded reference: 20 usable pack areas, 320 capacity units.
- Mapping: display_count = min(10, ceil(occupied_areas * 10 / usable_areas)).
- Validating only DS2.exe versions with expected original instruction bytes is required.

## Confirmed in-game evidence, October 10, 2026

1. Loaded the Pickup to 320 cargo size units, 200% game menu, 1,508.4 kg. Ten rear light segments were red. The Pickup did not accept additional cargo.
2. Unloaded to 160 cargo size units, 100% game menu, 728.0 kg. Five of ten rear segments were white: three on the left and two on the right.
3. The experimental build's status file reported STATE=READY and REAR_INDICATORS=READY_UNTESTED_IN_GAME. After promoting the release, that status becomes REAR_INDICATORS=ACTIVE after successful hook installation.

These observed states demonstrate that the indicators scale with the expanded default cargo capacity. Remaining validation targets: in-game saves/reload, other truck models, and 160/480 configurations. The core capacity mod had previously passed separate load/unload, drive and save/reload testing.

## Rollback

Set RearIndicatorScaling=0 and restart the game to keep capacity expansion while restoring vanilla lights. For a full rollback, close the game first and restore a previous PickupCargoCapacity.asi; avoid removing capacity expansion while the Pickup holds more than the original 160 units.
