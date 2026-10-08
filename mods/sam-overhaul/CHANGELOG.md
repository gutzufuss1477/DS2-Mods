# Changelog
## 1.1.0-dev.19 — native shelter circle diameter to radius alignment (2026-10-09)

- Confirmed via static native RVA 0x1D72260: DSOdradekEffectInstance+0x2A0 stores visual size and DS2 computes the actual radius as 0.5 * size. In the 200% game version actual protected/repair radius is 8m but Odradek visual size8 gives only 4m. An isolated temporary 16m test informed the fix; on 2026-10-09 the tester confirmed that the integrated dev.19 larger circle and repair effect now match in-game.
- Only the current owner-checked active Timefall Shelter Odradek receives visual diameter = twice actual target repair/protection radius; vtable-guard component +0x5C, resource +0x34 and native render instance +0x2A0 with original values and MEM_PRIVATE writable validation. No global particle effects, no synthetic repair hooks, no renderer code changes.
- The actual reflected OdradekEffectEnableRadius +0x30 is 30m vanilla and intentionally unchanged, as is OverrideSize +0x38 (require true). Delete the dev18 wrong attempt to double activation range.
- Existing dev18 active-only fast path and diagnostic timings preserved. Native repair radius +0x20 8m and protection +0x760 8m remain intact. In-game visual match confirmed; save-reload and broad regression remain pending. Discovery profiler showed expensive passes, with max 192371us in the observed session, requiring follow-up before public release.
- Development-branch checkpoint only; no Nexus/GitHub release or release tag.


## 1.1.0-dev.18 — Odradek circle gate and shelter frame-time profiling (2026-10-08)

- DS2 reflection proves separate OdradekEffectEnableRadius at resource +0x30 and OdradekEffectSize at +0x34. Preserve existing size scaling; additionally synchronize matching component+0x58 / resource+0x30 to 200% only with vtable, owner and known 4m/current-target checks. Unknown values are skipped, without changing OverrideSize.
- Read-only safety log tells us if resource OverrideSize+0x38 was OFF so that a carefully scoped follow-up live test can confirm its effect on ground circle, without unsafe global render patches.
- Add timing of active shelter discovery through QueryPerformanceCounter. Aggregates call count, worst microseconds, calls exceeding 2ms and 8ms, and expensive fallback scans. Writes one diagnostic line per 15s, not one per frame. No native gameplay repair/cargo changes.
- Existing successful native 200% rain protection and cargo repair preserved. Circle alignment and absence of FPS stutter require in-game confirmation; do not claim completed.
- No Git commit/push or Nexus release.


## 1.1.0-dev.17 — verified native repair radius (2026-10-08)

- Found native DSConstructionRepairSprayComponentResource radius at +0x20, consumed by the original DSBaggageComponent repair coverage routine (RVA 0x11B04F0). Live DS2 change 4.0 -> 8.0m produced visible repair cloud and fully restored cargo at a verified 4.98m (outside vanilla).
- Integrate safe radius synchronization for active shelters with exact owner/member/component/resource vtable checks. Idempotent on shared resources, fresh load and save reload; only +0x20 changed. No direct cargo writes, no synthetic native repair events and no effect strength changes.
- Preserve dev.16 active-only throttled shelter fastpath, protected config radius, two Jolt cylinders, Odradek visuals, and all generator and Sam Overhaul changes. Additional cylinder-first visual enumeration optimisation.
- In-game installed ASI testing, reload, circle and frame-time verification pending. No Git commit/push/Nexus release.


## 1.1.0-dev.16 — stable shelter hot-path performance (2026-10-08)

- Native dev.15 read-only RestoreCoating tracer collected 209 original calls, 17 positive returns around close range, no native calls in 5-7m outer protection radius. Removed all repair diagnostic native coating hooks and raw cargo writes (none existed). Open repair extension remains unresolved.
- Based on confirmed dev.12 timefall shelter module. Performance optimization: trust the original DS2 method-call this pointer and check its vtable / built stage directly before entering expensive native memory probes; query timers before vtable inspections; respect 700ms per-object throttle before memory validation.
- Prioritize Jolt cylinder component slots +0xF0/+0xF8 as verified, with full owner component fallback only once per 10 seconds for unrecognized layouts. All writes remain behind exact Jolt body validation, source vtable checks and the optional shelter 200% switch.
- Generator 200% physics and matching persistent blue charging circle untouched, timefall protection native config +0x760 untouched, footprint suppressor and legacy Sam mods untouched.
- In-game frame-time improvement and protection recheck required; not yet declared resolved. No Git push, no Nexus release.


## 1.1.0-dev.15 — original coating restoration call probe and CPU optimisation (2026-10-08)

