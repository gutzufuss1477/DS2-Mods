# Validation

## Test design

The functional validation used two prepared saves containing the same standard order at the delivery point:

- Save A: normal transport, without DHV Magellan transport history.
- Save B: same order after transport using the DHV Magellan.

This isolates the Magellan evaluation path while keeping the delivered order comparable.

## Reverse-engineering result

The affected reward component was traced upstream from the known DS2 reward/Like path.

Observed values:

| Path | Reward components |
| --- | --- |
| Normal | `81 / 200 / 100` |
| Magellan baseline | `65 / 200 / 100` |

Only the first component changed.

The relevant upstream sequence resolves a reward multiplier through a selector lookup. Reconstructed evaluator state:

| State | Selector `[RDX+0x5D]` | Flags `[RDX+0x60]` | Lookup input `ECX` |
| --- | ---: | ---: | ---: |
| Normal | `2` | `0x00` | `2` |
| Magellan | `2` | `0x03` | `0` |

The validated fix changes only the Magellan lookup input from `0` to `2`, after which DS2 performs its original multiplier lookup and reward calculation.

## Functional result

The v0.6 validation candidate produced:

- normal order reward: **81 Likes**
- Magellan order reward with fix: **81 Likes**
- normal final total: **381 Likes**
- Magellan final total with fix: **381 Likes**

Runtime validation log evidence:

```text
PASS selector=0x02; flags=0x00
PATCH ECX 0->2; selector=0x02; flags=0x03
```

The result screen still displays the Magellan delivery indicator. This is expected and intentionally not patched.

## Release boundary

v1.0.0 carries forward the validated v0.6 decision rule unchanged. Release changes are naming, build gating, optional logging and packaging/documentation; the reward-normalization condition itself is unchanged.
