**Description**

Coffin Board All-Terrain Speed gives the Coffin Board the same raised speed
limits on land and water in DEATH STRANDING 2: ON THE BEACH.

The packaged profile uses `SpeedPercent=500`, producing normal/boost caps of
`300/400 km/h` on both surfaces, and the tested `AccelerationPercent=400`
profile so the board reaches its higher speed sooner. Steering and wet grip
remain native, and runtime telemetry is disabled.

**Installation instructions**

1. Close DEATH STRANDING 2 completely.
2. Install a compatible external 64-bit ASI loader. If DS2 already loads other
   `.asi` mods correctly, the existing loader can be used.
3. Extract the downloaded archive.
4. Copy `ds2_coffin_board_all_terrain_speed.asi` and
   `ds2_coffin_board_all_terrain_speed.ini` into the game folder beside
   `DS2.exe`.
5. Fully restart the game and deploy a newly created Coffin Board.

To uninstall, close the game, remove both files, and restart. Do not replace,
remove, or unload the ASI while the game is running.

**Configuration**

```ini
[CoffinBoardAllTerrainSpeed]

; Top speed on land and water. 500 = 300 km/h normal / 400 km/h boost (range: 100-1000).
SpeedPercent=500

; Acceleration toward the new top speed. 400 = about 4x native (range: 100-500).
AccelerationPercent=400
```

`SpeedPercent` accepts `100-1000`. `AccelerationPercent` accepts `100-500`.
Restart the game after changing either value and deploy a new board so its
component receives the updated gearing.

**Main features**

- Same configurable normal and boost speed limits on land and water
- Tested `500%` speed profile with `300/400 km/h` configured caps
- Tested `400%` acceleration profile
- Native steering angle, steering response, and wet lateral grip
- No runtime speed or steering telemetry
- Coffin-only drive hook; other vehicles remain untouched
- Exact executable, resource, vtable, and instruction-signature validation
- Transactional resource writes with rollback on failure
- Bounded one-shot streaming listener removed outside callbacks
- Does not edit `DS2.exe`, game archives, or save files on disk

**Requirements and important notes**

- DEATH STRANDING 2: ON THE BEACH for PC / Steam
- Supported executable version: `DS2.exe 1.10.89.0`
- A compatible external 64-bit ASI loader
- Both included files placed beside `DS2.exe`

The game may retain a previously created Coffin Board's old gear table. Fully
restart DS2 and deploy a new board after installing the mod or changing its
configuration.

The listener removes itself after the Coffin resource is patched, or after a
60-second discovery window if the resource is not found. If a later region
transition appears to recreate the Coffin physics resource, restart DS2 before
deploying another board.

Extreme speeds can affect collision behavior, camera comfort, and world
streaming. Back up your save and test offline first. After a game update, do
not assume compatibility; remove the ASI until a matching release is available.

**Shout outs**

Thanks to ShadelessFox for Odradek and the DS2 RTTI research tools, and to the
players who tested the speed, acceleration, steering, and shutdown behavior.
