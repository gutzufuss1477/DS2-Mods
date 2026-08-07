Lost Cargo Likes Booster v1.0.1

Adds configurable Facility Likes for delivered Lost Cargo in DEATH STRANDING 2: ON THE BEACH (Steam PC build 1.10.89.0).

INSTALL
1. Install an external x64 ASI loader (not included).
2. Copy ds2_lost_cargo_likes_booster.asi and ds2_lost_cargo_likes_booster.ini beside the loader in the DS2 game directory.
3. Restart the game after changing the INI.

CONFIGURATION
[LostCargoLikesBooster]
ExtraLikes=5000

ExtraLikes accepts 1..1000000. Missing, zero, malformed, or out-of-range values use 5000.

v1.0.1
- Fixed Lost Cargo with different base Like rewards not receiving the configured bonus.
- No longer requires an exact 34-Like base reward component.
- Known regular-order RBX=1 signature remains excluded.
