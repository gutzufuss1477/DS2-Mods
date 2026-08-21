# ASI architecture

The ASI installs two one-byte breakpoint hooks at the validated Need and Current result sites. A vectored exception handler emulates the original MOV instruction and observes the returned six-slot material vectors.

## Pairing

Need and Current calls are paired by:

- current thread ID;
- one of 16 validated caller-pair IDs;
- a short cache lifetime.

This prevents unrelated infrastructure/menu consumers from being combined.

## Object identification

The infrastructure object is reconstructed from the validated RBX anchor and accepted only when its VTable and mode match one of these paths:

- road: modes 1 and 2;
- monorail: mode 1;
- mine: modes 1, 2 and 3.

## Policy

For every material slot:

```text
targetCurrent = Need - RemainingUnits
delta         = targetCurrent - Current
targetLive    = Live + delta
```

The slot is changed only when the native remainder is greater than `RemainingUnits`. This preserves already represented online/player contributions even when `Current` and the locally persisted `Live` value differ.

## Transactional writes

The live progress field and returned Current vector are written, read back and verified. On failure, applied fields are rolled back and the mod disables itself for the session.
