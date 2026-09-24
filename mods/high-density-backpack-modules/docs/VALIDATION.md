# Validation - v1.1.0

Date: 2026-09-24. Release based on the user's successful Test5 run.

## In-game evidence

The user confirmed correct appearance in gameplay and all tested menus after installing Test5. The reported case involved loading an existing save. The game log identifies Test5, `status=PATCH_APPLIED`, `GAMEPLAY_BACKPACK_VISIBILITY=ACTIVE` and `EQUIPMENT_PREVIEW_VISIBILITY=ACTIVE`. Earlier runs established 30 modules and seven currently available charms equipped through the eight-row menu.

The release changes Test5's version/log identity and introductory comment only. Gameplay, placement, rendering, charm and persistence code are unchanged. The final release-named binary was built and locally tested, but was not separately run in game.

## Local checks

- Windows x64 LLVM build with warnings as errors.
- Placement, visual packing, full-footprint bone references and byte-identical logical module records.
- Independent live gameplay/menu contexts with an empty customization-manager cache, alternating different layouts, null/teardown cases and exclusion of unrelated actors.
- Eight accessory rows, equipped/unlocked effect masks, battery deduplication, adapter register preservation and INI roundtrip/invalid data.
- 771 original native Cryptobiosis loop cases and original category-builder instructions for 256 equipment masks.
- Exact EXE hash, 92 byte/call guards, 63 non-overlapping patch windows and 1,938 direct branches.
- One-ASI ZIP: CRC, extracted bytes and reference ASI verified.

Actual charm effect magnitudes/triggers, restart persistence of extra slots across every save scenario, all animation/geometry cases and other-mod compatibility remain unmeasured or incomplete. Additional charm preferences apply installation-wide, filtered by unlock availability.

## Artifact

`DS2_HighDensityBackpackModules_v1.1.0.asi`

SHA-256: `4FAE089E17D86D8DA0DAE61C6F91A383864944C5918C94725A0C758947E35747`

Historical results: [Test3](VALIDATION-test3.md), [Test4](VALIDATION-test4.md), [Test5 development checks](VALIDATION-test5.md). The Test4 menu correction is retained together with Test5's live-player lookup.
