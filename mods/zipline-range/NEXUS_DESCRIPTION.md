**Description**

Zipline Range and Speed extends both the placement range and the actual rideable connection range of Ziplines in DEATH STRANDING 2: ON THE BEACH.

DS2 exposes two visible Zipline levels. The packaged `600 m / 600 m` defaults give Level 1 the full configured reach immediately, so an upgrade is not required just to obtain more range. Both values can be customized up to `1000 m`.

The mod also provides an independent travel-speed multiplier. The default is `2.0x`, and `4.0x` has been confirmed in gameplay. Values above `4.0x` and up to `10.0x` are available for experimental use.

Long-distance connections beyond the native rideable limit have been tested in both directions. The mod also adjusts the required endpoint realization and HUD/selection distances so remote Ziplines remain active and selectable.

**Installation instructions**

1. Install a compatible external 64-bit ASI loader. If DS2 already loads other `.asi` mods correctly, the existing loader can be used.
2. Extract the downloaded archive.
3. Copy `ds2_zipline_range.asi` and `ds2_zipline_range.ini` into the game folder beside `DS2.exe`.
4. Fully restart the game.
5. Edit the INI if desired, then restart the game again for changes to take effect.

To uninstall, remove the ASI and INI files and restart the game. Existing Zipline connections are not removed automatically.

**Main features**

- Configurable Level 1 range from `300–1000 m`
- Configurable Level 2 range from `350–1000 m`
- Level 1 and Level 2 default to the same `600 m` reach
- No Level 2 upgrade required to receive the full default range
- Fixes the separate native rideable-connection limit for long cables
- Keeps distant endpoints active, visible, and selectable
- Configurable actual travel speed from `0.25x–10.0x`
- `2.0x` default speed and successfully tested `4.0x` speed
- Preserves native normal, carrier, and ADS ride factors
- Does not alter global game time or merely accelerate the animation
- Exact executable and instruction-signature validation
- Does not edit `DS2.exe`, game archives, or save files on disk

**Requirements**

- DEATH STRANDING 2: ON THE BEACH for PC / Steam
- Supported executable version: `DS2.exe 1.10.89.0`
- A compatible external 64-bit ASI loader
- Both included files must be placed beside `DS2.exe`

The highest speed confirmed in gameplay is `4.0x`. Values above `4.0x` are experimental and may affect camera comfort, animation, audio, endpoint arrival, or world streaming. Back up your save and test offline first when using extremely long ranges or experimental speeds.

With the packaged configuration, terrain, obstruction, angle, construction-area, network, and other native placement checks remain unchanged. After a game update, do not assume compatibility; wait for a matching mod update.

**Shout outs**

Thanks to ShadelessFox for Odradek and the DS2 RTTI research tools, and to the players who proposed and tested long-distance, bidirectional Zipline travel and higher ride speeds.
