# Security notes

This ASI is an in-process DS2 plugin. It checks the executable timestamp and image size before enabling. It uses a hardware execute breakpoint and vectored exception handler at one validated writer, and changes only the validated `RCX` value for a lost-cargo component.

It does not call DS2 functions, edit save files, use networking, or modify the registry. The guard checks the validated caller, `RBX=0x22`, `R8=0`, readable record data, arithmetic bounds, and one application per record. The observed regular-order signature (`RBX=1`) is excluded.

The handler is owned by the worker thread: shutdown disables the hook, clears thread breakpoints, and removes the handler. DLL detach only signals shutdown and does not perform unsafe loader-lock cleanup. There is no timeout or hit limit; the hook remains active until game exit.
