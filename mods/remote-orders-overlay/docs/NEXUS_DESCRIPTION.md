# Remote Orders Overlay v0.2.0 (Interim)

Can't be bothered to travel across half of Australia just to discover who has the last orders needed to bring a facility to five stars?

This mod adds a compact Remote Orders Overlay to the world map. Hover over or select a facility to see where matching available orders can be collected and where they are going.

This is an interim overlay release. The long-term goal remains opening the game's native **Accept Orders** menu directly from the world map.

## Main features

- Displays up to ten available orders in two compact columns.
- Uses localized mission data from the game.
- Localizes the overlay interface in all 18 Steam interface languages.
- Includes an INI language override for systems where Windows and the game use different languages.
- Cleans up repeated localized facility-name variants produced by the native MissionMenu formatter.
- Shows the selected facility number.
- Click-through overlay that does not block the map.
- Read-only: no automatic order acceptance or mission-state changes.
- No persistent diagnostic logs in the release build.
- Compatible by design with Hill Assist and Truck/Vehicle mods.

## Installation

1. Close the game.
2. Install one working x64 ASI loader.
3. Remove older Remote Orders Overlay ASIs.
4. Copy `DS2_Remote_Orders_Overlay.asi` and `DS2_Remote_Orders_Overlay.ini` beside `DS2.exe`.
5. Start the game and open the world map.

`Language=auto` follows the Windows display language. If the game uses another language, edit the INI and set a code such as `fr`, `it`, `es-ES` or `es-419`.

## Requirements

- DEATH STRANDING 2: ON THE BEACH for Windows
- Steam game build v1.10.89.0
- One working x64 ASI loader

## Known limitation

This version displays an external overlay and does not yet open the native Accept Orders menu from the world map. A small number of Romance-language facility names may still contain an additional grammatical article before the label is shortened to fit the column.
