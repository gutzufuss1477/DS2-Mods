# DS2 Mod Suite — Rust desktop preview

Native Windows desktop candidate for Suite 1.13.0, using Rust and Tauri/WebView2.
This is an unpublished development preview, not a replacement for the public
release. Native mod antivirus findings remain unresolved. A clean result for the
manager by itself is not a clean result for the complete mod package.

## Player workflow

Extract the complete ZIP, start `DS2ModManager.exe`, select mods and choose Apply.
Steam libraries and the previous manager's game path are detected automatically.
Use Browse only if automatic detection cannot find your installation. Mod files
are included in the complete package; there is no second payload download, browser
folder permission flow, command prompt or manual INI copying.

The standalone manager-only ZIP is a diagnostic control and cannot install mods.
It must not be presented as the normal player download.

Windows 10/11 x64 with Microsoft Edge WebView2 is required. WebView2 is normally
already installed; this portable preview does not silently install a runtime or
download dependencies. Protected game directories require ordinary filesystem
write access; this preview does not implement automatic UAC elevation.

## Current implementation

- 29 components, 398 centralized settings, 24 INI templates, ten UI languages.
- Compiled catalog and SHA-256 verification of each local payload; no network
  updater, shell launcher, process injection, mod execution or antivirus changes.
- Automatic dependencies and exclusive backpack variants; game fingerprint and
  read-only running-process checks before installation.
- Existing INI values, comments and unknown keys retained; legacy field aliases
  migrated. Previous C# settings profiles are read without modifying that app's
  files. JSON import/export uses native file dialogs.
- File replacement through completed temporary files, checked backups and a
  persistent transaction journal. Known binaries alone are removed; personal INIs
  and unknown binaries remain. Changes made since inspection abort the plan.
- Recovery of the latest transaction with hash checks. Interrupted transactions
  must be restored before another installation. Closing the window is blocked
  while a native filesystem operation owns the engine lock.

Normal configuration, installation and recovery are implemented in Rust. The HTML
interface is embedded in the desktop application and never opens in the user's
browser. A localhost browser preview can exercise presentation only; it explicitly
does not have the native bridge and cannot access game files.

## Build and test

Requires Rust MSVC, Visual Studio C++ Build Tools, Node/npm and Python. Use the
checked-in Cargo/npm lockfiles. No post-build packer, obfuscator or encryption is
used. Native payloads remain external, unmodified files with their original hashes.

```powershell
python prepare.py --payload-root D:\path\to\verified\DS2-Mod-Suite
npm ci --ignore-scripts
cargo test --manifest-path src-tauri\Cargo.toml --locked -- --test-threads=1
npm run build
```

`prepare.py` derives definitions, default INI text and translations from the
existing source catalog. Generated data and native binaries are not committed.
For short build paths set `CARGO_TARGET_DIR` outside this long checkout path.
The resulting executable supports `--verify-package <report.json>` and
`--inspect-game <report.json>` for headless, read-only checks.

Tests cover default schema validity, dependencies, variant conflicts, exact uint64
values, migration, personal INI data, payload tampering, foreign binaries, stale
plans, game changes, rollback and backup restoration. They use temporary test game
directories and synthetic payloads; they do not install into a real game.

Native window interaction and a fresh in-game test are still required before a
public release. Browser preview testing is not equivalent to a native WebView2 UI
test. Filesystem permission/elevation UX also needs final release validation.
