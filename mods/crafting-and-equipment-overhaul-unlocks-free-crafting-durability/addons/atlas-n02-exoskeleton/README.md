# ATLAS N02 Exoskeleton Addon 1.0.1

Optional appearance addon for [Crafting & Equipment Overhaul](../../README.md).
Install `ds2_atlas_exoskeleton_addon.asi` beside `DS2.exe`. See [installation](README_EN.txt),
[Deutsch](README_DE.txt), [changelog](CHANGELOG.txt) and [build notes](BUILD.md).

Graphite-black N02 appearance, red accents and matching menu image for the ATLAS skeleton.

The main mod remains at 1.10.1. When combining these addons, update both to 1.0.1.
Normal main-mod updates retaining Addon API V1, Visual Router V1 and the ATLAS
contracts need no addon rebuild. Game updates are separately guarded.

The original archives and saves are not rewritten. Remove the ASI with the game
closed to recover the main mod's appearance; keep the main mod installed.

Release preparation: 127 runtime assertions, nine real-DLL
compatibility cases, and a byte-identical source rebuild passed. The visual fixes
were confirmed on Test20/Test21; the cleaned 1.0.1 binary still awaits the final
game smoke test. See [manifest](RELEASE_MANIFEST.json). Nothing has been uploaded.
