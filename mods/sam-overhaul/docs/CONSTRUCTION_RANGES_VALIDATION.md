# Sam Overhaul v1.1.0 - construction range development history

> Historical development journal. Early prototype entries describe superseded code and pending validation, not the final stable v1.1.0 release. See README.md and CHANGELOG.md for the confirmed public behavior, supported executable and configuration.

## Scope

Development only. Steam DS2.exe v1.10.89.0, executable SHA-256 `BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B`. Both INI sections, `[GeneratorRange]` and `[TimefallShelterRange]`, default to `Enabled=0`; known local tests used `Enabled=1`, `RangePercent=200`. No public release has been made.

## Observed in-game results

- Generator: the physical charging boundary and visible blue Odradek circle matched at 200% for the tested structure; the user also reloaded a save and confirmed persistence (2026-10-08).
- Shelter native protection: the engine's `DSRainShelter` native config property `Range` at `+0x760` was expanded from 4 to 8 m. User verified timefall no longer degraded carried cargo outside vanilla range (2026-10-08).
- Shelter native repair: `DSConstructionRepairSprayComponentResource+0x20`, native radius 4 -> 8 m. At a separately verified ~4.98 m, the original spray effect appeared and a damaged container's coating was restored to 100% (2026-10-08).
- Shelter ring: the visual size is a *diameter* in `DSOdradekEffectInstance+0x2A0`, halved by native getter RVA `0x1D72260`. The tested dev.19 ASI sets 16 m visual size for the intended 8 m repair/protection radius. An earlier observation suggested a match, but a **later screenshot following reload on 2026-10-09 shows the repair event only firing deeper inside the enlarged circle**. Exact 2D/3D boundary alignment is therefore unconfirmed. The genuine 30 m effect-activation distance remains unchanged.
- Footprint suppression still loads in the combined ASI. No separate ReShade or standalone footprint ASI is used.

## Technical constraints / non-features

- The old dev.9/dev.10 Jolt sphere candidate belonged to an unrelated entity and was removed. The dev.13 synthetic repair contact experiment was removed. The dev.15 read-only coating probes were removed. Current shelter repair uses native resource data; no direct cargo/coating writes.
- Both shelter Jolt protective cylinders are validated by owner/vtable/shape/body ID, and an engine Jolt `NotifyShapeChanged` refresh is scheduled once per registered body.
- Native protection and repair values, visual geometry and engine effect-activation distance are distinct; never change the activation distance to enlarge the circle.
- All source changes live on the development branch. Do not tag or publish on Nexus based on this partial verification.

## Diagnostics and open checks

The observed live dev.19 log contains `NATIVE_CONFIG_RANGE_SYNCED`, `REPAIR_RADIUS_NATIVE_SYNCED`, `CYLINDER_SCALED_AND_JOLT_REFRESHED`, `ODRADEK_VISUAL_DIAMETER_SYNCED`, and `ODRADEK_RENDER_INSTANCE_DIAMETER_SYNCED`.

The active shelter discovery profiler reported expensive calls (later observed peak 252,566 microseconds, usually 16–20 calls per 15 seconds, many >8 ms). These are native-hook discovery elapsed times, **not measured FPS or proven render-thread stalls**. Profiling/optimisation is open; do not claim it is resolved from a subjectively smooth test.

Remaining game regression: reload the same shelter save and ensure ring/repair alignment persists; test construction stages 1/2/3 and multiple generators; check cargo/coating/weather at the boundary; verify no adverse Autodrive, visibility, zipline/monorail, landing-roll, weapon or footprint interactions.

## Reproducible automated tests

Run `scripts/build-development.ps1`. It compiles the combined ASI and runs the original Autodrive regression, footprints/ABI and configuration checks, and binary/loader validation. It stages a local *development* package without installing it in the game. These static/host tests are not substitutes for the remaining gameplay checks.

## dev.21 geometry discovery (not yet tested in game)

Static JPH CylinderShape method RVA `0x278D790` (vtable `0x345CA98`) uses `+0x30` for Y half-height and `+0x34` for horizontal X/Z radius. In the live dev.20 session, both rain-shelter bodies were observed as `half-height=3, radius=8, convex=0.05`, at component offsets `+0xF8/+0x100` for the tested stage-3 shelter. The native repair source list `DAT_14623EAD8+0x548` contains the exact `DSConstructionRepairSprayComponent` instances, and the repair path RVA `0x11B04F0` checks their contact-active byte `+0x70` **before** checking native resource radius `+0x20`. This makes the extended 8 m repair scalar alone insufficient if contact activation remains limited. The Jolt half-height change is a testable hypothesis, not a proven fix for that contact byte.

