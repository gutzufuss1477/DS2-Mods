# Validation: main 1.10.1 and optional N02 1.0.0

The user confirmed the final combined game setup on 2026-10-24 after testing the
mixed-fabrication correction: main Test05, N02 Test04 and Chiral Boots 1.0.0.
The exact hashes and scope are in [game-test confirmation](validation/game-test-2026-10-24.json).
This supersedes the earlier pending Test05 status. It is not an exhaustive new
claim about every removal/load permutation or every future game update.

## Release identity

The source export reproduces the tested main Test05 binary byte for byte after
normalizing only the PE build timestamp. Main release 1.10.1 changes only the
runtime version label. N02 is shipped as the exact accepted Test04 binary; its
source rebuild likewise differs only in PE timestamp. No meshes, materials,
rendered icons, game logic or addon callbacks were redesigned for packaging.
See the two public-build verification JSON records in `validation/`.

## Offline verification

- Main policy/configuration regressions, including equipment, cosmetics, menu,
  previews and persistence, passed. Details: `validation/offline-release-tests.json`.
- Production mixed-fabrication callbacks: 235 assertions; production metadata
  IO/save/load wrappers: 19 assertions. Engine calls use isolated test fixtures.
- Six actual DLL-loading checks passed: master disabled, unsupported executable,
  invalid configuration and the corresponding PCC cases. No DS2 process used.
- Native baseline integration check: 95 exact signatures, no overlap, AMD64,
  ASLR/NX and no writable executable sections. Additional Test05 special-vehicle
  sites were verified before the accepted game test; active runtime sources are
  preserved. The baseline count does not claim to enumerate every later hook.
- Five actual release-binary discovery/API cases passed: no addon, boots only,
  N02 only, both and reverse module load order. These are offline compatibility
  checks, not five additional in-game rendering tests.
- Both stable-interface build gates passed. Published shoe addon source/ASI is
  unchanged. Packaged ZIP entries are flat and their bytes/CRC/SHA-256 verified.

## Scope

Supported DS2 PC build: 1.10.89.0, exact executable SHA recorded in validation.
Normal main-mod content/recipe/stat updates can retain existing addons by preserving
the frozen interfaces and ATLAS identities. This does not make unknown future game
layouts compatible. Appearance addon removal is separate from full main-mod removal.
The main mod still owns custom item definitions and stats. Keep personal INI and
Story appearance metadata; neither personal saves nor caches are distributed.

Nexus packages are ready for upload to mod 86 as MAIN and OPTIONAL files. No Nexus
upload is claimed by this release preparation.
