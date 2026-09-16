DS2 APAS Memory Costs v1.0.3

Installation:
Copy ds2_apas_memory_costs.asi and ds2_apas_memory_costs.ini beside DS2.exe.
A compatible 64-bit ASI loader is required.

Default:
GlobalCost=1

0 = free
1 = one APAS Memory point per enhancement
2..1000000 = exact custom global cost

Performance:
v1.0.3 no longer keeps a permanent APAS scan running during normal gameplay.
The worker exits after the APAS resource table is populated, patched and stable.

Restart the game after changing the INI.

The mod works at runtime only and does not modify DS2.exe or save files on disk.
