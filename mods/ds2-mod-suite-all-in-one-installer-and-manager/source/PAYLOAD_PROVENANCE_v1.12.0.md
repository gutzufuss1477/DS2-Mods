# Payload provenance — Mod Suite 1.12.0

Source baseline: `a24ca21` on origin/main, fetched 2026-10-15. No stable mod updates were found only on remote branches; the Sam development branch is superseded by the main-branch stable release.

- Sam Overhaul 1.1.0: copied byte-for-byte from the original release ZIP in the local release workspace. ZIP SHA-256 `6E3367FD18A02FA8C1EAC1484F7E9A75C1FF28BD2636FFDE417C3F04F05B8B8B`, matching the release checksum committed in `cdda096`. ASI and INI match that release manifest. MinHook license is included in the manager ZIP.
- Infrastructure One Unit 1.1.0: ASI and INI extracted unchanged from the committed release ZIP in `cd8d34b`; both match its SHA256.txt. Vendor copies avoid Git newline normalization of the released INI.
- Crafting & Equipment Overhaul 1.8.0: unchanged ASI and INI from `a24ca21`, verified against the committed release ZIP. The INI's first comment still says 1.7.0 in the standalone release; it is intentionally preserved verbatim. PCCStructures/UnlockAll is present and defaults to 1.
- Pickup Cargo Capacity 1.0.2: the release commit `c906b31` contains source and validation, but no final ASI/ZIP and no GitHub release asset. Rebuilt the unchanged source with its `scripts/build-msvc.cmd` after loading VS 2022 vcvars64. This is a new binary build, not claimed byte-identical to the standalone release. x64 version 1.0.2.0; MSVC /O2 /MT /GS /guard:cf, ASLR and NX. The documented standalone ASI hash is recognized as a known version for updating. The INI is unchanged from Git.

All 52 embedded files are SHA-256 pinned in catalog.json and resolved through payload-sources.json. Existing 1.11.0 binaries with unchanged filenames are identified by their exact old catalog hashes. Sam 1.0.0 uses the existing transactional obsolete-file migration.

New vendor payload hashes:

- `vendor/infrastructure-one-unit-v1.1.0/ds2_infrastructure_one_unit.asi`: `E8BB57DCE383CD8542041985E9165EF522FD02B40580A9EDAD715D8F5F1D2EEA`
- `vendor/infrastructure-one-unit-v1.1.0/ds2_infrastructure_one_unit.ini`: `56951CA933A6913C030B74768F2486DD9AF778DBE34166F5D3437BC4F400C40A`
- `vendor/pickup-cargo-capacity-v1.0.2/PickupCargoCapacity.asi`: `B5CF25CC85577FDA89A74080A82369DC2320BE86A1F67B7BAAA858E52ABEBC98`
- `vendor/sam-overhaul-v1.1.0/DS2_Sam_Overhaul_v1.1.0.asi`: `DCA56B593B0CF2B81A59CEA764455D7EEFF678371E492E6C6C610D2B5F6D9A3B`
- `vendor/sam-overhaul-v1.1.0/ds2_sam_overhaul.ini`: `7096EBB38553AB686A7E6E032408CEE2C6BFE30842591281AEF5D4708EA0D7BB`
