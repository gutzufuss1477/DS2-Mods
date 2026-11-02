# Distribution review — 2026-11-02

Later update: the APAS 4.1 lifecycle/rollback corrections and complete Rust
preview 2 were accepted by Nexus on the unpublished draft. Residual scan findings
and the unverified browser download are documented in [PREVIEW2.md](../PREVIEW2.md).
The earlier observations below remain historical results for their exact hashes.

## Result

The original APAS 3.0.0 ZIP offered by Nexus is not a clean contemporary control.
Its earlier VirusTotal report was 0/67 on 2026-09-25. Reanalysis of the **same
archive hash**, requested on 2026-11-02, returned **30/67** at 08:53:29 UTC.
Microsoft reported Undetected in that report; BitDefender, Avast/AVG, ESET and
Google were among the detecting engines. Detection labels are not a diagnosis.

- Nexus mod 79, file 327 remains offered with a Manual download button when
  observed. No Nexus file, description or publication state was changed.
- ZIP: `143af18e095bd0ed625df15fdfcac2a20e3b998a5e29e3fc56bb296d7eecedb0`.
- Original ASI: `8b1f9910a46b1e7929084acef23340131382bba2d059b2cd24a9bd86ea16e4b9`.
- The original ASI and the ASI in Rust preview 1 are **byte-for-byte identical**.
- A changed archive format, file name, mirror or downloading the same ASI later
  would not repair its behavior or resolve its classification.

[Original archive scan](https://www.virustotal.com/gui/file/143af18e095bd0ed625df15fdfcac2a20e3b998a5e29e3fc56bb296d7eecedb0)

## Focused static analysis

REA with Ghidra 12.1.4 decompiled all 13 locally identified functions; 17 imported
functions were external, with no failed or remaining functions in that inventory.
This is static analysis, not sandbox execution or a complete security assessment.

- The entry point starts a worker with CreateThread.
- The startup path checks the executable name DS2.exe, obtains its own process's
  main module, validates supported executable anchors, and installs game hooks.
- The read helper at `0x180002e60` calls ReadProcessMemory with GetCurrentProcess.
- The normal import table contains only KERNEL32.dll and 17 named imports, with
  no networking imports. This bounded observation does not prove that all
  possible runtime behavior is known.
- The hook installer at `0x180001c00` allocates writable memory and subsequently
  changes it to executable/readable memory; original code-page permissions are
  temporarily changed during hook installation. These are expected modding
  mechanisms but do not establish which vendor rule is triggering.
- The old read fallback still has the protection weakness already documented in
  the October review: after a failed read it changes a queried page to execute/read
  without first rejecting guard/no-access pages. This weakness was corrected in
  the later 4.0.0 candidate. That correction did not resolve Nexus quarantine.

Evidence IDs:

- Imports: `ev_6bc15c131314bd9704c592d28068d1d27a06ec506b85b83420078c4b06ced0f1`.
- Overview: `ev_8283ebd4c68cd76ec9b235e55e64270be609028dbe84279832ce9a6d8da604ab`.
- TLS/load configuration: `ev_d6eaf10191ce67cee89060128b25f3fd551ccc9abc6ad4296f72c94ac54ee7bc`.
- Decompiled inventory: `ev_627fed4ba881b391a6ae9c30575ac0a92e6a8490927da8db89f0dbdd8d100448`.

The evidence bundle and pseudocode are retained locally in the distribution-audit
folder. No mod code was executed, no live game files were changed, and the Ghidra
session was exported and closed.

## Previously attempted corrections

The October 4.0.0 rewrite used normal CRT/GS/CFG/ASLR/NX, removed executable relay
allocation and replaced the old prerequisite updater. The user tested that first
rewrite in Episode 2. Its standalone test was nevertheless quarantined (7/68 at
the time). A subsequent memory-protection correction passed offline regression
tests but was also quarantined as standalone file 385 (8/67 on 2026-10-16).
The later candidate has not had a new in-game acceptance test.

Corrected candidate ZIP:
`b464a188dadfc7c7eea05136680b685e6fd8e594df50e88142db02783ffd559f`.
Corrected candidate ASI:
`5191d9a1a39169d36e5dca613a83e985143650895ed714489cdb032bf3217a5a`.

The prepared source kit was extracted into an isolated local directory and built
on 2026-11-02. It reproduced that exact ASI SHA-256 (154,624 bytes). All offline
fixtures passed: configuration (30 cases plus packaged defaults), memory-read
protections, costs, menu/reload, installation, native accounting and constructor
ABI. No game was started and no save or live game file was accessed by the tests.
The candidate's new VirusTotal reanalysis request reached a CAPTCHA; it was not
completed. Its 8/67 figure remains the historical October report, not a fresh scan.

## Delivery architecture decision

An ordinary online installer is technically appropriate for keeping the player
workflow to Start → Select → Apply. It would fetch only selected, versioned release
artifacts over HTTPS, verify archive and member hashes, stage all files before any
game mutation, then reuse the Rust manager's central INI/backup transaction engine.
It must not contain the author's Nexus API key, require SDKs, or require manual
archive extraction/copying by players. Interrupted or blocked downloads must leave
the game unchanged. Releases require review of the actual delivered bytes and
provenance; changing distribution must not conceal unresolved detections.

This is a design decision, **not an implemented downloader or a cleared release**.
Moving the currently detected files to GitHub would not satisfy the release gate.
No mirror, disguised archive, encrypted payload or delayed payload fetch was added.

Nexus explicitly requires discussing utilities whose operation depends on internet
transfers with staff and providing source. That platform requirement is separate
from resolving vendor findings. A vendor review request is prepared locally; it
has not been sent, and no existing file was presented as a confirmed false positive.

References:

- https://help.nexusmods.com/article/128-anti-virus-false-positives
- https://help.nexusmods.com/article/117-why-has-my-mod-been-quarantined
- https://help.nexusmods.com/article/28-file-submission-guidelines

## Unresolved

Exact proprietary detection trigger; vendor assessment/correction; clean current
reports for each proposed payload; fresh in-game test of the corrected APAS build;
and any Nexus approval for an online installer. The overall release is still blocked.
