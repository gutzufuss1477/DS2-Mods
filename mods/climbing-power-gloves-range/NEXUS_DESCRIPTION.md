> **Do not paste this rendered Markdown page into the Nexus editor.** Switch
> the Nexus description editor to **BBCode mode** and paste the raw contents of
> [`NEXUS_DESCRIPTION_BBCODE.txt`](NEXUS_DESCRIPTION_BBCODE.txt) instead.

**Description**

Climbing Power Gloves - Extended Cargo Pickup Range lets both tiers of the
Climbing Power Gloves pull in cargo from much farther away in DEATH STRANDING
2: ON THE BEACH.

Level 1 and Level 2 use separate configurable ranges. The included profile
raises their native `8 m` and `10 m` values to `30 m` and `50 m`, preserving a
clear advantage for Level 2 while reducing the need to carry a Sticky Gun or
Sticky Cannon for ordinary remote cargo pickup.

Version 1.0.0 was tested in game: Level 2 picks up cargo from a long distance,
switching to Level 1 immediately reduces the reach, and removing the gloves
restores normal close-range pickup.

**Installation instructions**

1. Close DEATH STRANDING 2 completely.
2. Install a compatible external 64-bit ASI loader. If DS2 already loads other
   `.asi` mods correctly, the existing loader can be used.
3. Extract the downloaded archive.
4. Copy `ds2_climbing_gloves_range.asi` and
   `ds2_climbing_gloves_range.ini` into the game folder beside `DS2.exe`.
5. Fully restart the game.

To uninstall, close the game, remove both files, and restart. The mod does not
modify save files, `DS2.exe`, or game archives on disk.

**Configuration**

```ini
[ClimbingGlovesRange]
; Master switch. Set to 0 to leave all resource values unchanged.
Enabled=1

; Climbing Power Gloves Level 1: native 8 m, accepted range 8-100 m.
Level1RangeMeters=30

; Climbing Power Gloves Level 2: native 10 m, accepted range 10-100 m.
; Must be greater than or equal to Level1RangeMeters.
Level2RangeMeters=50

; Set to 1 for additional diagnostics in ds2_climbing_gloves_range.log.
; Critical errors are always logged.
DebugLog=0
```

Level 1 accepts `8-100 m`; Level 2 accepts `10-100 m` and cannot be lower
than Level 1. Setting both values to the same number gives both tiers the same
reach. Fully restart the game after changing the INI.

Invalid settings fail safely without changing either glove resource. Set
`DebugLog=1` if detailed startup diagnostics are needed.

**Main features**

- Separate configurable pickup ranges for Climbing Power Gloves Level 1 and 2
- Tested default profile of `30 m` for Level 1 and `50 m` for Level 2
- Preserves the normal close-range pickup distance when no gloves are equipped
- Does not change other gloves, pickup speed, throw distance, or global pickup
- Does not modify Sticky Gun or Sticky Cannon range and behavior
- Changes only the verified range field of the two exact glove resources
- Hook-free resource resolver with continuous reload reconciliation
- Exact executable, item-system, VTable, RTTI, and resource validation
- Conflict detection and transactional rollback instead of blind overwrites
- Does not edit `DS2.exe`, game archives, or save files on disk

**Requirements and important notes**

- DEATH STRANDING 2: ON THE BEACH for PC / Steam
- Supported executable version: `DS2.exe 1.10.89.0`
- A compatible external 64-bit ASI loader
- Both included files placed beside `DS2.exe`

This mod extends the gloves' native magnetic cargo pickup. It does not recreate
the Sticky Gun or Sticky Cannon targeting, rope, or other special functions.
Native visibility, obstruction, target-selection, cargo, and state rules still
apply, so some objects may remain unavailable despite being inside the selected
distance.

Save/load, area transitions, vehicle pickup, multiple cargo pieces, height
differences, and obstacles remain useful regression checks. After a game
update, do not assume compatibility: the mod rejects unknown executable builds
and may require an updated release.

**Shout outs**

Thanks to ShadelessFox for Odradek and the DS2 RTTI research tools, and to the
player who proposed the mod and confirmed the Level 2, Level 1, and no-glove
pickup-range separation in game.
