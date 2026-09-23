# Build and verify the unified candidate

Requirements: Windows x64, LLVM (`clang-cl.exe`, `lld-link.exe`), Python 3.
The ASI uses only KERNEL32 imports, without an external C/C++ runtime.
`LLVM_BIN` can override `C:\Program Files\LLVM\bin`.

From the mod directory:

```powershell
.\scripts\build.ps1
.\scripts\build.ps1 -Tests
python .\tests\run_tests.py 'C:\path\to\DS2.exe'
python .\tests\test_loader_lifetime.py
python .\scripts\package_release.py
```

The mapped-image tests require the exact supported executable, checked by SHA-256
before loading it with `DONT_RESOLVE_DLL_REFERENCES`. They do not execute the game
entry point, connect to Steam, or read/write saves. Test mutations remain private
to the Python harness process. The separate loader test loads the actual release
ASI into an unsupported host and verifies refusal, repeated-attach protection and
module pinning.

Build output is under ignored `build/`. The normal build has x64 unwind metadata,
relocations, ASLR/NX flags and reproducible linking. The package script emits one
ZIP with one ASI, one INI and one README, plus SHA-256 checksums. It verifies the ZIP
contents against the inputs and rejects extra active release ZIPs.

`src/target.h` contains reviewed anchors from Steam 1.10.89.0. The extraction helper
`tools/make_target_header.py` refuses any other SHA-256; it is not an automatic
compatibility updater. Porting requires new reverse engineering and testing.

The public source is `src/apas.cpp`. The old duplicate main/unlock source files have
been removed; one INI controls both features in the same binary.
