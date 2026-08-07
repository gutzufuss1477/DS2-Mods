# Validation

Validated target: Steam PC DS2 build 1.10.89.0, timestamp `0x6A3DAE46`, image size `0x0B292000`.

## v1.0.0 baseline

- The tested normal Lost Cargo writer used component sequence `34 + 17 + 12` and normally awarded `+63`.
- With `ExtraLikes=5000`, runtime validation observed `+5063` and `APPLIED=1`.
- A regular-order comparison had `RBX=1`.

## v1.0.1 change

- The target writer remains `DS2.exe+0x1E1335D`.
- The validated caller remains `DS2.exe+0x1E14C74`.
- The fixed `RBX == 0x22` requirement was replaced by `RBX > 1` so Lost Cargo with different base Like components can receive the configured bonus.
- `RBX == 1` remains excluded because it is the known regular-order signature.
- `R8 == 0`, readable-record checks, arithmetic bounds and duplicate protection are unchanged.
- The generated binary is PE32+ x86-64 and imports only KERNEL32.dll.
- In-game testing confirmed the update fixes the previously failing Lost Cargo case with a different/fragile-marked reward amount.

The production hook has no timeout or diagnostic hit cap and remains active until game exit.
