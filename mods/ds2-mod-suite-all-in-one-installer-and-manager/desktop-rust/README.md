# DS2 Mod Suite 1.15.0

Install, update and configure DS2 mods in one window. All mod files are included.

1. Close Death Stranding 2. Extract the **entire ZIP into a new folder**.
2. Start `DS2ModManager.exe` and accept the normal Windows administrator prompt.
3. Check the detected game folder, choose mods/settings, then **Apply changes**.

Existing installations are detected. Your current INI values, comments and
personal keys are retained. Do not copy the payload folders into the game by hand.
Keep the EXE and its payload folder together. The old manager does not need to run.

Windows 10/11 x64 and Microsoft Edge WebView2 Runtime are required. WebView2 is
normally present on Windows; if it is missing, install it from Microsoft:
https://developer.microsoft.com/microsoft-edge/webview2/
The manager checks the supported game fingerprint before making changes.

## Included

- 33 components, including All Containers Small, APAS and optional Free Mining.
- 401 editable settings in 24 INIs, with JSON profile import/export.
- English, German, French, Spanish, Italian, Simplified Chinese, Japanese,
  Korean, Brazilian Portuguese and Russian interface.
- Automatic dependencies, mutually exclusive variants and checked file backups.

**Crafting Overhaul** offers the stable 1.10.1 build and the optional ATLAS boot
model test. Select the test to choose a boot model. The Chiral Boots addon takes
priority. **Remote Orders** offers the multilingual overlay and the German/English
Nexus V4 overlay with adjustable scaling. Select one build per mod.

If you use a graphics mod with an additional dxgi.dll, winhttp.dll or d3d12.dll,
confirm the listed filenames belong to that graphics mod before applying changes.
Two recognized ASI loaders still require choosing a single loader.

**Free Mining is a beta alternative to Infrastructure One Unit.** Choose one.
Construction settings are shared. The manager sets the mining mode automatically;
the standard mining-cost value is preserved for switching back. Extraction with
zero crystals in inventory has not yet been confirmed in game.

APAS keeps its cost, Unlock All and Early Access settings. Progression-changing
options are off by default. Unlocks and other changes saved by the game can persist.
The new APAS lifecycle build has offline validation; a fresh in-game test of that
exact build is still outstanding.

## Update, remove and restore

Use the new manager with your existing game folder, review the detected selection
and apply. Deselected known mod binaries are removed; INIs and unfamiliar files
are retained. Select **Restore backup** to undo the latest operation by this
manager. Backups are in `DS2SuiteBackups` inside the game folder. Existing backups
from the old C# manager remain on disk; they are not imported into this restore UI.
Game saves are not backed up or restored by the manager.

Support: https://www.nexusmods.com/deathstranding2onthebeach/mods/72
Source: https://github.com/gutzufuss1477/DS2-Mods

## Build

The desktop application uses Rust/Tauri and a local embedded interface. Native
mods remain ordinary ASI/DLL/INI files, verified against the compiled catalog.
There are no remote payload downloads or changes to antivirus settings.

Use `stage_release.py --help` to stage the 1.14 baseline, Container ASI and
Nexus archives (files 243, 245, 260 and 414), then `build.ps1 -PayloadRoot <staged-folder> -OutputRoot <fresh-folder>`.
Rust MSVC, Visual Studio C++ Build Tools, Node/npm and Python are required. Cargo
and npm lockfiles are checked in. Release builds request administrator access;
debug/test builds use writable temporary fixtures without elevation.
