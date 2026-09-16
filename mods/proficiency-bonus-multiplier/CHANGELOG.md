# Changelog

## v1.0.0 - 2026-09-16

- Initial public release of Proficiency Bonus Multiplier.
- Added per-category `Level1Multiplier` through `Level5Multiplier` configuration.
- Added all 12 visible Sam's Stats proficiency categories.
- Added confirmed Melee Combat Attack Power scaling.
- Added confirmed Melee Combat Consecutive Attacks scaling with integer rounding.
- Added verified firearm bonus scaling for Handguns, Assault Rifles, Sniper Rifles, Machine Guns and Shotguns.
- Added safe factor scaling around neutral value `1.0`.
- Added additive scaling around neutral value `0.0` where applicable.
- Added reduction-factor clamp at zero, enabled by default.
- Added strict supported-build and vanilla descriptor/table verification before any write.
- Added clean launcher/helper-process handling to avoid bootstrap log spam.
- Shipped with neutral `1.0x` defaults for all public categories.
