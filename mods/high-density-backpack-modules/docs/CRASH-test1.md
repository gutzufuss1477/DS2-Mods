# Confirmed test1 crash and test2 correction

The user's game log and `DEATH STRANDING 2 - ON THE BEACH-v1.10.89.0 2026-09-23-23-20-18.mdmp` in Documents were inspected locally. The installed test1 ASI matches the delivered SHA-256 `5EDCD70DB5E0245142059BBF64F42C0A1508492C9772F882193B187EE5F52EF0`.

## Evidence

- Exception: `0xC000001D` (illegal instruction), thread 27824.
- Dump RIP: `0x7FF62C334C17`; DS2 image base: `0x7FF62A620000`; game RVA: `0x1D14C17`.
- ECX=1: the loop had advanced past its first charm entry. RAX points inside the mod's virtual charm table.
- The mod log reports `PATCH_APPLIED`, then an initial empty available-charm mask. An empty mask during startup is enough to take the faulty back edge; it is not evidence that later ownership enumeration works or fails.

The dump contains these instructions:

```text
1D14C10  41 8B CE             mov ecx,r14d
1D14C13  E8 E8 B3 1D FE       call near_relay
1D14C18  90 90                nop; nop
1D14C1A  74 1B                je 1D14C37
1D14C1C  FF C1                inc ecx
1D14C1E  48 FF C0             inc rax
1D14C21  83 F9 08             cmp ecx,8
1D14C24  72 F1                jb 1D14C17
```

Test1 replaced both the conditional pointer selection and the comparison at `1D14C17`. Replaying that comparison in the adapter handled the first iteration only. The second iteration branched into the last displacement byte (`FE`) of the inserted CALL, producing an invalid instruction. This is a confirmed mod defect, not a speculative driver or memory diagnosis.

## Correction

Test2 moves the seven-byte hook to `1D14C10`, replacing `mov ecx,r14d; cmove rax,r8`. The adapter reproduces the counter assignment and selects the virtual array. The original comparison at `1D14C17` and back edge remain intact. Runtime guards additionally check the complete setup and loop bytes before patching.

The native regression test executes the original loop bytes with the real installed hook/relay, substituting only its two exits with observable return values. It covers all 256 virtual masks with no native Cryptobiosis, the first native slot, and the second native slot, plus null and inactive-mod cases. The baseline verifier checks the fixture against the exact EXE and rejects direct branches into overwritten instruction interiors across all affected functions. It explicitly detects the old test1 defect.

These checks establish the correction for this specific crash. They do not replace an in-game test of loading, rendering, module capacity, charm ownership or effects. Test2 has not been run in the game by the developer.
