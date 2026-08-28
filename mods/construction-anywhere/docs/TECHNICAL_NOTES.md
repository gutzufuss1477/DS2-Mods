# Technical notes

## Target

- Game: DEATH STRANDING 2 ON THE BEACH, Steam PC
- File version: `1.10.89.0`
- PE timestamp: `0x6A3DAE46`
- PE image size: `0x0B292000`
- DS2.exe SHA-256: `BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B`

## Why these checks

Odradek RTTI identifies `DSConstructionBuildabilityCheckerConfig` and separates `NearbyFacility`, `UnconstructableArea`, `PermittedArea`, ground, material, road, Q-pid, and other checks. The installed resources confirm, for example:

- Generator (`DSChargerConstructionConfig`, `499:109723`) uses checker `499:108104`, with four local distances of `4.5` metres.
- Rain shelter (`DSRainShelterConstructionConfig`, `499:43476`) uses checker `499:22163`, also with four local distances of `4.5` metres.

The central native dispatcher at RVA `0x132E0A0` calls each check through its vtable and then copies the shared context's buildable state and reason flags to the placement result. Returning immediately from one exact checker therefore prevents only that checker from adding a failure reason. It does not force the overall result to true and does not bypass later checks.

## Memory patches

Each enabled patch replaces only the first instruction byte with x64 `RET` (`C3`). The following 19 bytes are left unchanged and used for read-back verification.

| INI option | Checker | RVA | Original first byte | Effect |
| --- | --- | ---: | ---: | --- |
| `RemoveProximityLimit` | `NearbyFacility` (enum 11) | `0x1337820` | `40` | Removes construction-to-construction distance and bounding-volume proximity failures |
| `AllowRestrictedAreas` | `UnconstructableArea` (enum 13) | `0x1339AE0` | `4C` | Removes tagged no-build-volume failures |
| `IgnorePermittedAreaWhitelist` | `PermittedArea` (enum 6) | `0x1336530` | `48` | Removes allowed-area whitelist enforcement; opt-in only |

All three 20-byte signatures occur exactly once in the supported executable. The mod verifies PE metadata and every enabled signature before writing. Writes use temporary page protection, instruction-cache flush, full signature read-back, protection restoration, and transaction rollback on failure.

## Deliberately preserved checks

The mod does not patch `QpidArea`, `CheckGround`, `AttachToGround`, `CheckGroundMaterial`, `PlannedHighwaySite`, `CollisionPlayerToFacility`, `CheckByBox`, flooding, wildfire, or construction-specific final checks. It also does not alter HUD strings or game resources.

## Scope and uncertainty

`NearbyFacility` applies broadly to local, online, and specialized construction categories. `UnconstructableArea` volumes can protect story and scenery geometry in addition to ordinary facilities. The two default bypasses were confirmed in game on 2026-08-28 with densely packed ordinary PCC structures inside a facility/building no-build area. `PermittedArea` is a whitelist and remains the highest-risk, default-off option; it has not been runtime-validated. Special monorail/track proximity logic remains native.
