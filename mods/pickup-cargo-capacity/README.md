# DS2 Cargo Capacity Expansion for Pickup

Version **1.0.2** for *DEATH STRANDING 2: ON THE BEACH*.

Increases the Off-road Pickup's real cargo capacity from the original 160 size units to a configurable value of 160-480 units, with a tested 320-unit default. **New in 1.0.2:** The ten cargo bars on the vehicle's rear now scale with the expanded capacity instead of saturating at the original 160 units.

## New: proportional rear cargo indicators

The five load bars on each side of the Pickup now reflect the configured maximum load.

- With capacity 320, at 160 units (the game's 100% display), **five of ten bars light white**.
- At 320 units (the game's 200% display), **all ten bars light red**.
- The existing loading, unloading, driving, and save-game mechanics are unchanged.
- The separate cargo management **menu percentage is still relative to vanilla 160 units**. It reads 200% at a full 320-unit pickup and 300% at a full 480-unit pickup. This is cosmetic, not an actual overload.

## Configuration

Install the ASI and INI next to DS2.exe with a 64-bit ASI loader.

    [PickupCargoCapacity]
    Enabled=1
    CapacityUnits=320
    RearIndicatorScaling=1

- **Enabled**: 1 to enable the mod, 0 for no game patches.
- **CapacityUnits**: 160 to 480, rounded down to a multiple of 16.
- **RearIndicatorScaling**: 1 (default) to scale the real-world rear cargo bars; 0 to retain vanilla rear-light behaviour while keeping increased capacity.

Item sizes: S=1, M=2, L=4, XL=6 size units.

## Install or update

1. Fully quit Death Stranding 2.
2. Replace the older PickupCargoCapacity.asi in the game directory (do not load two copies).
3. Install the supplied INI or keep your existing CapacityUnits and add RearIndicatorScaling=1.
4. Start the game. The STATUS.txt written alongside the ASI should report STATE=READY and REAR_INDICATORS=ACTIVE for capacity greater than 160.

To uninstall, first unload any additional cargo until the Pickup contains 160 size units or fewer.

## Compatibility / technical notes

Supported Steam executable: DS2.exe v1.10.89.0. SHA-256: BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B.

The original four verified pack-area patches are identical to v1.0.1. Two separately verified, display-only instruction hooks scale a temporary area-count representation for the rear light controller. The authoritative cargo container arrays are not modified by these hooks. On unsupported builds, the status file reports an error instead of patching mismatching instruction bytes.

The display hooks require a small, locally allocated, executable code island. They use no external process, network traffic, telemetry or persistent service.

## Validation

At capacity 320, loading to 100% of the vanilla display showed 5/10 rear segments white. Fully loading to 200% showed 10/10 red segments. Both were visually confirmed in game on 2026-10-10.

The capacity expansion was previously tested with over 1,800 kg and load/unload, driving, saving and restarting. The indicator feature is in-game tested at the 320-unit default; different configured values follow the same formula but have not all been separately tested in game.

For implementation notes and a regression test checklist, see TEST_REAR_INDICATORS.md. Build instructions are in BUILDING.md; security information is in SECURITY_NOTES.md.

## Distribution

Release ZIPs are built separately and not stored inside Git. No separate open-source licence is granted by this repository. All rights are reserved unless stated otherwise.
