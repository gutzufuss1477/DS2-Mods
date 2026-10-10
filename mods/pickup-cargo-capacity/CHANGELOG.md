# Changelog

## 1.0.2 (2026-10-10)

- Pickup rear cargo indicator now fills proportionally to expanded cargo capacity instead of saturating at original 160 units.
- Tested with CapacityUnits=320: at 160 units five of ten segments are white; at full 320 units all ten are red.
- Optional RearIndicatorScaling=1 INI setting, disabled by setting 0.
- Two separately byte-validated display detours, with cargo inventory logic and four capacity patches preserved.
- Cargo menu percentage still uses vanilla 160-unit baseline (200% at capacity 320).


## 1.0.1

- Added explicit Windows product and version metadata.
- Enabled ASLR, DEP and high-entropy ASLR in all release builds; the conventional MSVC build additionally enables Control Flow Guard.
- Added a conventional MSVC build using the static runtime and normal DLL startup.
- Removed hand-written `memcpy` and `_fltused` runtime symbols.
- Made the LLVM fallback build self-contained with a minimal import library.
- Kept the validated four-site in-process patch behaviour unchanged.
- Validated in game at the maximum 480-unit setting; the pickup accepted the expected 300% of the original 160-unit capacity.
- Verified the final ASI and release ZIP with current Microsoft Defender security intelligence without a detection.

## 1.0.0

- Initial release.
