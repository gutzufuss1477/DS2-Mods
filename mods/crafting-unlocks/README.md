# Crafting & Equipment Overhaul 1.7.0

Early unlocks, Free Crafting, equipment durability, ATLAS gear and four red suppressed weapons in one ASI and one INI for DEATH STRANDING 2: ON THE BEACH.

## Included features

- 132 individually configurable original recipes/items, including backpack modules, covers and charms.
- 11 usable enemy-drop weapons, including Ghost Blade; craftable Omnireflector Boots and Chiral Boots.
- ATLAS Boots combining secure footing, impact absorption, stronger kicks and quiet steps.
- ATLAS Skeleton combining Battle, Boost and Bokka effects at level 3, including double jump and simultaneous hip cargo.
- Four additional suppressed weapons with black/red bodies, red menu images and visible suppressors.
- Optional shared FreeCrafting, durability multiplier (1.0–1000.0) and Unbreakable mode. The new weapons and ATLAS gear follow these settings.

## New suppressed weapons

| Custom variant | Base weapon |
|---|---|
| Suppressed Assault Rifle [MP] Lv.2 | Standard Lv.2, 42 rounds |
| Suppressed Machine Gun [MP] Lv.2 | Standard Lv.2, 120 rounds |
| Suppressed Shotgun [MP] Lv.2 | Standard Lv.2, 10 rounds |
| Suppressed Big-Bore Handgun [MP] | Standard Big-Bore Handgun |

These are separate craftable items. Base damage, fire rate, recoil, projectiles, capacity, weight and material costs are retained. The rifle and shotgun grenade launchers remain unchanged and unsuppressed. MG and shotgun use existing suppressed rifle audio; the handgun uses existing suppressed pistol audio. Impacts, explosions and visible attacks can still alert enemies. Normal weapons retain their own appearance and values.

The redundant custom Machine Pistol is no longer craftable. Its old definition remains internally available for saved references from the test builds. No item IDs have been reassigned.

## ATLAS and default settings

The supplied Nexus INI enables ATLAS recipes, the gold ATLAS Skeleton skin and all four suppressed weapons. FreeCrafting and durability changes remain off by default.

```ini
[AtlasEquipment]
Enabled=1
GoldSkeletonSkin=1

[SuppressedWeapons]
Enabled=1
AssaultRifleL2=1
MachineGunL2=1
ShotgunL2=1
BigBoreHandgun=1
BlackRed=1
MGVisualSuppressor=1
ShotgunVisualSuppressor=1
BigBoreVisualSuppressor=1
Language=0
```

Set `GoldSkeletonSkin=0` to use the normal Boost Skeleton Lv.3 appearance. This changes only the ATLAS skin, in the menu preview and while equipped. All combined effects remain active. `BlackRed=0` restores the base weapon colors/menu images. The three VisualSuppressor settings control visible attachments, independently of suppressed firing. Language is automatic (`0`), German (`1`) or English (`2`).

ATLAS Boots use the Pizza Baker appearance, weigh 0.2 kg and have 3400 base durability. ATLAS Skeleton weighs 4.0 kg and has 20000 base durability. The carrying bonus is up to 180 kg powered / 100 kg without power; bonuses are not added together. Battery use and native jump/input conditions still apply. The double-jump hint can appear late on short jumps.

ATLAS and suppressed weapon recipes have no story or donor-item unlock requirements once an ordinary fabrication facility is accessible. ATLAS costs match Transport Boots / Battle Skeleton Lv.1. Recipe visibility is independent of `DefaultUnlock`. Original item progression remains available. Heavy Machine Gun [MP] remains excluded because it lacks a normal inventory handoff.

## Install or update

1. Close the game and back up your current mod INI.
2. Remove `ds2_overpowered_equipment.asi` and `ds2_more_silenced_guns.asi` if installed. Their functionality is integrated; do not load either alongside this version.
3. Copy `ds2_crafting_unlocks.asi` and `ds2_crafting_unlocks.ini` next to `DS2.exe`. A compatible 64-bit ASI loader is required separately.
4. Use the supplied INI for the new defaults, then reapply any personal settings. If keeping an older INI, add the ATLAS/weapon sections above or just the missing keys; do not duplicate existing keys. Old INIs without these options retain their previous opt-in behavior and normal ATLAS skin.
5. Restart after every INI change.

Global `[CraftingUnlocks] Enabled=0` installs no patches. `[AtlasEquipment] Enabled=0` and `[SuppressedWeapons] Enabled=0` hide recipes while retaining the corresponding saved resources. Individual weapon switches also hide recipes. The global master must stay on for save compatibility.

ATLAS IDs 103/104 and weapon IDs 300/301/303/304, plus hidden legacy 302, are retained. Disabling recipes is the supported way to stop fabricating them. Removing the ASI or downgrading removes custom definitions; return to a pre-mod save before uninstalling. Missing legacy weapon definitions previously caused a startup crash, so they remain in this build.

## Compatibility and validation

Supported executable: DS2 1.10.89.0, SHA-256 `bf3d1c665545930bc850d8f5df486f7395885bb729d4fd408fdb03390de0765b`. Exact executable and instruction signatures are checked. Game archives and save files are not modified by the installer.

The user accepted the standalone suppressed weapon build. ATLAS gold, fabrication/equipped appearance and save/load were confirmed with 1.6.1. The integrated 1.7.0 build passes the native ATLAS and weapon suites, real GPU upload/readback checks, combined configuration/cost/durability tests and Windows loader checks. All 94 native patch sites are checked for exact signatures and overlap. The combined 1.7.0 ASI still needs a gameplay check after migration; these local tests do not establish long-session stability or complete AI hearing behavior. Individual ATLAS footwear effects and combat protection have not all been separately demonstrated.
