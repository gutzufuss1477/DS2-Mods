# DS2 Mod Suite 1.6.0 validation

Validated on Windows on 2026-09-23 against repository base `1e0be00`.

## Release payloads

- Coffin Board Reworked 1.83.0: existing released ASI and INI, unchanged.
- APAS Memory Costs 1.1.0: existing optional replacement ASI, unchanged.
- Suite-specific APAS INI: `UnlockAll=0`; the standalone optional download remains unchanged.
- 21 mods, 18 configurable mods, 19 INI files and 177 settings.
- Catalog, executable metadata and application manifest report 1.6.0.

## Checks performed

`powershell.exe -NoProfile -ExecutionPolicy Bypass -File installer/build.ps1`
completed successfully, including compilation, catalog/schema validation, all
embedded payload SHA-256 checks, the complete self-test and ZIP packaging.

The release ZIP was extracted into a separate directory. Every entry in its
`SHA256SUMS.txt` matched. The extracted executable's `--self-test` also completed
with `SELF-TEST PASSED`.

Specific regression coverage:

- APAS fresh installation and legacy cost-only migration, each with and without
  a central profile: an explicit `UnlockAll=0` is present on disk.
- Existing explicit APAS unlock choices, custom costs, disabled cost
  modification, unknown keys, unrelated sections and comments are preserved
  when importing/migrating existing INIs.
- APAS opt-in and opt-out write the requested value; exactly one APAS ASI is
  installed. Repeating the same installation makes no changes.
- Missing APAS unlock keys are detected as configuration differences instead
  of being mistaken for the safe suite default (the native ASI defaults to 1).
- Coffin Board migration retains valid custom speed values, restores invalid
  values to release defaults and writes the new enable/network/mount switches.
- Existing tests cover installation, repair, obsolete-file migration,
  configuration preservation, removal guards, loader conflicts, transaction
  rollback, UAC result authentication and path guards.
- Added Git byte-preservation rules for INI payloads that otherwise change
  hashes on a Windows checkout. Other mods' committed payloads are unchanged.

No new in-game test was performed. This update integrates the existing mod
releases and validates manager behavior in temporary game directories.

## Updating this bundle after future mod releases

1. Update `catalog.json` version/descriptions/hashes and `payload-sources.json`.
2. Update `settings-schema.json` and migration handling for new/changed INI keys.
3. Keep optional progression-changing behavior opt-in; inspect the ASI's
   defaults for missing keys as well as the shipped INI.
4. Preserve exact payload bytes in `.gitattributes`.
5. Update suite version, executable/manifest metadata, documentation and tests.
6. Run `build.ps1`; deliver its ZIP and SHA-256 sidecar. The manager is offline,
   so pushing a standalone mod update does not refresh an already-built EXE.
