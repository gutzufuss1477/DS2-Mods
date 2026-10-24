# ATLAS N02 Exoskeleton Addon 1.0.0

Optional appearance addon for [Crafting & Equipment Overhaul](../../README.md).
Distribute under the same Nexus mod page as an optional file. Requires main mod
1.10.1 or a later host preserving Addon API V1, Visual Router V1 and ATLAS identities.
Chiral Boots 1.0.0 can be absent or present; its published ASI remains unchanged.

Install one ASI beside `DS2.exe`. Remove that ASI to restore the main mod's normal
ATLAS skeleton appearance while keeping the main mod and its stats/items.
See [English instructions](README_EN.txt), [German instructions](README_DE.txt),
[changelog](CHANGELOG.txt) and [build notes](BUILD.md).

The release uses the exact user-tested Test04 binary, SHA-256
`42dde96ca4e423efa73c90cf5a924799c1f17c1229da85f7c2aaaa801e2d0587`.
Its diagnostic startup line retains the Test04 build identifier. The public source
rebuild was byte-identical after normalizing only the PE build timestamp.
The user confirmed the final main-mod Test05 and both visual addons working together
on 2026-10-24. That combined test does not by itself establish every removal/load
permutation or compatibility with unknown future game updates.
