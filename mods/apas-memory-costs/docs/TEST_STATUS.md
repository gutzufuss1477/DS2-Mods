# 3.0.0 validation - 2026-09-25

## Live gameplay verification

The final Early Access implementation was tested on the user's Episode 2 save with:

```ini
[APASMemoryCosts]
Enabled=1
GlobalCost=1

[APASUnlocks]
UnlockAll=1
EarlyAccess=1
```

Confirmed in game:
- APAS is visible in the Ring Device before the normal story unlock.
- The APAS screen opens normally.
- APAS enhancements are displayed and available.
- Memory points can be spent.
- Enhancements can be unlocked successfully.
- The release cost path remains functional with GlobalCost=1.

This closes the previously missing early-game UI/progression verification for the
new feature.

## Automated verification

- Warning-as-error x64 LLVM build.
- Strict INI/default/invalid-input coverage.
- Cost rules across APAS node IDs, including native free/base nodes.
- Native Windows-x64 relay/trampoline execution.
- Mapped supported DS2 image installation for feature combinations.
- Native APAS accounting and activation checks.
- Patch conflict detection and rollback.
- Unsupported-host refusal, repeated DllMain guard and module pinning.
- Exact executable/code-anchor validation for Steam DS2.exe 1.10.89.0.

## Scope

EarlyAccess changes only the APAS Ring Device gate and preserves the Ring Device's
special contextual restriction. UnlockAll uses the native APAS locate/unlock path.
No global story/facility facts are written.

UnlockAll / located APAS nodes may be save-persistent. Back up saves before using
progression-changing options.

The implementation has no recurring gameplay resource scan or permanent polling
worker.
