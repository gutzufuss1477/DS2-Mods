DS2 APAS Memory Costs 3.0.0
Unified release

INSTALL
Copy ds2_apas_memory_costs.asi and ds2_apas_memory_costs.ini beside DS2.exe.
A compatible 64-bit ASI loader is required.
Replace older APAS files and remove separate/renamed APAS ASIs.
Restart the game after changing the INI.

DEFAULT SETTINGS
[APASMemoryCosts]
Enabled=1
GlobalCost=1

[APASUnlocks]
UnlockAll=0
EarlyAccess=0

COST SETTINGS
Enabled=1 applies the configured APAS Memory cost.
Enabled=0 keeps vanilla costs.
GlobalCost=0 makes normally paid enhancements free.
GlobalCost=1..1000000 sets their exact global cost.
Special base nodes and natively free nodes retain native costs.

OPTIONAL UNLOCKS
UnlockAll=1 bypasses APAS node prerequisites through the native APAS path.
EarlyAccess=1 exposes APAS in the Ring Device before its normal story unlock.
For complete early APAS access, enable both UnlockAll=1 and EarlyAccess=1.
The mod does not write global story/facility facts and preserves special Ring
Device restrictions.

SAVE WARNING
Back up your save before using UnlockAll or EarlyAccess. Native unlocks/located
nodes may persist in the save after disabling or uninstalling the mod.

COMPATIBILITY
Supported target: Steam DS2.exe 1.10.89.0.
Unknown builds or conflicting target code are rejected.
Check ds2_apas_memory_costs.log for READY and the active settings.
The mod changes memory only while the game runs; DS2.exe is not modified on disk.

TEST STATUS
Automated configuration, native accounting/activation, relay, mapped-image,
rollback and loader-lifetime tests pass.
Live Episode 2 testing confirmed the APAS menu appears with EarlyAccess enabled
and that enhancements can be opened, purchased and unlocked.
No recurring gameplay scanner or persistent polling worker is used.
