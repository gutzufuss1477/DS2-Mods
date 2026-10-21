# Building DS2 Vehicle Physics Core

Use a full DS2-Mods checkout on Windows x64 with Visual Studio C++ Build Tools
and the Windows SDK installed. Run `scripts/build-msvc.cmd` from this directory.
The shared `tools/native/build-hardened.ps1` discovers the compiler and SDK.

The ASI uses standard DLL CRT startup, static CRT, stack cookies (`/GS`), Control
Flow Guard, ASLR/NX and version resources. No extra runtime installation is needed.
`scripts/build-llvm.cmd` optionally uses clang-cl installed with Visual Studio
or located by `LLVM_BIN`; Microsoft's linker and runtime are still required.

Output: `build/DS2VehicleCore.asi`. Historical release/reference binaries stay
unchanged; a rebuilt binary has a different SHA-256.

From the repository root, build and run the isolated vehicle tests:

```powershell
./tools/native/build-hardened.ps1 -Mod vehicle -Tests
python ./mods/hill-assist-and-speed-boost/tests/run_tests.py
```

The tests cover truck/bike speed rules, slope matching and modification, duplicate
prevention, a guarded indirect RTTI callback, and repeated real DLL load/unload
cycles in an unsupported host. They do not launch DS2 or modify saves.
