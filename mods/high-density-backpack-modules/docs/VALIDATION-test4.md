# Validation - v1.1.0-test4

Date: 2026-09-24. **Test4 is locally checked; the user game test is pending.** Test3's successful user observations and remaining limitations are preserved in [test3 validation](VALIDATION-test3.md).

## Scope

Only the equipment-menu preview is added to visual context resolution. The existing packing, mesh-visibility and bone-placement code is reused. No new patch windows or hooks; no changes to logical placement, module limits, accessory slots, effect selection or INI format.

The primary evidence is the supported executable's B413F0 owner lookup, RTTI 4355C00 (`DSAkukanPlayerEquipmentManageComponent`), EECBE0's initialization of its +2D8 weak backpack pointer, and EEF960's equipment copy/update at EEFE89..EEFF03. This identifies an additional live preview that can be resolved independently of the backpack manager's three stored pointers. Whether this addresses the user's final cargo-menu image remains to be verified in game.

## Local checks

- Windows x64 LLVM build passes with warnings as errors; AMD64 DLL/ASI, KERNEL32-only imports, unwind directory present.
- Runtime tests pass for all previous placement, visibility, ownership, eight equipment slots, packing, bone references, battery deduplication and adapter checks, including 771 original Cryptobiosis loop cases.
- Added an unregistered equipment-preview fixture: the existing filter packs/hides its modules and supplies packed full-footprint bone references, while logical records remain byte-identical. An unrelated actor remains untouched. Null subspace, null owner, absent component and removed preview pointers all fall back without modifying mesh visibility.
- Charm state-file roundtrip and invalid-data handling pass.
- The original native category-builder instructions pass for 256 equipment masks with eight rows and exact counts.
- Exact EXE hash matches; 87 byte/call guards and 63 non-overlapping patch windows pass. Instruction boundaries and 1,938 direct branches are checked; the old test1 back-edge defect remains detected by the regression.
- ZIP contains exactly one ASI; CRC and extracted bytes match the build.
- Test3 archive and ASI remain unchanged, and their bytes still match.

## User check still needed

Open, rotate, close and reopen cargo management; switch its equipment view; modify a module in customization; return to normal gameplay. Look for `EQUIPMENT_PREVIEW_VISIBILITY=ACTIVE` in the Test4 log. This message establishes that the new context was processed, not that later engine rendering leaves it unchanged. See [German instructions](TESTING-test4.md).

Actual charm effect measurements, persistence after restarting/changing saves, broader menu lifecycle and other-mod compatibility retain the limitations from Test3. If the cosmetic issue needs more extensive changes, the user's chosen fallback is the tested Test3 behavior for the update.

## Artifact

`DS2_HighDensityBackpackModules_v1.1.0-test4.asi`

SHA-256: `30E3854EDF07448871BF09010E11962A47683D50CDB2D74914BF4AC2A4E347E8`

Source and package are local. Test4 was installed on 2026-09-24 at the user's request; GitHub and Nexus publication remain pending.

## Retest setup, 2026-09-24 09:07 local time

The user reported unchanged cargo-preview overlap, but the reviewed game log (08:50) identified Test3, and both Test3 and Test4 ASIs were present. That report cannot establish Test4's in-game behavior. No newer crash dump was found than the old Test1 dump.

At the user's request, with DS2 not running, the Test4 ASI was copied and its SHA-256 verified against the artifact above. Test3 was moved outside the game directory into the workspace's dated installation backup; a recursive Backpack-ASI check found only Test4 remaining. The existing charm INI was preserved byte-for-byte. Existing ASIs, log and INI were backed up before installation. The user will perform a new game test; the pre-launch log still identifies the earlier Test3 run until the next startup.

Shared 1.1.0 README/Nexus text and a one-ASI release packager have been drafted. Final source selection, release build/metadata and publication await this retest. These drafts do not mean 1.1.0 has already been built or published.

## Confirmed Test4 retest

The next user run had Test4 active with `status=PATCH_APPLIED` and `EQUIPMENT_PREVIEW_VISIBILITY=ACTIVE`. The user reports clean cargo-menu rendering, but excess models visible during normal gameplay after loading an existing save. The original three-context filter depends on the backpack manager's cache. Native B413F0 fills that cache while entering customization; Test4 has no independent live-player lookup. This is a code-supported explanation of the load-path difference, not a runtime trace proving its cause.
