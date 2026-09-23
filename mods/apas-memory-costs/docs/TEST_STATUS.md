# 3.0.0-rc.1 validation - 2026-09-23

## Passed

- Warning-as-error x64 LLVM compile/link with no external CRT.
- Normal PE `.text`, `.rdata`, `.data`, `.pdata`, `.reloc`; ASLR and NX flags.
- 15 INI cases: default UnlockAll=0, independent switches, free/custom/max cost,
  invalid booleans, negative/overflow/malformed/truncated input.
- Cost rules for IDs 0..55 at targets 0, 1, 25, 1000000; special base IDs and
  native free/invalid costs preserved; wrong resource type passed through.
- Actual generated relay/trampoline execution with native Windows-x64 arguments,
  resource and cache consistency, newly created later resource, and restoration.
- Actual mapped supported game image: all four feature combinations, exact
  instruction/jump targets, restored native bytes/protections, refusal for late
  manager creation, old Unlock All conflict and changed PE timestamp.
- Actual native Memory accounting and activation code, executed in isolation:
  nonzero base cost fails the zero-capacity check; zero base cost passes. Free,
  one-point, custom and maximum cost capacity boundaries behave consistently.
- Exact public ASI loaded in an isolated Python host: unsupported host refused,
  20 repeated process-attach calls do not launch workers or rewrite the log,
  and module pinning keeps the callback image loaded after FreeLibrary.

## Limited live-start evidence

A preliminary build was loaded with the user's ASI loader and `UnlockAll=0`.
Read-only inspection found the expected cost hook and valid original trampoline
in DS2. The log also contained a later rejected installation attempt despite
those initialized globals. A repeated-initialization guard and module pin were
added afterwards and passed the isolated loader test above.

The preliminary startup is NOT a gameplay test and NOT a live-start verification
of the final candidate. The user explicitly requested no further game control and
will perform game testing. The game was not subsequently controlled or closed by
the agent. The preliminary installed ASI must be replaced with the packaged final
candidate after the user fully exits the game.

## User report and publication scope - 2026-09-23

The user started a fresh save but found no APAS menu just after the intro. Further
early-game progression testing is unavailable. On an Episode 9 save, the user
reported all enhancements selectable/unlocked and one-point costs. Read-only file
inspection still found preliminary ASI SHA256
`C6DE980E070F42B72B8BF5023092D506F6B8B280BF173E613DC645B46C5A7CC0`
installed, with UnlockAll=0. This supports the preliminary cost behavior, not the
final startup protection or optional unlock mode. Existing unlocked enhancements
do not establish that UnlockAll was tested.

The published candidate ASI SHA256 is
`1E8176A00062342899B0A15F609F72B3C0EEB112F70791376D78CA7FB28D143F`.
The user authorized a single Nexus main release now, without further progression
testing. It retains the release-candidate version and discloses these limits.

## Remaining gameplay checks

- Actual final ASI startup reports READY with the user's loader.
- Existing-save cost display, activation/deactivation and memory accounting.
- Return to title, reload, switch saves and later progression unlocks.
- New-save APAS initialization and the reported inability to buy upgrades.
- UnlockAll off/on, including vanilla-cost plus UnlockAll mode.
- Persistence behavior using an expendable or backed-up save.
- Same-scene FPS comparison against no APAS mod.

No affected Nexus save was available. The code-level findings are not sufficient
to claim that the user's exact bug is reproduced or fixed. See `TESTANLEITUNG_DE.md`.
