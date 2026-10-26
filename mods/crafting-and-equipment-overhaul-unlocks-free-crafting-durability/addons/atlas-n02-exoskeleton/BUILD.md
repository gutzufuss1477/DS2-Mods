# Building ATLAS N02 Exoskeleton Addon 1.0.1

Use Windows x64, LLVM clang-cl/lld-link, Python 3 and Windows SDK headers.
Provision the exact local inputs in `config/local-inputs.json`, then run
`./build.ps1` and `./test-runtime.ps1`.
The build verifies local inputs before compilation. `/timestamp:0` gives a
reproducible PE; the public source rebuild matches the packaged ASI byte for byte.

Game-derived model/texture arrays and menu pixels remain local and ignored by Git.
A clean clone does not contain those assets or act as a complete extractor.
Compiled binaries, caches and private runtime captures are not committed either.

`validation/v1.0.1/` records runtime and real-DLL combination checks. The native
material tests simulate renderer changes, 600 texture replacements, shared use,
invalid owner/table rejection, and inactive-equipment cleanup. They do not launch
the game or prove visual results. Final release smoke testing remains explicit.

The boot peer ABI is additive; neither the host ABI nor its router changed.
The host's normal content updates remain compatible while the frozen contracts
stay stable. Native calls are guarded by DS2 executable and signature validation.

To repeat the DLL checks after building both addons, run
`./test-combination.ps1 -HostAsi "C:/path/to/ds2_crafting_unlocks.asi"`.
This runs five cases (none, each addon alone, both load orders). Optional
`-LegacyBoots` and `-IncompatibleBoots` paths add the four historical cases;
these fixture binaries are not stored in Git. Results go to `build/combination-checks/`.
The host file is mapped only to query its extension API; the game is not started.
