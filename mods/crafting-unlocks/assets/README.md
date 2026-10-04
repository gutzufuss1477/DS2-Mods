# Embedded weapon pixels

These unchanged native DS2 1.10.89.0 texture payloads are copied from the
accepted More Silenced Guns 0.5.2 build. Only the private GPU views for custom
weapons apply R/B/B/A channel mapping; alpha is preserved. Native textures
and original weapons are not overwritten.

| File | Source resource | SHA-256 |
|---|---|---|
| bigbore-albedo.bc1 | Color 499:40075 | 517319d1c6d53df4fc1cd8e3dd3c430f2b994db568e22e8be4fba7b789745420 |
| shotgun-icon.bc7 | UITexture 56:110242 | a86f3120e90b307746ed88b0fc8c3769c9927cfc50a505b1f8539b3ae1badb8b |
| bigbore-icon.bc7 | UITexture 56:75928 | d175202e1ce11337588cb46db06926ae09ae01320c7e1cdc6536f3bd53065f64 |

`tools/embed_weapon_pixels.py` checks lengths/hashes before generating the
build-only C++ arrays. The ASI embeds the resulting data; the Nexus ZIP needs
no separate texture installation. The AR, MG and shotgun bodies reference
the native red Mech resources through the existing material replacement path.
