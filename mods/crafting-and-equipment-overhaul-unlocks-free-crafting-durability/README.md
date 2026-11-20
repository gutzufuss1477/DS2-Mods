# Crafting & Equipment Overhaul 1.10.2

Crafting, equipment, early PCC unlocks, ATLAS gear, suppressed/Warrior weapons,
red Higgs guitar, red Story Coffin Board, and 52 outfits/accessories with color
selection. Free crafting and durability changes remain optional.

The red Story Coffin Board now works in mixed fabrication with ATLAS and other
items. The previous one-red-coffin-only transaction restriction is removed. Its
native vehicle/cargo identity owns the red appearance association, including menu
and 3D previews. Keep `ds2_story_coffin_variants.bin` alongside save backups.

## Published Nexus update

Main **1.10.2** was published on 2026-11-20 as primary file **430**.
The optional ATLAS Chiral Boots and N02 Exoskeleton visual addons are version
**1.0.1**, files **395** and **396**. The integrated boot selector replaces
the archived optional test file **414**. [Nexus downloads](https://www.nexusmods.com/deathstranding2onthebeach/mods/86?tab=files)

See [English changelog](CHANGELOG.txt), [German changelog](CHANGELOG_DE.txt),
[English installation](README_EN.txt), [German installation](README_DE.txt) and
[validation](VALIDATION.md). Existing INI settings and personal Story appearance
metadata should be kept when updating.

## Bokka Combat Skeleton and ATLAS boot models

The main ASI includes the 12-model ATLAS Boot Model Selector from Nexus test file 414
and the separate Bokka Combat Skeleton Lv.3 recipe. To use the Absolute Boots model,
set `BootAppearance=Absolute` under `[AtlasEquipment]` in the existing INI and restart
the game. The setting applies to the worn boots and the boot clip. The Chiral Boots
visual addon takes priority when installed. N02 and Chiral Boots are the two separate
visual addons.

The [November 18 tester reply](https://www.nexusmods.com/deathstranding2onthebeach/mods/86?tab=posts&jump_to_comment=180124950)
confirms the Absolute Boots test in 1.10.2-test2. Release 1.10.2 passed offline checks, including
loading the visual addons in either order. The in-game check for four mounted
cargo items and their persistence after a save and reload is pending.

## Independent optional appearances

- [ATLAS N02 Exoskeleton 1.0.1](addons/atlas-n02-exoskeleton/README.md): graphite black
  with red accents and a matching menu render; one ASI, no addon INI.
- [ATLAS Chiral Boots 1.0.1](addons/atlas-chiral-boots/README.md): stable gold appearance and corrected character colors.

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
The October 2026 game-test confirmation covers the Test05 main build with both addons.
