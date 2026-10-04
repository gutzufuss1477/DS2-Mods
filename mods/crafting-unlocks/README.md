# Crafting & Equipment Overhaul

Unlocks, Free Crafting, Durability & ATLAS Gear

Version 1.6.1 for DEATH STRANDING 2: ON THE BEACH.

Formerly Crafting Overhaul. The existing ASI and INI filenames are retained for upgrades.

Features
- Optional ATLAS Boots and ATLAS Skeleton, combining footwear and Battle/Boost/Bokka effects.
- Optional native gold Boost Skeleton Lv.3 appearance for ATLAS via `GoldSkeletonSkin=1`.
- 132 individually configurable supported recipes/items.
- Early fabrication unlocks while preserving native progression.
- 11 additional enemy-drop weapons that Sam can normally use, including Ghost Blade.
- Backpack modules, covers and charms through the native backpack menu.
- Craftable Omnireflector Boots and Chiral Boots.
- Optional FreeCrafting.
- Optional durability multiplier (1.0-1000.0) and Unbreakable mode.
- Dedicated durability handling for the 8 original craftable boots plus ATLAS Boots.
- Backpack Cover Lv.1/Lv.2 durability follows the same Multiplier/Unbreakable setting.
- Cargo/order cargo/container durability remains vanilla.
- Heavy Machine Gun [MP] is intentionally excluded because it has no normal cargo/inventory handoff.

Quick configuration
[CraftingUnlocks]
Enabled=1
DefaultUnlock=1
FreeCrafting=0

[AtlasEquipment]
Enabled=0
GoldSkeletonSkin=0

[Durability]
Enabled=0
Multiplier=2.0
Unbreakable=0

CraftingUnlocks.Enabled=0 is the global master switch and installs no patches.
Unbreakable=1 requires Durability.Enabled=1 and overrides Multiplier.

Compatibility
Supported DS2.exe: 1.10.89.0
SHA-256: bf3d1c665545930bc850d8f5df486f7395885bb729d4fd408fdb03390de0765b

The mod checks the exact executable and patch signatures before applying changes.

ATLAS equipment (optional, new in 1.6.0)

[AtlasEquipment]
Enabled=0
GoldSkeletonSkin=0

Set Enabled=1 in this section to fabricate ATLAS Boots and ATLAS Skeleton at ordinary fabrication facilities, without story or donor-item unlock requirements. `GoldSkeletonSkin=1` changes only the ATLAS Skeleton appearance to the native gold Boost Lv.3 variant. Both options default to 0. Restart after configuration changes.

ATLAS Boots combine secure footing, impact absorption, stronger kicks and quiet steps. They use the Pizza Baker boots appearance, weigh 0.2 kg and have 3400 base durability.
ATLAS Skeleton combines Battle, Boost and Bokka effects at level 3, including the Bokka double jump and simultaneous hip cargo. By default it uses the ordinary Boost Skeleton Lv.3 appearance; set `GoldSkeletonSkin=1` to use the native gold Boost Skeleton Lv.3 appearance. It weighs 4.0 kg and has 20000 base durability. The carrying bonus is up to 180 kg powered / 100 kg without power; the bonuses are not added together. Battery consumption and native jump/input conditions still apply.

FreeCrafting and the shared Durability Multiplier/Unbreakable options include both ATLAS items. With those options off, ATLAS retains its base durability and the material costs of Transport Boots / Battle Skeleton Lv.1. The 132 original item rules continue to control normal equipment; ATLAS fabrication is controlled by its own section independently of DefaultUnlock.

Names and descriptions are German in the German UI and English otherwise. Original equipment keeps its own values and appearance.

Migration and saves
Remove the old ds2_overpowered_equipment.asi when installing this version. Use only ds2_crafting_unlocks.asi and its matching INI. The stable ATLAS identities are retained for existing items.
Setting AtlasEquipment.Enabled=0 hides the two recipes while keeping resources and ATLAS effects available to existing saved items. CraftingUnlocks.Enabled must remain 1 for this compatibility. Turning the global master switch off, removing the ASI or downgrading below 1.6.0 removes that support; use a save from before ATLAS items were created for an uninstall.

Validation
The standalone ATLAS gameplay, early-save fabrication, persistence after restart/load and hip cargo attachment were confirmed by the user. The combined 1.6.1 build passes the full native instruction, resource, configuration, cost and durability test suite. The optional gold skin was confirmed in the fabrication preview and on the equipped ATLAS Skeleton on a non-Deluxe installation, and remained correct after save/load. Individual footwear effects and combat protection have not all been separately demonstrated in gameplay. The double-jump hint can appear late on short jumps, as in the accepted ATLAS test build.
