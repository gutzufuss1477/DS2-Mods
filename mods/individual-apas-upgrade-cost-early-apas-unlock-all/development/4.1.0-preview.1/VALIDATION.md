# Validation — 2026-11-02

## Functional result

The new regression fixture failed against the unchanged 4.0.0 implementation:
`FinalizationFailures`, C++ line 267, found the constructor hook still installed
after a page-protection finalization error. This was reproduced before changing
production code.

After the corrections:

- Configuration: 30 cases plus packaged defaults passed.
- MemoryReadProtections, CostRules, MenuAndReload, Installation,
  FinalizationFailures, InactiveCallback, NativeAccounting and ConstructorAbi passed.
- Injected transient errors at both protection-open calls, both protection-restore
  calls and cache flush leave original bytes, vtable pointer and permissions intact.
- The shipped ASI's explicit initialization export passed a separate process test:
  no observed initialization on LoadLibrary alone; 32 concurrent InitializeASI
  calls initialize once; module remains loaded after the caller releases its
  handle; the Python host is refused as a non-DS2 process.
- Two builds in separate output directories reproduced the exact ASI hash below.
- MSVC 14.44.35207, SDK 10.0.26100.0; normal CRT/GS/CFG/ASLR/NX verified.

These are offline tests. No live-game acceptance test or complete runtime security
assessment has been performed.

## Artifacts and screening

| Artifact | Bytes | SHA-256 |
| --- | ---: | --- |
| APAS ASI | 155136 | `e97b7f30a2e1ec063d56caa6a11aee9cad76b9964c2bcc08a466c153bf27afab` |
| Standalone ZIP | 84061 | `147b94fb0ded43a4c072a92f05e685090b9c06d3ebdba1a7000f4388070027a8` |

Nexus **unpublished** mod 97, file **402** shows a green individual-file indicator
and an enabled Manual download button. The page-level warning also covers older,
quarantined diagnostic files; it does not describe this file alone.

- [Nexus test files](https://www.nexusmods.com/deathstranding2onthebeach/mods/97?tab=files)
- [ZIP report](https://www.virustotal.com/gui/file/147b94fb0ded43a4c072a92f05e685090b9c06d3ebdba1a7000f4388070027a8):
  **0/66**, analysis 2026-11-02 09:33:26 UTC.
- [Contained ASI report](https://www.virustotal.com/gui/file/e97b7f30a2e1ec063d56caa6a11aee9cad76b9964c2bcc08a466c153bf27afab):
  **2/71**, analysis 2026-11-02 09:37:07 UTC. Cynet: Malicious (score: 100);
  Microsoft: Trojan:Win32/Wacatac.C!ml.
- Local Defender signature **1.463.83.0** found no threats in separate custom scans
  of the package and the raw ASI. Real-time protection remained enabled.

The ZIP and raw-file results are different observations. A green Nexus ZIP does
**not** establish zero detections for the delivered ASI. Neither the source fixes
nor these results identify the proprietary detection rule or prove a false positive.
No vendor submission was sent, as requested. No public release was replaced.
