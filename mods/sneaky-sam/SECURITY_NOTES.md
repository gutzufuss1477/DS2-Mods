# Security notes

Sneaky Sam is an in-process x64 ASI plugin for DEATH STRANDING 2.

The release validates the executable PE timestamp, image size, and the exact original call instruction at the supported baggage-visibility callsite before modifying code. If any baseline check fails, the hook is not installed.

The plugin changes one native visibility argument only for baggage slot types 4 through 7. It does not edit save files, inventory records, cargo ownership, weight, condition, capacity, AI state, enemy detection, networking, or the registry.

The hook uses a small executable near relay and immediately forwards to the game's original visibility function. It does not use PAGE_GUARD, hardware breakpoints, vectored exception handlers, single-step tracing, or polling threads after installation.

Validated target: DS2.exe v1.10.89.0. Revalidation is required after a game update.
