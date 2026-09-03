# Nexus upload sheet — v1.0.0

## Mod page

- Title: `Climbing Power Gloves - Extended Cargo Pickup Range`
- Category: `Miscellaneous`
- Suggested tags: `Gameplay`, `Quality of Life`, `Equipment`, `Unbalanced`
- Version: `1.0.0`
- Supported game build: Steam `DS2.exe 1.10.89.0`

## Main file

- Display name: `Climbing Power Gloves Extended Range v1.0.0`
- Filename: `DS2_Climbing_Power_Gloves_Range_v1.0.0.zip`
- File category: `Main Files`
- Version: `1.0.0`
- Archive SHA-256: `0DB190CF37A4B12E70CC6779DDFC80B8572863B2DCBD8AE84B9FC2BA2DCE321D`

## Short description

Extends Climbing Power Gloves cargo pickup to separate configurable 30 m and
50 m ranges while leaving normal hand pickup unchanged.

## Version changelog

- Adds separate configurable cargo-pickup ranges for Level 1 and Level 2.
- Includes tested defaults of `30 m` and `50 m` instead of native `8/10 m`.
- Preserves normal close-range pickup when the gloves are not equipped.
- Validates the exact Steam target and both glove resources before writing.
- Includes conflict detection, transactional rollback, and reload reconciliation.

## Requirements text

- DEATH STRANDING 2: ON THE BEACH on Steam
- Supported `DS2.exe 1.10.89.0`
- External 64-bit ASI loader

Use `NEXUS_DESCRIPTION.md` as the full description and
`NEXUS_SHORT_DESCRIPTION.txt` for the short-description field. Upload the
tracked v1.0.0 ZIP from the `release` folder as the only main file.
