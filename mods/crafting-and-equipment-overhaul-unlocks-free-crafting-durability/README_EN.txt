Crafting & Equipment Overhaul 1.10.1
DEATH STRANDING 2: ON THE BEACH | Windows x64 | Game build 1.10.89.0

NEW IN 1.10.1
The red Story Coffin Board now works in mixed fabrication with ATLAS and other
items. Supports the optional ATLAS N02 Exoskeleton Addon independently or together
with Chiral Boots Addon 1.0.0. No new INI settings are required.

UPDATE
Close DS2 and replace ds2_crafting_unlocks.asi beside DS2.exe. Keep your existing
ds2_crafting_unlocks.ini and ds2_story_coffin_variants.bin. The included INI is
for a fresh installation or resetting settings, not for replacing personal choices.
When updating from before 1.10.0, cosmetics are enabled even without [Cosmetics].
Copy that section from the included INI only if you want to customize it.
When updating from before 1.9.8, merge missing [WarriorWeapons], [RedGuitar] and
[Guitars] sections. Do not create duplicate sections or keys.

FRESH INSTALLATION
Copy ds2_crafting_unlocks.asi and ds2_crafting_unlocks.ini beside DS2.exe.
A compatible x64 ASI loader is required separately and is not included.
Remove old standalone ds2_overpowered_equipment.asi and ds2_more_silenced_guns.asi
if present; their features are already integrated.

RED STORY COFFIN BOARD
Select the Story entry in the fabrication menu. It can be fabricated in the same
transaction as ATLAS and other items. Each red coffin is associated with its own
native vehicle/cargo identity. The normal coffin remains available. Riding,
towing and hiding use native game behavior; red menu and 3D previews are included.
The first start builds ds2_crafting_story_cache beside DS2.exe, requiring roughly
1.5 GB per archive profile. Optional visual addons use matching separate caches;
several profiles may remain after switching combinations. Initial loading can
take longer. Generated caches can be removed while the game is closed and are
rebuilt when needed. Original archives are read only.
Keep ds2_story_coffin_variants.bin with your save backups and when moving to a
different PC. It stores the mod's appearance associations, is personal and is
never included in the download. Without those associations, saved Story coffins
can appear as normal coffins. The mod does not write into native save files.

EXISTING FEATURES AND DEFAULTS
Retains the 135 existing fabrication entries, ATLAS equipment, additional
suppressed weapons, Warrior weapons, red Higgs guitar and early PCC structure
selection. Also includes 52 native outfits/accessories and color choices:
outfits/camouflage, glasses, hats/bandanas, hoods, masks, BB pod designs and Dollman.
See cosmetics.native.tsv for the list. Native combination and DLC checks remain.
[Cosmetics] Enabled=0 disables the additional unlocks; UnlockColors=0 keeps native
color availability. The master switch, DefaultUnlock and [Items] rules still apply.
Free crafting and durability changes are OFF in the public default INI.
Restart after changing settings.

OPTIONAL ADDONS
ATLAS Chiral Boots Addon 1.0.0 remains unchanged; no replacement is needed.
ATLAS N02 Exoskeleton Addon 1.0.0 is a separate optional download on the same
Crafting Nexus page. Install its single ASI beside DS2.exe; no addon INI is needed.
The two visual addons work independently or together. Removing only a visual
addon restores the main mod's normal appearance and retains ATLAS items/stats.
Keep the main mod installed. Main-mod content, recipe and stat updates do not
require rebuilding addons while Addon API V1, Visual Router V1 and ATLAS identities
are preserved. N02 requires main mod 1.10.1 or a later compatible version.
Coffin Board Reworked 1.83.0 was used for compatibility testing. Different game
builds and other mods changing the same resources are not covered.
Do not replace streaming_graph.core or other original archives.

REMOVAL
Close DS2 before removing an ASI. Removing a visual addon is separate from removing
the main mod. Custom ATLAS/weapon variants still require the main mod's definitions;
native-ID Story Coffin compatibility is not a blanket removal guarantee for all
custom equipment. Keep a separate save and account for custom gear before removing
the main mod entirely. Keep your INI and appearance metadata for later reinstall.
