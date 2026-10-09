# Sam Overhaul v1.1.0

One configurable ASI for **DEATH STRANDING 2: ON THE BEACH** (Windows x64, Steam DS2.exe 1.10.89.0).

Sam Overhaul combines visual cargo hiding, movement improvements, faster Autodrive readiness and truck-weapon tuning. **Version 1.1.0 adds optional footprint removal and doubled generator / timefall shelter coverage**, including terrain-aware cargo repair at shelters.

## What's new in v1.1.0

### Footprints (optional)

- Hides both ordinary ground footprints and blue Odradek scan footprints.
- Applies to new footprints and existing footprints reconstructed when a saved game loads.
- Visual-only; the game save files are never edited.
- No ReShade, ShaderToggler or separate footprint ASI required.

Enable in the included INI:

~~~ini
[Footprints]
HideFootprints=1
~~~

### Generator range (optional)

- Extends the verified generator charging trigger and its Odradek visual circle.
- Tested with [GeneratorRange] RangePercent=200: twice the original radius.
- The native Jolt refresh ensures expanded charging physics and the visual range are synchronized for the tested structures.
- Generator levels and game versions not covered by the tested Steam build may behave differently.

~~~ini
[GeneratorRange]
Enabled=1
RangePercent=200
~~~

### Timefall shelter range, cargo repair and interaction (optional)

The expanded timefall shelter features are coordinated rather than blindly scaling one effect:

| Component | Range with recommended settings |
| --- | ---: |
| Timefall protection | 8.0 m radius (200%) |
| Blue ground circle | 8.0 m radius (200%) |
| Native cargo-container repair | 8.6 m radius (215%) |

The additional 0.6 m of cargo-repair reach compensates for the game's *3D distance check* on slopes, where the visible ground circle and repair cloud would otherwise differ. Minor terrain-dependent variation is normal.

German UI: misleading "In Bunker ausruhen" becomes "Verschnaufen" at the rain shelter. **The original native shelter-rest action is preserved**, including its normal in-game effects. This is a text correction, not an action swap.

~~~ini
[TimefallShelterRange]
Enabled=1
RangePercent=200
RepairRadiusPercent=215
FixRestPrompt=1
SpatialDiagnostics=0
~~~

The shelter system retains its own weather logic, native repair amount and other gameplay conditions. Native object lifecycle, reloads and renderer geometry are validated before expanding the active shelter effect. v1.1.0 also includes the steady-state performance optimization confirmed in-game without perceptible stutters.

## Existing Sam Overhaul features

### Cargo visibility
- Hide cargo attached to Sam's shoulders, hips or backpack.
- Hide spare shoes on the shoe clip, keeping currently worn shoes visible.
- Only the visuals change. Inventory, weight, ownership and condition are still native.

### Movement
- Monorail dismount/jump beyond the original final safety-height blocker.
- Normal zipline dismount from more positions.
- Qualifying landing rolls with backpack/cargo. This does not add an unrestricted manual combat-roll button.

### Autodrive
- Configurable native readiness timing via [AutoDrive] ActivationSeconds (recommended 2.0, accepted 0.5–5.0).
- The game still decides whether Autodrive is currently eligible; speed and road restrictions are unchanged.

### Truck-mounted weapons
Four independent INI profiles: Heavy Machine Gun, Mortar, Chiral Cannon and Missile Launcher. Configurable reach, aiming and attack cadence. Recommended range 175% and aim speed 250%. Chiral Cannon charging can be reduced from 3.0 to 0.75 seconds.

## Installation / updating

1. **Close the game.** Install a working Windows x64 ASI loader separately; it is not included.
2. Remove any older DS2_Sam_Overhaul_v*.asi from the game directory. Keep **only one** Sam Overhaul ASI active. Do not use the old standalone ds2_footprint_native_probe.asi at the same time.
3. Copy DS2_Sam_Overhaul_v1.1.0.asi beside DS2.exe. For a **fresh installation**, also copy ds2_sam_overhaul.ini there.
4. If updating from v1.0.0, **keep your existing INI/custom weapon and movement settings**. Add the three new INI sections shown above, or merge them from the included sample. Do not overwrite your custom INI without a backup.
5. Start the game; restart after changing INI options.

Typical Steam install directory:

    ...\Steam\steamapps\common\DEATH STRANDING 2 - ON THE BEACH\

To uninstall, remove the Sam Overhaul ASI. You can keep a copy of your custom INI for later. The mod does not modify DS2.exe or your save files on disk.

## Default behavior and compatibility

**All three newly added options are OFF by default for backwards compatibility**:

- [Footprints] HideFootprints=0
- [GeneratorRange] Enabled=0
- [TimefallShelterRange] Enabled=0

Enable them explicitly as shown above. With an old INI that lacks these sections, the new features remain disabled. This avoids silently changing existing users' games. The original cargo/movement/Autodrive/truck settings remain backward compatible.

RangePercent=100 means vanilla radius; 200 means twice the original. Both structure range options accept 100–400. RepairRadiusPercent accepts 100–400, with recommended 215 when timefall protection is 200. FixRestPrompt=0 disables the German label correction. SpatialDiagnostics=1 enables optional spatial developer logging and is **not** recommended for ordinary play.

Supported executable: **Steam DS2.exe 1.10.89.0** (Windows x64).

Expected SHA-256:

    BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B

Other game versions or Epic Games Store binaries have **not been validated**. An update to the game's executable may require an updated ASI. Mods changing the same native code paths could conflict.

A diagnostic file ds2_sam_overhaul.log is created beside DS2.exe. MinHook is linked into the ASI; see THIRD_PARTY_NOTICES.md and LICENSE_MINHOOK.txt.

## Test status

v1.1.0 was verified on the supported Steam build with footprint hiding, expanded generator reach and visible circle, timefall rain protection, shelter cloud repair and German interaction text. Timefall shelter range and performance were tested on sloping terrain with the recommended 8.0 m / 8.6 m settings. Native assembly jump, repair source lifetime, resource reload, optional INI, rendering and unsupported-host checks are covered by automated regression tests.

**The ASI loader is not included. There is no dependence on ReShade or external localization mods.**
