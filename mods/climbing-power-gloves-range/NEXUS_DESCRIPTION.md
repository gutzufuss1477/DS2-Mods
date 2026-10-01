[b]Climbing Power Gloves Range v1.1.0[/b]

Pick up cargo from much farther away with the Climbing Power Gloves, and now use magnetic remote cargo pickup with the Combat Power Gloves too.

[b]What's new in v1.1.0[/b]

Combat Power Gloves can now remotely pick up loose cargo beyond normal hand-pickup distance.

Their native combat parameters are preserved, normal striking still works, and the experimental development hook that caused a pickup crash is not part of this release.

[b]Climbing Power Gloves[/b]

Level 1: native 8 m, default 30 m
Level 2: native 10 m, default 50 m

Both ranges remain separately configurable.

[b]Combat Power Gloves[/b]

EnableCargoPickup=1 enables magnetic remote cargo pickup.

Combat Power Gloves do not have separate range values in this release. Their seven native combat parameters remain unchanged.

[b]Installation[/b]

1. Close the game.
2. Make sure a compatible 64-bit ASI loader is installed.
3. Copy ds2_climbing_gloves_range.asi and ds2_climbing_gloves_range.ini beside DS2.exe.
4. Fully restart the game.

To uninstall, remove both files and restart.

[b]INI settings[/b]

[ClimbingGlovesRange]
Enabled=1
Level1RangeMeters=30
Level2RangeMeters=50
DebugLog=0

[CombatGloves]
EnableCargoPickup=1

[b]Requirements[/b]

DEATH STRANDING 2: ON THE BEACH PC / Steam
Supported version: DS2.exe 1.10.89.0
Compatible external 64-bit ASI loader

[b]Tested[/b]

Combat Power Gloves: remote cargo pickup works from distance.
Combat Power Gloves: executing the pickup works without crashing.
Combat Power Gloves: normal strike/attack behavior still works.
Climbing Power Gloves: existing remote pickup still works.

The mod does not modify save files, DS2.exe, or game archives.
