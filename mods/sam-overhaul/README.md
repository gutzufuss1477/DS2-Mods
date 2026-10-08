# Sam Overhaul v1.1.0-dev.19

> Git-only development build. No Nexus/GitHub release or release tag is being published. The archived public v1.0.0 package remains unchanged.

Configurable all-in-one quality-of-life overhaul for **DEATH STRANDING 2: ON THE BEACH** on Steam PC.

Sam Overhaul combines the gameplay-tested cargo visibility, mobility, Autodrive and truck-weapon improvements developed for DS2.exe **1.10.89.0**.

### Optional construction radii (development, off by default)

- `[GeneratorRange] Enabled=1, RangePercent=200`: extends the verified Jolt charging trigger and generator's visible Odradek ring. The 200% charging boundary and circle were confirmed in-game on the tested generator, including a save reload. Other levels need broader regression coverage.
- `[TimefallShelterRange] Enabled=1, RangePercent=200`: doubles the native rain-protection radius (4 to 8 m), native cargo-coating repair radius (4 to 8 m), the validated Jolt protection cylinders, and the owner-specific visual circle. Cargo repaired to 100% with the spray cloud outside vanilla range in a prior live test. The integrated dev.19 visual circle and effect were confirmed matching at the larger radius on **2026-10-09**.
- Both switches default to off and only target the guarded Steam DS2.exe 1.10.89.0 native layout. The shelter's 30 m Odradek *effect activation distance* is unchanged.

These are still development features: repeated save reload, alternate construction stages, the full integrated gameplay suite and native hook performance require follow-up validation.

## Features

### Cargo visibility
- Hide shoulder cargo.
- Hide hip cargo.
- Hide backpack cargo.
- Hide spare shoes on the shoe clip while keeping Sam's currently worn shoes visible.
- Cargo remains carried, functional and part of the normal inventory/weight systems.

### Movement
- Jump/dismount from the monorail beyond the vanilla final safety blocker while preserving earlier native state checks.
- Use the normal zipline dismount in more positions while keeping the vanilla input and exit animation.
- Allow qualifying landing rolls while wearing the backpack/cargo. This does not add a free manual combat roll; normal landing conditions still apply.

### Optional footprint hiding
- Set `[Footprints] HideFootprints=1` in the existing `ds2_sam_overhaul.ini`.
- Hides both normal ground footprints and the footprints highlighted by an Odradek scan.
- Applies to new footprints and to saved footprints reconstructed when a save is loaded.
- Default is **0 (vanilla)**. Existing INIs without the new key stay unchanged.
- Restart after changing the setting. This is a display filter, not a save-file eraser.
- Runs inside the same Sam Overhaul ASI. No ReShade, ShaderToggler, or separate footprint ASI is needed.

### Autodrive
- Configurable Autodrive readiness time.
- Default: **2.0 seconds**.
- Valid range: **0.5 to 5.0 seconds**; 5.0 keeps vanilla timing.

### Truck weapons
Each weapon can be enabled/disabled and tuned independently.

- **Heavy Machine Gun**: longer range, faster aiming/lock-on, shorter burst and cooldown timing.
- **Mortar**: longer range, faster aiming/lock-on and faster shot cycle.
- **Chiral Cannon**: longer range including its long-range profile, faster aiming/lock-on, shorter cooldown and a tested charge-time reduction from 3.0 s to 0.75 s.
- **Missile Launcher**: longer range, faster aiming/lock-on and shorter recovery/cooldown.

Default range is 175% of vanilla and default aim speed is 250% for all four truck weapons.

## Installation

1. Close the game. Remove the previous Sam Overhaul ASI and the standalone `ds2_footprint_native_probe.asi` test module before installing this combined build. Keep only one Sam Overhaul version.
2. Use a working **64-bit ASI loader**. The loader is not included.
3. Copy these two files beside DS2.exe:
   - DS2_Sam_Overhaul_v1.1.0-dev.19.asi
   - ds2_sam_overhaul.ini
4. Start the game normally.

Steam default location:

    ...\Steam\steamapps\common\DEATH STRANDING 2 - ON THE BEACH\

Restart the game after changing the INI.

To uninstall, remove the Sam Overhaul ASI and INI. The mod does not edit DS2.exe or save files on disk.

## Configuration

The included INI preserves all 33 previous defaults and adds separate optional footprint and construction range settings. Existing custom INIs can be retained; add `[Footprints]`, `[GeneratorRange]` and `[TimefallShelterRange]` only when needed. All new options are off by default. The included INI is fully commented:

~~~ini
[CargoVisibility]
HideShoulderCargo=1
HideHipCargo=1
HideBackpackCargo=1
HideSpareShoes=1

[Movement]
JumpFromMonorailAnywhere=1
JumpFromZiplineAnywhere=1
LandingRollWithBackpack=1

