# Research / Handover Notes

## Goal

Make APAS Enhancement Memory requirements configurable through an INI file.

Primary use case:

```ini
GlobalCost=1
```

## Confirmed result

Runtime testing confirmed that changing:

```text
DSApasEnhancementResource +0x24
```

changes the actual APAS Memory requirement.

A prior debug build observed 46 APAS resources and successfully set all discovered
resources to the configured target with no invalid entries or failed writes.

## Static-analysis milestones

1. APAS-related reflection/debug strings identified.
2. `DSApasEnhancementResource` located.
3. APAS UI / manager consumers isolated.
4. APAS accounting function identified.
5. `entry +0x70 -> DSApasEnhancementResource*` confirmed.
6. `resource +0x20 -> EnhancementId` confirmed.
7. `resource +0x24 -> EnhancementPoint` confirmed.
8. Activation logic independently confirmed `+0x24` as the actual capacity cost.
9. Runtime alpha successfully changed loaded resources to cost `1`.

## Performance evolution

The initial public implementation kept a worker alive and repeatedly rescanned APAS
resources during gameplay.

User feedback reported a large FPS reduction with that approach.

v1.0.3 changed the worker model:

- delay initial work by 3 seconds;
- scan infrequently while only a partial APAS table exists;
- once 40+ resources appear, check only until the table is stable;
- exit after five stable passes with no new writes.

This v1.0.3 implementation was subsequently reported to perform better and was promoted
to the main release.

## Exact binary preservation

The `release/ds2_apas_memory_costs.asi` file in this package is the exact tested
v1.0.3 performance binary that was promoted to the main version.

It was intentionally not recompiled for this Git handover package.

The exported internal version string in that binary still reads:

```text
1.0.3-performance-test
```

This is expected and documents that the repository release binary is the exact tested
build, not a later rebuild with potentially different PE characteristics.

## Maintenance priority after a game update

Check in this order:

1. global APAS manager RVA (`0x623E5D0`);
2. entry array offset (`+0x30`) and entry count (`0x37`);
3. resource pointer (`entry +0x70`);
4. `EnhancementPoint` (`resource +0x24`).

The structure offsets may remain stable even if the absolute manager RVA moves.
