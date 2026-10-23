# Crafting and Equipment Overhaul - Unlocks - Free Crafting - Durability - ATLAS Gear

Customize crafting, durability and early PCC unlocks. Includes ATLAS gear, suppressed and Warrior weapons, the red Higgs guitar, a craftable red Story Coffin Board, and 52 outfits/accessories with color choices. Free crafting and durability changes are optional.

The Story Coffin Board belongs to the main mod and requires no shoe addon. It retains normal riding, towing and hiding, adds red menu/3D previews, and preserves its appearance through save/load and restarts. ATLAS Chiral Boots Addon 1.0.0 remains optional with its original API V1 and unchanged ASI. Coffin Board Reworked 1.83.0 was used for compatibility testing.

## Prepared update: 1.10.0

Source and packaging are updated to 1.10.0. The Nexus upload has not been performed; the table below records the existing published downloads. See [English changelog](CHANGELOG.txt), [German changelog](CHANGELOG_DE.txt), [English installation notes](README_EN.txt) and [German installation notes](README_DE.txt).

Fabricate one red coffin per transaction, without other selected items. Keep `ds2_story_coffin_variants.bin` when updating or transferring saves. Original game archives and save files are not modified. The main mod generates its own local resource cache (approximately 1.5 GB).

[Current downloads and installation instructions on Nexus Mods](https://www.nexusmods.com/deathstranding2onthebeach/mods/86)

## Current published files

| File / variant | Nexus file version | Nexus file ID |
|---|---|---|
| DS2_Crafting_Equipment_Overhaul_v1.9.9 | 1.9.9 | 389 |
| DS2 ATLAS Chiral Boots Addon V1.0.0 | 1.0.0 | 390 |

File versions above are the actual download versions; the Nexus page version can differ.
Only current published files are listed. Archived releases, comparisons and explicitly labelled tests are excluded.

## Source and build

The red coffin's gameplay, menu appearance, persistence and the cosmetics were tested in-game. The independent resource-loading change passed the actual Windows cache/graph/hook tests both without the addon (zero addon-file reads) and with the unchanged addon. The additional in-game test without the addon is pending. See [validation](VALIDATION.md) for scope and limits.

The optional Chiral Boots 1.0.0 source is included separately and is unchanged. Native textures, rendered game images and embedded resource payloads remain local build inputs. See [build instructions](source/BUILD.md); a clean clone does not contain those inputs.

See `nexus.json` for immutable published file IDs and the prepared update record. Existing custom ATLAS/weapon variants still require the main mod's item definitions; the Story coffin's native-ID save compatibility does not establish a blanket removal guarantee for all older custom equipment.
Build outputs, original game data, private diagnostics and credentials are kept out of Git.
Third-party licenses remain with their respective source directories.
