# Rust preview 2 with APAS 4.1 — 2026-11-02

Development version 1.13.0-preview.2 integrates the APAS 4.1.0-preview.1 lifecycle
and rollback corrections. Only the APAS ASI and default INI changed among the
54 native payload files; the other 52 payload files retain their exact hashes.
The published Suite and APAS pages, primary downloads and source catalog remain
unchanged. The development catalog records APAS 3.0.0 as an upgradeable previous
binary, preserving the existing 4.0.0 development hashes as well.

## Validation

- All **15 Rust tests** passed. The new APAS-specific test exercises upgrade,
  preservation of all four settings, inline comments and personal keys, and
  exact restoration of the previous ASI and INI.
- The packaged release executable verified **29 components, 398 settings and
  54 payload files** through its headless package-check command.
- The original 24 INI templates and ten locale dictionaries are retained.
- Local Defender **1.463.83.0** reported no threats in the complete archive.
- APAS's offline fixtures, real initialization-export test and reproducible
  native build are recorded in its development VALIDATION.md.
- No live game was modified or started; fresh in-game and native-window
  acceptance tests are still outstanding.

## Artifact

- ZIP: `DS2_Rust_Manager_1.13.0-preview.2_complete.zip`, **37,006,257 bytes**.
- ZIP SHA-256:
  `f79b10f43c52a2dc7b194ab64783f06877d8500d664d842427130208a18d462f`.
- Manager EXE SHA-256:
  `c7be31348e589effa5e3f9c8ecb9c0c66e45c294f52ad99d4cdf49b8c8ad1a3b`.
- APAS ASI SHA-256:
  `e97b7f30a2e1ec063d56caa6a11aee9cad76b9964c2bcc08a466c153bf27afab`.

## Screening

Uploaded as file **403** on **unpublished** Nexus test mod **97**. No public
release was replaced.

[Complete archive report](https://www.virustotal.com/gui/file/f79b10f43c52a2dc7b194ab64783f06877d8500d664d842427130208a18d462f):
**5/67**, analysis 2026-11-02 09:43:19 UTC.

| Engine | Label |
| --- | --- |
| AhnLab-V3 | Trojan/Win.Generic.C4946587 |
| Bkav Pro | W32.Malware.F82D6892 |
| DeepInstinct | MALICIOUS |
| Elastic | Malicious (moderate Confidence) |
| MaxSecure | Trojan.Malware.698973404.susgen |

Microsoft is Undetected in this **archive** report. The raw APAS ASI has a separate
**2/71** report including Microsoft and Cynet; its standalone ZIP is **0/66** and
downloadable on Nexus as file 402. These are not interchangeable results.

The complete archive's visible VirusTotal Relations list contains only 52 entries
out of the archive's 501 files and does not enumerate the native payloads. It
cannot isolate the remaining detections to a particular mod. No definitive
false-positive determination or proprietary-rule diagnosis has been made.

Nexus final status is recorded after its processing finishes.
