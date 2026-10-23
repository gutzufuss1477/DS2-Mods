# 1.10.0 validation

The red Story Coffin Board was tested in-game for riding, towing, hiding,
crafting/cargo previews and red appearance after loading in the same session and
after restarting. The outfit/accessory update was also confirmed in-game.
Coffin Board Reworked 1.83.0 was present in the combined game test.

The subsequent removal of the shoe-addon dependency changes only resource-cache
selection, graph composition and the guarded DirectStorage read hook. Gameplay,
menus, cosmetic policy and persistence source are unchanged from the confirmed
build. An additional in-game test without the addon is pending.

Offline checks for this change:

- Production Windows cache, graph and hook tests: 21 assertions without the addon
  and 21 with the unchanged addon. Standalone mode rejects any addon-cache reads.
- Generated standalone model metadata and links equal the game-tested model's
  bytes. Native archive payloads and reserved table bytes are preserved.
- Unchanged Addon API V1, original addon graph merge and eight pinned integration files.
- 121 verified native callsites, no overlap with the tested Reworked patch ranges.
- Core/equipment regressions, 353 cosmetic-policy and 58 cosmetic-runtime assertions.
- Session/persistence/fabrication regressions and six disposable-process DLL loader cases.
- Original game files, existing configuration and optional-mod binaries verified by hash.
- Exported public source rebuilt with local inputs and matched the packaged ASI byte for byte when using the same PE timestamp.

The generated cache is owned by the main mod; original game archives are read-only.
Appearance associations are stored in the separate `ds2_story_coffin_variants.bin`
file, not written into game save files. Removing the entire main mod with older
custom ATLAS/weapon items in a save has not been established as safe. Native menu
combination rules and DLC entitlement checks remain in effect.
