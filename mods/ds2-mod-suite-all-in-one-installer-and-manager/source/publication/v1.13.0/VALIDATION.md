# Suite 1.13.0 validation — 2026-11-02

All production payload hashes match the reviewed catalog. Updated ASIs and INIs
are byte-identical to their published standalone archives. Unchanged components
come from accepted no-APAS Suite file 379. APAS is original file 327 / version
3.0.0; neither experimental 4.0 rebuild is packaged. The manifest records the
final ZIP and manager executable hashes.

The production C# manager and a separate regression executable were freshly
compiled. The complete regression suite passed, including:

- Current and previous-version installation, repair, removal and rollback.
- Old central profiles, standalone INIs, opt-outs, inline comments and idempotence.
- 15 new fields, removal of five obsolete glove diagnostics from the UI, numeric
  bounds and the Warrior reserve-mode choices.
- Both ATLAS dependencies, old 1.0.0 upgrades, base deselection, incomplete
  installations, malformed dependency catalogs and invalid apply plans.
- APAS defaults, restoring a no-APAS profile and retaining explicit progression choices.
- Personal INI, backpack charm and Story Coffin metadata preservation.
- All 669 strings in ten languages, placeholders, persistent language choice,
  398 invariant settings and byte-identical generated INIs across languages.
- Payload tamper/missing-file checks, path guards and authenticated elevated results.

The final ZIP passed CRC, catalog/file-set, payload hash and SHA256SUMS checks.
It contains 54 payload files, the production manager, configuration, readme and
licenses; no self-test executable, credentials, personal metadata or nested ZIPs.

Microsoft Defender (signatures 1.463.73.0) reported no threats for the original APAS
payload, extracted build and final ZIP. This is one scanner's observation.
The separate Nexus result is recorded in manifest.json. No antivirus protections
were disabled, no binary bytes were altered to change a detection, and no
obfuscation or packing was added.

No new in-game test was performed for this suite build. Published component
runtime behavior remains the responsibility of the corresponding standalone
release; the manager tests use isolated temporary game-folder fixtures.

