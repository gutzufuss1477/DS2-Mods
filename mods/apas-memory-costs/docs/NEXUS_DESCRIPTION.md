# Prepared Nexus description - publish after gameplay validation

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

## Maintainer publication note (do not paste this section)

Current artifact is **3.0.0-rc.1**. Automated checks pass; gameplay verification
is pending. Do not describe the Nexus purchasing report or measured FPS as fixed
until the user has tested it. After validation, make the final source/binary/version
consistent, upload ONE Main File, and archive the three old active files (IDs
282, 283, 289) rather than deleting their history. Leave no active Optional files.
Use matching title/version fields and set the new main file as the primary download.
