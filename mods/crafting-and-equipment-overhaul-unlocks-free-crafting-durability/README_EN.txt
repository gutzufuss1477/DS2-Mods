Crafting & Equipment Overhaul 1.10.0
DEATH STRANDING 2: ON THE BEACH | Windows x64 | supported game build 1.10.89.0

NEW
The red Story Coffin Board is part of the main mod: fabrication, red menu thumbnail,
3D previews, riding, towing, hiding and saved appearance. No shoe addon is required.
Also adds 52 native outfits/accessories and color choices to customization.
See CHANGELOG.txt for the update summary.

UPDATING
Exit DS2 completely and replace ds2_crafting_unlocks.asi beside DS2.exe.
Keep your existing ds2_crafting_unlocks.ini and ds2_story_coffin_variants.bin.
The new cosmetics feature is enabled even if your INI has no [Cosmetics] section.
To customize it, merge that section from the supplied INI once, without duplicate
sections or keys. The supplied INI is for fresh installation or resetting settings.

FRESH INSTALLATION
Copy ds2_crafting_unlocks.asi and ds2_crafting_unlocks.ini beside DS2.exe.
A compatible x64 ASI loader is required separately and is not included.
Remove old standalone ds2_overpowered_equipment.asi and ds2_more_silenced_guns.asi
if present; their functionality is integrated. When updating from before 1.9.8,
merge missing [WarriorWeapons], [RedGuitar] and [Guitars] sections from the INI.

RED STORY COFFIN BOARD
Select the Story entry in fabrication. Fabricate exactly one red coffin at a time,
with no other items selected in that transaction. Repeat to make additional red
coffins. The normal board remains available. The Story variant retains its native
item identity and standard gameplay functions.

On first launch the main mod generates its own ds2_crafting_story_cache folder
beside DS2.exe. Allow approximately 1.5 GB of free space and extra startup time.
The optional shoe addon uses a separate compatible cache profile; switching can
leave both profiles present. With DS2 closed, this cache folder can be removed
and will regenerate when needed. Original archives are only read.

ds2_story_coffin_variants.bin beside DS2.exe stores the mod's appearance mappings.
Back it up alongside your saves and transfer it when moving to another PC.
This personal file is not part of the download; do not replace it during updates.
Without these mappings, saved red coffins appear as standard coffins again.
The mod does not modify original game archives or write to game save files.

OUTFITS AND ACCESSORIES
The 52 entries cover outfits/camouflage, glasses, hats/bandanas, hoods, masks,
BB pod designs and Dollman's outfit. See cosmetics.native.tsv for the full list.
Native combination rules and DLC entitlement checks remain in effect.
[Cosmetics] Enabled=0 disables the additional unlocks; UnlockColors=0 retains
native color availability. The global master switch, DefaultUnlock and individual
[Items] rules still apply.

EXISTING FEATURES AND DEFAULTS
The previous 135 item entries, ATLAS equipment, additional suppressed weapons,
Warrior weapons, red Higgs guitar and PCC selections remain included.
Free Crafting and durability modifications are disabled in the public default INI.
Restart DS2 after changing settings.

COMPATIBILITY
ATLAS Chiral Boots Addon 1.0.0 remains a separate optional download. Its ASI does
not need updating; Addon API V1 is preserved. Coffin Board Reworked was checked
in combination with version 1.83.0. Other game builds and conflicting resource
mods are not covered. Do not replace streaming_graph.core or original archives.

REMOVAL
Exit DS2 and remove the mod ASI. The Story Coffin extension uses native coffin IDs
and separate appearance metadata; removing it removes the red appearance.
This is not a blanket removal guarantee for older custom equipment in the main
mod: saved custom ATLAS/weapon variants still require their item definitions.
Keep a separate save backup and account for custom equipment before removing the
entire main mod. Keep the INI and appearance metadata for later reinstallation.
