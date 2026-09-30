# Sam Overhaul v1.0.0

Configurable all-in-one quality-of-life overhaul for **DEATH STRANDING 2: ON THE BEACH** on Steam PC.

Sam Overhaul combines the gameplay-tested cargo visibility, mobility, Autodrive and truck-weapon improvements developed for DS2.exe **1.10.89.0**.

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

1. Close the game.
2. Use a working **64-bit ASI loader**. The loader is not included.
3. Copy these two files beside DS2.exe:
   - DS2_Sam_Overhaul_v1.0.0.asi
   - ds2_sam_overhaul.ini
4. Start the game normally.

Steam default location:

    ...\Steam\steamapps\common\DEATH STRANDING 2 - ON THE BEACH\

Restart the game after changing the INI.

To uninstall, remove the Sam Overhaul ASI and INI. The mod does not edit DS2.exe or save files on disk.

## Configuration

The included INI is fully commented and uses simple sections:

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

Footprint suppression experiments are intentionally **not included** in v1.0.0.

## Source

Source code and validated releases are maintained in the DS2 Mods repository under mods/sam-overhaul.
