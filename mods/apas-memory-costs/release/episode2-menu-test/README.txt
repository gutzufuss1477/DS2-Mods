DS2 APAS Memory Costs 3.0.0-rc.3 - Episode-2 menu test
==========================================================

Purpose
-------
The earlier Episode-2 attempt was invalid: its log said
UNSUPPORTED_OR_CONFLICT, so the ASI installed no hook or UnlockAll patch. rc.2
identified a loader-only PE metadata change. rc.3 accepts that metadata-only
change but still requires the exact image format and every code/vtable anchor. If
startup says READY, it tests whether the native UnlockAll path creates the APAS
state required by the ring-menu item on a new Episode-2 save.

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
LATE_LOAD or CONFLICT means do not evaluate the test. If it reports
UNSUPPORTED_OR_CONFLICT, send the complete log: it names the failing image-format
or exact code/vtable anchor without changing game code.

Test result to report
---------------------
First report the complete ds2_apas_memory_costs.log. Only if it says READY, also
open the ring device menu at the first facility in Episode 2 and report:
- APAS item visible: yes/no
- if visible, can the APAS screen be opened: yes/no

Interpretation
--------------
If APAS appears, no separate early-menu patch is needed: the supported UnlockAll
path already creates the native APAS state the UI needs. If it does not appear,
the missing gate is outside APAS node creation and the next build will target that
specific UI predicate.

Files
-----
ds2_apas_memory_costs.asi
  SHA-256: DB1D9FDBC612E90452AD008B1CDAD926712A771F7F744BF9F563C7B1F3C3076F
