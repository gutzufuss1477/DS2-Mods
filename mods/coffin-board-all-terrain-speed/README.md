# Fast as Fuck Coffin Board + Floating Carrier v1.1.7

Build-locked ASI mod for DEATH STRANDING 2: ON THE BEACH, Steam PC `1.10.89.0`.

The mod gives the Coffin Board the same raised speed limits on land and water, adds the tested acceleration multiplier, and optionally lets the player ride while a loaded Floating Carrier remains attached. The carrier link is preserved through high-speed travel, water crossings, collisions, and large jumps that would normally trigger the native separation boundary.

v1.1.7 fixes the crash introduced by the Floating Carrier RideVehicle preservation hook when mounting non-Coffin vehicles such as the Tri-Cruiser. The native vehicle resolver is now executed first with its original call semantics; Coffin-only carrier handling is applied only after that native resolver returns. Pickup and Tri-Cruiser mounting therefore remain on the native path.

## Installation

The Nexus release archive contains exactly:

- `ds2_coffin_board_all_terrain_speed.asi`
- `ds2_coffin_board_all_terrain_speed.ini`

Copy both files beside `DS2.exe`. An external x64 ASI loader is required. Replace both files from any earlier version and fully restart the game. Deploying a newly created Coffin Board is recommended after changing speed settings because an existing physics component can retain its old gear table.

To uninstall, close the game, remove both files, and restart. Never replace, remove, or unload the ASI while DS2 is running.

## Configuration

```ini
[CoffinBoardAllTerrainSpeed]

; Top speed on land and water. 500 = 300 km/h normal / 400 km/h boost (range: 100-1000).
SpeedPercent=500

; Acceleration toward the new top speed. 400 = about 4x native (range: 100-500).
AccelerationPercent=400

; 1 lets you mount and ride the Coffin Board while a Floating Carrier is attached. 0 keeps the native restriction.
AllowFloatingCarrier=1
```

`SpeedPercent=500` derives normal/boost caps of `300/400 km/h` on land and water and changes the final gearing from `15` to `3`. Accepted values are `100-1000`.

`AccelerationPercent=400` publishes 20x drive force to compensate the 0.2x gear ratio and then provide nominally 4x native effective drive moment. Accepted values are `100-500`.

`AllowFloatingCarrier=1` enables Coffin-only mount and link-preservation guards. Set it to `0` to keep the native restriction. Restart DS2 after changing any setting.

## Floating Carrier behavior

With carrier support enabled, the currently linked loaded carrier remains attached when mounting the Coffin Board. While that exact board/carrier pair is active, the mod prevents the native high-separation detach transaction and suppresses its obsolete HUD notification and Sam dialogue. The carrier can still be detached and reattached normally after dismounting.

This feature does not make cargo invulnerable. Cargo and containers can still take native impact, fall, water, and collision damage.

## v1.1.7 vehicle compatibility fix

The v1.1.2 carrier-preserve trampoline altered the original RideVehicle resolver call semantics before the game had resolved the mounted vehicle. That happened to work for the Coffin Board but could crash the game when mounting the Tri-Cruiser.

v1.1.7 replays the original resolver block first, preserving the native register/call state. Only after the native resolver has completed does the mod inspect Coffin-specific state and decide whether the carrier-disconnect transaction should be bypassed.

Gameplay validation for v1.1.7 confirmed:

- Pickup mount works normally
- Tri-Cruiser mount works without CTD
- Coffin Board mount works normally
- Coffin Board can be mounted with a loaded Floating Carrier attached
- High-speed profile remains active
- Floating Carrier remains linked
- Obsolete carrier boundary warning / Sam dialogue remain suppressed

## Safety and validation

The ASI accepts only the validated Steam executable. It requires exact PE metadata, StreamingSystem functions, Coffin resource identity/layout, drive-hook signature, Coffin physics vtable, and carrier instruction anchors.

The speed-resource transaction changes only the four speed caps, final gearing, and the slip threshold required for the raised speed range. Drive force remains neutral until every resource write succeeds.

Extreme speeds can affect camera comfort, world streaming, and impacts. Back up your save and test offline first. After a game update, remove the ASI until a matching build-locked release is available.
