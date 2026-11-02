# APAS 4.1.0-preview.1 — development candidate

This is an isolated follow-up to the October APAS 4.0.0 rewrite. It is not the
published APAS release and is not enabled in the Mod Suite catalog. Acceptance
by an antivirus product or Nexus has not been established by the functional tests.

## Functional corrections

- Initialization uses the loader's exported `InitializeASI` callback instead of
  creating a worker from `DllMain`. The module is pinned **before** the worker is
  created; concurrent initialization calls are serialized with Windows InitOnce.
- A failure while restoring page permissions or flushing the instruction cache
  now rolls back installed hooks too. Only this plugin's expected bytes/pointers
  are replaced. Recovery failures are logged and the feature gate stays disabled.
- The constructor callback leaves costs unchanged until installation completes.
  A callback already in flight after a failed installation remains a pass-through.

Costs, Early Access, Unlock All, INI sections, keys and defaults are retained.
The existing rewrite's static constructor bridge, native node creation, guarded
reads and exact supported-function checks are retained. No executable relay is
allocated by the shipped plugin.

## Compatibility and limits

- Windows x64, Steam DS2.exe 1.10.89.0, reference SHA-256
  `bf3d1c665545930bc850d8f5df486f7395885bb729d4fd408fdb03390de0765b`.
- Requires an ASI loader that calls `InitializeASI` after loading plugins.
  Ultimate ASI Loader v9.7.2, supplied by the current suite, provides this entry
  point and defaults to `DontLoadFromDllMain=1`. LoadLibrary-only loaders will
  load this plugin without activating it.
- No live-game test of this candidate has been performed. The earlier Episode 2
  test concerned a different 4.0.0 build.
- Rollback is best effort if Windows continues to reject memory-protection
  changes, or another mod alters an installed hook. The log then requires a
  restart. The two-hook installation is not a global stop-the-world transaction;
  offline tests do not establish every possible startup thread interleaving.
- Unlock All can persist unlocked nodes in saves, as before. Back up the save
  before an in-game test. Removing the plugin does not undo saved unlocks.

## Build and validate

Use Python 3, Visual Studio 2022 C++ Build Tools, Windows SDK and LLVM's
`clang-cl` assembler. The local test executable is not distributed.

```powershell
python scripts/build.py --game C:/local/DS2.exe --output D:/APAS-candidate
python tests/run.py --game C:/local/DS2.exe --dll D:/APAS-candidate/tests/apas_rebuild_tests.dll
python tests/lifecycle.py --dll D:/APAS-candidate/apas/ds2_apas_memory_costs.asi --output D:/APAS-lifecycle-new
```

The lifecycle test needs a fresh output directory. It runs the real plugin in a
disposable Python host and verifies that a non-DS2 process is refused.
The other tests map the reference executable without running its entry point or
resolving game imports. They use isolated fixture data; no live installation or
save is accessed.

Compiler protections: standard static CRT, GS, CFG, ASLR, high-entropy VA and NX.
The build checks emitted PE metadata and bridge CFG targets, rejects permanent
RWX sections and rejects executable-allocation imports in the shipped ASI.
These checks are build properties, not a malware-free certification.

## Provenance

Baseline: local `mod-manager-v1.8.0/mods/apas-rebuilt`, corrected memory-read
candidate from October. Its reproduced ASI SHA-256 was
`5191d9a1a39169d36e5dca613a83e985143650895ed714489cdb032bf3217a5a`.
Baseline `src/apas.cpp` SHA-256:
`dec521ccb0e70026997ab123ca04b5a8bfd52031e76cc756ab66a22eafcb9fdc`.
Constructor assembly and target hashes are unchanged. Build helpers derive from
the existing suite's native build validator; source and artifact hashes are
recorded in the generated manifests.

References:

- [Ultimate ASI Loader v9.7.2 implementation](https://github.com/ThirteenAG/Ultimate-ASI-Loader/blob/v9.7.2/source/dllmain.cpp)
- [Microsoft DLL best practices](https://learn.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-best-practices)
