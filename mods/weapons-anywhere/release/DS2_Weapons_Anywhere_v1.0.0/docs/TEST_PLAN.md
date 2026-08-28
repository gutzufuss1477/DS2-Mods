# Runtime regression plan

Use a separate test save and begin offline. Firearms and grenades passed the initial 1.0.0 runtime check; this matrix remains useful for additional categories and future game versions.

## Baseline

1. Set `Enabled=0`, restart DS2 and enter a facility weapons-prohibited area.
2. Confirm that the warning ring/announcement appears and the selected weapon is blocked or holstered.
3. Record the exact facility, weapon and observed message.

## Patched run

1. Set `Enabled=1` and `DebugLog=1`, then restart DS2.
2. Confirm that the log contains `ACTIVE` and no signature error.
3. Enter the same area and confirm that the warning ring and announcement still appear.
4. Verify that the same weapon can be equipped, aimed and fired.

## Action matrix

- handgun or assault rifle: equip, ADS, fire, reload;
- bola/maser/sticky weapon: equip and fire;
- grenade or other throwable: ready and throw;
- melee: light and heavy attack;
- PCC/build tool: equip and place preview;
- strand and blood boomerang where available;
- vehicle-mounted weapon or turret where available;
- enter/leave the zone repeatedly with a weapon already equipped;
- save, reload and fast travel with the mod enabled;
- disable the mod through the INI and confirm the native restriction returns after restart.

## Failure criteria

Stop the test and remove the ASI if the game crashes, the ring/announcement disappears, a facility enters an invalid state, or a save/load action behaves unexpectedly. Preserve the debug log and identify the exact action that caused the problem.
