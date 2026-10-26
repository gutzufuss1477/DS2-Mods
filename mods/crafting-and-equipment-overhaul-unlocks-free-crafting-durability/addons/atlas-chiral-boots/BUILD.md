# Building ATLAS Chiral Boots Addon 1.0.1

Use Windows x64, LLVM clang-cl/lld-link, Python 3 and Windows SDK headers.
Provision the exact local inputs in `config/local-inputs.json`, then run
`./build-api.ps1` and `./test-runtime.ps1`.
The build verifies local inputs before compilation. `/timestamp:0` gives a
reproducible PE; the public source rebuild matches the packaged ASI byte for byte.

Game-derived model/texture arrays and menu pixels remain local and ignored by Git.
A clean clone does not contain those assets or act as a complete extractor.
Compiled binaries, caches and private runtime captures are not committed either.

`validation/v1.0.1/` records runtime and real-DLL combination checks. The native
material tests simulate renderer changes, 600 texture replacements, shared use,
invalid owner/table rejection, and inactive-equipment cleanup. They do not launch
the game or prove visual results. The final 1.0.1 smoke test was confirmed by the user on 2026-10-26; see the validation record.

The boot peer ABI is additive; neither the host ABI nor its router changed.
The host's normal content updates remain compatible while the frozen contracts
stay stable. Native calls are guarded by DS2 executable and signature validation.
