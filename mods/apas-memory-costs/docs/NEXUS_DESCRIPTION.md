# Nexus description - unified release candidate

**Individual APAS Upgrade cost - Unified edition**

Configure APAS Memory costs and optional early unlocks in one INI. One ASI replaces
the old Main, Performance Hotfix and Unlock All downloads.

## Default behavior

- Normally paid APAS Enhancements cost 1 Memory point.
- Normal APAS progression remains enabled (`UnlockAll=0`).
- Special base nodes and naturally free nodes keep their original costs.
- No recurring resource scan or persistent background worker.

## Installation

Close the game and copy `ds2_apas_memory_costs.asi` and `ds2_apas_memory_costs.ini`
beside `DS2.exe`. A compatible 64-bit ASI loader is required.
Replace the previous APAS files and remove any additional/renamed APAS ASIs.
Install only this single package. Restart after editing the INI.

```ini
[APASMemoryCosts]
Enabled=1
GlobalCost=1

[APASUnlocks]
UnlockAll=0
```

`GlobalCost=0` makes normally paid enhancements free. Values 1..1000000 set an exact
global cost. `Enabled=0` keeps vanilla costs. `UnlockAll=1` independently bypasses
APAS prerequisites once the native APAS system is available; enhancements still
need to be activated/equipped manually.

Back up your save before enabling Unlock All. Unlocked enhancements may persist
in the save; disabling this option does not relock saved enhancements.

## Compatibility and support

Supported executable: Steam DS2.exe 1.10.89.0. Unknown executable builds and
conflicting code changes are rejected. See `ds2_apas_memory_costs.log` for startup
status and settings. Include that log, the affected enhancement and your game
version in bug reports.

The mod changes memory while the game runs; it does not modify DS2.exe on disk.

## Release status: 3.0.0-rc.1

This is a release candidate with limited gameplay coverage. An Episode 9 save
showed selectable enhancements and one-point costs with a preliminary build.
The packaged build adds startup/lifetime protection and passed automated checks;
its final loader behavior and the Unlock All toggle still need gameplay confirmation.
The APAS menu was not yet available in a fresh save just after the intro.
UnlockAll does not unlock that menu. The reported early-game purchasing issue
has not been reproduced from an affected save, and no measured FPS comparison
is available. The performance rewrite removes recurring scans; a separate
performance hotfix is no longer needed.

## Maintainer publication note (do not paste this section)

The user authorized publication on 2026-09-23 with the limits above. Keep the
**3.0.0-rc.1** designation and exact tested binary. Do not describe the Nexus
purchasing report or measured FPS as fixed. Upload ONE Main File and archive
the three old active files (IDs
282, 283, 289) rather than deleting their history. Leave no active Optional files.
Use matching title/version fields and set the new main file as the primary download.
