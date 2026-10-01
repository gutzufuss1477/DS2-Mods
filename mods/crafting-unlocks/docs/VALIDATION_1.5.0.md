# Validation - 1.5.0

## Automated validation
- Full Windows test suite passed on the final regenerated v1.5.0 build.
- 335434 core policy/layout/selection/SHA assertions passed.
- 132 supported INI entries: 103 normal fabrication + 19 backpack modules + 2 covers + 8 charms.
- 97152 native instruction cases / 291956 assertions / 88 Windows unwind checks passed.
- 26900 transactional patch assertions passed.
- 31 targeted Usage=None UI-filter cases passed.
- 1001836 scoped durability assertions passed for 132 crafted baggage IDs + 8 boot IDs.
- FreeCrafting and durability allowlists include the 11 new supported enemy-drop weapons.
- Heavy Machine Gun [MP] recipe IDs remain excluded.

## In-game verification
- Assault Rifle [RB] Lv.1-3 appeared in fabrication and could be crafted/equipped.
- Shotgun [RP] Lv.1-3 appeared in fabrication and could be crafted/equipped.
- Tranq Grenade Launcher, Electric Rod, High-Voltage Rod and Twin Rod could be crafted/equipped.
- Ghost Blade could be crafted and used successfully.
- Heavy Machine Gun [MP] fabrication was accepted but no normal inventory/cargo handoff occurred; excluded from final release.

## Scope
The 1.5.0 release adds only enemy-drop weapons already usable by Sam. Cosmetic/red duplicates and Heavy Machine Gun [MP] are excluded.