The dev.20 player distance probe incorrectly read the WorldPosition triple from `+0xF0`, excluding the real X coordinate. Correct offset is `+0xE8` (X, Y, Z), Jolt uses vertical Y. The dev.20 square-distance records must not be used for calibration. dev.21 logs X/Z horizontal and Y vertical as a passive proxy for cargo position. A true repair event must be observed with damaged cargo.

## dev.21 global live test / dev.22 scoped source gate — 2026-10-09

The first 50s direct branch probe at native `DSBaggageComponent` query RVA `0x11B0698` passed preflight, bypassed `JE +0xC5` for 50s, then restored the original bytes. The original code was verified afterwards in the running process. A longer, manually controlled live probe used the same test patch and the tester observed coating restored from uphill slightly inside the large circle, and from downhill approximately on the visible edge. These are **user-observed visual results**, not a geometric distance measurement. The live patch remains process-memory-only until manual off/game exit.

The dev.22 new *unverified* integrated method does **not** reproduce the global bypass. An x64 MASM gate stub at native branch `0x11B0698` checks the original `DSConstructionRepairSprayComponent` vtable `0x03297208`, resource vtable `0x03296670`, owner `0x03119BC8`, owner member offsets `+0x28/+0x30`, and the exact native radius configured from the INI. It only permits eligible sources to reach the original native 3D distance/strength calculation; all other sources retain original `+0x70` gate semantics. Register and alternate-path behavior was verified with the **production assembly code** in fourteen synthetic ABI tests. Native instruction signature, relay, ASLR/PE baseline and default-off behavior are guarded.

Upcoming validation: fresh launch dev.22 with global live patch automatically absent, repeat from both directions using damaged cargo, verify no unrelated repairs, performance, multiple structures, circle/spray and reloading. The old 'In Bunker ausruhen' and 'Verschnaufen' rest labels remain unresolved as a separate localization/action question.

## dev.22 crash / dev.23 safe branch repair (2026-10-09)

The integrated dev.22 ASI loaded its native hook successfully but DS2.exe crashed while loading the saved game. The first observed 6-byte trampoline patch used E9 + rel32 + NOP but mistakenly calculated the rel32 displacement relative to site+6 instead of the E9 instruction's correct site+5; the destination was **relay minus one byte**. dev.22 was removed and the SHA256-verified dev.21 binary restored with the user's INI unchanged. Windows Error Reporting did not immediately present a contemporaneous DS2 exception record, so root-cause assignment remains a code-level diagnosis, not a completed crash-dump analysis.

dev.23 fixes the relative jump computation and validates the exact E9 target both in production and in synthetic x64 real CPU execution tests. To avoid dereferencing owner/resource/member pointers of native repair sources before their object lifecycle stabilizes, dev.23's MASM hook uses an active-shelter-validated, bounded source identity registry with 13-second expiration. Unknown sources retain DS2's original active-contact test. All source/radius/cargo mechanics are unchanged. The new dev.23 ASI still requires an actual save-load game test and FPS check.

## dev.23 production installer / relay / native branch integration tests

An isolated executable image was reserved in a separate Windows test process with the exact CMP at DS2 RVA 0x11B0693, the original JE +0xC5 at RVA 0x11B0698, the original native continuation opcode at 0x11B069E, and a simulated skip branch at 0x11B0763. SamConstructionRepairGateInstall from the production C++ object was called against this image, and the real E9+NOP, FF25 near relay, MASM gate, source registration and native branch return paths were EXECUTED on the CPU. 20 checks passed, including keeping original vanilla behavior for unregistered sources and falling back safely for resource/owner pointer changes. Test source: tests/native_repair_gate_integration_tests.cpp, tests/native_repair_gate_integration_wrapper.asm; runner tests/build-run-native-repair-gate-integration.cmd.

These tests close the dev.22 test coverage gap (the previous tests ran the MASM stub separately from the production installer). They are NOT a substitute for a real DS2 save-load test of the installed dev.23 ASI. No rollback tests of dev.21 are required; dev.21 is already a verified backup.

## dev.24 source whitelist loss after game pause

