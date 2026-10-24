# Crafting & Equipment Overhaul 1.10.1

Crafting, equipment, early PCC unlocks, ATLAS gear, suppressed/Warrior weapons,
red Higgs guitar, red Story Coffin Board, and 52 outfits/accessories with color
selection. Free crafting and durability changes remain optional.

The red Story Coffin Board now works in mixed fabrication with ATLAS and other
items. The previous one-red-coffin-only transaction restriction is removed. Its
native vehicle/cargo identity owns the red appearance association, including menu
and 3D previews. Keep `ds2_story_coffin_variants.bin` alongside save backups.

## Prepared Nexus update

Main mod **1.10.1** and optional **ATLAS N02 Exoskeleton 1.0.0** are prepared for
the [existing Nexus page](https://www.nexusmods.com/deathstranding2onthebeach/mods/86).
These records do not claim that an upload has happened. Existing file IDs in
`nexus.json` are historical verified publication records, not a fresh Nexus listing.

See [English changelog](CHANGELOG.txt), [German changelog](CHANGELOG_DE.txt),
[English installation](README_EN.txt), [German installation](README_DE.txt) and
[validation](VALIDATION.md). Existing INI settings and personal Story appearance
metadata should be kept when updating.

## Independent optional appearances

- [ATLAS N02 Exoskeleton 1.0.0](addons/atlas-n02-exoskeleton/README.md): graphite black
  with red accents and a matching menu render; one ASI, no addon INI.
- ATLAS Chiral Boots 1.0.0: published binary and source remain unchanged.

Either addon can be used alone, or both together. Stats, recipes and item identities
stay in the main mod. Removing a visual addon restores the corresponding main-mod
appearance while retaining the main mod. Normal content, recipe and stat updates
do not require rebuilding addons while API V1, Router V1 and ATLAS identities remain
stable. This is not a guarantee for unknown future game layout changes.

Removing the main mod is different: custom ATLAS/weapon variants still need its
definitions. No blanket main-mod removal guarantee is made for custom equipment.

## Source

See [build instructions](source/BUILD.md). Native textures, rendered game images
and embedded model/link payloads remain explicit local build inputs. A clean clone
does not include them or an end-to-end extractor. Generated caches, original game
archives, saves and personal configuration are excluded from Git and public packages.
The game-test confirmation refers to the exact Test05 main build with both addons;
release 1.10.1 changes only the main version label and was rebuilt and checked offline.
