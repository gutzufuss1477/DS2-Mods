# DS2 Mod Suite 1.11.0 validation

Validated on Windows, 2026-10-04. Base: origin/main at `0aba9147b339308af3a1134de8713b24b922015f`.

## Release scope

- Jump Ramp Unlimited 1.0.0 and Beach Jump with Cargo 1.0.0 added.
- Sam Overhaul 1.0.0 replaces Sneaky Sam. Source and release imported from `origin/chatgpt/sam-overhaul-v1.0.0` at `779f7cf`; payload ASI and INI match the published ZIP exactly. The INI checkout line-ending conversion was removed and prevented through .gitattributes.
- Climbing/Combat Gloves updated from 1.1.0 to 1.1.1.
- Crafting & Equipment Overhaul updated from 1.5.0 to 1.7.0, retaining the stable mod/config identifiers. 132 original item choices plus two ATLAS and four suppressed-weapon recipes; 138 recipes does not mean 138 Items keys.
- 26 distinct mods, 27 selectable entries, 377 settings, 24 INIs, 23 configurable mods.

Nexus was cross-checked directly in the browser: manager 72, Sam Overhaul 88, Jump Ramp 91, Beach Jump 92, Crafting 86 and Gloves 76. Some standalone page version headers lag behind their newer Git releases; the suite uses the verified repository release ZIPs, not those stale page headers.

## Passed checks

- Full executable self-tests, both from the build and again from a separately extracted final ZIP. All six manifest-covered package files match SHA-256.
- Exact source/ZIP byte comparison for ASI and INI payloads of all five changed/new mods. Other payloads remain unchanged; Sneaky Sam is removed from the embedded payload set.
- All 325 previous schema fields are unchanged; 52 settings added. Defaults match the standalone release INIs. Existing explicit ATLAS opt-outs, weapon language, crafting master and custom comments survive old-profile upgrades.
- New-mod installation, standalone INI adoption with and without a central profile, numeric bounds, reapply idempotence and customized INI preservation on removal.
- Known Sneaky Sam 1.0.0/1.0.1 migration paths, automatic selection for upgrade, removal of the old ASI in the same transaction, and refusal to modify unknown old binaries. Synthetic trusted-file fixtures exercise the transaction; the catalog contains the actual released hashes.
- ATLAS, suppressed-weapons and Jump Ramp probe conflict detection in inspection, preflight, apply and final verification. Unknown standalone files are retained.
- All existing loader, variant, backup, rollback, path and elevated-transport tests.
- Ten UTF-8 language dictionaries, 615 strings each: complete keys, placeholders, concatenation whitespace, source coverage and persistence. All 377 field identities and 24 generated INIs stay identical across languages.
- Actual WPF main and all five new/updated settings pages rendered offscreen in ten languages. Ninety images include scrolled Crafting/ATLAS/weapon controls. Crafting `inherit` and independent in-game weapon language values round-trip correctly. Settings scroll resets on mod selection. Selected German, French, Russian and Simplified Chinese views visually inspected.
- Compact Nexus description (538 words), short description, file description and changelog prepared together.

No desktop or game control, real game-file modification or new in-game compatibility test was performed. The standalone mods' documented gameplay coverage remains the applicable evidence. The combined Crafting 1.7.0 package has not received a new suite gameplay/save-load test; Backpack Classic Overlap remains untested in game. Translations are AI-assisted and have not had comprehensive native-speaker review.

## Package

`DS2_Mod_Suite_v1.11.0.zip`

SHA-256: `FAAE9E0E9737C841622ED1C2B575BA328816B79E70FCF22094FCC9A38B31B032`
