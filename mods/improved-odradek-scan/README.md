# Improved Odradek Scan

[Nexus Mods](https://www.nexusmods.com/deathstranding2onthebeach/mods/85) · [GitHub release](https://github.com/gutzufuss1477/DS2-Mods/releases/tag/improved-odradek-scan-v1.0.0)

**Version 1.0.0** · by gutzufuss1477 · Steam PC 1.10.89.0 (Windows x64)

A configurable **360-degree Odradek scan with a tested 500 m default radius**.
Reveals distant eligible cargo, including usable cargo you have carried and dropped,
and places the distance after the recipient and likes. Spent/destroyed cargo keeps
vanilla nearby visibility without the range extension. Native progressive scan
movement, marker expiry and budgets remain in use.

## Install

1. Close the game. Use a working x64 ASI loader; the loader is not included.
   [Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases)
   is available separately. Keep your existing loader if ASI mods already work.
2. Extract `ds2_odradek_scan.asi` and `ds2_odradek_scan.ini` beside `DS2.exe`.
   Steam: Library > DS2 > Manage > Browse local files.
3. Start the game and scan normally.

## Configure

Edit the INI and restart the game:

```ini
[OdradekScan]
Enabled=1
RangeMeters=500
VisualWaveScale=1
FullCircle=1
FullCircleMode=Sphere
ExtendCargoMarkers=1
DebugLog=0
```

`RangeMeters` accepts 50–1000. **500 m is the recommended gameplay-tested preset.**
Higher settings cannot guarantee detection at the full radius: earlier 1000 m tests
encountered an additional first-detection limit around 700 m. Below 200 m the native
wave may exceed the configured cargo display radius. Keep `VisualWaveScale=1` for
the tested wave appearance. Set `Enabled=0` to disable or `DebugLog=1` to diagnose.

## Update, removal and compatibility

Close the game before replacing files. Preserve your custom INI when updating.
Remove this mod's ASI and INI to uninstall; keep a loader used by other mods.
Only Steam build **1.10.89.0** is validated. Exact build/code checks prevent
unsupported patching, and game updates may require a mod update. Mods patching
the same scan/HUD code may conflict. Do not load older test builds simultaneously.
This mod changes runtime behaviour, not save-file data.

## Build and verify

Requires Windows x64, PowerShell, Python 3 and LLVM (`clang-cl`, `lld-link`). Set
`LLVM_BIN` if LLVM is not installed in its usual Windows location.

```powershell
./scripts/build-llvm.ps1
./scripts/package-release.ps1 -TargetExe 'C:/path/to/DS2.exe'
```

Packaging runs the complete synthetic and mapped-image test suite against the
supported executable, then creates a ZIP with ASI, INI, README and changelog.
The game entry point is never executed by the tests. The game binary is not included.
Build-only output is under `build/public/`; ready-to-use files are under `release/`.

User gameplay tests confirmed cargo, wave appearance and the after-likes layout
at the 500 m preset. Local checks cover configuration, exact hooks/rollback,
actual native sensor boundaries, cargo filters and native text formatter behaviour.

[German instructions](README_DE.md) · [Release notes](CHANGELOG.md) ·
[Technical summary](docs/IMPLEMENTATION.md)
