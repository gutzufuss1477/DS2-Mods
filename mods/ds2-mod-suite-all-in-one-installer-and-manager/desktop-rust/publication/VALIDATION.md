# Rust preview 1 — validation, 2026-11-02

Version: 1.13.0-preview.1. Native Rust/Tauri desktop candidate; not a public release.
The browser-only concept was discarded because manual copying would regress the
normal player workflow.

## Local checks

- Rust 1.99.1 MSVC, Tauri 2.12.5; dependency versions fixed in lockfiles.
- Release compilation succeeded without a packer or obfuscator.
- 14 Rust tests passed, including installation/removal/restoration, interrupted
  writes, stale plans, changed game build, unknown binaries, payload tampering,
  dependencies, exclusive variants, INI aliases/comments/custom keys and uint64.
- The release executable's headless package check verified all 54 payload files,
  29 components and 398 settings.
- Headless read-only game inspection detected the actual installation and matched
  its supported executable hash. Two local development builds were correctly
  identified as unknown/modified. No files in the live game were changed.
- Browser presentation checks covered dependency selection, searching settings,
  German and Simplified Chinese. All ten existing locale dictionaries are included.
- Microsoft Defender 1.463.83.0 reported no threats in the complete ZIP. Real-time
  protection remained enabled. This does not predict other scanners' results.

## Artifacts

| Variant | Bytes | SHA-256 |
| --- | ---: | --- |
| Manager-only diagnostic | 4185491 | `9f759d9ebaed737173ae2394aa211d7b7f41b38bbf8e92c809525160de1e9790` |
| Complete with APAS | 36933975 | `85fed044eff003f892e282a47eb9aadb2aa0d305e586d603daba4feccd5897b7` |

Both archives contain the identical manager EXE:
`81a96b3ccdbf807fb895c83d595b24bd2c4a868ffeb72eace31b8329e557ebb7`.
The complete archive adds the original 54 payload files without altering their
hashes. The manager-only archive cannot install mods and is a diagnostic control.

## Private Nexus test

New mod 97, **not published**:
https://www.nexusmods.com/deathstranding2onthebeach/mods/97?tab=files

- File 400: manager-only diagnostic, uploaded 2026-11-02 at 08:28:48 UTC.
  Nexus shows a green scan indicator and enables Manual download. VirusTotal:
  **0/66**. The actual Rust manager executable is **0/69** in the contained-file
  report, analysed 2026-11-02.
- File 401: complete package with APAS, uploaded at 08:29:10 UTC.
  Nexus **quarantined** the file. The observed VirusTotal report is **21/52**.
  The Rust rewrite therefore has not solved distribution of the complete package.
- Both files remain on the **unpublished** draft. No alternate delivery path or
  automatic payload download was introduced to bypass the quarantine.
- Original mod 72 was not edited in this task. Its description and primary file
  379 were compared against the prior saved state and remained unchanged.

Reports:

- Manager archive: https://www.virustotal.com/gui/file/9f759d9ebaed737173ae2394aa211d7b7f41b38bbf8e92c809525160de1e9790
- Manager executable: https://www.virustotal.com/gui/file/81a96b3ccdbf807fb895c83d595b24bd2c4a868ffeb72eace31b8329e557ebb7
- Complete archive: https://www.virustotal.com/gui/file/85fed044eff003f892e282a47eb9aadb2aa0d305e586d603daba4feccd5897b7

Counts are observations, not permanent guarantees. The visible contained-file
list for this archive is incomplete, so it does not by itself isolate one ASI as
the only cause. Earlier per-file findings for APAS and other native mods remain
separate evidence with their own analysis dates.

## Remaining release validation

The native WebView2 window has not been driven with desktop automation, respecting
the user's browser-only control preference. Native UI/file-dialog integration and
a fresh in-game test are still required before promotion. Protected-directory UAC
UX is not yet implemented in this portable preview. Antivirus results for native
mod payloads remain a separate issue; a clean manager control would not certify
the complete release or establish that earlier findings were false positives.
