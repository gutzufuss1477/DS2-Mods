# Build and package

## Windows

Use LLVM with `clang-cl.exe` and `lld-link.exe`. Run `build.ps1` from this mod's
folder. `-LlvmBin` can point to another LLVM bin directory. The compiler target
is explicitly Windows AMD64; no game executable is required or modified.
The kernel32 import library is generated from `src/kernel32.def`; the runtime
uses no imported Visual C++ CRT. Build intermediates remain under `build/`.
The output is `release/ds2_crafting_unlocks.asi`.

For 1.6.0, run `scripts/build_tests.cmd` and `scripts/test_atlas.ps1` after
building. They exercise production policy and machine-code thunks in separate
Windows test processes, including ATLAS recipe toggles and saved identities.
Run `python tests/verify_build.py` for the PE/INI checks and
`python tests/verify_integration.py` for all 91 exact native signatures and
hook overlap checks. The ATLAS tests and integration check need Python with
Capstone and the verified local game EXE at `analysis/DS2.exe` in the workspace.
The runtime is compiled into the same ASI from `src/atlas/`; it has no second
DllMain, worker, INI or log. The standalone 0.2.4 native logic is retained.

The tracked release INI is already complete. Regeneration requires Python 3.9+
and `python tools/generate_english_config.py`. This command writes the default
INI; do not point it at a player's personalized game configuration.

## Linux build / host tests

Run `bash tools/validate.sh` using clang++, clang-cl, lld-link, objdump and
Python 3.9+. It runs the production-core tests, the original 90-entry regression
fixture and the actual 132-entry release configuration. Core, original-config
and backpack tests run again with AddressSanitizer and UndefinedBehaviorSanitizer.
It then cross-compiles the Windows ASI and verifies its PE structure and bindings.
This is not native Windows or game execution.

Build compiler/options are recorded in `validation/TOOLCHAIN.txt`. The packaged
release is freshly linked; another toolchain/link timestamp can produce a
different binary hash without changing the source. Preserve the supplied
SHA256 manifest for the exact uploaded release artifact.

## Packaging

Run `python scripts/package_release.py` after building and validating. This
refreshes release file checksums and creates the standalone ZIP under `release/`.
Only ASI, INI, user documentation and checksums are included. No import library,
object file, development log, source dump, loader or game asset is in that ZIP.

The Git package adds `mods/crafting-unlocks/` to the existing DS2-Mods repository.
It does not integrate the mod into the all-in-one installer. Do not replace the
repository's README or installer files with an old full-repository snapshot.
