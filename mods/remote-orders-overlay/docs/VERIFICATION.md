# Release Verification

## v0.2.0 runtime validation

The source-compiled v0.2.0 ASI started successfully and displayed expected facility orders during in-game smoke tests. German, French, Italian and Spanish overlay configurations were exercised. French duplicate-name cleanup was confirmed in-game; Italian and Spanish may retain a rare additional grammatical article before column truncation.

The release remains read-only and does not accept orders or modify mission/save state.

## v0.2.0 binary

The current production ASI is built deterministically by `scripts/build_source_release.ps1` with PE timestamp zero.

- ASI SHA-256: `02C3046773001DC6499B6B670C6DD0903F37F29EF5D19F1DB4A8DB1F2F183821`
- Nexus ZIP SHA-256: `CF348945B3F898A1145F61EB3800E5A3FF5610842833F1D89CE35538023C4BFF`

All release hashes are recorded in `SHA256SUMS.txt`.

## Legacy binary lineage

Validated working baseline:

`A325FEC6CAD46F478FA1812DF93BE75D8EC63EE7AFC1618C9B3272F28E8CBD25`

Runtime-tested v0.1.3-alpha no-log release:

`696D05285C23F2D73EE27C23827A27BA347C76D79DF0A84F7DB29E164DCB695C`

The only changes are ten fixed-width UTF-16 path replacements to the Windows `NUL` device. Executable code and the critical PE runtime sections remain unchanged.

## Historical withdrawn build

A historical source-cleanup v0.1.3 build with modified initialization did not allow the game to start and was withdrawn. It is unrelated to the subsequently corrected and in-game-tested v0.2.0 source build.
