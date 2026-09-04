**Description**

Coffin Board Overdrive combines configurable all-terrain speed with reliable
Floating Carrier support in DEATH STRANDING 2: ON THE BEACH.

The packaged profile uses `SpeedPercent=500`, giving the Coffin Board
`300/400 km/h` normal/boost caps on both land and water. The tested
`AccelerationPercent=400` profile helps it reach the higher speed sooner.

With `AllowFloatingCarrier=1`, you can mount and ride the Coffin Board while
a loaded Floating Carrier remains attached. The exact linked carrier is
preserved through high-speed travel, water crossings, collisions, and large
jumps that would normally exceed the native separation boundary. After
dismounting, it can still be detached, reattached, and used again normally.

Steering, wet grip, generic vehicle handling, cargo damage, and collision
damage remain native.

**Installation instructions**

1. Close DEATH STRANDING 2 completely.
2. Install a compatible external 64-bit ASI loader. If DS2 already loads other
   `.asi` mods correctly, you can use the existing loader.
3. Extract the downloaded archive.
4. Copy `ds2_coffin_board_all_terrain_speed.asi` and
   `ds2_coffin_board_all_terrain_speed.ini` beside `DS2.exe`.
5. Fully restart the game and deploy a newly created Coffin Board.

Replace both files when updating from an earlier version. To uninstall, close
the game, remove both files, and restart. Never replace or unload the ASI while
the game is running.

**Configuration**

```ini
[CoffinBoardAllTerrainSpeed]

; Top speed on land and water. 500 = 300 km/h normal / 400 km/h boost (range: 100-1000).
SpeedPercent=500

; Acceleration toward the new top speed. 400 = about 4x native (range: 100-500).
AccelerationPercent=400

; 1 lets you mount and ride the Coffin Board while a Floating Carrier is attached. 0 keeps the native restriction.
AllowFloatingCarrier=1
```

`SpeedPercent` accepts `100-1000`.
`AccelerationPercent` accepts `100-500`.
`AllowFloatingCarrier=1` enables the new feature; `0` restores the native
mount restriction. Restart DS2 after changing a setting.

**Main features**

- Same configurable normal and boost speed limits on land and water
- Tested `500%` speed and `400%` acceleration profile
- Mount and ride with a loaded Floating Carrier still attached
- Coffin-only protection against native high-separation detach events
- Exact-pair filtering of the obsolete overextension HUD warning and Sam
  dialogue
- Native manual detach and reattach behavior after dismounting
- Native steering, wet grip, cargo damage, and collision damage
- Gameplay-validated startup ordering for speed and Floating Carrier hooks
- Exact executable, resource, vtable, and instruction-signature validation
- No edits to `DS2.exe`, game archives, or save files on disk

**Requirements**

- DEATH STRANDING 2: ON THE BEACH for PC / Steam
- Supported executable: `DS2.exe 1.10.89.0`
- A compatible external 64-bit ASI loader
- Both included files placed beside `DS2.exe`

The game may retain a previously created Coffin Board's old gear table. Fully
restart DS2 and deploy a new board after installing the mod or changing its
speed configuration.

Gameplay validation included land and water travel, collisions, and large
jumps. One confirmation run recorded 77 native overextension notifications
and suppressed all 77; no warning text or Sam dialogue appeared and the
carrier stayed attached.

This does not disable cargo damage. Hard impacts, falls, water, and collisions
can still damage cargo normally. Extreme speeds can also affect camera comfort
and world streaming. Back up your save and test offline first.

Public v1.1.2 excludes the ineffective experimental carrier follow-step hook
and discarded global “distance” writes. After a game update, remove the ASI
until a matching build-locked release is available.

**Shout outs**

Thanks to ShadelessFox for Odradek and the DS2 RTTI research tools, and to the
players who tested speed, acceleration, shutdown behavior, high-speed carrier
jumps, warning suppression, manual reconnection, and native cargo damage.
