# Enemy-drop weapon candidates for Crafting Overhaul v1.5

Scope: only weapons Sam can already pick up/equip/use normally.
Excluded: cosmetic/red/ghost duplicates of otherwise identical player weapons.

## Confirmed v1.5 candidates

| Recipe ID | Catalogue name | Baggage ID | Status |
|---|---|---|---|
| 0x6C3A478B | Assault Rifle [RB] Lv.1 | 0x4BC8A3D8 | include |
| 0x1EA3AF0B | Assault Rifle [RB] Lv.2 | 0x39514B58 | include |
| 0x6CC82C08 | Assault Rifle [RB] Lv.3 | 0x4B3AC85B | include |
| 0x1E51C488 | Shotgun [RP] Lv.1 | 0x39A320DB | include |
| 0x380248E3 | Shotgun [RP] Lv.2 | 0x1FF0ACB0 | include |
| 0x4A69CBE0 | Shotgun [RP] Lv.3 | 0x6D9B2FB3 | include |
| 0x38F02360 | Tranq Grenade Launcher | 0x1F02C733 | include |
| 0x14889C47 | Electric Rod | 0x337A7814 | include |
| 0x4B7F7765 | High-Voltage Rod | 0x6C8D9336 | include |
| 0x3914F466 | Twin Rod | 0x1EE61035 | include |

## Separate identification / gameplay tests

Ghost Blade has two distinct recipe/baggage entries:
- 0x0CE5E07A -> baggage 0x2B170429
- 0x64CE664E -> baggage 0x433C821D

Heavy Machine Gun [MP] also has two distinct entries:
- 0x66E31F44 -> baggage 0x4111FB17
- 0x17B359C8 -> baggage 0x3041BD9B

Test2 selection:
- Ghost Blade: 0x0CE5E07A / baggage 0x2B170429
- Heavy Machine Gun [MP]: 0x66E31F44 / baggage 0x4111FB17

The alternate entries remain explicitly blocked for the test build.
Heavy Machine Gun remains the final optional experiment because its carry/ammo rules differ from normal weapons.

## Final v1.5 decision

Ghost Blade was verified in-game and is included. Heavy Machine Gun [MP] was successfully accepted by fabrication but cannot be handed off through the normal cargo/inventory path, so it is excluded from v1.5.0.
