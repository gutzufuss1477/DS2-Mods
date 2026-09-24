# Validation - v1.1.0-test5

Date: 2026-09-24. Test5 passed local checks and was installed at 09:37 local time. Its in-game result is pending.

## Reason for the change

The confirmed Test4 run produced a clean cargo-management preview, but the user reported visible excess modules in gameplay after loading an existing save. The log identifies Test4 and reports its preview path active. [Test4 validation](VALIDATION-test4.md) preserves the preceding results.

The original filter recognizes three pointers cached by the backpack customization manager. Native B413F0 populates those pointers through the active player and equipment components. Test5 adds the same live-player resolution without initializing or mutating that cache, covering a loaded player's backpack before customization has registered it. This is a code-supported explanation and targeted correction; the user's exact rendering sequence has not been traced in a live debugger.

The Test4 live equipment-preview lookup remains. Each added context has its own visual-layout cache. Neither addition changes inventory entries, save coordinates, charm selection or effect code. There are no new hooks or patch windows.

## Verification

- Runtime tests pass, including placement, packed visibility, native bone references, equipment-menu lifecycle, eight charm rows, effect-selection fixtures, state-file roundtrip and 771 original Cryptobiosis loop cases.
- Added a fresh-load fixture with no backpack manager: distinct player and menu copies alternate rendering with different visible subsets. Both resolve their own layout; player logical records remain byte-identical. The fixture checks full-footprint bone positioning, missing player/components/owners, null list entries, invalid list counts and exclusion of non-player entries.
- Original DS2 category-builder instructions pass for 256 charm masks and eight selectable rows.
- Exact supported EXE hash, 92 byte/call guards, 63 non-overlapping windows and 1,938 direct branches pass. The original Test1 crash regression remains covered.
- Windows x64 LLVM build passes with warnings as errors. The ZIP has exactly one ASI and matching CRC/extracted bytes.
- With DS2 closed, the installed ASI hash was verified; a recursive game-folder check found only Test5. The existing charm INI remained byte-identical. Previous ASIs, log and INI were backed up outside the game folder.

## Artifact and pending work

`DS2_HighDensityBackpackModules_v1.1.0-test5.asi`

SHA-256: `7EDD9276DC4E5B44CEE07C7D32DBCA1EC55CFEE161EB21DEADA6FE95B38ECFC9`

The user will test direct gameplay after loading an existing save, then menu/gameplay transitions. See [test instructions](TESTING-test5.md). Real charm-effect measurements and broader save/menu compatibility retain earlier limitations.

Release 1.1.0 documentation and packaging are drafted, not finalized or published. If this narrow correction fails, the requested fallback is Test3 behavior with the cosmetic cargo-menu limitation.
