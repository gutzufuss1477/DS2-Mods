# Research / Handover Notes

## Goal

Make APAS Enhancement memory requirements configurable through an INI file,
with the primary use case being `GlobalCost=1`.

## Confirmed result

Functional test confirmed that setting `resource + 0x24` to `1` causes all loaded
APAS Enhancements to cost one APAS Memory point.

A runtime test observed 46 APAS resources and successfully brought all discovered
resources to the configured target without invalid entries or failed writes.

## Static-analysis milestones

1. APAS-related reflection/debug strings identified.
2. `DSApasEnhancementResource` and `EnhancementPoint` located.
3. APAS UI / manager consumers isolated.
4. APAS accounting function identified.
5. Resource relationship confirmed:
   `entry + 0x70 -> DSApasEnhancementResource*`.
6. `resource + 0x20` identified as `EnhancementId`.
7. `resource + 0x24` identified as `EnhancementPoint`.
8. Activation logic independently confirmed that `+0x24` is the capacity cost field.
9. First alpha successfully patched all discovered resources to cost `1`.

## Maintenance priority after a game update

If the mod stops working after an update, check in this order:

1. global APAS manager RVA (`0x623E5D0`);
2. entry array offset (`+0x30`) and entry count (`0x37`);
3. resource pointer (`entry +0x70`);
4. `EnhancementPoint` (`resource +0x24`).

The resource-layout offsets are likely more stable than the absolute manager RVA.
