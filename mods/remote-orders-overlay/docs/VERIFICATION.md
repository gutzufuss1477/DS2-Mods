# Release Verification

## Runtime validation

The no-log ASI was tested successfully by the mod author on the host gaming PC. The game started normally and the Remote Orders overlay displayed the expected facility orders.

## Binary lineage

Validated working baseline:

`A325FEC6CAD46F478FA1812DF93BE75D8EC63EE7AFC1618C9B3272F28E8CBD25`

Runtime-tested no-log release:

`696D05285C23F2D73EE27C23827A27BA347C76D79DF0A84F7DB29E164DCB695C`

The only changes are ten fixed-width UTF-16 path replacements to the Windows `NUL` device. Executable code and the critical PE runtime sections remain unchanged.

## Withdrawn build

A source-cleanup v0.1.3 build with modified initialization did not allow the game to start and was withdrawn. It is not included in this repository or release history.
