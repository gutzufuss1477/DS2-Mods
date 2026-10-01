# Backpack Cover durability research

This folder contains the local analysis used to add Backpack Cover Lv.1/Lv.2
durability support for Crafting Overhaul 1.4.0.

- `ghidra-scripts/`: targeted read-only Ghidra searches used to identify the
  cover state and relevant access paths.
- `research/`: temporary disassembly helpers and captured outputs retained as
  technical evidence for the chosen hook.
- `live-test/`: the process-memory sampling scripts used during the in-game
  validation session.

Key runtime finding:
- active cover flag: manager + 0x44A0
- cover type/index: manager + 0x44A4
- current cover durability: manager + 0x44A8
- durability hook site: RVA 0x00B394C2

The 1.4.0 implementation modifies genuine durability loss only. Cover repair,
removal and type changes remain native. The final in-game test kept Backpack
Cover Lv.2 at exactly 4500/4500 for 45 seconds of Timefall with Unbreakable=1.

The live-test scripts contain the PID from that historical validation run and
are retained as evidence, not as general-purpose user tools.
