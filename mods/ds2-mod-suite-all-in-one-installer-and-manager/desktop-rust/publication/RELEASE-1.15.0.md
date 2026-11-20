# Suite 1.15.0

- Added All Containers Small 1.0.0 and the optional ATLAS Boot Model Selector with a central boot model setting.
- Added the German/English Remote Orders V4 overlay with adjustable scaling alongside the multilingual edition.
- Recognizes existing Tri-Cruiser and BT Cord Cutting installations and backs up the original files during migration.
- Supports graphics proxies alongside a verified ASI loader. Lost Cargo and Chiral Bandwidth now use the standalone Nexus builds.
- Preserves UTF-16 INIs and shared settings when switching variants. The reported gameplay issues are still under investigation.

Nexus mod 72, main file 424 (20 November 2026). The downloaded ZIP matched SHA-256
`83cc3fb33004cd9f396e39ff1c5ee8db2e694a4db78402d7c2359faa31fcb9c2`. All 508 archive entries matched the Nexus manifest. Microsoft Defender reported
no threats in the release ZIP.

22 manager regression tests and one migration test with the official 1.14 package
and Nexus standalone binaries passed. The migration test checked installation,
repeat apply, settings retention and byte-for-byte restoration in a temporary folder.
The standalone Chiral Bandwidth source reproduced the shipped ASI and passed its
synthetic runtime tests. Lost Cargo matched the supported local executable and
had one matching connection signature. Gameplay verification for Lost Cargo,
Chiral Bandwidth and the reported N02 finale crash remains open.
