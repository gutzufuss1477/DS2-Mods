# Nexus upload sheet — v0.2.0

## File

- Display name: `Remote Orders Overlay v0.2.0`
- Filename: `DS2_Remote_Orders_Overlay_v0.2.0.zip`
- Category: Main Files
- Version: `0.2.0`
- Supported game build: Steam `DS2.exe v1.10.89.0`
- ZIP SHA-256: `CF348945B3F898A1145F61EB3800E5A3FF5610842833F1D89CE35538023C4BFF`

## Suggested short description

Shows available facility orders directly on the world map in a localized, click-through overlay.

## Version changelog

- Localized the overlay interface for all 18 Steam interface languages.
- Added an INI override when Windows and the game use different languages.
- Preserved native localized mission destinations and facility names.
- Fixed repeated French facility names and improved duplicate cleanup across languages.
- Added Arabic right-to-left alignment and safer truncation of long localized labels.
- Promoted the source build after successful German, French, Italian and Spanish in-game smoke tests.

## Known limitation

A small number of Italian or Spanish facility labels may retain an additional grammatical article before the text is shortened to fit the column. The destination remains identifiable.

Use `docs/NEXUS_DESCRIPTION.md` as the full description.
