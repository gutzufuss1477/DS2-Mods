# Coffin Board Reworked v1.83.0

Build-locked ASI mod for DEATH STRANDING 2: ON THE BEACH, Steam PC 1.10.89.0.

The Coffin Board gets configurable all-terrain speed and acceleration, loaded
Floating Carrier support, Chiral Network boundary crossing, and mounting after
dismounting outside the Chiral Network. The outside remount workflow is
verified in-game with an attached Floating Carrier.

## Features

- 500% speed and 400% acceleration defaults, configurable in the INI.
- Same raised speed limits on land and water.
- Ride with a loaded Floating Carrier attached.
- Preserve the linked carrier through high-speed travel, water crossings,
  collisions, and large jumps.
- Cross the Chiral Network boundary while riding the Coffin Board.
- Dismount and remount the Coffin Board outside the Chiral Network.
- Keep Pickup and Tri-Cruiser mounting on their native paths.

## Installation

The Nexus archive contains exactly these two root-level files:

- `ds2_coffin_board_all_terrain_speed.asi`
- `ds2_coffin_board_all_terrain_speed.ini`

Close the game, extract the archive, and place both files beside `DS2.exe`.
A compatible external 64-bit ASI loader is required. Replace both files when
updating. Restart DS2 after changing any INI setting.

## Configuration

```ini
[CoffinBoardAllTerrainSpeed]
Enabled=1
SpeedPercent=500
AccelerationPercent=400
AllowFloatingCarrier=1
EnableNetworkTraversal=1
EnableOutsideNetworkMount=1
```

`SpeedPercent` accepts 100-1000. `AccelerationPercent` accepts 100-500.
`AllowFloatingCarrier=1`, `EnableNetworkTraversal=1`, and
`EnableOutsideNetworkMount=1` enable the corresponding reworked features.

## Validation and compatibility

The ASI validates the exact supported Steam executable, resource identity,
vtable slots, and instruction anchors before applying changes. Version 1.83.0
was tested in-game for boundary crossing, outside dismount/remount, and the
same flow with an attached Floating Carrier.

Steering, wet grip, manual carrier detach/reattach, cargo damage, and
collision damage stay native. The mod does not modify DS2.exe, game archives,
or save files on disk. Remove the ASI after a game update until compatibility
is confirmed.

See `NEXUS_DESCRIPTION_BBCODE.txt` for the publish-ready Nexus description,
`CHANGELOG.md` for release history, and `docs/NEXUS_V1.83.0_UPLOAD.md` for the
exact Nexus file metadata.
