# Changelog

## v1.0.0

- Initial public release.
- Raises Floating Carrier cargo footprint from 36 to 240 units.
- Raises both native carrier packer sides from 18 to 120 units.
- Supports mixed S/M/L/XL cargo using the native footprint sizes S=1, M=2, L=4, XL=6.
- Raises Level 1 weight limits from 300 / 150 / 300 kg to 5000 / 2500 / 5000 kg.
- Raises Level 2 weight limits from 900 / 450 / 900 kg to 5000 / 2500 / 5000 kg.
- Uses the live global `DSBaggageConfig` plus constructor fallbacks for both carrier levels.
- Keeps the game's native visible vertical stacking.
- Removes all development diagnostics, STATUS output, F-key controls, PAGE_GUARD, breakpoints and single-step tracing.
