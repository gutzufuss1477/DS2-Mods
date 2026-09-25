# Classic Overlap, optional v1.1.0 build

This build uses the same executable guards, one-cell placement patches, 30-slot limit, charm menu, effect adapters and charm INI as standard v1.1.0. It excludes the three visual hooks that repack and hide module models. It also excludes the four automatic-anchor hooks. The game's own one-cell placement and visual calls therefore handle the chosen logical position and keep native module meshes visible, even if their geometry overlaps.

An occupied logical cell cannot hold a second module. Choose another free cell in the menu; the mod will not pick one for you. The two physical charms remain visible and the six additional charm slots remain invisible. Only equipped, unlocked charms contribute to the extended checks.

Install only `DS2_HighDensityBackpackModules_v1.1.0_Classic_Overlap.asi` in the active ASI loader folder. Remove the standard v1.1.0 ASI first. Keep `DS2_HighDensityBackpackModules.charms.ini` when switching variants. The log identifies `v1.1.0 Classic Overlap`, `visibility=CLASSIC_OVERLAP` and `placement=MANUAL_ONE_CELL` after successful patching.

The standard 1.1.0 binary is byte-identical to the previously published release after this build option was introduced. The optional binary passed the strict LLVM build, archive integrity check and local source/native instruction regression checks. The optional visual and menu behavior has not yet been tested in game.

Build with `scripts/build-llvm.ps1 -VisualStyle Overlap` or `VISUAL_STYLE=Overlap ./build.sh`, then package with `python scripts/package-release.py --visual-style Overlap`.
