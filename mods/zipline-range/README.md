# DS2 Zipline Range and Speed v1.0.0

Configurable Zipline connection range and travel speed for **DEATH STRANDING 2: ON THE BEACH** on PC.

The mod extends both the placement range and the actual rideable connection range. It also raises the realization and HUD/selection distances needed to keep a remote endpoint active and selectable. Long-distance connections beyond the native limit have been confirmed in both directions.

`SpeedMultiplier` changes actual progress along the cable, not global game time or only the animation. DS2's native factors for normal rides, one or two carriers, and ADS rides remain part of the calculation. Speeds up to `4.0x` were tested successfully; higher values up to `10.0x` are available as an experimental option.

## Installation

The release archive contains exactly two files:

- `ds2_zipline_range.asi`
- `ds2_zipline_range.ini`

Copy both files into the game folder beside `DS2.exe`. A compatible external 64-bit ASI loader is required. Fully restart the game after installing the mod or changing the INI.

To uninstall, remove the ASI and INI files and restart the game. Existing Zipline connections are not deleted automatically.

## Configuration

The packaged defaults are:

```ini
[ZiplineRange]
Enabled=1
Level1RangeMeters=600
Level2RangeMeters=600
SpeedMultiplier=2.0
DebugLog=0
```

DS2 exposes two visible Zipline levels. Both defaults are set to `600 m`, so a Level 1 Zipline immediately receives the full configured range and does not need a Level 2 upgrade just to reach farther.

`Level1RangeMeters` accepts `300–1000`, and `Level2RangeMeters` accepts `350–1000`. The Level 2 value must be at least as high as the Level 1 value. Set both values to the same distance if upgrades should not affect range. The mod preserves DS2's native 1 m placement tolerance internally.

`SpeedMultiplier=1.0` keeps native travel speed. The packaged default `2.0` doubles actual travel progress. Values from `0.25` to `10.0` are accepted. `4.0` is the highest setting confirmed in gameplay; values above `4.0` are experimental and may cause uncomfortable camera movement, endpoint arrival, audio, animation, or world-streaming behavior.

`DebugLog=1` creates `ds2_zipline_range.log` next to the ASI. Errors are logged even when optional debug logging is disabled.

## Compatibility and safety

- Supported executable: Steam PC `DS2.exe 1.10.89.0`
- The mod verifies the executable build, instruction signatures, and relevant resource types before applying changes.
- Unsupported or conflicting builds fail closed and are reported in the log.
- The mod does not edit `DS2.exe`, game archives, or save files on disk.
- With the packaged configuration, terrain, obstruction, angle, construction-area, network, and other native placement checks remain unchanged.
- After a game update, wait for a compatible mod update before using it again.

Back up your save and test offline first, especially when using very long connections or experimental speeds above `4.0x`.

## Confirmed behavior

- Level 1 and Level 2 can use the same configured maximum range.
- Connections beyond the native 351 m rideable limit work in both directions.
- Remote endpoint markers remain visible and selectable beyond the native 400 m HUD limit.
- Actual travel speed responds to the INI multiplier.
- `4.0x` travel speed was tested successfully on a long-distance connection.
