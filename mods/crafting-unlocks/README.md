# Crafting Overhaul - Unlocks, Free Crafting & Durability

Version 1.3.0 for DEATH STRANDING 2: ON THE BEACH.

Features
- 120 individually configurable supported recipes/items.
- Early fabrication unlocks while preserving native progression.
- Backpack modules, covers and charms through the native backpack menu.
- Craftable Omnireflector Boots.
- Optional FreeCrafting.
- Optional durability multiplier (1.0-1000.0) and Unbreakable mode.
- Dedicated boots wear handling for the 7 supported craftable boots.
- Cargo/order cargo/container durability remains vanilla.

Quick configuration
[CraftingUnlocks]
Enabled=1
DefaultUnlock=1
FreeCrafting=0

[Durability]
Enabled=0
Multiplier=2.0
Unbreakable=0

CraftingUnlocks.Enabled=0 is the global master switch and installs no patches.
Unbreakable=1 requires Durability.Enabled=1 and overrides Multiplier.

Compatibility
Supported DS2.exe: 1.10.89.0
SHA-256: bf3d1c665545930bc850d8f5df486f7395885bb729d4fd408fdb03390de0765b

The mod checks the exact executable and patch signatures before applying changes.
