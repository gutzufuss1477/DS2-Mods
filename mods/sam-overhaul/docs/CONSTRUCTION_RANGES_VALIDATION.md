# Construction range validation — Sam Overhaul 1.1.0-dev.19 (2026-10-09)

## Scope

Development only. Steam DS2.exe v1.10.89.0, executable SHA-256 `BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B`. Both INI sections, `[GeneratorRange]` and `[TimefallShelterRange]`, default to `Enabled=0`; known local tests used `Enabled=1`, `RangePercent=200`. No public release has been made.

## Observed in-game results

- Generator: the physical charging boundary and visible blue Odradek circle matched at 200% for the tested structure; the user also reloaded a save and confirmed persistence (2026-10-08).
- Shelter native protection: the engine's `DSRainShelter` native config property `Range` at `+0x760` was expanded from 4 to 8 m. User verified timefall no longer degraded carried cargo outside vanilla range (2026-10-08).
- Shelter native repair: `DSConstructionRepairSprayComponentResource+0x20`, native radius 4 -> 8 m. At a separately verified ~4.98 m, the original spray effect appeared and a damaged container's coating was restored to 100% (2026-10-08).
- Shelter ring: the visual size is a *diameter* in `DSOdradekEffectInstance+0x2A0`, halved by native getter RVA `0x1D72260`. The tested dev.19 ASI sets 16 m visual size for the intended 8 m repair/protection radius. The user confirmed **larger visible circle and spray effect match** on 2026-10-09. The genuine 30 m effect-activation distance remains unchanged.
- Footprint suppression still loads in the combined ASI. No separate ReShade or standalone footprint ASI is used.

## Technical constraints / non-features

- The old dev.9/dev.10 Jolt sphere candidate belonged to an unrelated entity and was removed. The dev.13 synthetic repair contact experiment was removed. The dev.15 read-only coating probes were removed. Current shelter repair uses native resource data; no direct cargo/coating writes.
- Both shelter Jolt protective cylinders are validated by owner/vtable/shape/body ID, and an engine Jolt `NotifyShapeChanged` refresh is scheduled once per registered body.
- Native protection and repair values, visual geometry and engine effect-activation distance are distinct; never change the activation distance to enlarge the circle.
- All source changes live on the development branch. Do not tag or publish on Nexus based on this partial verification.

## Diagnostics and open checks

The observed live dev.19 log contains `NATIVE_CONFIG_RANGE_SYNCED`, `REPAIR_RADIUS_NATIVE_SYNCED`, `CYLINDER_SCALED_AND_JOLT_REFRESHED`, `ODRADEK_VISUAL_DIAMETER_SYNCED`, and `ODRADEK_RENDER_INSTANCE_DIAMETER_SYNCED`.

The active shelter discovery profiler still reported expensive calls (maximum samples from ~115,000 to 192,371 microseconds, usually 16–20 calls per 15 seconds, many >8 ms). These are native-hook discovery elapsed times, **not measured FPS or proven render-thread stalls**. Profiling/optimisation is open; do not claim it is resolved from a subjectively smooth test.

Remaining game regression: reload the same shelter save and ensure ring/repair alignment persists; test construction stages 1/2/3 and multiple generators; check cargo/coating/weather at the boundary; verify no adverse Autodrive, visibility, zipline/monorail, landing-roll, weapon or footprint interactions.

## Reproducible automated tests

Run `scripts/build-development.ps1`. It compiles the combined ASI and runs the original Autodrive regression, footprints/ABI and configuration checks, and binary/loader validation. It stages a local *development* package without installing it in the game. These static/host tests are not substitutes for the remaining gameplay checks.