Live dev23 DS2 PID7628, ASI module 0x7FF9FEFD0000; E9, near relay and MASM stub addresses matched the actual linked map. DS2 native source registry at RVA 0x623EAD8 contained two eligible RepairSpray sources (radius8) and one vanilla radius4. The MASM registry count was three, but source pointers in all published entries were zero. Last validation tick ages 335–477 seconds matched stopped native game updates while the process remained open. The old 13-second real-time TTL removed correct sources during pause/background.

dev24 does not expire registered native objects based on wall time. On its existing worker thread, a short nonblocking shared-lock read validates native table membership plus exact component/resource/owner/radius/member tuple, and invalidates only removed or changed objects. The cargo-query MASM remains identical. Game result pending.

## dev.24 corrected player geometry and shelter rest-state passive observation

The current WorldPosition double triple at +0xE8 is X,Y,Z. DS2 game-space Z is elevation and X/Y is the terrain plane (verified by earlier native geometry and dev13 diagnostics). The old dev21/developed dev23 probe mislabeled Y as vertical and used X/Z as ground plane, so previous horizontal-only numerical labels must not be used for distance claims. dev24 logs correct XY ground squared distance and vertical Z squared difference. It additionally reads shelter +0x422 (native contact flag set by enter/exit MsgDsNotify 0x1C705F2D / 0x0A5CCAE8) as rest_contact, without writing any state or synthesizing messages.

The native rest prompt is controlled by actual contact enter/exit events and cannot be fixed by renaming "Verschnaufen" or by the baggage-only native source gate. Rest-contact activation and the user-facing "In Bunker ausruhen" wording remain an independent issue after native 8m repair range parity.

## dev.25 German Timefall Shelter rest label

Tested live 2026-10-09: two known loaded LocalizedTextResource objects (vtable DS2.exe RVA 0x3455CC0, UUIDs RestInShelter_A 6c045dfc-2718-6043-96bb-1049deaeebac and RestInShelter_B de6d51ad-045d-3846-9ff4-e36be4ef337b) contain exactly 18 bytes "In Bunker ausruhen". A bounded, reversible in-process test changed the text to 12-byte "Verschnaufen" and length 12 in both; the user confirmed the UI correction while the native action was untouched. Other resources e.g. "Take a Breather" and "Rest" have separate UUIDs.

dev.25 production implementation adds an exact-UUID-and-original-bytes filter to the already active Sam Overhaul streaming listener; no extra native hook, DLL or localization file. It writes in-place only if 18-byte writable buffer is still the verified German text, and preserves original game memory ownership. It is enabled by the new TimefallShelterRange/FixRestPrompt switch, default on when the shelter feature is enabled. The new integrated ASI requires a fresh game load regression; no install while DS2 is running.

3D repair geometry note: for Z offset ~2.75m at real spherical repair radius 8m, horizontal XY threshold is sqrt(64-7.5625)~7.513m; projected 8m ring will extend roughly 0.49m further on the upper slope. This is natural DS2 native geometry; do not adjust native repair radius/visual circle to compensate in one direction.

## dev.26 independent native coating radius / terrain slope

Real in-game test on dev25: downhill coating cloud approximately matches terrain-projected blue 8m circle, uphill 8m native 3D sphere falls inside the same circle due to Z elevation delta ~2.75m. Native geometric horizontal reach sqrt(8^2-2.75^2)=7.513m. A separate 215% repair sphere (8.6m) offers sqrt(8.6^2-2.75^2)=8.148m XY reach, while rain protection/visual ring/Jolt physics remain 200%/8m. Both native repair component range and exact matching SamGateRadiusBits use the SAME INI RepairRadiusPercent. The user's game INI is not modified; missing key defaults to 215% in code. dev26 gameplay validation outstanding.

## dev26: handle localization resources preloaded before native listener
dev25's log printed STREAMING_LISTENER_REGISTERED but no NATIVE_REST_TEXT_PATCHED; user continued to see native "In Bunker ausruhen" at the outer edge. The native text objects were loaded before Sam installed its streaming listener. A validated live text-only patch on dev24 had already proved that changing the two exact RestInShelter UUIDs fixes the HUD without changing the action. dev26 supplements the listener with a time-sliced one-shot scan for existing LocalizedTextResource instances on committed writable MEM_PRIVATE pages. Each worker poll is strictly limited to 64MiB or ~12ms; no per-frame work, no additional DLL or process, no modifications unless the exact vtable/UUID/original German text matches. The scanner is disabled after both known UUIDs are changed. The existing listener remains for later reloads. The scan uses ReadProcessMemory(GetCurrentProcess()) into a fixed 1MiB scratch buffer, so a concurrently unloaded region fails the read safely without SEH runtime/CRT dependencies; a matching live object is revalidated by exact type, UUID, pointer page permissions, original text and native length before patch. The native preload 9-case mock and full label 25-case test pass; a fresh in-game load is still required.

