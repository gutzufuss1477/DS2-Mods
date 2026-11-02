# DS2 Mod Suite 1.14.0

Portable offline manager for 30 DS2 components, with 398 editable central INI
settings, ten languages, automatic dependencies, verified backups and restore.
Includes APAS 4.1.0-preview.1 and optional Free Mining beta.

[Download on Nexus](https://www.nexusmods.com/deathstranding2onthebeach/mods/72?tab=files&file_id=405)
· [Usage and build instructions](desktop-rust/README.md)
· [Validation and scan evidence](desktop-rust/publication/RELEASE-1.14.0.md)

Extract the complete ZIP into a new folder, open `DS2ModManager.exe`, accept the
normal Windows administrator prompt, review your detected mods/settings and apply.
Windows x64 with WebView2 is required. The Rust/Tauri manager keeps ordinary local
ASI/DLL/INI payloads; there is no separate payload download. Existing INIs and
older profiles are preserved. Infrastructure and Free Mining share their settings
and are mutually exclusive, as are the two Backpack variants.

The full ZIP was accepted on the unpublished test page (file 404). Original mod
72/file 405 is uploaded and primary, but its Nexus virus scan is still processing.
Previous file 379 remains listed as a fallback. All 19 offline tests passed and local Defender reported no threats. The
archive still has a 5/67 VirusTotal report; acceptance is not a zero-detection
claim. Native startup confirmation and fresh APAS 4.1 / zero-inventory Free Mining
in-game testing remain outstanding. Free Mining is explicitly optional beta.

The production sources and catalog are in `desktop-rust/`. `source/` and its
publication records retain the historical C# releases and their scan results.
Game assets and compiled payloads are not stored in Git.
