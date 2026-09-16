# Technical Notes

## Confirmed APAS runtime path

Static analysis and runtime testing established:

```text
DS2.exe + 0x623E5D0
    -> global APAS manager pointer

manager + 0x30 + index * 8
    -> APAS entry pointer

entry + 0x70
    -> DSApasEnhancementResource*

resource + 0x24
    -> EnhancementPoint / APAS Memory cost
```

The APAS manager contains `0x37` entry slots.

## Why `resource + 0x24` is the real APAS cost field

The APAS accounting function identified at RVA `0xBE3D00` walks active APAS entries,
resolves `entry + 0x70`, and adds `[resource + 0x24]` to occupied APAS Memory.

The activation path independently reads the same field and compares it against:

```text
total APAS capacity - currently occupied APAS Memory
```

The field therefore participates in both:

1. occupied-memory accounting;
2. the activation-capacity check.

It is not merely a UI display value.

## Confirmed reflection/resource terminology

Static analysis identified:

- `DSApasEnhancementResource`
- `DSApasEnhancementResources`
- `EnhancementPoint`
- `EnhancementCategory`
- `EnhancementId`
- `InitialMemoryCapacity`
- `AdditionalMemoryCapacityByHouseholdFriendshipLevel`
- `apas_enhancement_array`

## v1.0.3 implementation

Constants:

```text
Manager global RVA : 0x623E5D0
Entry count        : 0x37
Entry array offset : +0x30
Resource pointer   : entry +0x70
EnhancementPoint   : resource +0x24
```

The worker:

```text
startup
  -> Sleep(3000 ms)
  -> PatchPass()

if fewer than 40 resources are available:
  -> Sleep(5000 ms)
  -> retry

once 40+ resources are available:
  -> check once per second
  -> require five stable passes with no new writes
  -> terminate worker thread
```

The patch pass validates the complete manager entry-array region once instead of calling
`VirtualQuery` on every array slot.

For each populated slot it only validates:

- the small `entry +0x70` region needed to read the resource pointer;
- the four-byte writable `resource +0x24` cost field.

Writes use an atomic compare/exchange.

## Performance rationale

Earlier builds continued to walk the APAS resource table throughout gameplay.
User feedback reported a severe FPS reduction with that implementation.

v1.0.3 removes permanent gameplay polling. Once the APAS table has reached its normal
populated/stable state, the worker returns and no further APAS scanning occurs.

## Version policy

There is deliberately no hard PE timestamp, image-size, or signature gate.

The offsets above were discovered and functionally tested on:

```text
Steam DS2.exe 1.10.89.0
```

After a future game update, validate the runtime path before assuming the constants
remain correct.
