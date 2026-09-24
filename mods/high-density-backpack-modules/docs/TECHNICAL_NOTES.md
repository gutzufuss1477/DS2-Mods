# v1.1.0 technical notes

## Logical equipment and visual placement

The native 5×6 owner grid remains the storage format. Twenty-four instruction patches make reference collection B3FD10, replacement collection B40090 and validation/commit B40310 use one cell. Wrappers at B43043, B42E79, B431A8 and B431D4 retain nearest-free/self-owned anchoring. The user confirmed 30 modules with test2. The vanilla full-grid replacement workflow remains; this build does not extend the serialized module array.

Test2 selected visible models at their saved logical anchors. This left holes and also let the patched B3FD10 alter the bone averaging used by native model placement. Test3 separates both concerns:

- A visual layout packs original dimensions (+24/+28 in BAD600 definitions) and the saved rotation into the 5×6 surface. Small representatives of different types go first, followed by further copies that fit. The layout may differ from the saved cursor positions and does not modify them.
- B3AA9C and B3AB46, the two visual-only calls to B3FD10 inside B3A860, use a full-footprint bone-reference builder. Installed local modules use the packed anchor. Previews retain their native full-size anchor/fallback search. The builder uses native A2F50 cleanup, A2E20 capacity growth and string reference counting. Rotations too large to fit the visual grid get one valid bone to keep the hidden/preview transform finite; they are excluded from the visible layout.
- The existing B3D386 hook runs after native shadow update 33B480, supplying RDI=info and RSI=slot×60 through the relay. It checks the entity/model identity and local manager contexts. A layout/model change triggers native B3A860 to reposition eligible models. Original module records stay untouched.
- Mesh output uses 22A8D0(renderer,index,chosen && inheritedVisibility). Model nodes are at +58 with count +50, stride 30; owner +8, renderer owner+C8, mesh index +28, inherited flag +2D. Native node flags remain unchanged, allowing previously hidden models to reappear.

The cache has five slots: the three native local manager contexts (45B0/45B8/45C0), the live equipment preview, and the live gameplay backpack; invalidation includes IDs, types, positions, dimensions, rotations and active state. The packing guarantees disjoint grid rectangles, not triangle-level geometry or animation separation.

## Eight accessory rows

The native two-byte storage stays intact. Six extra slot IDs live in the ASI, persisted as `Equipment/InvisibleSlots` in `DS2_HighDensityBackpackModules.charms.ini`. It is an installation-wide preference shared across save files. Effects are filtered by the current available-item mask; default extra slots are empty.

The category-1 call at 15285F7 wraps 1528E90 and supplies a temporary eight-entry vector using the original 38-byte entry format. The category cap at 1528EDB becomes eight. The native function creates the actual list rows and header count; no fake labels or overlay are used. Native slot indexing runs in reverse, so the last two rows remain physical slots 1 and 0, preceded by invisible slots 7..2.

The row-index call 1523706 and duplicate checks 15270F6/1529510 use the eight-slot mapping. The eight-byte read at 1523742 redirects to an adapter returning the selected slot value in ECX while preserving RAX and other volatile state. Selection writes at 1527139 and cancellation tail-jump 15272AC dispatch to native physical-slot writes or extra-slot state. The latter remains a JMP, preserving the caller's stack/return convention. Bulk removal's index arithmetic at 1524D19 changes to eight; native bounds checks still limit game-memory writes to two slots. The B393E0 call at 1524DF6 additionally removes selected extra rows. The close-menu B41A60 call at 15189BA saves additional choices.

The menu build logs eight slots; save failures are logged. INI decoding rejects invalid IDs and duplicates among extra slots. It creates no game save fields and does not expand any native two-byte array.

## Effects and availability

Native B6FC20(catalogue,5,vector), global 623E540, enumerates available items. Entries use low 12 bits of key+8; definition+20 holds the hash. Mapping 2DD37D0 covers IDs 1..8. B7E840 destroys the temporary vector. Refresh occurs at most once a second during native backpack updates/capacity queries.

