# Crafting & Equipment Overhaul 1.6.1 — validation

Version 1.6.1 adds the optional `GoldSkeletonSkin` setting under `[AtlasEquipment]`. The public default is `0`, which preserves the normal Boost Skeleton Lv.3 appearance. `1` selects the native gold Boost Skeleton Lv.3 visual while retaining ATLAS item ID 104 and all Battle/Boost/Bokka gameplay behavior.

Old INIs remain compatible: if `GoldSkeletonSkin` is absent, the parser defaults to the normal skin. The skin selection is applied to both the fabrication preview and the equipped ATLAS Skeleton.

The full Windows test suite passes: 335437 host assertions, 9297 configuration assertions, 13603 backpack assertions, 98560 native FreeCrafting cases with 296180 assertions and 88 unwind checks, 26900 transactional assertions, 35 targeted Usage=None UI cases, 1001906 durability assertions and 198 ATLAS integration/config assertions.

The dedicated ATLAS native suite passes all 20 tests with 110208 branch cases. Both visual selectors are tested: normal Boost Lv.3 ID 26 and gold Boost Lv.3 ID 35.

Structural validation confirms 91 exact native hook signatures with no overlaps. The release ASI is AMD64, uses ASLR/NX, has unwind and relocation data, contains no writable executable section and matches SHA-256 `8c46ed002732c6c8d3c7bd68b17a3d780201a7248ce8474bb59fd2aa0735c2df`.

The gold skin was confirmed by the user on a non-Deluxe installation in the fabrication preview and on the equipped ATLAS Skeleton. Save/load persistence was also confirmed.

See `validation/summary-1.6.1.json`, `validation/integration-1.6.1.json` and `docs/BUILD_VALIDATION.json` for machine-readable details.
