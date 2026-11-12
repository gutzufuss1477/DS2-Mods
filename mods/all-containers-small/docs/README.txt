ALL CONTAINERS SMALL 1.0.0
DEATH STRANDING 2: ON THE BEACH - PC / Steam 1.10.89.0
Author: gutzufuss1477

WHAT IT DOES
Standard cargo containers use size S, one size unit and a compact S-case
model. This includes material containers, ladders, PCC packages and
ordinary packaged weapons and equipment. No configuration is required.

Material amounts, item identity, weight and durability fields are not
changed. Example: Resins [320] still contains 320 materials and weighs
32 kg, but its case is size S. This is not an unlimited-weight mod.

EXCEPTIONS
Floating Carriers/EnemyCart, captured animals and their cages, BB pods
and prototype, bodies, vehicles, Coffinboard, GhostMech guns, one scripted
special object, NonBaggage and Human/Coffinboard volume classes keep their
original definitions and models. These functional objects require their
own components; replacing their whole entity can break the game.

INSTALL
1. Close the game.
2. Use an existing compatible x64 ASI loader. Ultimate ASI Loader by
   ThirteenAG is available at https://github.com/ThirteenAG/Ultimate-ASI-Loader
   The loader is not included. Keep a working existing loader.
3. Copy ds2_all_containers_small.asi into the folder containing DS2.exe.
4. Start the game normally and load your save. No INI or hotkey needed.

Optional: install.ps1 checks the exact game version and package hash and
backs up an existing copy. The default path is Steam's standard game
folder; use -GameDir with your actual folder if different.

REQUIREMENTS AND COMPATIBILITY
Windows x64; Steam DS2.exe 1.10.89.0 with SHA256:
bf3d1c665545930bc850d8f5df486f7395885bb729d4fd408fdb03390de0765b
Other executable builds are rejected without installing hooks. Conflicts
at the same hook sites block activation. Compatibility with every mod
combination is not guaranteed. The game EXE and archives are not patched
on disk. The game's normal saving remains active.

TESTED
The author confirmed successful startup, cargo collection, size S in
inventory, unchanged displayed weights and small cases on the backpack
on 12 November 2026. Screenshots show this in-game result. The public
release uses that 0.1.1 runtime with only the version label changed.
17 local tests also passed. All equipment deployment paths, long sessions
and saving/reloading repacked cargo have not been comprehensively tested.

TROUBLESHOOTING
ds2_all_containers_small.log is written beside the ASI. HOOKS_READY and
CATALOGUE_READY indicate activation. VERSION_BLOCKED, HOOK_BLOCKED or
LATE_LOAD_BLOCKED indicate that the mod could not activate.
CATALOGUE_BLOCKED means the catalogue was unexpected and sizes were not
changed. Restart the game after changing ASI files.

UNINSTALL
Close the game and remove ds2_all_containers_small.asi. After restarting,
original sizes apply. Before removing it, arrange cargo so it also fits
with its original sizes. Keep a separate save before changing cargo mods.

SOURCE
https://github.com/gutzufuss1477/DS2-Mods/tree/main/mods/all-containers-small
