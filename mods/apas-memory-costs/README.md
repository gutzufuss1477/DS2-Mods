# DS2 APAS Memory Costs - unified edition

**3.0.0-rc.3: release candidate with limited gameplay coverage.**

One `ds2_apas_memory_costs.asi` and one `ds2_apas_memory_costs.ini` provide both
configurable APAS Memory costs and optional early APAS unlocks. No separate
Performance Hotfix or Unlock All build is needed.

```ini
[APASMemoryCosts]
Enabled=1
GlobalCost=1

[APASUnlocks]
UnlockAll=0
```

- `GlobalCost=0`: normally paid enhancements are free.
- `GlobalCost=1..1000000`: exact global cost for normally paid enhancements.
- Special base nodes (IDs 0..3) and natively free nodes retain their native costs.
- `Enabled=0`: vanilla costs. This switch does not disable Unlock All.
- `UnlockAll=0`: normal APAS progression; the safe default.
- `UnlockAll=1`: bypass APAS prerequisites when the native APAS system is available.
  Unlocks nodes without automatically equipping them or unlocking the APAS menu.

Back up saves before enabling Unlock All. Native unlocks may persist in saves;
turning the option off or uninstalling does not undo already saved unlocks.

## Install

Close the game. Extract the single ZIP in `release/` and copy its ASI and INI beside
`DS2.exe`. A compatible 64-bit ASI loader is required. Replace the old APAS files;
remove additional or renamed APAS ASIs from loader folders. Restart after INI edits.
Mod Suite 1.7.0 embeds this unified candidate. Older suite versions still contain
older APAS builds and can overwrite this candidate during an APAS repair/update.

## Implementation

Costs are applied on the native APAS node-construction path, before the game copies
the cost into its node cache. Later-created nodes and loaded saves take the same
path. There is no timer, recurring table scan, retained resource-pointer cache or
permanent background worker.

The exact supported research target is **Steam DS2.exe 1.10.89.0**. Code anchors,
PE metadata, resource type and startup state are checked. Unknown builds and
conflicting patches are rejected. The INI and startup result are recorded in the
local `ds2_apas_memory_costs.log`; check for `READY`. The image format and exact
code/vtable anchors are mandatory. A rejected startup records the failed target
component without applying any change.

## Verification and limits

Automated checks cover strict INI parsing, all node IDs, base/free-node preservation,
late node creation, native calling convention, executable relays, rollback, all four
feature combinations, unsupported/conflicting targets, loader lifetime, and the
actual game accounting/activation functions in an isolated mapped image.

These checks do **not** establish that the specific Nexus purchasing report is fixed
or measure gameplay FPS. Affected saves were not available. The user reported
working one-point costs on an Episode 9 save with the preliminary build, and
authorized Nexus publication with the remaining checks disclosed. Final startup,
new-game, UI, load/save and Unlock All gameplay verification remains incomplete.

- [German game-test instructions](docs/TESTANLEITUNG_DE.md)
- [Current test status](docs/TEST_STATUS.md)
- [Technical notes](docs/TECHNICAL_NOTES.md)
- [Reproducible build](docs/BUILD.md)
- [Prepared Nexus description and consolidation](docs/NEXUS_DESCRIPTION.md)

Older version notes remain historical evidence; Git history retains the old builds.