- Native RTTI closure from DSBaggageComponent::RestoreCoating identifies RVA 0x11B4460; read-only MinHook pass-through counts original calls and positive restoration return values per user/shelter distance band. Strict original-prologue signature, optional existing TraceRepairContacts=1, no repair effect modifications.
- Native repair contact log at 4.6 m did not match visible spray at 1-2 m. Correct the previously mislabelled RepairSpray component +0x71 field as one-shot notification/like state, not active visual spray.
- Restore original 200% rain shelter protected radius and generator fixes. Optimise the shelter physics trigger discovery using validated owner component offsets +0xF0/+0xF8 first, bounded fallback only for other construction levels. This reduces repeated costly VirtualQuery calls from approximately 160 to two in the known layout.
- Existing diagnostic is event-limited, 5-second heartbeat at <=15 m, read-only; report engine restoration counts and caller categories, without any native repair contact or cargo field writes.
- No Git commit, no Git push, no Nexus release. Repair extension remains open until positive native restoration is proven at distance and visuals match.


## 1.1.0-dev.14 — Read-only timefall-shelter repair tracing (2026-10-08)

- Revert failed dev.13 synthetic native repair contact dispatch entirely, returning to proven dev.12 shelter implementation.
- Optional default-OFF TraceRepairContacts INI: event-only logging of real player distance bands (DS2 X/Y plane, vertical Z), native RepairSpray contact, matching player reference, spray-emitted flag and native shelter fact.
- Rate-limited to active loaded shelter ticks (at most once per 700ms/instance), log transitions/12-second heartbeat, cap 180 entries. Absolutely no repair gameplay writes, extra collision bodies, native contact invocations or visual patch guesses.
- Existing 200% generator ring/charging and timefall shelter protection untouched. Real cargo restoration radius and repair fog remain UNKNOWN; next in-game pass will collect the precise native transition for genuine repair.
- NO Git commit/push, NO Nexus release.


## 1.1.0-dev.13 — experimental native RepairSpray contact extension (2026-10-08)

- Identified DSConstructionRepairSprayComponent original native enter/exit callbacks at RVA 0x1357270/0x1357420, with original owner/player state and native +0x70 contact status.
- Add default-off experimental INI option TimefallShelterRange/ExtendRepairContacts=0. Local opt-in =1 to invoke original contact callbacks at doubled validated shelter radius, preserving original cargo-health and message logic.
- Validate shelter/owner/repair resource, primary DSPlayerEntity identity, both native function prologues, vertical/horizontal distance; rate-limit native messages (minimum 1.9 seconds) and trigger native exit when leaving; INI hot opt-out every ~1.6 seconds.
- The prior unrelated dev.9 sphere modification remains removed. Generator, footprint suppression and native rain protection unchanged.
- Real extended cargo REPAIR effect and visually matching repair clouds are NOT tested/confirmed yet. Build only; no Git push or Nexus release.


## 1.1.0-dev.12 — native protection and removal of incorrect repair patch (2026-10-08)

- Preserve the confirmed 200% generator physics + blue ring fix.
- Keep the directly verified DS Rain Shelter ConstructionConfig Range +0x760 native gameplay change, which prevented cargo durability loss at the expanded 8 m distance in the user live test. Guard per-shelter category resource and throttle reconciliation before the vanilla shelter tick; correct native radius 4->8m only when the optional INI setting is enabled.
- Confirmed by spatial inspection that the previous dev.9/dev.10 RepairSpray child sphere was a FALSE POSITIVE: referenced entity ~169–269 m away from tested shelters, not their spray emitter. REMOVE all incorrect sphere modifications, jobs, caches and expensive scans. Clean baseline from before dev.9.
- Retain only two validated rain shelter Jolt protective cylinders and verified Odradek component/resource visual-range fields. Scan Odradek only when a valid shelter protection cylinder is present to limit CPU cost.
- Native DS2 funcs at RVA 0x1306A40 and RVA 0x1306B90 confirm real game radius consumption (distance squared and search filter).
- **Open:** Repair cloud size, true cargo restoration radius, actual outline freshness after reloading. Bunker rest vs crouching are different game actions; their vanilla priority was not modified.
- Fresh-game performance, protection, and post-reload visual checks pending. No Git commit/push, no Nexus release.


## 1.1.0-dev.11 — native rain shelter gameplay range (2026-10-08)

