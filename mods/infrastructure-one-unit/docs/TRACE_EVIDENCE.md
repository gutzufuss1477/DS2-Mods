# Runtime evidence summary

The progression model was validated through targeted Requirement/Current traces and one-shot prototypes for:

- road stage 1;
- road stage 2;
- monorail stage 1;
- mine stages 1, 2 and 3.

The final integrated ASI was then validated without Cheat Engine. Its log showed successful `APPLY` events for RoadBuild, RoadUpgrade, MonorailBuild, MineBuild, MineUpgrade2 and MineUpgrade3. It also handled a road with existing online contributions where the visible native remainder was 320 / 400 / 520 and reduced it to the configured remainder.
