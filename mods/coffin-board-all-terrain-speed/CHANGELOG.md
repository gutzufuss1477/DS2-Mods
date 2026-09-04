# Changelog

## 1.1.0

- Added optional Floating Carrier support. With
  `AllowFloatingCarrier=1`, a loaded carrier can remain attached while the
  player mounts and rides the Coffin Board.
- Preserved the exact linked carrier through high-speed travel, water
  crossings, collisions, and large jumps while retaining native manual
  detach and reattach behavior after dismounting.
- Suppressed only the now-invalid native overextension HUD notification and
  Sam dialogue for the exact active Coffin Board/carrier pair. Gameplay
  validation recorded 77 producer events and 77 successful suppressions.
- Fixed an intermittent startup race that could leave the configured speed
  inactive. The bounded discovery worker now rechecks the already-loaded
  Coffin resource every 500 ms until the patch completes.
- Expanded build-locked executable validation and synthetic coverage for the
  Coffin-only mount, link-preservation, detach, warning, and late-resource
  discovery paths.
- Kept native cargo and collision damage unchanged.
- Removed ineffective experimental carrier follow-step scaling and incorrectly
  identified global distance writes from the public release path.

## 1.0.0

- Finalized the tested `500%` all-terrain speed and `400%` acceleration profile.
- Restored native steering and wet lateral grip by removing their test keys.
- Disabled runtime telemetry and added a dedicated drive trampoline containing
  no telemetry sampling instructions.
- Reduced the release INI to two settings and added package-level enforcement.
- Added final-profile synthetic checks for unchanged handling values and
  inactive steering/telemetry paths.
- Added exact validation of native StreamingSystem vtable slot 4
  (`RemoveListener`, RVA `0x26F6EE0`) before listener registration.
- Made completion publication and listener ownership atomic across the
  streaming callback and worker threads.
- Finalized the listener as a bounded one-shot lifecycle: the worker removes
  it outside callbacks immediately after patch completion, or after a
  60-second discovery window.
- Added timestamped lifecycle results and synthetic Add/Remove, mismatch,
  callback-order, exactly-once, idempotence, and timeout-path checks.
