# Technical design — v1.1.0

Supported executable: `DS2.exe` (Steam PC build `1.10.89.0`), SHA-256:
`BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B`.

## Normal grade, no forced S

The game computes a normal grade before the Magellan-only evaluation overrides it. In `FUN_141449650`, it chooses the already computed rank in R8B or the Magellan special value zero. The game then sets `1 << chosen_rank` in the mission best-rank mask.

The main fix hooks the game's `CMOVE ECX,EAX` at `DS2.exe+0x1449D0F`. When the Magellan-specific state and Standard Order category are verified, and the engine's native rank is a valid value 1–8, the mod allows the **same original rank** to be chosen. There is no hard-coded 5, S or max grade.

The Likes hooks restore the original per-order reward selector 1–8 instead of the penalized selector 0. Reward tables, amounts and scaling remain native.

## Guards and runtime behavior

All 16 hook signatures are validated against the exact build. Eight reward-lookup sites, four display ranking sites, one result record site, one persistence site, one preliminary evaluator and the final native best-rank instruction are guarded. Mismatching versions or conflicting hook signatures prevent activation.

The rank interception checks mission type and flag state, EAX/R8B values and override state; it changes only the CPU zero flag for the original instruction. Other code paths retain normal behavior.

No game executable, asset table, save file or achievement flag is directly edited. The original game writes the earned result into its own save normally.

## Limitations

The confirmed live test was Standard Order 310 (native selector 5) using the Magellan on Steam build 1.10.89.0. Other selectors, category/difficulty combinations, Steam achievement processing and ASI conflicts were not individually validated. These should not be described as universally proven.
