# DS2 Construction Anywhere v1.0.0

ASI mod for **DEATH STRANDING 2 ON THE BEACH**, Steam PC build **1.10.89.0**.

It independently bypasses the construction `NearbyFacility` distance check and `UnconstructableArea` no-build volumes. Both default features were confirmed in game for version 1.0.0. A riskier, still experimental `PermittedArea` whitelist bypass is included as an opt-in setting. Terrain, material, road, grounding, Q-pid/chiral-network, monorail-route, and other collision checks remain unchanged.

Copy `ds2_construction_anywhere.asi` and `ds2_construction_anywhere.ini` next to `DS2.exe`, using an existing compatible ASI loader. It can run alongside the separate Weapons Anywhere mod.

The default configuration is:

```ini
[ConstructionAnywhere]
Enabled=1
RemoveProximityLimit=1
AllowRestrictedAreas=1
IgnorePermittedAreaWhitelist=0
DebugLog=0
```

Back up the save and test offline first. Structures placed in protected geometry can collide, become inaccessible, or fail online synchronization. The mod fails closed on unsupported executable builds or signature mismatches and never edits `DS2.exe` on disk.

Special monorail/track proximity checks are intentionally preserved and can still display their own “too close” warning.

See `README_DE.md` and `docs/TEST_PLAN.md` for details.