- Reverse-engineering discovery: DS Rain Shelter ConstructionConfig reflected property Range at +0x760 is the native gameplay protection radius, vanilla 4 m; changed to 8 m in a guarded live DS2 test. The user confirmed cargo stopped losing durability outside the old visible ring and a shelter rest action became available.
- Add guarded Range synchronization to the existing TimefallShelterRange INI multiplier via native per-shelter category/resource lookup, before the vanilla shelter update, with 750ms throttling across all native shelter callbacks, strict vtable and writable/private-memory validation and resynchronization after save reload.
- Do not alter the vanilla choice between rest-in-shelter and crouch/Verschnaufen, nor the rain/weather/likes logic. Generator implementation untouched.
- Existing 200% protective cylinders, repair sphere and Odradek effect from dev.10 retained. Actual cargo REPAIR RANGE and visual repair fog/circle remain open, not claimed as fixed.
- Combined ASI needs gameplay validation after a fresh launch and save reload. NO Git commit/push, NO Nexus release.


## 1.1.0-dev.10 — bounded RepairSpray discovery / lag regression (2026-10-08)

- Reverted the dev.9 in-game performance regression to confirmed dev.7 on the local game installation before this build.
- Restrict extra RepairSpray child scans to physically active shelters with validated native cylinder triggers; avoid scanning all streamed/dormant shelters in the game update hook.
- Only inspect the nine fixed-stride RepairSpray child-entity slots, with guarded per-object pointer validation, and cache the correctly expanded native sphere so repeated ticks do not rescan it.
- On save reload or reused pointers, validate the cached sphere type and current radius; if it returns to vanilla, allow native body re-discovery and requeue.
- Failed child searches are rate-limited to once every two seconds per instance. Log the first active scan/cache hit and inactive skip for diagnosis.
- The 4-to-8-m child sphere change remains **experimental**. Actual timefall protection/repair and performance require new in-game verification. Generator 200% fix unchanged.
- No Git commit/push; no Nexus release.


## 1.1.0-dev.9 — repair sphere proof / integration test (2026-10-08)

- Rain shelter protected/visual ring: 200% Jolt cylinder and Odradek live tests confirmed the visual boundary, but the user confirmed continued cargo degradation and original repair effects in the expanded area.
- A distinct Jolt SphereShape of vanilla radius 4 m exists in a DSConstructionRepairSprayComponent child entity. Its effect on gameplay is NOT YET CONFIRMED.
- Add guarded scanning of child RepairSpray sphere triggers, validated shelter/repair/child ownership, component/shape vtables, native BodyID, opt-in 4-to-8-m scaling at 200%, and per-body NotifyShapeChanged.
- Do not change global repair duration, cargo health, timefall damage settings, or the confirmed generator module.
- Fresh DS2 launch and save-reload regression of the combined ASI still REQUIRED, followed by shelter cargo protection and repair tests.
- No Git commit/push; no Nexus release.


## 1.1.0-dev.8 — timefall shelter visual marker test (2026-10-08)

- Confirmed live shelter has two active RotatedTranslatedShape(CylinderShape) physics triggers at radius 8 m after the existing 200% Jolt refresh.
- Discovered shelter DSConstructionOdradekEffectComponent and its linked resource still carried vanilla radius 4 m each; live set 4->8 m on both.
- Integrate guarded shelter Odradek instance (+0x5C) and resource (+0x34) radius synchronisation only when two validated shelter Jolt cylinders are at the target radius; include save-reload reconciliation.
- Generator v1.1.0-dev.7 was verified by user: blue circle matches charging boundary and survives save reload.
- Shelter's actual timefall/cargo/BT protection at the new boundary and combined dev.8 ASI still require gameplay verification.
- No Git commit/push and no Nexus release.


## 1.1.0-dev.7 — Odradek visual radius, local testing (2026-10-08)

- In-game live test confirmed the blue generator circle matches charging range after save reload when BOTH DSConstructionOdradekEffectComponent instance +0x5C and its resource +0x34 are scaled (9->18 m, 12->24 m).
- Remove the disproven LineEffectComponent +0x1DC write, which is not a stable radius field after reload.
- Add guarded Odradek instance/resource synchronization with per-generator owner and component vtable checks, Jolt sphere verification, no double scaling, and safe shared-resource handling.
- Keep deferred generator candidates until the actual Jolt body and both visual fields have been reconciled, or until timeout.
- Shelter radius support remains in the local experiment; actual protection outside the vanilla shelter is not yet gameplay-confirmed.
- The dev.7 combined ASI itself must be tested following a fresh DS2 launch and save reload. No Git commit/push and no Nexus release.


## 1.1.0-dev.6 — local visual and shelter test (2026-10-08)

- Generator: sync per-instance DSConstructionLineEffectComponent blue-circle radius with verified Jolt charging radius on load/reload (9 to 18 m, 12 to 24 m at 200%).
- Guard all LineEffect modifications by generator ownership, trigger and physics resource types; unrelated line effects remain unmodified.
- Shelter: hook DSRainShelter update and discover both RotatedTranslatedShape/CylinderShape protective triggers. Scale cylinder radius 4 to 8 m at 200% and refresh each live body through the verified Jolt function.
- TimefallShelterRange is still default OFF. The staged gameplay-test INI can enable it separately from GeneratorRange.
- Actual shelter protection, circle persistence and combined ASI stability are not yet validated in-game. No Git push or release.


