# Functional validation — v1.1.0

## Verified successful case

- Steam PC build: `1.10.89.0`.
- Standard Order 310, DHV Magellan transport, native selector 5.
- Debug entries: `BESTRANK PATCH rank=0x05; flags=0x03`, `LIKES PATCH selector=0x05; flags=0x03`.
- The result screen displayed the standard **Legend (S)** grade.
- Orders Search displayed the appropriate gold best-rank badge.
- The original save's best-rank mask for mission ID 210008 moved from `0x00000001` (Magellan-only) to `0x00000020` (ordinary Legend).
- Control records for Order 259 and Order 265 retained their old masks `0x48` and `0x08`.

This verifies the grade was persisted by the game, not just displayed.

## Behavior encoded in source

The final rank hook never assigns an artificial grade. It uses the engine's already computed grade from R8B. The Likes normalization uses the original selector argument for the game's own reward lookup rather than setting a fixed or maximum Like total. Both require matching Magellan flags and validated mission/build constraints.

## Still unverified

Selector 7 and every other possible selector, other order categories/difficulty combinations, conflicts with other intercepting ASIs, and specific Steam achievement triggers. The mod does not patch Steam achievements. The functional validation establishes one qualifying case, not a broad regression certification.
