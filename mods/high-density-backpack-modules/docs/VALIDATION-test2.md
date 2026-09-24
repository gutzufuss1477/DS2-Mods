# Validation — v1.1.0-test2

Date: 2026-09-23. **Test1 crashed in the user's game; test2 has only been validated locally.** The user performs the next in-game test. See [the confirmed crash diagnosis](CRASH-test1.md).

## Completed locally

- Windows x64 LLVM build with warnings treated as errors, no CRT dependency, and unwind tables enabled.
- Native `/noentry` test DLL: 30 placements, 31st rejection, moves to self-owned cells, invalid IDs and extreme cursor coordinates.
- Exhaustive layout checks for dimensions 1..6, all 30 anchors and all four rotations. Selected footprint masks stay in bounds and never overlap.
- Renderer mock executes the real production wrapper: hides excess module meshes, restores a newly eligible module, respects inherited hidden state, calls the original shadow function and leaves the complete logical backpack record byte-identical.
- All 256 available-charm masks: native pair union, deduplication, invalid/None IDs and null context behavior.
- Battery mock: absent, one or duplicate native Electrobiosis slots produce one bonus; disabled backpack and no unlocked bonus retain native behavior.
- Native enumeration mock: category-5 items, key-to-resource mapping, duplicate records and vector cleanup.
- Assembly fixtures: GP/XMM preservation, output pointer and Cryptobiosis counter initialization, both directly and through the real dynamically allocated near relays.
- Actual native Cryptobiosis loop bytes execute through the installed hook and near relay: 768 mask/native-slot combinations, null component, and inactive-mod cases (771 cases total). The native comparison and back edge stay intact. The fixture is checked byte-for-byte against the exact EXE.
- Scoped module-context relay, real hook installation and rollback on a private fake executable allocation. Every patched window restores its original bytes.
- Exact local DS2.exe SHA-256 matches. Baseline verifier checks 64 byte/call guards, 50 non-overlapping patch windows and instruction boundaries using Capstone. Also checks 1,688 direct branches in affected functions for destinations inside overwritten windows; confirms the checker rejects the old test1 hook.
- Binary inspection: AMD64 DLL, KERNEL32 imports only; C++ and adapter stack frames carry unwind information.

## Not established by these tests

- Actual renderer behavior, culling, animation and appearance inside/outside the customization menu.
- Actual maximum capacity and native special-module limits in all combinations.
- Save/load and restart behavior with 30 modules.
- Native catalogue availability at runtime and the real gameplay outcome of each charm under its situational conditions.
- Compatibility with other mods altering these paths.

See [the in-game checklist](TESTING-test2.md). Historical v1.0.0 in-game results are in [v1.0.0 validation](v1.0.0-VALIDATION.md) and are not claimed for this build.

## Artifact

`DS2_HighDensityBackpackModules_v1.1.0-test2.asi`

SHA-256: `E1CFCC7FBDF6E1F70D315A3A922E914E2802C13CA6B7734DDE6379FDAFDE0E74`

The test ZIP contains this single ASI. Source changes and the artifact are local; no GitHub publication or game installation was performed.
