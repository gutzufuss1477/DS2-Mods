# Validation record — 1.0.0

## Live gameplay evidence

On 2026-10-03, the author confirmed that private **v0.6-test5** allowed many successive ramp tricks and traversal over a large part of the map. The corresponding local diagnostic log recorded at least 26 accepted repeat-helper returns. This observation followed an earlier attempt with insufficient airtime.

- Confirmed private ASI SHA-256: `53bd5b503d55058ac014dc8d3b84f4ed28ea79819f6e20a9bc7cdf1be0f3c676`.
- The observed component's ramp cap was **2**.
- The cap-3 path was not separately verified in live gameplay.
- Raw memory traces, local paths and the game's extracted code are not distributed.

## Release checks

The public source retains the cap-aware repeat behavior. Release changes remove telemetry counters and runtime polling, rename the files, add version resources and strengthen startup validation and memory-protection handling.

The native test allocates an isolated image inside Python, reads the dispatcher from the user's own supported executable and executes it with the production code cave. Engine helper functions are explicit test doubles. Neither the installed executable nor the game process is modified.

- **24,576 cases:** enabled/disabled, active/inactive, input/no input, ready/blocked cooldown, all 256 stage-byte values and caps 0, 1, 2, 3, 4 and 255.
- **2,000 repeat cycles:** 1,000 each at cap 2 and cap 3, without incrementing or wrapping the final stage.
- **68 signature-byte mutations** and **6 build-identity mutations** rejected.
- **14 INI parser cases** including valid values, whitespace, malformed input and overflow-sized input.
- Conflicting hook bytes preserved; test-only disable restores the original bytes in the isolated image.
- Code cave finishes as `PAGE_EXECUTE_READ`.
- Actual ASI loads through the Windows DLL loader in Python and remains inactive outside DS2.
- Two consecutive builds match byte-for-byte; PE file and product versions are 1.0.0.
- ZIP has exactly the ASI and default INI; extracted bytes match release files and checksums.

Machine-readable results and artifact/source hashes are in [native-verification.json](native-verification.json), [manifest.json](../release/manifest.json) and [SHA256SUMS.txt](../SHA256SUMS.txt).

**Limits:** these automated checks do not exercise actual animation or input acceptance in the engine. The final 1.0.0 ASI has not separately been tested in a live session. Evidence for gameplay comes from the confirmed predecessor's repeat logic. No compatibility claim is made for other game builds, storefronts, all ramp levels or combinations with other ramp mods.
