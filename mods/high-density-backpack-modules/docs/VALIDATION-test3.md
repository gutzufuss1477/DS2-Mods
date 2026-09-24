# Validation — v1.1.0-test3

Date: 2026-09-24. **The user has tested test3 in game.** Accessory selection and module visibility in backpack customization and normal gameplay were reported working. The user previously confirmed test2 functioning through 30 installed modules.

## User test and log review, 2026-09-24

- The accessory menu shows seven equipped charms and one empty row. The user could equip all currently available charms; two models are visible.
- The mod log identifies test3, reports `status=PATCH_APPLIED`, initializes eight accessory rows and reports `UNLOCKED_CHARM_MASK=0x0000007F`. IDs 1 through 7 are available; ID 8 is not currently available through the game's enumeration. The separate `available_items=72` count is not a charm count.
- The extra-slot INI contains `InvisibleSlots=045673`: five selected extra charms and one empty extra slot. Together with the two visible charms in the screenshot, this agrees with the seven equipped rows. Writing this preference does not establish successful restoration after restarting or changing saves.
- The user reports acceptable packing and hiding of excess module models in customization and normal gameplay. Different equipment combinations produce different visible subsets, as intended by the packing policy.
- **Known cosmetic issue:** the cargo-management character preview shows overlapping module models again. Its exact cause is not established. The visibility hook filters to the three backpack contexts registered by the local manager; an additional preview context or a later visibility reset needs investigation before changing that filter.
- No failure entry was found in the mod log. The game log reviewed through 00:12:19 contains an unsupported Anti-Lag2 message and four input-device setup failures, but no reported fatal exception. The newest crash dump remains the test1-era dump from 2026-09-23 23:20:18; no newer dump was found. This is not proof of complete stability.
- Actual charm effect magnitudes and triggers were not measured by the user and are not recorded by these logs. Local tests and the equipped selection support the implementation, but do not establish all effects working in real gameplay.

## Local checks

- Windows x64 LLVM build, warnings as errors, no CRT, unwind metadata.
- Placement and footprint tests: 30 cells, all four rotations, dimensions 1..6, occupied/invalid/full-grid conditions.
- Separate visual packing: every selected footprint is in bounds and disjoint; original logical records stay unchanged. Mocked native repositioning is cached and repeated when the layout changes. Full-footprint bone references, reference counts, preview placement and oversize-rotation fallback are checked.
- Eight-row menu integration: selection indices, duplicate exclusion, replacement, removal, preview restoration and virtual effects only from equipped slots.
- The original DS2 category-builder instructions execute in a private test allocation for all 256 eight-slot equipment combinations. Tests verify eight item/empty rows, native row types, accessory IDs and the exact header count. Only the row allocator is substituted. This is not a full UI test.
- The menu inline adapter and the cancellation tail-jump execute through installed hooks. The existing register-preservation fixtures and rollback tests remain.
- INI roundtrip using the Windows profile APIs, plus duplicate/invalid/empty stored-slot decoding.
- Existing ownership enumeration and battery deduplication mocks; all 256 charm masks.
- 771 native Cryptobiosis loop cases retain the test1 crash regression.
- Exact EXE hash, 84 byte/call guards and 63 non-overlapping patch windows. Capstone checks instruction boundaries and 1,938 direct branches in affected runtime-function ranges, including rejection of the old test1 hook.
- ZIP verification: exactly one ASI, matching extracted bytes and CRC.

## Still requiring the user's game test

Complete menu navigation and commit/cancel behavior across all entry/exit routes, broader animation/geometry and refresh/culling coverage, real effect measurements, saving/reloading and compatibility with other mods. Cargo-preview rendering remains a known issue. Additional-slot persistence is an installation-wide INI preference, not embedded in individual save files.

See [the test guide](TESTING-test3.md). Older results are recorded separately in [test2 validation](VALIDATION-test2.md).

## Artifact

`DS2_HighDensityBackpackModules_v1.1.0-test3.asi`

SHA-256: `BBC0177FA3EA43D368F8695141917914767F8AF37FFA4EC42E4B61ACCA71053E`

Source and package are local. No game installation or GitHub publication was performed.
