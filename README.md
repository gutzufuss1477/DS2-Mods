# DS2 Mods

Source code, documentation and validated releases for my DEATH STRANDING 2 mods.

## Mods

* [Hill Assist and Speed Boost](mods/hill-assist-and-speed-boost/)
- [Remote Orders Overlay](mods/remote-orders-overlay/)
- [Pickup Cargo Capacity](mods/pickup-cargo-capacity/)
- [Tri-Cruiser Cargo Capacity](mods/tri-cruiser-cargo-capacity/)
- [Floating Carrier Cargo Capacity](mods/floating-carrier-cargo-capacity/)
- [Coffin Board Reworked](mods/coffin-board-all-terrain-speed/)
- [Sam Stats Booster](mods/sam-stats-booster/)
- [Lost Cargo Likes Booster](mods/lost-cargo-likes-booster/)
- [No Magellan Evaluation](mods/no-magellan-evaluation/)
- [Weapons Anywhere](mods/weapons-anywhere/)
- [Construction Anywhere](mods/construction-anywhere/)
- [Construction Max Level on Build](mods/construction-max-level-on-build/)
- [Zipline Range and Speed](mods/zipline-range/)
- [Chiral Bandwidth Costs](mods/chiral-bandwidth-costs/)
- [Climbing Power Gloves Range](mods/climbing-power-gloves-range/)
- [Extended BT Cord Cutting Range](mods/extended-bt-cord-cutting-range/)
- [APAS Memory Costs](mods/apas-memory-costs/)
- [Proficiency Bonus Multiplier](mods/proficiency-bonus-multiplier/)

* [High-Density Backpack Modules](mods/high-density-backpack-modules)

Additional DS2 mods can be added later as separate folders under `mods/`.

## All-in-one installer

The bilingual Windows GUI, stable-mod catalog, transactional installer and
release build are maintained under [installer](installer/). Version 1.6.0 manages
21 validated stable mods, including Coffin Board Reworked 1.83.0 and APAS Memory Costs 1.1.0 with optional Unlock All disabled by default. The English/Deutsch
interface includes one central **Mod Settings** menu while keeping
the individual runtime INIs expected by each ASI plugin. The manager detects
the Steam game directory and supported `DS2.exe`, reuses or installs the tested
x64 ASI loader, preserves customized INIs and supports hash-verified update,
repair, rollback and removal. Catalog, settings schema and payloads are embedded
in the release EXE.
