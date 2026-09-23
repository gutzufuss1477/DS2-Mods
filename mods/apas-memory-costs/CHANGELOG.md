# Changelog

## 3.0.0-rc.1 - Unified release candidate

- One ASI and one INI. Unlock All is now an independent INI option, off by default.
- Replaced the polling worker with a native node-construction hook. No recurring
  resource scans or permanent background worker; later nodes and reloads use the hook.
- Changes costs before the native copy into the node's cached cost.
- Preserves special base nodes (IDs 0..3) and natively free nodes.
- Rejects unsupported executables, modified anchors and installation after the APAS
  manager already exists. Supports the researched Steam 1.10.89.0 executable only.
- Validates settings strictly and records startup results in a local log.
- Uses aligned atomic code replacement, preserves native instruction boundaries,
  rolls back owned changes on a partial installation failure, and retains callback
  code for the process lifetime. Repeated initialization is ignored.
- Unlock All skips this APAS updater's fact and grade prerequisites while retaining
  native system availability guards and native unlock/creation behavior.
- Added executable relay, mapped-image, actual native accounting/activation,
  configuration and loader-lifetime regression tests.
- Candidate only: the specific Nexus purchasing report, in-game UI, progression,
  save reload and gameplay FPS still require user testing. No stable release claim.

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
