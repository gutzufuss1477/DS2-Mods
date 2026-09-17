# APAS Unlock All - Technical Notes

## Purpose

The optional Unlock All build extends the normal APAS Memory Costs mod without
globally altering story, facility or Porter Grade progression.

It bypasses only the APAS-specific prerequisite branch and then lets the game's native
APAS unlock routine create/locate the Enhancement normally.

## Confirmed unlock path

Static analysis identified the APAS unlock/update path around:

```text
RVA 0xBE39C9
```

Native bytes at the patch point:

```text
33 DB                xor ebx, ebx
48 83 C7 2C          add rdi, 2Ch
90                   nop
8B 37                mov esi, [rdi]
```

The surrounding native loop evaluates APAS `DevelopmentCondition` values before
reaching the game's own locate/unlock call.

Native APAS locate/unlock routine:

```text
RVA 0xBE1640
```

Native call path reached after the prerequisite checks:

```text
RVA 0xBE39EE
```

The optional build replaces only the first two bytes at `0xBE39C9`:

```text
33 DB
```

with:

```text
EB 23
```

This short jump skips the APAS prerequisite checks and lands on the existing native
unlock path.

## Safety gate

There is intentionally no global game-version check.

However, the code patch is fail-closed at the patch site:

1. the memory region must be readable;
2. the exact expected 9-byte native signature must match;
3. only then are the two patch bytes written.

If the expected code is different, Unlock All is not installed.

## Why this approach was chosen

The executable contains APAS-specific fields and functions including:

- `DevConditionFact`
- `DevelopmentCondition`
- `IsApasEnhancementLocated`
- `GetApasEnhancementLv`
- `DebugActivateApasEnhancement`
- `NotifyUnlockedApasEnhancementsForTelemetry`
- `MissionClearOrFriendShipUILevel1..5`
- `FriendShipUILevel1..5`
- `UnlockedIdListBeforeEnding`
- `UnlockedIdListBeforePlatinumTrophy`

Rather than changing those underlying global game facts, the optional build bypasses
only the APAS updater's condition branch.

This keeps the modification scoped to APAS Enhancements.

## Cost logic remains unchanged

The existing APAS Memory Costs runtime path remains:

```text
DS2.exe + 0x623E5D0     -> APAS manager
manager + 0x30 + i*8    -> APAS entry
entry + 0x70            -> DSApasEnhancementResource*
resource + 0x24         -> EnhancementPoint
```

The v1.0.3 low-overhead worker behaviour is preserved.

## Persistence warning

Because the bypass feeds into the game's native APAS unlock routine, the resulting
unlock state may be saved by the game.

Removing the mod should not be expected to relock Enhancements already saved as unlocked.
