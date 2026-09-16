# Short description

Scale Sam's proficiency level bonuses per category and per Level 1-5 through a simple INI, without changing proficiency gain, unlock thresholds or save data.

# Description

**Proficiency Bonus Multiplier** lets you independently scale the gameplay bonuses granted by Sam's proficiency levels.

The mod modifies the runtime bonus tables used by the proficiency system. It does **not** boost proficiency XP/gain, unlock levels, alter level thresholds or permanently edit your save.

The included configuration is completely neutral: every multiplier is set to `1.0`, so nothing changes until you edit the INI.

Examples:
- `1.0` = vanilla bonus strength
- `2.0` = double the vanilla bonus strength
- `0.5` = half the vanilla bonus strength
- `0.0` = remove the supported level-dependent bonus

You can configure Level 1 through Level 5 separately for each supported category:

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

Melee Combat scales both **Attack Power** and **Consecutive Attacks**. Consecutive Attacks are rounded to a whole number and never go below 1.

For firearm proficiencies, the verified level-dependent tables include **reload speed, reticle movement and recoil**, plus one additional internal firearm row from the same descriptor table.

## Safety

The default setting `ClampReductionFactorsAtZero=1` prevents large multipliers from pushing reduction-type internal factors below zero. This effectively caps effects such as recoil/reticle reduction at `-100%` instead of allowing negative internal multipliers.

The mod also verifies the supported game build and all known vanilla proficiency descriptor tables before applying any changes. If validation fails, it aborts instead of writing to an unexpected structure.

## Important: level preview vs active level

Sam's Stats can preview higher levels even if Sam has not unlocked them yet. The preview will show the modified bonus table for the selected level, but the active gameplay bonus is still determined by Sam's currently unlocked proficiency level.

# Installation instructions

1. Install/use your normal DS2 ASI loader.
2. Copy `DS2_ProficiencyBonusMultiplier.asi` and `DS2_ProficiencyBonusMultiplier.ini` into the folder from which your DS2 ASIs are loaded, normally next to `DS2.exe`.
3. Edit the INI to your preferred multipliers.
4. Start the game and load a save.

Restart the game after editing the INI because the proficiency tables are patched once after the system initializes.

**Supported game build:** `DS2.exe 1.10.89.0`

Do not use older ProficiencyBonus proof/mapping/test ASIs at the same time.

# Uninstallation

Remove `DS2_ProficiencyBonusMultiplier.asi`. You can also remove the INI and generated log file. Runtime changes disappear on restart.

# Changelog

## v1.0.0
- Initial public release.
- Per-category Level 1-5 bonus multipliers via INI.
- Supports all 12 visible proficiency categories.
- Confirmed firearm reload/reticle/recoil bonus scaling.
- Confirmed Melee Combat Attack Power and Consecutive Attacks scaling.
- Safe reduction-factor clamp enabled by default.
- Strict DS2 build and vanilla descriptor verification before patching.
- Neutral `1.0x` defaults for all public categories.
