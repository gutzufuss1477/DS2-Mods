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
were confirmed on Test20/Test21. On 2026-10-26 the user also confirmed the final
1.0.1 pair: save loading, Gold/Silver suit switching, outdoor boot gold and swapping
ATLAS/normal boots worked without a crash during the requested test. See the
[game-test record](validation/v1.0.1/game-test-result.json) and
[manifest](RELEASE_MANIFEST.json). Ready for publication; not uploaded yet.
