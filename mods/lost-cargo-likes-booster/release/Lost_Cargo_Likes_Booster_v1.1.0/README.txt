Lost Cargo Likes Booster v1.1.0

DEATH STRANDING 2: ON THE BEACH - Steam PC build 1.10.89.0

WHAT IT DOES
Adds the configured bonus when returning Lost Cargo and synchronizes the same amount to the matching Facility Connection Points, so the bonus contributes to actual star progression.

The mod does NOT directly force a facility to 5 stars. DS2 still evaluates its normal star thresholds.

INSTALL
1. Install an external x64 ASI loader (not included).
2. Copy ds2_lost_cargo_likes_booster.asi and ds2_lost_cargo_likes_booster.ini beside the loader in the DS2 game directory.
3. Restart the game after changing the INI.

CONFIGURATION
[LostCargoLikesBooster]
ExtraLikes=5000

ExtraLikes accepts 1..1000000. Missing, zero, malformed, or out-of-range values use 5000.

v1.1.0
- Extra Lost Cargo Likes now also increase the matching Facility Connection Points.
- Star progression therefore follows the configured bonus.
- Bonus is applied once per detected Lost Cargo reward transaction, preventing repeated bonuses from internal reward components.
- Supports different Lost Cargo base Like amounts, including the previously reported fragile-marked case.
- No direct instant-5-star override is used.

IMPORTANT
The mod is validated for DS2 Steam PC build 1.10.89.0. Revalidation is required after game executable updates.
