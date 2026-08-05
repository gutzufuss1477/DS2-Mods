# Security Notes

The release is an unsigned native x64 ASI plugin.

Observed capabilities:

- reads memory in the current DS2 process;
- creates a worker thread;
- resolves USER32/GDI32 functions dynamically;
- installs a UI-thread hook;
- creates a transparent, topmost, click-through overlay window.

Not present in the reviewed release:

- network APIs;
- Windows registry APIs;
- shell or command execution;
- external process creation or injection;
- `WriteProcessMemory`;
- `VirtualProtect`;
- persistent DS2 executable code patches;
- active file logging in the no-log release.

Version v0.2.0 additionally reads `DS2_Remote_Orders_Overlay.ini` beside the ASI to select an overlay language. It does not write to the configuration file.

Unsigned game plugins can still trigger heuristic antivirus detections because memory reading, hooks and dynamically resolved UI APIs resemble techniques used by other native software.
