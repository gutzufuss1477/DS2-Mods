# Crafting and Equipment Overhaul 1.10.0

Use Windows x64, Python 3 and LLVM (`clang-cl`/`lld-link`). Policy tests additionally
need the MSVC/Windows SDK headers and runtime. The supported game build is
1.10.89.0. No game process is needed to compile.

Game-derived build inputs are deliberately absent from Git. `config/local-inputs.json`
lists every required file, size and SHA-256. The eight inputs are six texture
payloads and two generated Story resource/link headers. They must be provisioned
locally from the matching owned game data and resource transformation; these are
not arbitrary substitute textures or an addon download. This source checkout
alone does not include an end-to-end native asset extractor. The header payload
is the same model metadata used for both standalone and optional-addon loading.

Run `python tools/check_local_inputs.py` to list missing/mismatched inputs. Once
provisioned, run `./build.ps1`; it writes the ASI and, if absent, the default INI
to `release/`. Existing output INIs are retained. `-LlvmBin` and `-OutputDirectory`
can select other paths. The script verifies inputs and the optional-addon API
before compiling; the addon does not need to be installed to build the main mod.

`./scripts/test_release.ps1` runs the core, equipment, coffin, cosmetics, persistence
and repeat-fabrication regressions. These are offline tests, not game automation.
`python scripts/smoke_loader.py --pcc` tests DLL loading in disposable Python
processes. `tests/verify_integration.py --version 1.10.0 --pcc` reads the exact
game executable using `DS2_TEST_EXE`; it does not modify it.

`tests/story_independence_windows_test.cpp` exercises the production cache builder,
graph transformation and hook installer in an isolated test process. It needs
local source/expected graph fixtures and the matching game archives. Its default
read-only game root is the standard Steam path declared in the test. Pass
`standalone <source-graph> <expected-graph>` or `addon <source-graph> <expected-graph>`.
The addon case additionally checks the unchanged addon binary/cache. Outputs and
the simulated IAT are owned by the test process, never by DS2.

`tools/enforce_addon_api_v1.py` pins eight interface/integration files after LF
normalization. Optionally pass `--addon-zip <published-1.0.0-zip>` to verify the
original published binary too. The checked-in graph delta programs contain
structural registrations; complete game graphs, caches, textures and personal
appearance metadata must remain outside Git.

The optional Chiral Boots 1.0.0 source lives in the sibling `addons` directory.
Its original mesh/texture/cache inputs are likewise not redistributed. Published
Nexus downloads remain the end-user installation path; the prepared 1.10.0
package has no Nexus file ID until it is uploaded.
