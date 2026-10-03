# Implementation notes

## Supported executable

Steam x64 `DS2.exe` version **1.10.89.0**:

- SHA-256: `bf3d1c665545930bc850d8f5df486f7395885bb729d4fd408fdb03390de0765b`
- PE timestamp: `0x6A3DAE46`
- SizeOfImage: `0x0B292000`

At runtime the mod checks the process basename, AMD64 / PE32+ headers, timestamp, image size and six instruction signatures. It does not hash the running image. Unsupported signatures or an already loaded private probe leave it inactive.

## Hook behavior

The five-byte hook is at RVA `0x106153C` in the ramp dispatcher at `0x1061470`. Here `ECX = stage - 1`, `RBX` points to the jump-action component and the existing function frame provides the helper-call shadow space and alignment.

| State | Behavior |
| --- | --- |
| Stage 0 or 1 | Original dispatch, unchanged |
| Stage 2, cap 2 | Request trick 2 again |
| Stage 2, other cap | Original request for trick 3 |
| Stage 3, cap 3 | Request trick 3 again |
| Other stage | Original return path |

Component offsets are stage `+0x1DFB`, cap `+0x1DFC`, cooldown `+0x1DF4`. Before repeating, the stage is temporarily set to `requested - 1`. The original helper at `0x1061A40` is called with the existing component and requested trick. This matters because the helper can report success based on the resulting stage equality; leaving the final stage unchanged would allow false positives without fresh input.

If the helper returns false, the stage is restored and dispatch returns normally. If true, the hook rejoins the original success path at `0x1061561`, including the native cooldown call. The hook neither injects input nor replaces the animation logic.

The 99-byte cave is allocated near the hook, populated while writable, changed to execute/read and flushed before installing the relative jump. Jump displacements are range checked. If the jump has been written but a subsequent cache/protection operation fails, the cave is retained and a warning is logged so the jump cannot point into freed memory.

## Lifecycle

The ASI loader loads the mod at process startup. `DllMain` starts a worker, which validates, reads the INI, installs once and exits. There is no runtime polling, telemetry counter, toggle key or live unpatching. Uninstall and setting changes require a full game restart. Manual unloading from a running game is unsupported because the hook remains installed for the process lifetime.

The lightweight Win32 declarations and import library allow a CRT-free LLVM build. Only Kernel32 is imported. No loader or game executable/code extraction is bundled.

## Test boundary

`tests/native_harness.cpp` compiles the production source into a separate DLL and exposes test-only functions. Its restore/toggle functionality is confined to the allocated test image and is not exported by the release ASI. `tests/verify_native_dispatcher.py` reads a local supported game executable and substitutes engine helpers to verify dispatch and emitted machine code. Set `DS2_EXE` and `LLVM_BIN` when running the Python test directly with non-default paths.
