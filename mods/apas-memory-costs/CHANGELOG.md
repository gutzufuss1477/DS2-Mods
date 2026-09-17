# Changelog

## v1.1.0 - Optional Unlock All

- Added optional replacement build with `UnlockAll=1`.
- Includes the full v1.0.3 APAS Memory Costs functionality.
- Bypasses only the APAS `DevelopmentCondition` prerequisite branch.
- Uses the game's native APAS locate/unlock routine.
- Does not globally spoof mission, facility friendship or Porter Grade state.
- Unlock patch uses an exact local code signature before modifying the APAS branch.
- Keeps the v1.0.3 low-overhead cost worker.
- Rebuilt with conventional `.pdata` and `.reloc` PE sections for Nexus compatibility.
- Added static-analysis research and maintenance notes.
- Added save-persistence warning for early unlocks.

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

## v1.0.1

- Removed unnecessary memory-protection changes from the cost-only path.
- Reduced antivirus heuristic surface.

## v1.0.0

- Initial public release.
- Added global INI-controlled APAS Enhancement Memory cost.
- Default `GlobalCost=1`.
