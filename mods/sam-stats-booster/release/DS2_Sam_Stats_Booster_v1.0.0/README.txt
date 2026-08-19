SAM STATS BOOSTER v1.0.0
DEATH STRANDING 2: ON THE BEACH

PURPOSE
Multiplies only the positive progression gains that DS2 normally awards to
Sam's 12 visible activity-based Stats. DS2 remains responsible for eligibility,
storage, thresholds, level effects, the Stats menu and save persistence.

SUPPORTED GAME BUILD
Steam PC DS2.exe 1.10.89.0
DS2.exe SHA-256:
BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B

INSTALLATION
1. Install a compatible external x64 ASI loader for Death Stranding 2.
2. Copy SamStatsBooster.asi and SamStatsBooster.ini beside your other DS2 ASI mods.
3. Start the game.
4. SamStatsBooster.log should report State: ACTIVE.

CONFIGURATION
The default multiplier is 5.0x. Edit SamStatsBooster.ini while the game is
closed. The supported range is 1.0x to 100.0x. Each visible category can be
enabled or disabled independently.

Recommended values:
- 2.0x: light acceleration
- 3.0x: balanced acceleration
- 5.0x: clearly faster progression (release default)
- 10.0x or higher: very fast progression/testing

IMPORTANT UI BEHAVIOUR
The mod does not add or change HUD level-up notifications. Progress is applied
immediately through DS2's original mutators, but Sam's Stats menu is the
reliable place to check the new level and progress bar.

COMPATIBILITY
Designed to coexist with Porter Grade Booster. Another mod that patches the
same dedicated Sam Stats producer windows may conflict. On an unsupported or
mismatched executable, this ASI fails closed and does not install its hooks.

UNINSTALLATION
Remove SamStatsBooster.asi, SamStatsBooster.ini and the generated
SamStatsBooster.log. Progress already committed through DS2 remains in the save.

ASI LOADER
Not included.
