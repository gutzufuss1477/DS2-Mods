# Crafting and Equipment Overhaul 1.9.9

Use the pinned local DS2 build and Windows LLVM toolchain. `build.ps1` compiles
the published 1.9.9 host source. Native game textures and generated fixture data
are deliberately local inputs: `tools/embed_weapon_pixels.py` lists the expected
asset names, sizes and hashes. Export these from a legitimately owned matching
installation into `assets/`; no originals are stored in Git. Then run the existing
build entrypoint. Do not use the separate private 1.9.23 workspace as this release.

The optional Chiral Boots 1.0.0 source lives in the sibling `addons` directory.
Its original mesh/texture/cache inputs are likewise not redistributed. The
published Nexus downloads remain the end-user installation path.
