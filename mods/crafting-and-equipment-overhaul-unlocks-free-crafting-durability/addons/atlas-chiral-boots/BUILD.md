# ATLAS Chiral Boots Addon 1.0.0

The source is the published single-ASI addon implementation. It uses the
Crafting Addon API V1. Required game-derived model/texture/cache inputs stay
local; the embedding scripts record their expected hashes and names. Provide
those inputs locally and configure the local Python/LLVM paths before using
`build-api.ps1`. No game archives, generated resource arrays or precompiled ASIs
are tracked by this import. No new game compatibility is claimed.
