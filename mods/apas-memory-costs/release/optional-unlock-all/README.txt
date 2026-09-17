DS2 APAS Memory Costs - Unlock All v1.1.0
=================================================

OPTIONAL REPLACEMENT VERSION

This version already includes the normal APAS Memory Costs functionality.
Do NOT install it together with the normal APAS Memory Costs ASI.

What it does
------------
- Sets APAS Enhancement Memory costs through the INI.
- Default: every APAS Enhancement costs 1 point.
- Unlocks all valid APAS Enhancements by bypassing only the APAS prerequisite check.
- Uses the game's own native APAS unlock routine after the prerequisite bypass.

Default configuration
---------------------
[APASMemoryCosts]
Enabled=1
GlobalCost=1

[APASUnlocks]
UnlockAll=1

Installation
------------
1. Remove/replace the normal ds2_apas_memory_costs.asi if already installed.
2. Copy ds2_apas_memory_costs.asi and ds2_apas_memory_costs.ini beside DS2.exe.
3. A compatible 64-bit ASI loader is required.
4. Fully restart the game.

Important
---------
This optional version can unlock Enhancements earlier than intended by normal
story/facility progression.

The unlock is performed through the game's native APAS unlock path and may therefore
be persisted in your save. Removing the mod does not guarantee that already unlocked
Enhancements will become locked again.

Back up your save before first use if you may want to return to vanilla progression.

The mod does NOT globally complete missions, change facility friendship, alter Porter
Grade, or spoof unrelated unlock systems.

Performance
-----------
The APAS cost worker keeps the optimized v1.0.3 behaviour:
it stops scanning once the APAS resource table is populated, patched and stable.

Uninstall / return to normal version
------------------------------------
Replace this ASI and INI with the normal APAS Memory Costs version, or remove both files.

Already unlocked Enhancements may remain unlocked if the game has saved that state.
