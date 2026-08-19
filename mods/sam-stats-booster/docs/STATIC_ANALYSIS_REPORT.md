# Static Analysis Report

## Result

The supplied DS2 build contains a dedicated, compact dispatch complex for Sam's proficiency gains. Three producer functions cover all 12 visible radar-chart categories. This provides a substantially safer implementation route than intercepting generic play-record traffic.

## Key findings

1. The player-skill enum contains 13 internal types. Twelve correspond to the visible Stats chart; `StealthTakeDown` is a thirteenth internal skill.
2. Skill availability is checked before each producer reaches the selected hook site.
3. The event payload passed to the producers is a delta consumed by an additive DSPlayRecordManager mutator.
4. Physical stats use a double-precision record path, combat/weapon/Strand stats use a Q32 record path, and Lung Capacity uses a Q64 path.
5. The original game caps and downstream progression logic can remain untouched.

## Signature validation

All producer signatures were searched in the `.text` section of the supplied `DS2.exe` and each resolved exactly once:

| Producer | Unique RVA |
|---|---:|
| Q32 combat/Strand | `0x0D92F30` |
| Q64 Lung Capacity | `0x0D92FF0` |
| Double physical stats | `0x0D93040` |

Decoded support addresses:

- DSPlayRecordManager global slot: `DS2.exe+623E5E8`
- Double-record cap: `DS2.exe+3462020`
- Double cap value: `999,999,999,999.99`

The machine-generated output is preserved in `STATIC_VALIDATION_OUTPUT.txt`.

## Rejected implementation routes

- Global hook at `C00D70`, `C00E60`, or `C00FA0`: rejected because these are shared play-record mutators.
- Direct level writes: rejected because they could bypass threshold and unlock processing.
- Save-file modification: rejected because it is unnecessary and less robust.
- Stats-menu UI patch: rejected because the UI is only a consumer of the stored progression state.
- Broad Cheat Engine scans: not required for the first build because the static producer closure is sufficient.

## Runtime closure

Runtime verification confirmed that the dedicated producer deltas reach DS2's stored play records exactly after multiplication. The Double path was validated across all four physical categories, the Q64 path through Lung Capacity, and the shared Q32 path through Melee Combat and Assault Rifles. The remaining Q32 categories use the same producer and mutator with distinct statically recovered record IDs.
