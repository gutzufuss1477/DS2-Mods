# DS2 Mod Suite v1.11.0

Clean all-in-one mod manager for **DEATH STRANDING 2: ON THE BEACH**.

## Quick start

1. Close the game completely.
2. Extract the complete ZIP to any folder.
3. Start `DS2ModManager.exe`.
4. Choose your preferred language in the language menu (ten languages).
5. Check that the game version and ASI Loader were detected correctly.
6. Select the mods you want to use.
7. Select **Apply changes** to install them.
8. Open **Mod Settings** to configure the installed mods in one place, then select **Save settings**.
9. Select **Apply changes** again to write the saved settings to the game folder.

The checkboxes always describe the desired final state. Installed mods are selected automatically. Deselecting a managed mod removes its verified suite binaries when changes are applied.

## Languages

English, German, French, Spanish, Italian, Brazilian Portuguese, Russian, Simplified Chinese, Japanese and Korean. The language is saved and can be switched in the main window. Menus, mod descriptions, setting labels, help and common validation messages are translated. Language resources are embedded; the manager works offline. Official mod names, filenames, INI keys and machine values remain unchanged. Technical logs and operating-system diagnostics may remain in their original language. Translations were AI-assisted and checked for coverage and formatting; native-speaker feedback is welcome.

Existing valid central choices retain priority. Newly introduced settings are imported from installed INIs before release defaults are used. The interface language never changes INI keys or values.

## Central mod settings

The **Mod Settings** window lists only installed mods that provide configurable INI settings. Uninstalled mods and mods without an INI are hidden. Internally, the visible mods still use their own individual INI files; the manager creates and updates those files automatically because this is the format the ASI plugins read directly.

- Existing installed INIs can be imported on first use.
- Values are validated before saving and again in the elevated installation process.
- Comments, blank lines, unknown keys and unrelated sections are preserved whenever the runtime format allows it.
- Known duplicate keys in older Sam Stats Booster and Porter Grade Booster files are normalized safely.
- Advanced and experimental options are hidden by default.
- **Save settings** stores the central profile. The actual game INIs are changed only after **Apply changes**.
- INI changes use the same backup, atomic replacement, verification and rollback transaction as mod installations.
- The game must be restarted before changed mod settings take effect.

The manager language and central settings profile are stored under:

```text
%LocalAppData%\SimonMods\DS2ModSuite\
```

Customized INIs are retained when a mod is removed so personal settings are not lost.

## APAS optional progression

The suite bundles the stable APAS Unified 3.0.0 ASI. Unlock All and Early Access are **off by default**. Install APAS, open **Mod Settings → APAS Memory Costs → APASUnlocks** and select either option, save, apply changes and restart the game. Early Access exposes the APAS menu in Episode 2; Unlock All bypasses node prerequisites. They can be used independently. Located nodes and unlocks may persist in the save after disabling the options. Back up your save before enabling them. Set APASMemoryCosts / Enabled to off if you want vanilla costs.

Old INIs receive explicit UnlockAll=0 and EarlyAccess=0 during updates, including updates without a saved central profile. Existing explicit progression choices and valid cost settings are retained. Coffin Board updates preserve valid existing settings and add missing reworked options with the release defaults.

## Changes in 1.11.0

- Added Jump Ramp Unlimited 1.0.0 and Beach Jump with Cargo 1.0.0.
- Replaced Sneaky Sam with Sam Overhaul 1.0.0: cargo visibility, movement, Autodrive and truck weapons. Known Sneaky Sam 1.0.0/1.0.1 ASIs are backed up and removed during migration.
- Updated Crafting & Equipment Overhaul to 1.7.0 with ATLAS equipment, gold skeleton appearance and four suppressed weapons; all new options are centrally configurable.
- Updated Climbing and Combat Power Gloves to 1.1.1, fixing right-side vehicle cargo stowing with Combat gloves.
- Expanded central configuration to 377 settings across 24 INIs for 23 configurable mods. Saved choices, explicit opt-outs and comments are preserved; missing options use release defaults.
- Added blocking checks for old standalone ATLAS, suppressed-weapons and Jump Ramp probe ASIs. Unknown files are retained.
- Updated all ten interface languages and compact Nexus documentation. Includes 26 distinct mods and 27 selectable entries.

Sam Overhaul replaces Sneaky Sam entirely. Known old ASIs are selected for upgrade automatically and removed in the same backup/rollback transaction. Unknown old versions block the replacement. The source/release is imported unchanged from `origin/chatgpt/sam-overhaul-v1.0.0` at `779f7cf`.

Crafting retains its stable `crafting-unlocks` identity and `ds2_crafting_unlocks.ini`. It has 132 original item choices plus two ATLAS recipes and four suppressed-weapon recipes (138 total). ATLAS equipment, gold appearance and suppressed weapons default on in the public 1.7.0 release; explicit existing opt-outs are retained. Free Crafting and durability remain off by default. Turning off ATLAS/suppressed recipes does not remove existing saved equipment. The weapon-name language setting (game / German / English) is independent of the manager's ten interface languages.

Combat glove diagnostic settings are advanced and retain the exact 1.1.1 release defaults. Keep them unchanged unless troubleshooting. Level 2 pickup range must be at least Level 1.

Beach Jump carries cargo attached to Sam, not vehicles, Floating Carriers or ground cargo. Jump Ramp repeats the final available aerial trick using normal input.

Move `ds2_overpowered_equipment.asi`, `ds2_more_silenced_guns.asi` or `ds2_jump_ramp_probe.asi` out of the game folder before installing the corresponding integrated mod. These standalone files are never silently deleted.