## dev.27 – duplicated renderer diameter and premature text scan

Live DS2 PID20924 dev26 read-only/native inspection confirmed shelter native RepairSpray Resource Range+0x20=8.6m, Jolt cylinders radius+0x34=8m, half-height+0x30=6m, Odradek component +0x5C=16m and resource +0x34=16m, **renderer instance +0x2A0=32m**, which FUN_141D72260 interprets as 16m radius. This explains the photographed unexpectedly huge blue ring. A guarded reversible live patch to renderer+0x2A0=16m retained value after repeated reads and did not change the 8m/8.6m native features. dev27 production now allows correcting this exact doubled-native-diameter case, while refusing unknown renderer values. A pure radius acceptance policy tests all known and unknown cases.

The dev26 log printed PRELOAD_SCAN_COMPLETE before LIVE_SHELTER_SEEN; neither RestInShelter asset was patched and the HUD alternated between generic "Verschnaufen" and RainShelter "In Bunker ausruhen". A verified PID-locked live patch of the two loaded UUID text resources to "Verschnaufen" succeeded without changing action input. dev27 starts a bounded resource scan only after a real shelter is observed, then waits 2s before beginning. It retries at most once, ~6.5s after the first full pass. The streaming listener remains enabled for future resource groups. Automated native scanner mock requires the shelter-seen callback before reconciliation.

## dev.28: guarded native Timefall Shelter steady-state fast path

Starting from user-confirmed dev27, original values remain exactly ring8m, protection8m, native cargo repair8.6m, source renderer16m diameter and native rest text "Verschnaufen". The prior 133–158ms maximum callback timings are time spent in the complete native discovery callback, not proven FPS drops. Independent syscall microbenchmarks showed VirtualQuery ~2.3us and local file write ~0.003ms median, insufficient to explain these durations alone.

An exact validated shelter snapshot is recorded only after two different Jolt trigger bodies and native generation IDs, valid owner/repair source, and completed correct Odradek visual sync. The fast path verifies game vtables and pointer ownership, native Jolt BodyID generation, native body+shape identity and radii, Odradek component/resource/renderer and 16m size, RepairSpray identity+8.6m native radius. A change of any value or object causes immediate fallback to original complete sync on the next shelter callback. Repair registration is still reaffirmed every 10s. Unknown levels/layouts without full proof are never cached.

New performance counter columns stable, stableMaxUs, fullMaxUs split stable and full callback elapsed time for on-device verification. Expensive passive player-distance debug snapshots are behind optional SpatialDiagnostics=1 (default0), protecting normal use without losing the essential aggregated metrics. Added tests tests/shelter_steady_state_tests.cpp (29 checks) and build-run-shelter-steady-state.cmd into standard build-development.ps1. In-game performance test of the integrated dev28 is pending; no public release.

## dev.28 – in-game performance verification (2026-10-09)

User loaded the dev.28 ASI and reported all previously confirmed Timefall Shelter functionality still works, with no perceptible game stutters. Game process DS2 PID19984, startup 23:08:47 local, only active game ASI DS2_Sam_Overhaul_v1.1.0-dev.28.asi. Log shows native RepairSpray radius synchronized, active repair source registered, Odradek visual diameter synchronized, native gate patched, and spatial debugging disabled (zero SPATIAL_SNAPSHOT lines).

Five dev.28 log measurement windows: 38 complete discover callbacks across windows, 8 classified as stable fast path. Fast-path maximum wall times were 22.6-32.8 ms, full-discovery maximum wall times 102.2-182.2 ms, overall max full 182.2 ms. Therefore the shortcut IS active and has materially lower elapsed wall time than the full discovery on this system, but not every callback can use it. These measurements are wall times, NOT measured FPS drops. User reports no visible stutters and the previous game's behavior unchanged. No further risk-bearing optimization warranted without an actual user-visible performance issue. Confirmed functional + perceptual performance target for dev.28; no Git push/Nexus release yet.