## 1.1.0-dev.5 — generator update fallback (2026-10-08)

- Add guarded, per-instance DSCharger runtime update/scan hooks to find generators missed during their initial trigger setup.
- Scale only the verified generator Jolt SphereShape (level 1: 9 to 18 metres at 200 percent); queue an engine NotifyShapeChanged for that exact body.
- De-duplicate refresh requests by validated world, Jolt BodyID, body pointer and shape pointer to avoid notifying every frame.
- Exact DS2 v1.10.89.0 signature checks, 700-ms per-instance throttle, existing gameplay changes preserved.
- The blue visual ring is still vanilla; its rendering radius remains under investigation.
- Local in-game testing required, no Git commit/push and no Nexus release.



## 1.1.0-dev.4 – local development / in-game test pending (2026-10-08)

- Generator Stufe 1 live proof: scaling the 9 m Jolt sphere to 18 m plus NotifyShapeChanged works outside the original blue ring from several directions.
- Dev.3 itself did not enlarge charging for the tested generator; its Jolt refresh reached two other bodies.
- Added deferred candidate processing for generator triggers that initialize before owner+0xA8 has been populated, including an owner/body validation retry window.
- Deferred queueing is only accepted after active Jolt BodyID and shape ownership match.
- Existing generator triggers with correctly sized Jolt shapes also receive a one-time refresh via the same path.
- The visual blue circle is not yet enlarged. The runtime success of this combined dev.4 build is not yet confirmed.
- Timefall shelter range remains experimental, disabled for the isolated generator test.
- No Git commit/push and no Nexus release.


## 1.1.0-dev.3 – local test only (2026-10-08)

- Added guarded deferred Jolt BodyInterface::NotifyShapeChanged for verified generator physics trigger bodies.
- The previous 200% sphere update and config changes alone did not enlarge the working charging area.
- Range refresh uses matching generator owner, valid BodyID and same registered sphere shape.
- Added diagnostic entries: QUEUE_DETECTED, NOTIFY_INVOKED, or SKIPPED.
- This is NOT an established fix. In-game verification of the real charging boundary is required.
- No Git commit/push, Nexus release, or public package.


## 1.1.0-dev.1 — unreleased development build

- Integrate the in-game-confirmed footprint filter into the existing Sam Overhaul ASI.
- Add optional `[Footprints] HideFootprints=0/1` to `ds2_sam_overhaul.ini`; default and missing-key fallback are 0.
- Hide normal and scan-highlighted footprints, including their reconstruction when loading saved worlds.
- Preserve the tested resource matching and complete native argument forwarding; unrelated decals pass through.
- Add executable SHA-256/prologue guards, standalone-probe conflict detection and limited diagnostics in the existing Sam Overhaul log.
- Preserve all 33 previous INI defaults and the existing cargo, movement, Autodrive and truck-weapon code paths.
- Add footprint configuration/ABI/concurrency, combined-ASI loading and binary-integrity tests; retain the Autodrive regression suite.
- Git development build only: no Nexus release, release tag, Mod Suite package update or change to the v1.0.0 release archive.
- Combined in-game regression remains pending; the footprint core was gameplay-validated separately.

## 1.0.0 - 2026-09-30

First public release of Sam Overhaul.

- Added configurable hiding for shoulder cargo, hip cargo, backpack cargo and spare shoes.
- Keeps currently worn shoes visible and leaves inventory, weight and cargo ownership native.
- Added monorail dismount beyond the final vanilla safety/height blocker while preserving earlier state checks.
- Added wider zipline dismount availability using the native input and exit animation.
- Added qualifying landing rolls with backpack/cargo while preserving native landing thresholds and fall-damage logic.
- Added configurable Autodrive activation time with a tested 2.0-second default.
- Added independent Heavy Machine Gun tuning: range, aim speed, lock-on, burst duration and cooldown.
- Added independent Mortar tuning: range, aim speed, lock-on, recovery and cooldown.
- Added independent Chiral Cannon tuning: range, long-range profile, aim speed, lock-on, recovery, cooldown and charge time.
- Chiral Cannon default charge reduced from 3.0 seconds to 0.75 seconds after live A/B testing.
- Added independent Missile Launcher tuning: range, aim speed, lock-on, recovery and cooldown.
- Removed unsuccessful footprint/worn-path experiments from the public release.
- Added strict target-build/context validation and local startup/configuration logging.
- Final build passes the existing 105,704-check Autodrive regression suite.