Unlike test2, the effect mask is now `native pair UNION (available mask AND equipped extra mask)`. Eight immutable ID arrays implement deduplicated membership. Bell consumers E52048/1058FC5/19C373E, Combat DACA7F, Animal 1A6FF0C, Like 1E1B657, Stamina E6C605, Cryptobiosis 1D14C10 and Porter 17C8F49 retain the verified native effect paths. Electrobiosis uses B3E470 wrappers at B3CFC6/EA8633/EA7214 to add the missing extra bonus once; disabled and unrelated backpacks retain native behavior.

## ABI, guards and verification

Near relays use a RIP-indirect absolute jump that preserves RAX. Inline adapters preserve volatile GP/XMM state except the outputs written by the replaced instructions. The Cryptobiosis hook leaves the comparison at 1D14C17 and its back edge intact, preserving the test1 crash correction. Stack frames carry Windows x64 unwind metadata.

All windows and native signatures are checked before writing. Relays are prepared first, the ASI is pinned, pointer hooks precede wider loop bounds, and rollback restores the bounds first. The mutex excludes duplicate Backpack versions. PE metadata is checked at runtime; exact EXE hashing and branch/interior checks are performed by the baseline verifier.

Tests include the original category-builder instructions with a substituted row allocator for all 256 equipment masks, the actual Cryptobiosis loop, real installed inline/tail relays, state-file roundtrip, placement and visual reference/packing mocks. The user confirmed the final gameplay/menu appearance. Broader lifecycle, animation/culling and actual effect measurements retain the limits documented in VALIDATION.md. See VALIDATION.md for the final user confirmation and remaining limits.

## Equipment-menu preview (test4)

The equipment-view component has a separate backpack copy. RTTI 4355C00 identifies `DSAkukanPlayerEquipmentManageComponent` (name at 4354710). B413F0 resolves this component via global 623EA00, owner +38, native lookup 11FFA0(owner+80, RTTI). EEF960 copies the player's equipment into the preview backpack addressed by component +2D8, then weak-pointer +340 (component +360 minus the weak interface +20). Instructions at EEFE89 establish these offsets; EEFF03 invokes B3B170 for that copy.

Test4 resolves this live preview as visual context 3. Both module mesh visibility and full-footprint bone placement use the existing packing policy for this context. The three original manager contexts retain precedence. The additional resolver does not change charm ownership, effects, native equipment records or save coordinates, and rejects unrelated actors. Null owner, absent component and preview teardown leave native rendering in control. No preview-owner pointer is retained across calls.

No patch sites or hooks were added (still 63 windows). Added guards cover 11FFA0, the equipment component's size metadata and EEFE89's preview-field instructions. `EQUIPMENT_PREVIEW_VISIBILITY=ACTIVE; render_mesh_only` is logged on the first processed preview model. It confirms that the fourth context is reached, not that no later engine pass restores visibility. The user subsequently confirmed clean cargo-menu rendering in Test4 and all tested views in Test5.

## Live gameplay backpack after save load (test5)

The manager's 45B0/45B8/45C0 cache can be absent before entering customization. B413F0 first obtains global 623DF40's list (count +38, pointers +40), finds entry +70 == 0, uses entry +48 as the actor, resolves RTTI 435ECE0 at actor+A0, then follows equipment +58's weak owner and resolves RTTI 435D5A0 at owner+80. The backpack info is component+360.

Test5 duplicates only this read-only discovery through native 11FFA0, with null/count checks. It never invokes B413F0's initialization or refresh side effects. The live gameplay info is visual context 4; equipment preview remains context 3 and the original three cached contexts keep priority. Five separate layout caches prevent menu/gameplay subsets from overwriting one another. Full-footprint bone reference lookup covers all five contexts. No additional effect or menu patches are introduced.

Additional baseline guards cover B41428/B41440 (player enumeration), B41478/B41499 (component and weak-owner lookup), and B414C1 (info offset). `GAMEPLAY_BACKPACK_VISIBILITY=ACTIVE; live_player_lookup` reports first use of the live gameplay fallback. Its absence is expected when an original cached context already matches. Native patch windows remain at 63.
