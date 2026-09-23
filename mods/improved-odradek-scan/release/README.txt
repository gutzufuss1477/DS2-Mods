Improved Odradek Scan 1.0.0
by gutzufuss1477
DEATH STRANDING 2: ON THE BEACH - Steam PC 1.10.89.0 (Windows x64)

FEATURES
- 500 metre default scan range, configurable in the INI.
- Horizontal 360-degree scan with native progressive wave movement.
- Extended visibility for recently scanned, eligible cargo, including usable
  cargo you have carried and dropped.
- Cargo distance after the recipient and likes; updates as you move.
- Spent/destroyed cargo keeps its normal nearby visibility without the extension.

INSTALL
1. Close the game.
2. Use a working x64 ASI loader, such as ThirteenAG's Ultimate ASI Loader:
   https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases
   The loader is a separate dependency and is not included in this archive.
   If you already use ASI mods successfully, keep your existing loader.
3. Extract ds2_odradek_scan.asi and ds2_odradek_scan.ini next to DS2.exe.
   Steam: Library > DS2 > Manage > Browse local files.
4. Start the game and scan normally.

CONFIGURATION
Edit ds2_odradek_scan.ini while the game is closed. Restart after changes.
RangeMeters=500         Radius in metres. Parser accepts 50-1000; 500 recommended.
FullCircle=1            Enable the horizontal full-circle scan.
FullCircleMode=Sphere   Recommended circular ground-scan mode.
ExtendCargoMarkers=1   Extend visibility of recently scanned eligible cargo.
VisualWaveScale=1       Keep the tested wave appearance.
DebugLog=0              Set to 1 only for troubleshooting.
Enabled=1               Set to 0 to disable the mod on the next game start.

The 500 m preset, cargo detection, wave appearance and distance placement were
confirmed in game. Larger values do not guarantee detection at that distance;
game-side limits were observed around 700 m in earlier 1000 m tests. Values below
200 m can leave the visible wave larger than the configured cargo display radius.
Native display expiry and marker budgets still apply.

UPDATE / UNINSTALL
Close the game before replacing files. Keep a copy of your custom INI if updating.
Remove this mod's ASI and INI to uninstall. Leave a shared ASI loader in place if
other mods need it. Do not load older Odradek Scan development ASIs alongside this.

COMPATIBILITY
Only Steam game build 1.10.89.0 has been validated. Exact build and code checks
prevent unsupported patching; game updates may require a mod update. Mods that
patch the same scan or cargo HUD code may conflict. Other combinations are not
universally tested. The mod changes runtime behaviour, not save-file data.

SOURCE
https://github.com/gutzufuss1477/DS2-Mods/tree/main/mods/improved-odradek-scan
