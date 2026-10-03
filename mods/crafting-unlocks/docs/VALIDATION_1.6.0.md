# Crafting & Equipment Overhaul 1.6.0 — local integration validation

See `validation/summary-1.6.0.json` for the precise build hash and checks.

The runtime contains one DllMain and initialization worker. It verifies the supported EXE once, pins its module, then installs the 36-site ATLAS group before native catalogue construction. The original 132 configured recipes and gameplay resources retain their existing behavior. Optional FreeCrafting and durability use the same settings and include the two stable ATLAS identities.

The public ATLAS default is off; missing sections in old INIs also default off. The local test INI enables it. With ATLAS off, resources and scoped effects remain registered for saved-item compatibility, but Usage=None hides the two recipes. CPU tests verify only that byte changes, and both the production menu policy and native UI thunk reject these hidden recipes. The global master switch still installs no hooks when off.

Windows suites: core, legacy config, backpack config, all 44 native cost thunks, 49-site transactional failure injection, special recipe UI, durability and ATLAS config all pass. The embedded ATLAS native suite passes 20 tests including 110208 branch cases and 336 original hip-validator cases. All 91 possible hook ranges match the exact game EXE and are mutually disjoint. The ASI is AMD64, imports only KERNEL32, has unwind/relocation directories and ASLR/NX, and contains no writable executable PE section.

Local game PID34572 loaded only ds2_crafting_unlocks.asi. Read-only checks confirmed 39 active hooks, the 52 disabled optional sites remaining native, all 19 ATLAS resource/visual/progression checks and unchanged original item parameters. Worn item IDs103/104, boot maximum3400/current3392.0205, three powered/equipped level-3 skeleton flags and +180kg were observed. No game controls or save writes were automated.

The user reports "ok scheint zu klappem" after the integrated installation: initial integrated smoke test accepted. This is not a separate confirmation of every option or a new save/load cycle.

The user's earlier standalone checks cover fabrication/equip, restart/load persistence, early-save availability and hip cargo attachment. A separate new save/load cycle, the recipe-off transition and ATLAS-specific free/durability in-game checks remain pending. The individual footwear effects and combat protection were not exhaustively gameplay-tested. Foreign Steam helper processes may log VERSION_BLOCKED: the main DS2 process passes the exact-build gate.

The prior standalone binary is backed up and renamed with a non-ASI suffix. The migration backup and installed hashes are in `validation/install-1.6.0.json`. Existing 1.5.0 and standalone source/release backups were preserved. No Nexus publication was performed.

Publication name: Crafting & Equipment Overhaul — Unlocks, Free Crafting, Durability & ATLAS Gear. The final runtime rebuild changes only the log banner from the tested 1.6.0 binary. The tested and final hashes are recorded separately in the validation summary. The running game was not restarted for this branding change.
