# Security notes

This ASI is an in-process DS2 plugin. It checks the executable timestamp and image size before enabling.

The v1.1.0 implementation uses hardware execute breakpoints plus a vectored exception handler on two validated paths:

1. The Lost Cargo Like writer at `DS2.exe+0x1E1335D` with validated caller `DS2.exe+0x1E14C74`.
2. The Connection update path located at runtime through the validated instruction signature used by the connection-level update.

The Like hook requires the validated caller, `RBX > 1`, `R8 == 0`, readable record data and arithmetic bounds. The observed regular-order `RBX == 1` signature remains excluded.

After the Like bonus is armed, Connection synchronization is allowed only when the same thread reaches the matching Connection record, the Like record is exactly `ConnectionRecord + 0x18`, and the visible Like total equals the current Connection Points plus `ExtraLikes`. Only then is the current Connection Point value synchronized to the boosted visible total.

The plugin does not directly write the star level and does not force level 5. DS2 continues through its own Connection update logic after the point synchronization.

The ASI does not edit save files directly, use networking, or modify the registry. It is build-guarded to the validated executable. Revalidate after a DS2 game update before publishing or using the mod on the new build.
