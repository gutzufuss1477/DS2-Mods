# In-game regression plan

Use a backed-up save. The default proximity and facility/no-build options passed the initial 1.0.0 gameplay test; this plan remains for regression checks, future game versions and the optional whitelist bypass. Disable online play if practical and test only disposable structures.

## 1. Loader and version guard

1. Copy only this mod's ASI and INI next to `DS2.exe`.
2. Set `DebugLog=1` and start the game fully.
3. Confirm `ds2_construction_anywhere.log` contains `ACTIVE` and no `ERROR` or `CRITICAL` line.
4. Exit the game before changing the INI.

Expected: unsupported builds or signature differences produce an error log and no memory patch.

## 2. Proximity check in isolation

1. Set `RemoveProximityLimit=1`, `AllowRestrictedAreas=0`, and `IgnorePermittedAreaWhitelist=0`.
2. Place a generator closer than 4.5 metres to a rain shelter or another structure that normally blocks it.
3. Try a second pair of ordinary PCC structures.

Expected: the proximity warning no longer prevents placement. Terrain, roads and physical placement rules should still reject invalid positions.

## 3. Facility/no-build volume in isolation

1. Set `RemoveProximityLimit=0`, `AllowRestrictedAreas=1`, and `IgnorePermittedAreaWhitelist=0`.
2. Return to the facility location shown in the reference screenshot.
3. Try a small, disposable structure near the boundary, then farther inside the restricted area.

Expected: an `UnconstructableArea`-based warning no longer blocks the build. If the same warning remains, record the exact location and proceed to step 4 only on the backed-up test save.

## 4. Experimental permitted-area whitelist

1. Set `IgnorePermittedAreaWhitelist=1` while keeping `DebugLog=1`.
2. Repeat the exact failed placement from step 3.
3. Disable this option again after identifying whether it is required.

Expected: whitelist-controlled areas become available. Do not leave this enabled unless needed; it can affect special or mission-limited construction areas.

## 5. Preserved safety checks

Verify that at least these cases remain blocked where the unmodified game blocks them:

- unsuitable or excessively steep ground
- road/highway construction lanes
- outside the required Q-pid/chiral-network area
- a placement that physically cannot attach to the ground

## 6. Coexistence

After this mod passes alone, add the separate `ds2_weapons_anywhere.asi` and its own INI. Repeat one proximity and one restricted-area test.

Record whether the hologram is valid, whether the final build command succeeds, whether the structure persists after save/reload, and whether online mode synchronizes it.

## Known preserved limit

Monorail rebuilders and track infrastructure have a dedicated proximity warning and specialized route constraints. These remain intentionally active; a “too close” message around monorail infrastructure is not a failure of the generic `NearbyFacility` bypass.
