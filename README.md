# DS2 Mods

Source code, documentation and validated releases for my DEATH STRANDING 2 mods.

## Mods

* [Hill Assist and Speed Boost](mods/hill-assist-and-speed-boost/)

- [Remote Orders Overlay](mods/remote-orders-overlay/)
- [Pickup Cargo Capacity](mods/pickup-cargo-capacity/)
- [Tri-Cruiser Cargo Capacity](mods/tri-cruiser-cargo-capacity/)
- [Floating Carrier Cargo Capacity](mods/floating-carrier-cargo-capacity/)
- [Sam Stats Booster](mods/sam-stats-booster/)
- [Lost Cargo Likes Booster](mods/lost-cargo-likes-booster/)
- [No Magellan Evaluation](mods/no-magellan-evaluation/)
- [Weapons Anywhere](mods/weapons-anywhere/)
- [Construction Anywhere](mods/construction-anywhere/)
- [Zipline Range and Speed](mods/zipline-range/)
- [Chiral Bandwidth Costs](mods/chiral-bandwidth-costs/)


* [High-Density Backpack Modules](mods/high-density-backpack-modules)

Additional DS2 mods can be added later as separate folders under `mods/`.

## All-in-one installer

The bilingual Windows GUI, stable-mod catalog, transactional installer and
release build are maintained under [installer](installer/). Version 1.1.0 adds
an English/Deutsch selector and one central **Mod Settings** menu while keeping
the individual runtime INIs expected by each ASI plugin. The manager detects
the Steam game directory and supported `DS2.exe`, reuses or installs the tested
x64 ASI loader, preserves customized INIs and supports hash-verified update,
repair, rollback and removal. Catalog, settings schema and payloads are embedded
in the release EXE.
