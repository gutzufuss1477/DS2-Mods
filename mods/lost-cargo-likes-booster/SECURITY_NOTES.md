# Security notes

The ASI is an in-process DS2 plugin. It validates the target DS2 executable timestamp and image size before enabling. It uses a bounded VEH hardware execute breakpoint at one validated DS2 writer and only adjusts the CPU `RCX` context for the validated lost-cargo component. It does not call DS2 functions, edit save files, use networking, or modify registry settings.

The effect is guarded by the observed DS2 build, writer/caller path, `RBX=0x22`, `R8=0`, record validity, arithmetic checks, and a one-per-record sequence guard. Regular orders with the observed `RBX=1` signature are excluded.
