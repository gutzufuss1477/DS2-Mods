# Release review: 1.1.2

Review date: 2026-10-02. This document records the source cleanup and local verification. It does not claim that Nexus or every antivirus engine accepts this release.

## Reported issue

Nexus quarantined the 1.1.1 upload (mod 76, file 349). The public VirusTotal reports showed 9/68 detections for the ZIP and 15/71 for its ASI at inspection time. Several vendors used generic or machine-learning labels. Those labels alone do not identify the source-level cause. Local Microsoft Defender, with signatures 1.459.516.0, did not reproduce the detection in either the old ZIP or the new release.

- Nexus file page: https://www.nexusmods.com/deathstranding2onthebeach/mods/76?tab=files
- 1.1.1 ZIP report: https://www.virustotal.com/gui/file/614998619f871c2dea72df0307d5e866152484731b92c100996fa09d265d0a3e
- 1.1.1 ASI report: https://www.virustotal.com/gui/file/58f7007f9c4727d6173d623a5fb5762bf48dc720d88a265f6d88874dbdb93373

## Changes for review

- Removed 19 development/diagnostic hooks, development interaction scans, and experimental INI switches. Seven functional hooks remain: action availability, action cost, right/left/shared vehicle pickup gates, native right catch selection, and the scoped equipped-glove check.
- Preserved the native right-hand catch transition from 1.1.1, including checks for Combat Gloves, vehicle state, a fresh pending cargo ID, a free hand, and an inactive pickup action.
- Added file/product version 1.1.2 and descriptive Windows VERSIONINFO metadata. Metadata is not an Authenticode signature; the ASI remains unsigned.
- Validate every target signature before any hook writes. On a write failure, attempt restoration of this install's sites and disable further installation attempts. Retain trampoline storage for callbacks that may already be running. Rollback failure is logged and requires a game restart.
- Verify all installed jump bytes, including their destination displacement. Build generated code in writable memory, then change it to executable/read-only before use.

The source still performs the in-process memory patching necessary for an ASI game mod. It is not packed or obfuscated. There is no downloader, separate injector executable, network client, subprocess launcher, scheduled task, or antivirus configuration change in the release. `ReadProcessMemory` is used with `GetCurrentProcess()` for guarded reads of the loaded game. `VirtualProtect` is used for the documented game-code hooks, and the previous protection is restored after writes. The external ASI loader is not included.

## Exact build and verification

Build from the mod directory using:

```powershell
.\scripts\package-release.ps1 -Toolchain MSVC
```

Local tools: MSVC compiler 19.44 / linker 14.44.35228.0 and Windows SDK resource compiler 10.0.26100.0. MSVC was also used for 1.1.1; this cleanup does not switch compilers to seek a different detection result. LLVM build support remains available but was not tested locally for 1.1.2.

- Build succeeds with warnings treated as errors.
- Synthetic resource, configuration, native catch, and hook installation/execution tests pass.
- ZIP contains exactly `ds2_climbing_gloves_range.asi` and `ds2_climbing_gloves_range.ini` at its root.
- ASI size: 24,576 bytes (1.1.1: 39,936 bytes).
- The PE imports only KERNEL32.dll. ASLR, high-entropy addresses, and NX compatibility are enabled; no PE section is both writable and executable.
- File version and original filename match the release.
- Local Microsoft Defender custom scans of both the ASI and ZIP returned no threats, with antivirus and real-time protection enabled and signatures 1.459.516.0. Scan remediation was disabled for these individual diagnostic scans; protection settings were not changed.
- SHA-256 values are recorded in SHA256SUMS.txt and beside the ZIP.

## Remaining checks

The new binary has not yet passed a manual game restart test or a Nexus/VirusTotal review. The author requested local package delivery and declined uploading this build to VirusTotal. Keep the working 1.1.1 backup until the vehicle and ordinary glove actions have been checked. Do not describe this build as antivirus-cleared based only on the local scan.

If Nexus still quarantines it, provide the exact file hash, matching source, build command, and scan report to Nexus for manual review. If a vendor continues to flag the ASI, use that vendor's false-positive submission process. No antivirus exclusions are required by this mod.

- Nexus guidance: https://help.nexusmods.com/article/128-anti-virus-false-positives
- Nexus quarantine guidance: https://help.nexusmods.com/article/117-why-has-my-mod-been-quarantined
- VirusTotal guidance: https://docs.virustotal.com/docs/false-positive
