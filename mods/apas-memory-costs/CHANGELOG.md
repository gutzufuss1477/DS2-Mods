# Changelog

## v1.0.3

- Promoted the performance-test implementation to the main release.
- Removed permanent 1-second APAS scanning during normal gameplay.
- Reduced `VirtualQuery` calls during each patch pass.
- Uses light polling only while the APAS resource table is still loading.
- Worker exits completely after the APAS table is populated and stable.
- APAS cost behaviour remains unchanged.
- No hard game-version verification.

## v1.0.2

- Rebuilt the ASI with a more conventional PE layout to avoid Nexus quarantine.
- Kept the runtime APAS modification logic unchanged.

## v1.0.1

- Removed `VirtualProtect` usage.
- APAS resources are modified only when their memory page is already writable.
- Removed unnecessary persistent lock-file behaviour.

## v1.0.0

- Initial public release.
- Added global INI-controlled APAS Enhancement Memory cost.
- Default `GlobalCost=1`.
