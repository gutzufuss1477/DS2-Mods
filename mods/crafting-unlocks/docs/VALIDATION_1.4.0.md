# Validation - 1.4.0

## Automated validation
- Full Windows test suite passed.
- 1,001,627 scoped durability assertions passed.
- 121 crafted baggage IDs validated.
- 8 supported boot baggage IDs validated, including Chiral Boots.
- Backpack Cover wear multiplier and Unbreakable logic validated.
- Cover repair/increase, removal and type-change fallbacks remain native.
- Cargo/container +0x84 durability is not patched.
- FreeCrafting transaction, rollback, signature and ABI/unwind tests passed.

## In-game verification
- Chiral Boots appear in fabrication.
- Chiral Boots were successfully crafted and equipped.
- Backpack Cover Lv.2 was created and equipped.
- With Durability.Enabled=1 and Unbreakable=1, the live cover value remained exactly 4500/4500.
- Direct process-memory sampling showed no change over a 45-second Timefall exposure.
- The terminal continued to report 100% durability after the test.

## Scope
The public release defaults Durability.Enabled=0. The in-game verification above used
Unbreakable=1 specifically for the durability test. Cargo, order cargo and cargo
containers remain outside the durability patch scope.
