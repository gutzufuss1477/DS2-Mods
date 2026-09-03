# In-Game Test Plan

This plan applies to `v1.0.0` with the default ranges of `30 m` for Level 1 and `50 m` for Level 2.

## Confirmed release behavior

The following behavior was confirmed in game before the v1.0.0 release:

| Equipment state | Observed result |
|---|---|
| Climbing Power Gloves Level 2 | Cargo can be picked up from a long distance. |
| Switch from Level 2 to Level 1 | Pickup range becomes noticeably shorter immediately. |
| No Climbing Power Gloves | Cargo requires the normal close pickup distance. |

This confirms tier separation and confirms that the ordinary no-glove pickup range is not globally increased.

## 1. Preconditions

1. Validate `DS2.exe` with `tools/validate_target.py`; every check must report `OK`.
2. Confirm the supported Steam build: `DS2.exe 1.10.89.0`, SHA-256 `BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B`.
3. Install the ASI and INI next to `DS2.exe`, then fully restart the game.
4. For diagnostics, start with `DebugLog=1` and verify that both glove resources are detected successfully.

## 2. Native baseline

1. Set `Enabled=0` and restart the game.
2. Test Level 1 around 6, 8, 10, and 15 m.
3. Test Level 2 around 8, 10, 12, and 15 m.
4. Expected result: approximately `8 m` for Level 1 and `10 m` for Level 2. Record any variation caused by cargo size or player position.

## 3. Tier separation with default values

1. Set `Enabled=1`, Level 1 to `30`, and Level 2 to `50`, then restart the game.
2. Test Level 1 at approximately 10, 20, 29, 31, and 40 m.
3. Expected result: pickup works to approximately 30 m; 31 and 40 m remain outside the configured range.
4. Test Level 2 at approximately 12, 30, 49, 51, and 60 m.
5. Expected result: pickup works to approximately 50 m; 51 and 60 m remain outside the configured range.
6. Equip Level 1 again and confirm that it does not inherit the 50 m Level 2 range.
7. Remove the gloves and confirm that normal close-range pickup is restored.

The qualitative Level 2, Level 1, and no-glove separation in steps 4 through 7 has already been confirmed. Exact distance-boundary measurements remain useful for regression testing.

## 4. Scope and gameplay regression

- Test without Climbing Power Gloves and with other glove types; normal hand pickup must not be extended.
- Test one cargo item, several nearby items, and items of different sizes.
- Test height differences, slopes, obstacles, and targets outside the view direction; native restrictions should remain in effect.
- Test pickup on foot and from the Pickup Off-Roader.
- Check the Sticky Gun and Sticky Cannon; their range and behavior must remain unchanged.
- Play normally for several minutes and watch for incorrect targets, animation problems, cargo teleport issues, stutter, or crashes.

## 5. Persistence regression

These checks are still recommended even though the core tier behavior is confirmed:

- Save and reload while either glove tier is equipped.
- Change areas and return.
- Sleep, fast travel, or use another normal transition that reloads gameplay resources.
- Repeatedly switch between Level 1, Level 2, and no gloves after each transition.
- Confirm after every transition that Level 2 remains longest, Level 1 remains shorter, and no gloves remain close-range only.

## 6. Configuration and fail-safe behavior

- Set both values to the same range, for example `30/30`; both tiers should then have equal reach.
- Test the native lower bounds `8/10` and the upper bounds `100/100`.
- Test invalid input: Level 1 below 8, Level 2 below 10, either value above 100, or Level 2 below Level 1. The mod must abort safely and log the reason.
- `Enabled=0` must leave both native resource values untouched.
- The patch must remain inactive on an unsupported or modified executable.

## 7. Issue report checklist

Include the following with any reproducible issue:

- Game version and EXE hash
- Complete INI configuration
- Equipped glove tier
- Approximate distance and cargo type
- Whether the player was on foot or in a vehicle
- Relevant excerpt from `ds2_climbing_gloves_range.log`
- Short reproduction steps and, if practical, a screenshot or video
