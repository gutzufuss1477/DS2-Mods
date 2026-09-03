# Changelog

## 1.0.0

- Promoted the mod from the test release to the first stable release after successful in-game validation.
- Confirmed that Level 2 provides a long cargo pickup range, switching to Level 1 reduces the range, and removing the gloves restores normal close-range pickup.
- Retained separate, configurable Level 1 and Level 2 ranges with defaults of `30 m` and `50 m`.
- Retained the exact, isolated patch of `Params[6]` for Item IDs 53 and 54; global pickup range, other gloves, Sticky Gun, Sticky Cannon, and `Params[7]` remain unchanged.
- Standardized the primary release documentation and INI guidance in English and added separate German documentation.
- Save/load and area-transition persistence remain recommended regression checks.

## 0.1.0-test

- Published the first test build for Steam PC `DS2.exe 1.10.89.0`.
- Added separate, configurable pickup ranges for Climbing Power Gloves Level 1 and Level 2.
- Verified native resource values of `8 m` for Level 1 (Item ID 53) and `10 m` for Level 2 (Item ID 54).
- Set defaults to `30 m` and `50 m`, respectively, with a configurable maximum of `100 m`.
- Limited the patch to `Params[6]` of the two exact `DSItemParameter` resources. The global hand-pickup range, other gloves, and `Params[7]` remain unchanged.
- Added the hook-free resolver through the canonical global `DSItemSystem` pointer and its validated item array.
- Added exact build, initialization, lookup, RTTI, VTable, and type-name validation with a safe abort on mismatches. Factory metadata is used only for static type and layout validation and is not hooked.
- Added the offline validator and reproducible in-game test plan.
- Marked practical in-game confirmation of extended cargo pickup as pending.
