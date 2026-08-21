# Validation

## In-game

Confirmed:

- each supported construction/upgrade path reaches the configured remainder;
- `RemainingUnits=1` consumes one native unit from larger containers;
- overflow remains in the container or is returned to the private locker normally;
- completed objects persist after save/reload;
- loaded objects can update in world/overview displays before menu interaction;
- existing online/player contributions are preserved.

## Offline

- reference policy tests;
- caller-pair table contains 16 exact Need/Current pairs;
- PE32+ x64 DLL validation;
- only KERNEL32.dll imported;
- ASLR, high-entropy ASLR and DEP flags;
- release payload and SHA-256 validation.
