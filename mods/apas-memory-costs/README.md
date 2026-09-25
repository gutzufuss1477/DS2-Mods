# DS2 APAS Memory Costs

**Version 3.0.0**

One ASI and one INI provide configurable APAS Memory costs plus two optional
progression features: Unlock All and Early Access.

```ini
[APASMemoryCosts]
Enabled=1
GlobalCost=1

[APASUnlocks]
UnlockAll=0
EarlyAccess=0
```

## Settings

- `Enabled=1`: apply the configured Memory cost.
- `Enabled=0`: keep vanilla APAS costs.
- `GlobalCost=0`: normally paid enhancements are free.
- `GlobalCost=1..1000000`: exact cost for normally paid enhancements.
- Special base nodes and natively free nodes retain their native costs.
- `UnlockAll=1`: bypass APAS node prerequisites using the game's native APAS path.
- `EarlyAccess=1`: expose the APAS Ring Device entry before its normal story unlock.
- For full early APAS access, use `UnlockAll=1` and `EarlyAccess=1` together.

Both progression options are disabled by default. Early Access changes only the
APAS menu gate; it does not write global story or facility facts. Special Ring
Device restrictions remain vanilla.

Back up your save before enabling Unlock All / Early Access. Native APAS unlocks
or located nodes may persist after the option is disabled or the mod is removed.

## Install

Close the game and copy `ds2_apas_memory_costs.asi` and
`ds2_apas_memory_costs.ini` beside `DS2.exe`. A compatible 64-bit ASI loader is
required. Replace older APAS files and do not keep separate Performance Hotfix or
Unlock All ASIs installed. Restart the game after INI changes.

Mod Suite 1.8.0 embeds this stable APAS 3.0.0 release. Older suites still contain
previous APAS builds and may overwrite standalone v3.0.0 during repair/update.

## Compatibility

Supported executable: **Steam DS2.exe 1.10.89.0**.
Unknown builds and conflicting target code are rejected before hooks are installed.
The local `ds2_apas_memory_costs.log` records configuration and startup status.

The mod patches memory only while the game is running. It does not modify DS2.exe
on disk.

## Verification

Version 3.0.0 passed automated INI, native cost/accounting, executable relay,
mapped-image, rollback and loader-lifetime tests. Live testing on an Episode 2
save confirmed that Early Access exposes the APAS menu and that APAS enhancements
can be opened, purchased and unlocked with the release configuration.

The implementation uses native APAS creation/locate paths and has no recurring
gameplay scan or persistent polling worker.
