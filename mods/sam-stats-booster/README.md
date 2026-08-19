# Sam Stats Booster

Configurable progression multiplier for Sam's activity-based proficiency stats in **DEATH STRANDING 2: ON THE BEACH**.

The mod multiplies only positive gains that DS2 already awards through normal gameplay. It does not set levels directly, edit save files, replace level thresholds, or grant progress for activities that would normally award zero.

## Features

- Configurable `1.0x` to `100.0x` multiplier
- Independent switches for all 12 visible Sam Stats
- Uses DS2's original eligibility checks, play-record storage, caps, level thresholds, UI and save system
- No direct level or save-file editing
- Fails closed when the supported executable or validated hook sites do not match
- Designed to coexist with Porter Grade Booster

## Boosted categories

- Terrain Traversal
- Carrying Capacity
- Endurance
- KO Resistance
- Lung Capacity
- Strand Skills
- Melee Combat
- Handguns
- Assault Rifles
- Sniper Rifles
- Machine Guns
- Shotguns

An internal thirteenth `StealthTakedown` record was also identified. It is configurable but disabled by default because it is not one of the 12 visible radar-chart categories.

## Supported target

- Death Stranding 2 `DS2.exe` version `1.10.89.0`
- SHA-256: `BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B`
- PE timestamp: `0x6A3DAE46`
- Image size: `0x0B292000`

The ASI validates the executable header, producer signatures, patch windows, native getters and mutators before installing all three hooks.

## Installation

1. Install a compatible external x64 ASI loader for Death Stranding 2.
2. Copy `reference/SamStatsBooster.asi` and `config/SamStatsBooster.ini` beside the other DS2 ASI mods used by that loader.
3. Start the game.
4. Confirm that `SamStatsBooster.log` is created beside the ASI and reports `State: ACTIVE`.

The ASI loader itself is not included.

A ready end-user package is available at `release/DS2_Sam_Stats_Booster_v1.0.0.zip`.

## Configuration

```ini
[SamStatsBooster]
Enabled=1
Multiplier=5.0

[Stats]
TerrainTraversal=1
CarryingCapacity=1
Endurance=1
KOResistance=1
LungCapacity=1
StrandSkills=1
MeleeCombat=1
Handguns=1
AssaultRifles=1
SniperRifles=1
MachineGuns=1
Shotguns=1
StealthTakedown=0
```

The multiplier range is `1.0` to `100.0`. The release default is `5.0`. Values between `2.0` and `5.0` are recommended for ordinary progression; extreme values can move through several levels in very few native gain events.

## Level-up display

The mod does not patch DS2's HUD or notification system. Stat progress is committed immediately through the game's original play-record mutators, but the reliable place to confirm the resulting level and progress bar is **Sam's Stats** menu. A missing gameplay notification does not mean the gain was rejected.

## Validation

Runtime tests confirmed that the configured multiplier reaches DS2's stored progression totals exactly:

- At `20.0x`, Terrain Traversal, Carrying Capacity, Endurance and KO Resistance stored the complete requested boosted gains.
- At `50.0x`, Lung Capacity stored `2150` from `43` native points.
- At `50.0x`, Melee Combat stored `100` from `2` native points.
- At `50.0x`, Assault Rifles stored `100` from `2` native points.
- The resulting levels were visible in Sam's Stats menu, including rapid Lung Capacity progression to level 5.

The remaining firearm and Strand categories share the same validated Q32 producer and native mutator path, with their own confirmed record-ID mappings.

See `docs/VALIDATION.md` and `docs/TECHNICAL_NOTES.md` for details.

## How it works

The mod patches three narrow native skill-gain producer paths instead of globally intercepting shared play-record mutators:

- Q32 producer for combat, firearms, Strand Skills and the internal stealth-takedown record
- Q64 producer for Lung Capacity
- Double producer for Terrain Traversal, Carrying Capacity, Endurance and KO Resistance

DS2 performs its normal eligibility checks before these hook sites are reached. The ASI scales the positive native delta, then calls the corresponding original DS2 mutator. The game remains responsible for storage, caps, thresholds, levels, bonuses, UI and persistence.

## Compatibility

`Sam Stats Booster` and `Porter Grade Booster` patch different producer locations and are designed to run together. The Q64 validation accepts the known Porter Grade detour whether it loads before or after this ASI.

Another mod that patches any of the exact producer windows documented in `docs/TECHNICAL_NOTES.md` may conflict. On a mismatch, Sam Stats Booster remains inactive rather than applying an uncertain patch.

## Uninstall

Remove:

- `SamStatsBooster.asi`
- `SamStatsBooster.ini`
- generated `SamStatsBooster.log`

Progress already committed to the save remains, because it was added through DS2's original system.

## Build

Run `scripts/build-llvm.ps1` with LLVM/Clang and `lld-link` available.

The reference ASI is built from the included source with no default runtime libraries and no import directory.

### Reference ASI SHA-256

`E930E2B6C59A955789A2E5F4D967D9BFC30F57CA2DBA9061039841A1F4E8AB99`

## Version

`v1.0.0`
