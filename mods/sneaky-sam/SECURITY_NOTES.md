# Security notes

Sneaky Sam is an in-process x64 ASI plugin for DEATH STRANDING 2.

The plugin validates the executable PE timestamp, image size, and exact original call instruction at the supported baggage-visibility callsite before modifying code. If a baseline check fails, the hook is not installed.

v1.0.1 is built with Microsoft Visual C++ / Windows SDK and includes a normal Windows version resource.

The near relay is allocated as read/write memory, populated, and then changed to read/execute before use. It is never allocated as read/write/execute.

The plugin changes one native visibility argument only for baggage slot types 4 through 7. It does not edit save files, inventory records, cargo ownership, weight, condition, capacity, AI state, enemy detection, networking, or the registry.

The hook does not use PAGE_GUARD, hardware breakpoints, vectored exception handlers, single-step tracing, or persistent polling after installation.

Validated target: DS2.exe v1.10.89.0. Revalidation is required after a game update.
