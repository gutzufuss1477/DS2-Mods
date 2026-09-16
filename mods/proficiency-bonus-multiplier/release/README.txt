DS2 Proficiency Bonus Multiplier v1.0.0
=======================================

Game: DEATH STRANDING 2: ON THE BEACH
Supported build: DS2.exe 1.10.89.0

PURPOSE
-------
This mod lets you scale the gameplay bonuses granted by Sam's proficiency
levels independently for Level 1 through Level 5 and for each supported
proficiency category.

It changes the level bonus tables only. It does NOT:
- increase proficiency gain/XP
- unlock proficiency levels
- change the level thresholds
- permanently modify your save

DEFAULT CONFIGURATION
---------------------
The included INI is neutral: every visible category and every level uses 1.0x,
which is exactly the vanilla bonus strength.

Examples:
- 2.0 = double the vanilla bonus
- 0.5 = half the vanilla bonus
- 0.0 = remove the supported level-dependent bonus

INSTALLATION
------------
1. Make sure your normal DS2 ASI loader is installed and working.
2. Copy these two files into the folder from which your DS2 ASIs are loaded
   (normally the game folder next to DS2.exe):
   - DS2_ProficiencyBonusMultiplier.asi
   - DS2_ProficiencyBonusMultiplier.ini
3. Edit the INI as desired.
4. Start the game and load a save.

Restart the game after changing the INI. The tables are patched once after
DS2's proficiency system has initialized.

CONFIGURATION
-------------
Each visible proficiency category has:
- Enabled=1 or 0
- Level1Multiplier through Level5Multiplier

Supported visible categories:
- Terrain Traversal
- Carrying Capacity
- Endurance
- KO Resistance
- Lung Capacity
- Strand Skills
- Melee Combat
- Handguns
- Assault Rifles
- Sniper Rifles
- Machine Guns
- Shotguns

Melee Combat scales both Attack Power and Consecutive Attacks. Consecutive
Attacks are rounded to a whole number and never go below 1.

Firearm categories scale the confirmed level-dependent firearm bonus rows,
including reload speed, reticle movement and recoil. A fourth internal firearm
row is also scaled because it belongs to the same verified descriptor table.

SAFETY
------
[Safety]
ClampReductionFactorsAtZero=1

Leave this enabled for normal use. It prevents reduction-type internal factors
from going below zero when very large multipliers are used. In the UI this
means reductions such as recoil/reticle movement stop at -100% instead of
crossing into negative internal multipliers.

The mod validates the exact supported DS2 build and all verified vanilla
proficiency descriptor tables before writing anything. If validation fails,
the patch is aborted.

A diagnostic file named ds2_proficiency_bonus_multiplier.log is written next
to the ASI when DS2 runs.

IMPORTANT UI NOTE
-----------------
Sam's Stats lets you preview higher proficiency levels even when they are not
unlocked yet. The preview shows the modified table values for that level, but
Sam's active gameplay bonus still comes from his currently unlocked level.

COMPATIBILITY
-------------
Do not run older ProficiencyBonus proof/mapping/test ASIs at the same time.
Other mods that edit the same proficiency descriptor tables can cause the
strict validation to abort, by design.

UNINSTALLATION
--------------
Delete DS2_ProficiencyBonusMultiplier.asi and optionally its INI/log file.
Because the mod patches runtime data only, restarting without the ASI restores
vanilla behavior.
