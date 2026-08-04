# DS2 Update Checklist

Use this checklist after every DEATH STRANDING 2 game update.

## 1. Record the new executable

- Record the visible game version.
- Calculate the SHA-256 hash of `DS2.exe`.
- Keep the previous executable metadata for comparison.
- Do not mark the existing build as compatible automatically.

## 2. Static validation

Review every build-specific value in `src/PickupTruckHillAssist.cpp`, including:

- streaming-manager global offset;
- listener registration function;
- RTTI and type-name access;
- load-group structure;
- `DSPhysicsVehicleResource` layout;
- slope-table pointer and count offsets;
- exact slope-table signatures;
- expected late streaming-group window.

## 3. Build

Run `BUILD_RELEASE.cmd` from an x64 Native Tools Command Prompt for Visual Studio.

Record:

- compiler and linker versions;
- build command;
- output SHA-256;
- warnings and errors.

## 4. Safe runtime validation

Before normal use:

- retain the current working release;
- test in a controlled save and location;
- verify that the ASI loads once;
- verify that the expected streaming window is reached;
- verify that exactly the intended slope tables are patched;
- confirm that no unrelated resources are modified;
- inspect the full runtime log;
- test driving, saving, loading and restarting.

## 5. Release metadata

Update together:

- version number;
- supported game build;
- `DS2.exe` SHA-256;
- ASI SHA-256;
- README;
- release notes;
- Nexus description;
- GitHub tag and release.

Do not publish compatibility for a new build until the runtime validation passes.
