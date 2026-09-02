**Description**

DS2 Mod Suite is a clean, standalone all-in-one installer and manager for 17 DEATH STRANDING 2: ON THE BEACH mods.

Choose exactly which mods you want, review their current installation state and apply everything through one English or German interface. The manager detects the Steam game directory, verifies the supported game build, installs missing mods, updates or repairs known versions and safely removes deselected suite mods.

All stable mod files and the tested Ultimate ASI Loader are embedded in the application. No additional downloads are required while the manager is running, and it does not connect to the internet.

This is more than a loose file bundle: the custom manager adds installation-state detection, centralized configuration, file validation, conflict protection, backups and transactional rollback.

Version 1.3.0 adds Coffin Board All-Terrain Speed 1.0.0 and supports the Steam PC executable version `DS2.exe 1.10.89.0`.

**Quick start**

1. Close the game completely.
2. Download the archive manually and extract the complete ZIP to any folder.
3. Start `DS2ModManager.exe`. Do not run it directly from inside the ZIP.
4. Select `DS2.exe` manually if the Steam installation was not detected automatically.
5. Choose English or German and check that the supported game version was detected.
6. Select the mods you want and choose **Apply changes**.
7. To customize an installed mod, open **Mod Settings**, save the desired values and choose **Apply changes** again.
8. Fully restart the game.

A Windows UAC prompt may appear when the game is installed in a protected directory.

This is a standalone installer. Do not install the downloaded archive through Vortex or another mod manager.

**Main features**

- Installs and manages 17 compatible DS2 mods from one application
- Clean English and German interface
- Automatic Steam game-directory detection
- Verifies the exact supported `DS2.exe` build
- Detects installed, outdated, incomplete and modified suite files
- Installs the tested Ultimate ASI Loader x64 v9.7.2 automatically when required
- Central settings interface for 14 configurable mods
- Validates 85 settings across 15 individual INI files
- Imports existing installed INI values
- Hides advanced and experimental settings by default
- Supports installation, update, repair and safe removal
- Creates backups before replacing or removing files
- Uses SHA-256 verification, atomic replacement and rollback protection
- Preserves customized INI files when a mod is removed
- Does not require an internet connection
- Does not directly edit `DS2.exe`, game archives or save files

The checkboxes represent the desired final state. Installed mods are selected automatically. Deselecting a managed mod removes its verified suite binary after **Apply changes**.

**Included mods**

**Vehicles and Cargo**

- Hill Assist & Speed Boost 1.1.0 - improves uphill driving and provides configurable Pickup and Tri-Cruiser speed settings
- Pickup Cargo Capacity 1.0.1 - increases the configurable cargo capacity of the Off-road Pickup
- Tri-Cruiser Cargo Capacity 1.1.0 - increases the Tri-Cruiser's usable cargo capacity
- Floating Carrier Cargo Capacity 1.0.0 - increases the Floating Carrier's usable cargo capacity
- Coffin Board All-Terrain Speed 1.0.0 - adds configurable land and water speed limits and acceleration while retaining native steering and wet grip

**Equipment and Progression**

- High-Density Backpack Modules 1.0.0 - allows significantly more functional backpack modules
- Sam Stats Booster 1.0.0 - provides a configurable boost to selected Sam-stat progression
- Porter Grade Booster 1.0.0 - provides a configurable boost to delivery category points

**Deliveries and Rewards**

- Lost Cargo Likes Booster 1.1.1 - awards a configurable Like bonus when returning lost cargo
- No Magellan Evaluation Penalty 1.0.0 - removes the Like penalty from Magellan deliveries

**Construction and World**

- Construction Anywhere 1.0.0 - loosens general PCC distance limits and tested restricted construction zones
- Construction Max Level on Build 1.0.0 - upgrades newly built supported Player PCC structures through the native flow to their maximum level
- Weapons Anywhere 1.0.0 - removes selected weapon restrictions in protected areas
- Zipline Range & Speed 1.0.0 - configures Zipline connection range and actual travel speed
- Chiral Bandwidth Costs 1.0.0 - sets exact bandwidth costs for supported constructions
- Infrastructure One Unit 1.0.0 - reduces remaining material requirements for supported infrastructure

**Interface**

- Remote Orders Overlay 0.2.0 - displays a facility's available orders directly on the world map

Only stable release builds are included. TEST, TRACE, diagnostic and obsolete reference builds are excluded.

**Central mod settings**

The **Mod Settings** window lists only currently installed mods that provide configurable settings. Installed mods without an INI and all uninstalled mods remain hidden.

The central interface does not replace the individual INI files used by the ASI plugins. It validates and safely manages those files in one place.

- Existing installed INIs can be imported
- Comments, blank lines, unknown keys and unrelated sections are preserved whenever supported
- **Save settings** stores the central profile
- The game files are changed only after **Apply changes**
- INI writes use the same backup, verification and rollback protection as mod installation
- Settings take effect after restarting the game
- Customized INIs are retained when a mod is removed

For a newly selected mod, install it first. It will then appear in **Mod Settings** if it provides configurable options.

**Updating and repairing**

Extract a newer suite release, start the manager and apply the desired selection again. Known older or incomplete suite files are updated or repaired automatically.

Modified or unknown files are never silently overwritten. The manager reports conflicts and stops when it cannot determine that a replacement is safe.

**Removing mods**

Deselect a suite mod and choose **Apply changes**. Only binaries matching known suite versions are removed. Customized INI files are retained so personal settings are not lost.

The automatically installed ASI Loader is intentionally retained because other ASI mods may still depend on it.

To remove the manager itself, delete the folder to which the downloaded ZIP was extracted. Remove `winmm.dll` manually only when you are certain that no remaining ASI mod requires it.

**Requirements**

- DEATH STRANDING 2: ON THE BEACH for PC / Steam
- Supported executable version: `DS2.exe 1.10.89.0`
- 64-bit Windows
- Microsoft .NET Framework 4.8
- Permission to write to the game directory

A separate ASI Loader download is not required.

**Safety and compatibility**

The manager validates the embedded catalog and settings schema, then verifies every mod payload and the ASI Loader with SHA-256 before installation. Unknown or multiple proxy DLLs are not overwritten and block potentially unsafe operations.

Every replacement is backed up and verified. If an installation step fails, the manager attempts to restore the previous state automatically. Removal is limited to exact known suite binaries; modified and foreign files are retained and reported.

The manager itself does not directly edit save files. Some included gameplay mods can nevertheless create results that the game stores normally. Back up an important save before using construction, infrastructure or experimental settings.

After a game update, do not assume compatibility. The manager refuses unsupported game executables. Wait for a release that explicitly supports the new build.

**Windows SmartScreen**

The application is not digitally code-signed, so Windows SmartScreen may display an unknown-publisher warning. This is expected for the current release. Download the archive only from the official Nexus page and verify the published SHA-256 checksum when desired.

**Source code**

The installer source and build scripts are available at:

https://github.com/gutzufuss1477/DS2-Mods/tree/main/installer

**Shout outs**

Thanks to ThirteenAG for Ultimate ASI Loader:

https://github.com/ThirteenAG/Ultimate-ASI-Loader

Thanks to ShadelessFox for Odradek and the DS2 RTTI research tools, and to everyone who proposed, tested and documented the individual mods included in this suite.

This is an unofficial community project and is not affiliated with KOJIMA PRODUCTIONS or 505 Games.
