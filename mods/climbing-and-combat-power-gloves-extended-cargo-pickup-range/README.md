# Climbing and Combat Power Gloves - Extended Cargo Pickup Range

Climbing and Combat Power Gloves use the same magnetic cargo pickup range for each level. The defaults are 30 m for Level 1 and 50 m for Level 2.

[Current downloads and installation instructions on Nexus Mods](https://www.nexusmods.com/deathstranding2onthebeach/mods/76)

## Updating to 1.1.3

Close the game, replace `ds2_climbing_gloves_range.asi` beside `DS2.exe` and keep your existing INI. Both glove types now read `Level1RangeMeters` and `Level2RangeMeters` on startup. Set them to 8 and 10 for the original Climbing Gloves distances. Combat Gloves require `EnableCargoPickup=1`.

## Release

[Download version 1.1.3 from Nexus Mods](https://www.nexusmods.com/deathstranding2onthebeach/mods/76?tab=files&file_id=429). The archive contains the ASI and default INI for the Steam PC build 1.10.89.0.

The 8 m / 10 m Combat Gloves fix was tested in game. Automated tests also cover the default ranges, fractional distances, the maximum range and combat action costs.

See [nexus.json](nexus.json) for the release archive checksum and [the test plan](source/TEST_PLAN.md) for verification details.
