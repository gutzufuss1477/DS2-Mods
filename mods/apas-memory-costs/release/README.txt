DS2 APAS Memory Costs 3.0.0-rc.5
Unified release candidate - limited gameplay coverage

ONE ASI, ONE INI
Copy ds2_apas_memory_costs.asi and ds2_apas_memory_costs.ini beside DS2.exe.
A compatible 64-bit ASI loader is required.
Replace the old APAS files. Remove any renamed or extra APAS ASIs in loader folders.
Do not keep a separate Performance Hotfix or Unlock All ASI installed.
If you use the Mod Suite manager, its old payload can overwrite this candidate
when you repair/update APAS. Install this standalone candidate after that operation.

DEFAULT SETTINGS
[APASMemoryCosts]
Enabled=1
GlobalCost=1

[APASUnlocks]
UnlockAll=0

GlobalCost: 0 = free; 1 = one point; 2..1000000 = custom global cost.
Special base nodes and natively free nodes retain their native costs.
Enabled=0 keeps vanilla costs. UnlockAll remains independent of Enabled.
UnlockAll=1 bypasses APAS prerequisites when the native APAS system is available.
It unlocks nodes; you still activate/equip enhancements yourself.
Restart the game after editing the INI. Use 0 or 1 for switches.
Put comments on separate lines; do not append them to setting values.

SAVE BEHAVIOR
Back up your save before enabling UnlockAll=1. Native unlocked enhancements may
be saved permanently. Turning the option off or removing the ASI does not relock
already saved enhancements. Costs are changed in memory; the EXE file is untouched.

COMPATIBILITY AND DIAGNOSTICS
Supported research target: Steam DS2.exe 1.10.89.0.
Other executables or conflicting code changes are rejected without installing hooks.
The local ds2_apas_memory_costs.log records settings and startup status.
READY means installation succeeded; it is not proof of a completed gameplay test.
LATE_LOAD means restart the game with the ASI loader loading the mod at startup.
UNSUPPORTED_OR_CONFLICT means the image format or an expected code anchor does
not match. The image format plus all exact code/vtable anchors must match. The
loader may change PE metadata in memory and make code execute-only; this is logged
but does not replace the code-anchor checks. Validation may temporarily add read
permission to one code page, then restores its original protection before it
continues. A rejected log names the exact AnchorRva when applicable.

WHAT CHANGED
No recurring APAS scans or persistent background worker.
Costs are applied during native node creation, before the cached node cost is copied.
Base nodes retain their native costs. New nodes and save reloads use the same hook.
One file replaces the former main, performance-hotfix and unlock-all variants.

TEST STATUS
Automated INI, cost, executable relay, native accounting/activation, patch rollback
and module-lifetime checks pass. The reported Nexus purchasing issue has not been
reproduced from a user's affected save. New-game behavior, APAS UI, load transitions,
Unlock All progression and actual gameplay FPS still need user verification.
An Episode 9 save showed selectable enhancements and one-point costs on a
preliminary build. The packaged build adds startup/lifetime protection; its final
loader behavior has automated coverage but no completed gameplay verification.
A fresh save just after the intro had no APAS menu yet. UnlockAll does not unlock
the APAS menu itself. The original early-game purchasing report remains unconfirmed.
