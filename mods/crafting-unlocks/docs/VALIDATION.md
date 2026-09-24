# Validation scope - 1.0.0

## User-reported game results

The user reported the expanded normal fabrication menu at the first terminal
in Episode 2. They then specifically reported crafting the Battle Guitar and
having it in inventory. After testing the backpack extension, they reported
all backpack equipment, rain covers and charms unlocked.

These reports belong to the candidate builds in this conversation. No fresh
backpack log was uploaded with the final confirmation. The earlier normal-menu
log records native=28, added=83, total=111. It is not a backpack log and is not
used as proof of backpack equipment application.

Confirmed scope: early recipe visibility and the stated Battle Guitar
fabrication/inventory result; user-reported visibility/unlock of the backpack
categories. Not explicitly established: equipping every backpack item, exact
resource deduction, persistence after a full save/restart, every recipe in every
facility, all later story rewards/notifications, every charm effect, or other
mods/storefronts.

## Promotion to 1.0.0

All runtime source files were compared with the tested 0.2.0-alpha source.
Only the version string in src/mod.cpp changed. core.hpp, backpack.hpp, the
119 configuration keys/default values, native hook RVAs and all safety checks
remain unchanged. Source equality after normalizing that one version string
is asserted in `validation/RELEASE_AUDIT.json`.

The release binary was cross-built, not just renamed. It has not independently
been loaded in a Windows DS2 process in the build environment. Rebuilding the
unmodified 0.2.0 source with this toolchain reproduces the tested candidate's
PE section contents; the COFF timestamp differs. Shortening the version banner
can change instruction/data placement in the 1.0.0 binary. Therefore the release
is not advertised as byte-identical to the tested binary.

## Automated tests rerun for release

- Core policy/layout/selection/SHA: 335426 assertions.
- Original 90-entry English configuration fixture: 9295 assertions.
- Actual 119-entry INI and both route policies: 13675 assertions.
- The same three suites also run with ASan and UBSan.
- PE machine/DLL flags, imports, unwind/relocation directories and hook bindings.
- Source-only version delta, default-value equality, package member allowlist,
  and checksum/ZIP integrity checks.

The standard runs total 358396 assertions. Sanitized repetitions are not counted
as additional distinct coverage. Native engine execution and OS patch-thread
interleavings are not simulated as proven by these tests.

Exact outputs: docs/CORE_TESTS.txt, CONFIG_R1_TESTS.txt, BACKPACK_TESTS.txt,
CORE_SANITIZERS.txt, CONFIG_SANITIZERS.txt, BACKPACK_SANITIZERS.txt and
BUILD_VALIDATION.json. Packaging results are in validation/RELEASE_AUDIT.json.

The optional APPLY_AND_PUSH.ps1 import helper is supplied for review; it was not
executed in Windows PowerShell in this environment. No remote push, release
upload or installer rebuild was performed.
