DS2 APAS Memory Costs 3.0.0-rc.1 - Episode-2 menu test
==========================================================

Purpose
-------
This package tests whether the existing APAS-specific native UnlockAll path creates
the APAS state required by the ring-menu item on a new Episode-2 save.

It does NOT patch the UI or a story fact. It uses the same verified executable
anchors and native APAS node-creation route as the unified release candidate.

Install
-------
1. Back up the test save. UnlockAll may save unlocked nodes permanently.
2. Fully close the game.
3. Copy both files beside DS2.exe, replacing any old APAS ASI/INI.
4. Ensure no other APAS ASI is loaded from an ASI-loader folder.
5. Start the game and load the Episode-2 save.

Expected startup check
----------------------
ds2_apas_memory_costs.log must report READY and UnlockAll=1.
LATE_LOAD, UNSUPPORTED_OR_CONFLICT, or CONFLICT means do not evaluate the test.

Test result to report
---------------------
Open the ring device menu at the first facility in Episode 2 and report only:
- APAS item visible: yes/no
- if visible, can the APAS screen be opened: yes/no
- ds2_apas_memory_costs.log

Interpretation
--------------
If APAS appears, no separate early-menu patch is needed: the supported UnlockAll
path already creates the native APAS state the UI needs. If it does not appear,
the missing gate is outside APAS node creation and the next build will target that
specific UI predicate.

Files
-----
ds2_apas_memory_costs.asi
  SHA-256: 1E8176A00062342899B0A15F609F72B3C0EEB112F70791376D78CA7FB28D143F
