# Build and package

## Windows

Use LLVM with `clang-cl.exe` and `lld-link.exe`. Run `build.ps1` from this mod's
folder. `-LlvmBin` can point to another LLVM bin directory. The compiler target
is explicitly Windows AMD64; no game executable is required or modified.
The kernel32 import library is generated from `src/kernel32.def`; the runtime
uses no imported Visual C++ CRT. Build intermediates remain under `build/`.
The output is `release/ds2_crafting_unlocks.asi`.

The tracked release INI is already complete. Regeneration requires Python 3.9+
and `python tools/generate_english_config.py`. This command writes the default
INI; do not point it at a player's personalized game configuration.

## Linux build / host tests

Run `bash tools/validate.sh` using clang++, clang-cl, lld-link, objdump and
Python 3.9+. It runs the production-core tests, the original 90-entry regression
fixture and the actual 119-entry release configuration. Core, original-config
and backpack tests run again with AddressSanitizer and UndefinedBehaviorSanitizer.
It then cross-compiles the Windows ASI and verifies its PE structure and bindings.
This is not native Windows or game execution.

Build compiler/options are recorded in `validation/TOOLCHAIN.txt`. The packaged
release is freshly linked; another toolchain/link timestamp can produce a
different binary hash without changing the source. Preserve the supplied
SHA256 manifest for the exact uploaded release artifact.

## Packaging

Run `python scripts/package_release.py` after building and validating. This
refreshes release file checksums and creates the standalone ZIP under `dist/`.
Only ASI, INI, user documentation and checksums are included. No import library,
object file, development log, source dump, loader or game asset is in that ZIP.

The Git package adds `mods/crafting-unlocks/` to the existing DS2-Mods repository.
It does not integrate the mod into the all-in-one installer. Do not replace the
repository's README or installer files with an old full-repository snapshot.
