# Suite 1.14.0 — release validation, 2026-11-02

The Rust desktop manager is the successor to the C# Suite. It keeps the portable
ZIP / EXE / local payload workflow. `desktop-rust/catalog.json` is its release
catalog; the older `source/` directory documents the historical C# release.

## Contents and provenance

- 30 components; 398 editable fields and one automatically managed variant flag.
- 24 canonical INI templates, ten interface languages and 56 payload files.
- Baseline: the verified Suite 1.13.0 payloads, already compared with all 26 owned
  standalone Nexus mod pages on 2 November. No further production Git changes
  were found at `origin/main` (`68f0854`).
- APAS: the lifecycle/rollback correction `4.1.0-preview.1`, standalone test file
  402 and previously accepted complete test file 403. ASI SHA-256:
  `e97b7f30a2e1ec063d56caa6a11aee9cad76b9964c2bcc08a466c153bf27afab`.
- Free Mining: optional Nexus 67/file 371. Local ASI and INI hashes exactly match
  the [Nexus manifest](https://mod-file-manifests.nexusmods.com/c0/14/c3/c014c39e-afb7-4307-84b4-c2a6f4261a88).
  Source commit: `b06121c8dfcf9c384f4f4639a2caaf5dab2145e4` on
  `codex/infrastructure-mine-free-nexus-beta1`. ASI SHA-256:
  `e4bf086cbbaa927deaf310507f53c79ca792ed1be4092fbb76e4590fd703577b`.

Free Mining and Infrastructure One Unit share canonical setting IDs. The chosen
variant controls `MineProductionFree`; the standard crystal-cost setting is
retained for switching back and ignored by the Free Mining native implementation.
Only one variant is selected and installed at a time. No binary or INI payload
was altered merely to change scan classification.

## Validation

- All **19 Rust tests passed**, covering installation/removal, known old and
  renamed binaries, exact variant detection and switching both ways, older JSON
  profiles, APAS migration, comments/personal values, file tampering, stale plans,
  game fingerprint changes, transaction rollback and exact backup restoration.
- Browser UI: both variants exclude each other, shared edited values survive
  switching, and Free Mining title/description/settings render in all ten
  languages. No JavaScript errors were observed. This is a browser test, not a
  native WebView2 runtime test.
- ZIP CRC, complete manifest and all 56 catalog payload hashes passed. The
  embedded backend data and UI data are identical.
- The compiled release PE manifest was extracted with the Windows SDK manifest
  tool: `requireAdministrator`, `uiAccess=false`. Debug tests run without
  elevation; no protections or permissions were bypassed.
- Defender custom scan of the ZIP and extracted package: no threats, exit 0,
  signatures `1.463.83.0`. Remediation was disabled for this read-only scan.

## Artifacts

`DS2_Mod_Suite_v1.14.0.zip`, 37,033,375 bytes:
`6f533ff26aa9a3c9c5c21e45067a94b0d2c64f77a6e531ef8ec2e738c88e0d51`

`DS2ModManager.exe`:
`a93377e360ffdbbc29eb2108cc9a2e44437fd76ec6697bc5f34d6f9837505c31`

## Publication and remaining limits

The exact complete ZIP was uploaded to unpublished Nexus test mod 97 as file 404.
Scan result and final original-mod publication are recorded separately below
when observed. No manager-only or remote-download substitute is being promoted.

Native window/UAC startup confirmation is pending. Fresh APAS 4.1 in-game testing
and Free Mining with an actually empty crystal inventory remain outstanding.
Existing historical antivirus findings in `scan-results.json` are retained;
Nexus acceptance and one local clean scan are not a universal safety guarantee.
The old manager's backups remain on disk but are not imported into the Rust
restore UI. Game-save changes are outside the file-transaction backup.