[Footprints]
HideFootprints=0

[AutoDrive]
ActivationSeconds=2.0

[TruckHeavyMachineGun]
Enabled=1
RangePercent=175
AimSpeedPercent=250
LockOnSeconds=0.20
BurstSeconds=1.50
CooldownSeconds=0.50

[TruckMortar]
Enabled=1
RangePercent=175
AimSpeedPercent=250
LockOnSeconds=0.40
RecoverySeconds=0.40
CooldownSeconds=0.75

[TruckChiralCannon]
Enabled=1
RangePercent=175
AimSpeedPercent=250
LockOnSeconds=0.20
RecoverySeconds=0.40
CooldownSeconds=1.00
ChargeSeconds=0.75

[TruckMissileLauncher]
Enabled=1
RangePercent=175
AimSpeedPercent=250
LockOnSeconds=0.40
RecoverySeconds=0.40
CooldownSeconds=0.75
~~~

For percentage settings, **100 = vanilla**. Range accepts 100-300 and Aim Speed accepts 100-500. Time values are in seconds.

## Compatibility

- Validated target: **Steam DS2.exe 1.10.89.0 (Windows x64)**.
- Supported executable SHA-256: BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B.
- The ASI validates the expected PE baseline and exact native patch contexts before applying code changes.
- Unsupported game updates may require a mod update.
- Epic Games Store has not been validated for this release.
- Mods patching the same cargo visibility, landing, Autodrive or vehicle-weapon paths may conflict.

A local ds2_sam_overhaul.log is written beside the game executable for startup/configuration diagnostics.

## Validation

Gameplay testing confirmed:
- shoulder/hip/backpack/spare-shoe hiding;
- monorail and zipline dismount changes;
- backpack/cargo landing rolls;
- 2-second Autodrive activation;
- faster Heavy MG and Mortar targeting/fire behaviour;
- Chiral Cannon range/targeting changes and the 0.75-second charge profile;
- Missile Launcher targeting against MULEs.

Repeated Missile Launcher cadence is harder to evaluate because normal MULE targets die from a single hit; its values remain fully configurable.

The final v1.0.0 source builds with MSVC /W4 /WX. The existing Autodrive regression suite passes **105,704 checks** across 451 timing settings and four frame rates.

The footprint filter was confirmed in game as a standalone probe: new footprints disappeared, a previously footprint-heavy save reloaded without the old footprints, and the normal non-highlighted ground footprints were also observed to disappear. This development build integrates the unchanged filter core.

The combined build passes the preserved 105,704-check Autodrive suite, the footprint ABI/concurrency tests, 104 configuration/guard checks, a combined-DLL load test, and binary/default-INI checks. **The shelter's 200% visual circle and spray effect were confirmed on the integrated dev.19 ASI on 2026-10-09; a full combined gameplay regression remains pending.** See `docs/FOOTPRINTS_VALIDATION.md`.

## Development build

Run `scripts/build-development.ps1` on Windows x64 with Visual Studio 2022 Build Tools and Python available. It builds and tests the combined ASI, then stages the files under `development/DS2_Sam_Overhaul_v1.1.0-dev.19/`. Dependencies are vendored; no download occurs during compilation. It does not install into the game, create a release archive, or publish anything.

`package-release.ps1` deliberately refuses development versions. The existing public release folders, Nexus descriptions, and Mod Suite release metadata are not changed by this development update.

## Source and remaining checks

Development source is in the DS2 Mods repository under `mods/sam-overhaul`. The original v1.0.0 public package remains unchanged; dev.19 is **not** a Nexus/GitHub release.

**Shelter implementation:** native rain-protection configuration and cargo-coating restoration radius use the game's own native data. Shelter Jolt bodies use an owner/shape-validated one-time notification. The Odradek effect-size getter at RVA `0x1D72260` halves its stored size, so the current 8 m actual radius needs 16 m visual size in the active renderer; the native effect activation distance remains 30 m. The incorrect unrelated repair sphere from dev.9/dev.10 and experimental synthetic coating/contact hooks were fully removed.

**Confirmed:** the tester saw the larger repair effect and visible shelter circle matching on integrated dev.19 (2026-10-09); earlier tests verified actual shelter rain protection and spray restoring cargo to 100% beyond vanilla. The matching generator ring was also previously confirmed.

**Open before public release:** repeat after save reload, test other shelter/generator construction levels and interactions, verify remaining combined Sam features, and profile the shelter discovery hot path. The dev.19 diagnostics recorded expensive discovery passes (peak ~192 ms in the observed session); timing alone does not establish visible frame drops. This performance issue must not be marked resolved without further profiling.

Technical test record: `docs/CONSTRUCTION_RANGES_VALIDATION.md`.
