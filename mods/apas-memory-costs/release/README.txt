DS2 APAS Memory Costs v1.0.0
================================

What it does
------------
Makes the APAS Memory cost of all APAS Enhancements configurable through an INI file.

Default:
GlobalCost=1

With the default configuration, every loaded APAS Enhancement costs only 1 APAS Memory point.

Installation
------------
1. Copy ds2_apas_memory_costs.asi and ds2_apas_memory_costs.ini beside DS2.exe.
2. A compatible 64-bit ASI loader must already be installed.
3. Start the game.

Configuration
-------------
[APASMemoryCosts]
Enabled=1
GlobalCost=1
DebugLog=0

GlobalCost:
0 = free
1 = one APAS Memory point per enhancement
2..1000000 = exact custom cost

Restart the game after changing the INI.

Notes
-----
- The mod changes the runtime EnhancementPoint value used by the APAS system.
- It affects both APAS cost accounting and the activation capacity check.
- It does not modify DS2.exe on disk.
- It does not modify save files on disk.
- There is no hard game-version check.
- The current offsets were identified and tested on Steam DS2.exe 1.10.89.0.
  A future game update may require an offset update.

Uninstall
---------
Remove ds2_apas_memory_costs.asi and ds2_apas_memory_costs.ini, then restart the game.
