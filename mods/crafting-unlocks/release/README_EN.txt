DS2 CRAFTING & EQUIPMENT OVERHAUL 1.6.1
Unlocks, Free Crafting, Durability & ATLAS Gear

Features:
- 132 configurable supported items/variants.
- 11 additional enemy-drop weapons, including Ghost Blade.
- Early unlocks while preserving native progression.
- Optional FreeCrafting.
- Craftable Omnireflector Boots and Chiral Boots.
- Optional durability multiplier (1.0-1000.0).
- Optional Unbreakable mode.
- Backpack Cover Lv.1/Lv.2 durability support.
- Optional native gold Boost Lv.3 skin for the ATLAS Skeleton.
- Heavy Machine Gun [MP] intentionally excluded.

Install:
Copy ds2_crafting_unlocks.asi and ds2_crafting_unlocks.ini next to DS2.exe.
A compatible 64-bit ASI loader is required separately.
Restart the game after changing the INI.

Supported DS2.exe: 1.10.89.0

ATLAS equipment (optional, new in 1.6.0)

[AtlasEquipment]
Enabled=0
GoldSkeletonSkin=0

Set Enabled=1 in this section to fabricate ATLAS Boots and ATLAS Skeleton at ordinary fabrication facilities, without story or donor-item unlock requirements. Set GoldSkeletonSkin=1 for the native gold Boost Lv.3 appearance. Both options default to 0. Restart after configuration changes.

ATLAS Boots combine secure footing, impact absorption, stronger kicks and quiet steps. They use the Pizza Baker boots appearance, weigh 0.2 kg and have 3400 base durability.
ATLAS Skeleton combines Battle, Boost and Bokka effects at level 3, including the Bokka double jump and simultaneous hip cargo. It uses the ordinary Boost Skeleton Lv.3 appearance by default, or the native gold Boost Lv.3 appearance with GoldSkeletonSkin=1. It weighs 4.0 kg and has 20000 base durability. The carrying bonus is up to 180 kg powered / 100 kg without power; the bonuses are not added together. Battery consumption and native jump/input conditions still apply.

FreeCrafting and the shared Durability Multiplier/Unbreakable options include both ATLAS items. With those options off, ATLAS retains its base durability and the material costs of Transport Boots / Battle Skeleton Lv.1. The 132 original item rules continue to control normal equipment; ATLAS fabrication is controlled by its own section independently of DefaultUnlock.

Names and descriptions are German in the German UI and English otherwise. Original equipment keeps its own values and appearance.

Migration and saves
Remove the old ds2_overpowered_equipment.asi when installing this version. Use only ds2_crafting_unlocks.asi and its matching INI. The stable ATLAS identities are retained for existing items.
Setting AtlasEquipment.Enabled=0 hides the two recipes while keeping resources and ATLAS effects available to existing saved items. CraftingUnlocks.Enabled must remain 1 for this compatibility. Turning the global master switch off, removing the ASI or downgrading below 1.6.0 removes that support; use a save from before ATLAS items were created for an uninstall.

Validation
The combined 1.6.1 build passes the full native instruction, resource, configuration, cost and durability test suite. ATLAS fabrication, gameplay, hip cargo and save/load persistence were confirmed by the user. The optional gold skin was confirmed in the fabrication preview and on the equipped ATLAS Skeleton on a non-Deluxe installation, including after save/load. Individual footwear effects and combat protection have not all been separately demonstrated in gameplay. The double-jump hint can appear late on short jumps.
