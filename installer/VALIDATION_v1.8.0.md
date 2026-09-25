# DS2 Mod Suite 1.8.0 validation

Validated on Windows on 2026-09-25 from Git `origin/main` at `af0a07f`.

## Source and Nexus comparison

- Crafting Overhaul 1.3.0 release ZIP: `831AC1335860E6612BA720A491321FD8DFC58032C8A92FC5638B699F3BEAC084`, matching the Nexus main file for mod 86. The current Nexus file display name still says V1.2.0; its ZIP bytes match the repository's 1.3.0 release.
- APAS Unified 3.0.0 release ZIP: `143AF18E095BD0ED625DF15FDFCAC2A20E3B998A5E29E3FC56BB296D7EECEDB0`, matching the Nexus main file for mod 79. EarlyAccess and UnlockAll default to 0.
- Backpack Classic Overlap 1.1.0 ZIP: `C999FCF113EDC4F4FAC044F4A709B44B129FCC2EFE3B287EF69D84BCCDFA46CE`, matching the Nexus optional file for mod 68. The standard 1.1.0 binary is unchanged.

## Manager integration

- 23 distinct mods and 24 entries because the two Backpack variants are mutually exclusive.
- 20 configurable mods, 21 INI files, 312 validated settings. Crafting exposes 120 item switches in advanced settings; existing item choices and inline comments are retained.
- APAS 3.0.0 replaces the prior RC binary. Existing UnlockAll choices are retained, and a missing EarlyAccess switch is initialized to 0. Both switches default to 0 for a new installation.
- Switching Backpack variants removes the known opposite ASI and retains the game-managed charm INI. The manager rejects a plan selecting both variants and blocks switching over an unknown modified opposite binary.

`powershell.exe -NoProfile -ExecutionPolicy Bypass -File installer/build.ps1` passed compilation, catalog/schema checks, every source and embedded payload SHA256 check, full executable self-test and deterministic ZIP packaging.

The ZIP was extracted separately. All six packaged files matched `SHA256SUMS.txt`. The extracted EXE completed its full self-test with exit code 0 and `SELF-TEST PASSED`. The self-tests include variant switching, conflict and rollback protections, Crafting configuration adoption, APAS migration, existing Odradek/Backpack/Coffin migrations, idempotence, safe removal and authenticated elevated results.

The expanded 312-field profile exceeded the Windows command-line budget when passed uncompressed to the elevated installer. v1.8.0 uses bounded GZip transport for that internal argument; the full profile round-trip, malformed input rejection and size limit pass the extracted EXE's self-test.

No game was controlled and no installed game files were changed. These tests validate manager integration in temporary game directories; Classic Overlap still awaits in-game visual/menu verification. Standalone Crafting and APAS gameplay evidence is documented in each mod's repository.

ZIP SHA256: `C4DE6967AA392E56B54D19D27C9A3CC8590BC260A9C03A81A57CA4AA6EED3515`
