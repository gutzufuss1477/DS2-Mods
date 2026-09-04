# Changelog

## 1.1.2

- Added optional Floating Carrier support with `AllowFloatingCarrier=1`.
- Keeps the linked loaded Floating Carrier attached while mounting and riding the Coffin Board.
- Preserves the carrier through high-speed travel, water, collisions, and large jumps.
- Suppresses the obsolete Floating Carrier boundary HUD warning and Sam dialogue for the active Coffin Board/carrier pair.
- Restored the gameplay-validated startup ordering after the intermediate startup-race cleanup caused `SpeedPercent` to remain inactive.
- Confirmed in-game with `SpeedPercent=500` and `AccelerationPercent=400`: high speed works, the carrier remains attached, and the boundary warning/dialog spam stays suppressed.
- Keeps native manual detach/reattach behavior and native cargo/collision damage.
- Removes the experimental follow-speed, global distance-write, and public trace paths from the release behavior.

## 1.0.0

- Finalized the tested `500%` all-terrain speed and `400%` acceleration profile.
- Restored native steering and wet lateral grip by removing their test keys.
- Disabled runtime telemetry and added a dedicated drive trampoline containing no telemetry sampling instructions.
- Reduced the release INI to two settings and added package-level enforcement.
- Added final-profile synthetic checks for unchanged handling values and inactive steering/telemetry paths.
- Added exact validation of native StreamingSystem vtable slot 4 (`RemoveListener`, RVA `0x26F6EE0`) before listener registration.
- Made completion publication and listener ownership atomic across the streaming callback and worker threads.
- Finalized the listener as a bounded one-shot lifecycle.
