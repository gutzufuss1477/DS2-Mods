# Runtime architecture and release delta

The only runtime source delta from the user-tested 0.2.0-alpha candidate is the
startup banner version. See `../validation/RUNTIME_SOURCE_DIFF.patch` and
`../validation/BASELINE.json`. The production parser, selection kernels, data
layouts, 29-entry backpack allowlist, thread/patch mechanisms and hash gates
are unchanged.

## Bindings

| Route | Callsite RVA | Original target | Bytes before patch |
|---|---|---|---|
| Normal fabrication | 0x0171DB1D | 0x00B6F9C0 | E8 9E 1E 45 FF |
| Backpack customization | 0x01529896 | 0x00B6FC20 | E8 85 63 64 FF |

Both original producers are called once. The normal menu retains stable private
snapshots; the backpack menu uses native vector-copy routine 0x00B7E950 so its
allocation and later destruction remain engine-owned. Neither hook writes a
global availability predicate or the catalogue's original 0x8000 unlock bits.

The backpack route accepts only the 29 allowlisted recipe NameCodes (19 modules,
2 covers, 8 charms). It does not infer that every Usage=5 record is a supported
unlock. All 29 IDs are checked against loaded native type tables before patching.
Native entries, including otherwise unsupported entries, are copied unchanged.

INI value 0 affects only optional early additions. A normal native unlock is
preserved with identical instance bytes even when its override is 0. The game
can set native manufacture/purchase flags (notably 0x20000) and can save equipment
and resources. Thus lack of direct global-unlock writes is not a claim that
crafting has no persistent consequences.

The native payment, placement, equipment and charm-effect code is not patched.
The code has conservative snapshot/list limits; a refusal returns to the native
list instead of silently truncating additions. The DLL is pinned and has no
live hot-unload or live configuration reload.

The startup thread suspension method is unchanged and is not a formal proof
against every concurrently created external thread. A Windows game process is
not reproduced by the host policy tests. Hook overlap and combined-mod use
require their own compatibility checks.

## Evidence

The `evidence/backpack/` and `evidence/targeted/` files preserve the source excerpts.
The backpack CALL bytes are calculated from the recovered direct-call addresses
and verified in memory; they were not presented as raw bytes in the original
master. `docs/historical_v020/BACKPACK_EXTENSION.md` retains the pre-test analysis.
Historical "not tested" statements there describe that stage, not the subsequent
user report. Current test scope is in `VALIDATION.md`.

## Public references consulted

These document tools/ABI, not DS2 recipe behaviour:
- https://clang.llvm.org/docs/UsersManual.html
- https://learn.microsoft.com/en-us/cpp/build/x64-calling-convention
- https://git-scm.com/docs/git-add
- https://git-scm.com/docs/git-apply
- https://github.com/ThirteenAG/Ultimate-ASI-Loader

Configuration keys and grammar are taken from the included production parser,
not from generic Windows INI semantics or another mod's settings.
