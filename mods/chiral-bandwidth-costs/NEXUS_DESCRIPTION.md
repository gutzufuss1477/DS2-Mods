**Description**

Chiral Bandwidth Costs lets you set the exact chiral-bandwidth cost of supported player constructions in DEATH STRANDING 2: ON THE BEACH.

Every construction has its own English INI entry. Use `Native` to preserve the original game value, `0` to make that construction free, or any whole number from `0` to `1000000` for an exact custom cost. All entries default to `Native`, so installing the mod does not change anything until you choose a value.

Version 1.0.0 has live placement-HUD confirmation with a custom cost of `5` for both Generator and Zipline on Steam PC `DS2.exe 1.10.89.0`. The release also includes strict executable, resource-type, conflict, write, read-back, and rollback checks.

**Installation instructions**

1. Install a compatible external 64-bit ASI loader. If DS2 already loads other `.asi` mods correctly, the existing loader can be used.
2. Extract the downloaded archive.
3. Copy `ds2_chiral_bandwidth_costs.asi` and `ds2_chiral_bandwidth_costs.ini` into the game folder beside `DS2.exe`.
4. Edit the INI and replace `Native` only for the constructions you want to customize.
5. Fully restart the game after installing the mod or changing the INI.

To uninstall, remove both files and restart the game. The ASI loader is not included.

**Configuration example**

```ini
[ChiralBandwidthCosts]
Enabled=1
DebugLog=0

Generator=100
Zipline=250
Bridge=500
```

`Native` keeps the original value. `0` means no chiral-bandwidth cost. Values must be plain whole decimal numbers without signs, fractions, hexadecimal notation, or inline comments. Invalid enabled configurations fail closed without applying cost changes.

**Supported constructions**

- Postbox
- Safe House
- Watchtower
- Cargo Catapult
- Generator
- Timefall Shelter
- Zipline
- Standard Bridge, including 30 m, 45 m, and 80 m variants
- Chiral Bridge
- Jump Ramp
- Hot Spring Digger (experimental)

The Transponder is intentionally unsupported because DS2 treats it as a tool outside normal Q-pid bandwidth accounting.

**Requirements and safety**

- DEATH STRANDING 2: ON THE BEACH for PC / Steam
- Supported executable version: `DS2.exe 1.10.89.0`
- A compatible external 64-bit ASI loader
- Both included files placed beside `DS2.exe`

Back up your save and test offline first. Keep configured costs unchanged after constructing anything with the mod in that save. DS2 may use the currently loaded construction cost when dismantling an existing structure, which could make bandwidth accounting inconsistent if the INI value is changed later.

The mod does not edit `DS2.exe`, game archives, or save files on disk. It does not intentionally alter materials, placement restrictions, upgrades, repairs, Q-pid coverage, or total available chiral bandwidth. Unexpected executable builds, resource types, or third-party field values are rejected instead of overwritten.

After a game update, do not assume compatibility. Remove the ASI until a matching release is available.

**Shout outs**

Thanks to ShadelessFox for Odradek and the DS2 RTTI research tools, and to the player who proposed the mod and provided live placement-HUD confirmation.
