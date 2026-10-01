# DS2 Mod Suite 1.9.0 validation

Validated on Windows on 2026-10-01. Mod source baseline: `origin/main` at `d1a8bb9`.

## Included changes

- Sneaky Sam 1.0.1, including known 1.0.0 filename migration; no configuration file.
- Climbing Power Gloves Range 1.1.0, including CombatGloves / EnableCargoPickup (release default 1). Climbing range defaults remain 30/50 m; decimal values are supported and Level 2 must be at least Level 1.
- Crafting Overhaul 1.5.0, including Chiral Boots, 11 enemy-drop weapons and durability support for backpack covers. All 132 items are configurable in 13 groups. Choices support explicit 0/1 and the mod's `inherit` value.
- Improved Odradek Scan 1.0.1, with Fan360 as the terrain-safe release default and preserved compatible existing mode values.
- 24 distinct mods, 25 selectable entries, 20 configurable mods, 21 INIs and 325 fields.

## Payload checks

Crafting, Gloves and Sneaky Sam ASIs exactly match their corresponding repository release ZIPs. The Gloves INI was normalized to the exact released ZIP bytes and marked `-text` in `.gitattributes`; its settings and comments are unchanged. Odradek uses the current repository release ASI/INI. Every other existing mod's complete catalog record is unchanged from the source baseline.

The build verified every source and embedded payload against the catalog SHA-256, compiled the executable and completed the full self-test before creating the portable ZIP. The finished ZIP was extracted into a separate directory; every packaged file matched its manifest, and the extracted executable completed its full self-test with exit code 0.

## Configuration and installation regression coverage

- Glove migration with and without a central profile, incomplete old INIs, explicit Combat opt-out, decimal ranges, numeric limits, Level 1/2 relationship, repeated Apply and INI preservation on removal.
- Invalid glove ranges reject installation and roll back newly staged ASI/loader files without changing the original INI.
- Upgrading a 312-field v1.8 profile to 325 fields preserves saved central choices. Newly introduced values are first imported from an installed INI, then defaulted only when absent/invalid. Explicit new Crafting/Combat choices and `inherit` are preserved.
- Crafting's 132 item choices, labels, groups, defaults, inheritance, inline comments, installation/adoption, repeated Apply and removal.
- Sneaky Sam fresh install, synthetic known legacy filename migration, unknown legacy conflict rejection, repeated Apply, removal and exclusion from the INI settings list.
- Existing suite tests: APAS default-off progression, Odradek modes/bounds, Backpack variant exclusivity and charm preservation, Coffin migration, catalog validation, loader guards, transactional backup/rollback and authenticated/compressed elevated profile transport.

## Interface verification

The actual WPF settings window was instantiated and rendered offscreen in English and German. Gloves displayed five controls and Crafting displayed 139 controls, with its item groups under advanced settings. A displayed Crafting choice round-tripped to the literal INI value `inherit`. Sneaky Sam did not appear in the configurable-mod selector. German screenshots were visually inspected; no desktop or game controls were used.

No installed game files or real central profile were changed. Tests use isolated temporary directories. No new in-game test was performed; the standalone mods' gameplay validation is documented in their respective directories. Backpack Classic Overlap remains without new in-game verification.

Archive: `DS2_Mod_Suite_v1.9.0.zip`

SHA-256: `8995F7365DDA48BB5EF8F4235A55BDF06E4949608371D63E3609D5DF3A856751`
