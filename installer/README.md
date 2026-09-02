# DS2 Mod Suite v1.2.0

Clean all-in-one mod manager for **DEATH STRANDING 2: ON THE BEACH**.

## Quick start

1. Close the game completely.
2. Extract the complete ZIP to any folder.
3. Start `DS2ModManager.exe`.
4. Optionally choose **English** or **Deutsch** in the language menu.
5. Check that the game version and ASI Loader were detected correctly.
6. Select the mods you want to use.
7. Select **Apply changes** to install them.
8. Open **Mod Settings** to configure the installed mods in one place, then select **Save settings**.
9. Select **Apply changes** again to write the saved settings to the game folder.

The checkboxes always describe the desired final state. Installed mods are selected automatically. Deselecting a managed mod removes its verified suite binaries when changes are applied.

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
- High-Density Backpack Modules 1.0.0
- Sam Stats Booster 1.0.0
- Porter Grade Booster 1.0.0
- Lost Cargo Likes Booster 1.1.1
- No Magellan Evaluation Penalty 1.0.0
- Construction Anywhere 1.0.0
- Construction Max Level on Build 1.0.0
- Weapons Anywhere 1.0.0
- Zipline Range & Speed 1.0.0
- Chiral Bandwidth Costs 1.0.0
- Infrastructure One Unit 1.0.0
- Remote Orders Overlay 0.2.0

TEST, TRACE, diagnostic and older reference builds are not included.

## Deutsch

1. Das Spiel vollständig beenden.
2. Das vollständige ZIP entpacken und `DS2ModManager.exe` starten.
3. Oben rechts bei Bedarf **Deutsch** auswählen.
4. Spielprüfung kontrollieren und die gewünschten Mods anhaken.
5. **Änderungen anwenden** wählen, um die Mods zu installieren.
6. Über **Mod-Einstellungen** die installierten Mods konfigurieren und **Einstellungen speichern** wählen.
7. Erneut **Änderungen anwenden**, um die gespeicherten Werte in den Spielordner zu schreiben.

Das zentrale Menü zeigt ausschließlich installierte Mods mit konfigurierbarer INI. Nicht installierte Mods und Mods ohne INI werden ausgeblendet. Es ersetzt die mod-eigenen INIs nicht, sondern verwaltet sie sicher an einer Stelle. **Einstellungen speichern** legt zunächst nur das zentrale Profil ab. Erst **Änderungen anwenden** schreibt die einzelnen INIs mit Sicherung, Prüfung und Rollback-Schutz in den Spielordner. Änderungen werden nach einem Neustart des Spiels aktiv. Eigene INIs bleiben beim Entfernen einer Mod erhalten.

## Build

On Windows with Visual Studio Build Tools 2022:

```powershell
.\build.ps1
```

The build validates the catalog, settings schema and unique payload mapping, embeds every installation file in the EXE, runs the complete self-test and creates a deterministic portable release ZIP under `dist\`.

This is an unofficial community project and is not affiliated with KOJIMA PRODUCTIONS or 505 Games.
