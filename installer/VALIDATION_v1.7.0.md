# DS2 Mod Suite 1.7.0 validation

Validated on Windows on 2026-09-24. Base: `0271711`; incorporates the APAS
release/publication commits `4f615f7` and `f90996d` from the local APAS branch.

## Included changes

- Improved Odradek Scan 1.0.0: released ASI/INI, with seven central settings and
  the gameplay-tested 500 m default. Advanced modes/logging remain advanced.
- High-Density Backpack Modules 1.1.0: released ASI; exact-hash migration removes
  the known 1.0.0 filename. The game-managed charms INI is never a suite payload.
- APAS Unified 3.0.0-rc.1: unchanged published ASI/INI. This is explicitly a
  release candidate with limited gameplay evidence, not a newly validated stable
  mod. Unlock All remains off by default; existing choices are retained.
- 22 mods, 19 configurable mods, 20 INI files and 184 validated settings.

## Verification

`powershell.exe -NoProfile -ExecutionPolicy Bypass -File installer/build.ps1`
passed compilation, embedded catalog/schema validation, every payload SHA-256
check, the complete self-test and release ZIP packaging.

The ZIP was extracted separately. Every packaged SHA256SUMS entry matched and
the extracted executable completed its full self-test with exit code 0 and
`SELF-TEST PASSED`.

New tests cover Backpack old-filename detection and replacement, blocking an
unknown legacy ASI, idempotence, removal and byte-preservation of charm state.
The old catalog hash was also checked against the real released 1.0.0 ASI.
Odradek tests cover bounds/modes, a 1.6 central profile upgrade, required-key
completion for an incomplete standalone INI, customized settings, repeated
apply and removal with INI preservation. Existing APAS opt-in/out, migration,
rollback, loader conflict, UAC authentication and path guards all pass.

No game was controlled and no installed game files were changed. This validates
manager integration in temporary game directories, not additional gameplay
coverage for the bundled mods. See each mod's own evidence and limitations.

ZIP SHA-256:
`74FB47DCF84849AF5C68DC6DD88DCE7E5ABB1090C4AAB480550AAC7C38BD0FDA`
