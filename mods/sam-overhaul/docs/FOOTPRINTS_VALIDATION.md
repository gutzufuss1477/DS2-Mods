# Footprint integration validation — 1.1.0-dev.1

## Scope and status

This is an unreleased, Git-only Sam Overhaul development build. It incorporates the tested native footprint suppression inside the existing ASI. No Nexus/GitHub release, release tag, release archive or Mod Suite publication is part of this update.

The combined build has passed automated regression and loader checks. **Combined gameplay testing with cargo visibility, movement, Autodrive and truck-weapon options remains pending.** Automated checks must not be presented as that gameplay test.

## Gameplay evidence reused from the confirmed native probe

The tester confirmed that newly walked footprints did not appear after an Odradek scan. Reloading a previously footprint-heavy save also removed its reconstructed footprint display; ordinary non-highlighted ground footprints were observed to disappear too.

The corresponding probe log recorded 2,981 blocked footprint append calls, 23 unrelated decal calls passed through, and zero invalid-context events. All three footprint instance buffers were empty after reload. These counts are from the probe test, not the integrated build.

Confirmed probe ASI SHA-256:
`87ab34f8d44c5aed47ec857bc8427379eadef5cb3225d684d63471d956e492a7`

Target: Steam DS2.exe 1.10.89.0, SHA-256:
`bf3d1c665545930bc850d8f5df486f7395885bb729d4fd408fdb03390de0765b`

The copied `footprint_core.h` has no functional edits. Its UTF-8/LF-normalized SHA-256 is:
`e054c990d66126e04bc0b21068d4f1877e4adffb4b73580cc029cd2c1cd16e63`

Private full logs, process-memory inventories, save backups and the large investigation manifest are deliberately not committed to Git.

## Automated integration checks

| Check | Result |
|---|---|
| Original Autodrive suite | PASS: 105,704 checks, 451 durations, four frame rates |
| Footprint runtime harness | PASS using production no-CRT compilation and byte primitives |
| Original argument forwarding | PASS: all twelve x64 arguments, including float/byte stack parameters |
| Selective resource suppression | PASS: three known resource IDs and verified metadata membership |
| Unrelated/invalid contexts | PASS: forwarded without changing caller LastError |
| Concurrency | PASS: 8,000 calls across four native worker threads |
| Hook removal | PASS: original code bytes restored and native behavior resumes |
| INI/integration guards | PASS: 104 checks; missing, legacy, malformed, truncated, disabled and enabled settings |
| Unsupported executable | PASS: enabled setting rejected before hook creation |
| Combined ASI loader | PASS: LoadLibrary, exported version/status, unsupported-host baseline rejection |
| Existing defaults | PASS: all 33 v1.0.0 option values retained |
| PE inspection | PASS: x64, ASLR/NX/high entropy, no TLS/CRT initialization section |
| Dynamic dependencies | KERNEL32.dll and bcrypt.dll only; no additional runtime DLL |

Run `scripts/build-development.ps1` to reproduce the build and these checks. Generated reports are written under `build/validation/`; staged development files have their own `SHA256SUMS.txt`.

## Integration decisions

The public setting is `[Footprints] HideFootprints=0/1`, default 0. Old INIs require no migration and retain vanilla footprints. Explicit 1 activates the copied filter after executable hash and native prologue validation. A failed footprint guard is logged and does not disable the base Sam Overhaul functions.

This is a startup setting, consistent with the existing Sam Overhaul INI. Restart the game after changing it. Saved footprints are prevented from being rendered when reconstructed during load; the mod is not a save-file eraser and does not forcibly edit an existing render list mid-session.

No second worker, log, INI or ASI is introduced. The existing version-independent worker exclusion is retained through the v1.0.0-compatible mutex. The standalone probe must be removed before using the combined build.

## Remaining gameplay regression before public release

Start with only one Sam Overhaul ASI and a normal x64 ASI loader. Compare HideFootprints=0 and 1 after separate restarts. Test both a previously footprint-heavy save and newly walked circles, with and without a scan. Check cargo visibility, spare shoes, monorail/zipline dismounts, a qualifying landing roll, Autodrive timing and the four truck weapons. Observe unrelated ground effects and confirm there is no duplicate standalone-probe hook.

No public release should be inferred until that combined gameplay check is recorded.