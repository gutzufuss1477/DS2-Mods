# DS2 Update Checklist

Use this checklist after every game update.

## 1. Preserve the working release

- Keep the currently working ASI, INI files and logs.
- Record the old game version and `DS2.exe` SHA-256.
- Do not overwrite the working release before validation.

## 2. Record the new build

- Record the visible DS2 version.
- Calculate the new `DS2.exe` SHA-256.
- Compare executable sections and relevant resource-loading code.

## 3. Revalidate build-specific assumptions

Review the source for:

- streaming-manager global offset;
- listener registration entry point;
- streamed group layout;
- type and RTTI access;
- vehicle and bike resource layouts;
- slope-friction table signatures;
- normal-speed fields at `+0x40` and `+0x44`;
- boost top-speed field at `+0x94`;
- streaming window groups 4300–4800;
- expected 14 Hill Assist entries;
- expected two pickup profiles and one bike profile.

## 4. Build and inspect

- Build with `scripts\build-llvm.cmd`.
- Record compiler and linker versions.
- Calculate the new ASI SHA-256.
- Review imports and PE sections.
- Confirm that no new dependencies were introduced.

## 5. Controlled runtime test

- Start with both modules disabled.
- Enable only Hill Assist and verify the log.
- Enable only Vehicle Speed Boost and verify the log.
- Enable both modules and verify that only one listener is registered.
- Test pickup and Tri-Cruiser normal driving.
- Test pickup and Tri-Cruiser boost.
- Test slopes, braking, loading, unloading, saving and reloading.
- Restart the game and load the save again.
- Confirm that no unrelated vehicle resources are changed.

## 6. Release update

Update together:

- core version;
- supported game build;
- `DS2.exe` hash;
- ASI hash;
- README;
- build information;
- release notes;
- Nexus package;
- GitHub tag or commit.

Do not declare compatibility until all runtime checks pass.