Backpack has two mutually exclusive variants. Classic Overlap has not been tested in game. Before removal, unequip extra charms, reduce modules to a vanilla-compatible layout, save and close the game. The charm-state INI is retained.

## Safety

- Supports only the verified Steam build `DS2.exe 1.10.89.0`.
- Validates the embedded catalog and settings schema and verifies every installation payload by SHA-256 before installation.
- Automatically installs the tested Ultimate ASI Loader x64 v9.7.2 as `winmm.dll` when required.
- Accepts only allow-listed loader hashes. Unknown or multiple proxy DLLs are never overwritten and block installations and updates.
- Creates a backup before replacing or removing files.
- Replaces files atomically and verifies both installation and rollback results.
- Removes only unchanged managed binaries with known hashes.
- Retains modified or foreign files and reports them explicitly.
- Revalidates the immutable configuration plan in the elevated process and authenticates its result.

Without administrator rights, backups and logs are stored below the LocalAppData path above. Elevated write operations store them in the protected `.ds2-mod-suite\` folder inside the game directory. The automatically added loader is intentionally retained when all suite mods are deselected because other ASI mods may still depend on it.

## Included mods

- Hill Assist & Speed Boost 1.1.0
- Pickup Cargo Capacity 1.0.1
- Tri-Cruiser Cargo Capacity 1.1.0
- Floating Carrier Cargo Capacity 1.0.0
- Coffin Board Reworked 1.83.0
- High-Density Backpack Modules 1.1.0
- Climbing Power Gloves Range 1.1.1
- Sam Stats Booster 1.0.0
- Porter Grade Booster 1.0.0
- Lost Cargo Likes Booster 1.1.1
- No Magellan Evaluation Penalty 1.0.0
- Construction Anywhere 1.0.0
- Construction Max Level on Build 1.0.1
- Weapons Anywhere 1.0.0
- Zipline Range & Speed 1.0.0
- Chiral Bandwidth Costs 1.0.0
- Infrastructure One Unit 1.0.0
- Remote Orders Overlay 0.2.0
- Extended BT Cord Cutting Range 1.0.0
- APAS Memory Costs 3.0.0
- Proficiency Bonus Multiplier 1.0.0
- Improved Odradek Scan 1.0.1
- High-Density Backpack - Classic Overlap 1.1.0
- Crafting & Equipment Overhaul 1.7.0
- Jump Ramp Unlimited 1.0.0
- Beach Jump with Cargo 1.0.0
- Sam Overhaul 1.0.0

26 distinct mods, 27 entries (two Backpack variants), 377 settings, 24 INIs and 23 configurable mods. Only release payloads are bundled; old Sneaky Sam binaries are not bundled.

## Deutsch

1. Das Spiel vollständig beenden.
2. Das vollständige ZIP entpacken und `DS2ModManager.exe` starten.
3. Oben rechts bei Bedarf **Deutsch** auswählen.
4. Spielprüfung kontrollieren und die gewünschten Mods anhaken.
5. **Änderungen anwenden** wählen, um die Mods zu installieren.
6. Über **Mod-Einstellungen** die installierten Mods konfigurieren und **Einstellungen speichern** wählen.
7. Erneut **Änderungen anwenden**, um die gespeicherten Werte in den Spielordner zu schreiben.

Das zentrale Menü zeigt ausschließlich installierte Mods mit konfigurierbarer INI. Nicht installierte Mods und Mods ohne INI werden ausgeblendet. Es ersetzt die mod-eigenen INIs nicht, sondern verwaltet sie sicher an einer Stelle. **Einstellungen speichern** legt zunächst nur das zentrale Profil ab. Erst **Änderungen anwenden** schreibt die einzelnen INIs mit Sicherung, Prüfung und Rollback-Schutz in den Spielordner. Änderungen werden nach einem Neustart des Spiels aktiv. Eigene INIs bleiben beim Entfernen einer Mod erhalten.

APAS Unified 3.0.0 bietet optional **Unlock All** und **Early Access**. Beide Optionen sind standardmäßig ausgeschaltet. Aktivierung: **Mod-Einstellungen → APAS Memory Costs → APASUnlocks**, speichern, Änderungen anwenden und das Spiel neu starten. Freischaltungen und gefundene Knoten können im Spielstand bleiben. Vor dem Aktivieren den Spielstand sichern. Ältere INIs erhalten fehlende Optionen ausdrücklich mit Wert `0`.

Die neuen Handschuh-Einstellungen trennen Kletterhandschuh-Reichweiten vom Kampfhandschuh-Schalter. Stufe 2 muss mindestens die Reichweite von Stufe 1 haben. Vorhandene Werte bleiben erhalten; bei alten zentralen Profilen werden neue Optionen zuerst aus der installierten INI übernommen. Die 132 Crafting-Gegenstände sind unter den erweiterten Einstellungen in 13 Gruppen geordnet. „Standard übernehmen“ folgt dem Wert DefaultUnlock. Sam Overhaul ersetzt Sneaky Sam und bündelt Frachtsichtbarkeit, Bewegung, Autopilot und Lkw-Waffen mit 33 zentralen Optionen. Bekannte alte Sneaky-Sam-Dateien werden gesichert und entfernt. Neu sind außerdem Jump Ramp Unlimited und Beach Jump with Cargo sowie Crafting 1.7.0 und Handschuhe 1.1.1.

## Build

On Windows with Visual Studio Build Tools 2022:

```powershell
.\build.ps1
```

The build validates the catalog, settings schema and unique payload mapping, embeds every installation file in the EXE, runs the complete self-test and creates a deterministic portable release ZIP under `dist\`.

This is an unofficial community project and is not affiliated with KOJIMA PRODUCTIONS or 505 Games.
