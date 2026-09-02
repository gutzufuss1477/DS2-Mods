# Changelog

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
