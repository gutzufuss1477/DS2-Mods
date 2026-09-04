# Coffin Board Overdrive v1.1.0

Build-locked ASI mod for **DEATH STRANDING 2: ON THE BEACH**, Steam PC
`1.10.89.0`.

The mod gives the Coffin Board the same raised speed limits on land and water,
adds the tested acceleration multiplier, and optionally lets the player ride
while a loaded Floating Carrier remains attached. The carrier link is
preserved through high-speed travel, water crossings, collisions, and large
jumps that would normally trigger the native separation boundary.

The final profile deliberately leaves steering, wet grip, generic vehicle
handling, cargo damage, collision damage, game archives, and save files
unchanged.

## Installation

The release archive contains exactly:

- `ds2_coffin_board_all_terrain_speed.asi`
- `ds2_coffin_board_all_terrain_speed.ini`

Copy both files beside `DS2.exe`. An external x64 ASI loader is required.
Replace both files from any earlier version, fully restart the game, and deploy
a newly created Coffin Board because an existing physics component can retain
its old gear table.

To uninstall, close the game, remove both files, and restart. Never replace,
remove, or unload the ASI while DS2 is running.

## Configuration

The complete release INI is:

```ini
[CoffinBoardAllTerrainSpeed]

; Top speed on land and water. 500 = 300 km/h normal / 400 km/h boost (range: 100-1000).
SpeedPercent=500

; Acceleration toward the new top speed. 400 = about 4x native (range: 100-500).
AccelerationPercent=400

; 1 lets you mount and ride the Coffin Board while a Floating Carrier is attached. 0 keeps the native restriction.
AllowFloatingCarrier=1
```

`SpeedPercent=500` derives normal/boost caps of `300/400 km/h` on
both land and water and changes the final gearing from `15` to `3`.
Accepted values are `100-1000`.

`AccelerationPercent=400` publishes 20x drive force to compensate the 0.2x
gear ratio and then provide nominally 4x native effective drive moment.
Accepted values are `100-500`.

`AllowFloatingCarrier=1` enables the Coffin-only mount and link-preservation
guards. Set it to `0` to keep the game's native restriction. Restart DS2
after changing any setting.

## Floating Carrier behavior

With carrier support enabled, the currently linked loaded carrier remains
attached when mounting the Coffin Board. While that exact board/carrier pair is
active, the mod prevents the native high-separation detach transaction and
suppresses its obsolete HUD notification and Sam dialogue. The carrier can
still be detached and reattached normally after dismounting, then used again
on a later ride.

Gameplay validation covered high-speed land and water travel, collisions, and
large jumps. One confirmation run recorded 77 native overextension
notifications and suppressed all 77; the carrier stayed attached and neither
the warning text nor Sam's reaction appeared.

This feature does not make cargo invulnerable. Cargo and containers can still
take native impact, fall, water, and collision damage.

## Safety and validation

The ASI accepts only the validated Steam executable. It requires exact PE
metadata, StreamingSystem functions, Coffin resource identity/layout, drive
hook signature, Coffin physics vtable, and Coffin-only carrier instruction
anchors. Other vehicles are not targeted.

The speed-resource transaction changes only the four speed caps, final
gearing, and the slip threshold required for the raised speed range. Drive
force remains neutral until every resource write succeeds; failures roll back
completed writes.

The listener is registered early and checks the resident Coffin group every
500 ms during its bounded 60-second discovery window. This closes the startup
timing gap that could previously leave `SpeedPercent=500` inactive when the
resource became resident around listener registration. After successful
patching, or when the discovery window ends, the worker removes the listener
outside callbacks and exits the streaming lifecycle.

Public v1.1.0 does not include the ineffective experimental carrier follow-step
scaling hook or the discarded global “distance” writes. The stable feature is
implemented by exact Coffin-only mount, link-preservation, detach-event, and
notification guards.

Extreme speeds can affect camera comfort, world streaming, and impacts. Back
up your save and test offline first. After a game update, remove the ASI until
a matching build-locked release is available.
