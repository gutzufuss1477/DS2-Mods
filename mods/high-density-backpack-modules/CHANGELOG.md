# Changelog

## v1.1.0 Classic Overlap (optional) - 2026-09-25

- Adds a separate ASI with the 1.1.0 eight-charm system and 30 one-cell module slots.
- Leaves native model placement and visibility active so modules on different cells can overlap.
- Uses manual placement: occupied cells are not automatically redirected to another free cell.
- Shares the same charm INI and loader mutex with standard 1.1.0. Install one variant at a time.
- Built and checked locally; this optional variant has not yet been confirmed in game.

## v1.1.0 - 2026-09-24

- Uniform one-cell logical placement supports up to 30 modules; occupied anchors automatically move to free cells.
- Separate visual packing preserves native model size and hides excess models while retaining installed equipment records.
- Applies the visual layout in customization, cargo management and normal gameplay, including the live player after loading an existing save.
- Eight selectable charm slots: two visible and six invisible; effect checks include equipped, currently unlocked charms.
- Extra charm selections persist in an automatically created installation-wide INI; physical charms retain native save handling.
- Retains the Test1 crash correction and local native-instruction regression tests.
- Release based on the user's successful Test5 check. Actual effect magnitudes and every save/menu route have not all been measured.
- One-ASI download. Replace older Backpack ASIs instead of keeping multiple versions active.

## v1.0.0

- First stable release.
- Reduces functional backpack components to one logical anchor cell.
- Automatically relocates occupied or edge-invalid placements to the nearest free, full-shape-valid anchor.
- Retains native visible component size and native outer-boundary checks.
- Preserves native IDs, levels, effects, object-list entries and save/load handling.
- Validated with 16 mixed installed components, save/load persistence and stacked battery capacity.
- Fails closed on unsupported executable or patch-site mismatches.
