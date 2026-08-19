# Build Validation – v1.0.0

- Target: `x86_64-pc-windows-msvc`
- Compiler: Clang/LLVM 17
- Linker: LLD 17
- Runtime libraries: none (`/nodefaultlib`)
- Architecture: PE32+ x64 DLL/ASI
- Import directory: absent
- ASLR, high-entropy VA and NX compatibility: enabled
- Producer signatures and complete patch windows: passed against the supplied `DS2.exe`
- Q32, Q64 and Double native getter/mutator signatures: passed
- Hook object relocations and control flow: inspected
- Porter Grade Booster Q64 entry-detour compatibility: handled explicitly
- Release change from the validated v0.1.2 binary: embedded version label only; hook and mutation logic retained

- Release ASI SHA-256: `E930E2B6C59A955789A2E5F4D967D9BFC30F57CA2DBA9061039841A1F4E8AB99`
