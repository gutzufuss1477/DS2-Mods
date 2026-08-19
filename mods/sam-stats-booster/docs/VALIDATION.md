# Runtime Validation – v1.0.0

## Test environment

- Supported executable: `DS2.exe` version `1.10.89.0`
- Executable SHA-256: `BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B`
- All three producer hooks reported installed and the ASI reported `State: ACTIVE`.

## Double-backed physical stats

At `Multiplier=20.0`, getter-before / original-mutator / getter-after verification produced exact requested storage:

| Category | Native gain | Requested | Stored |
|---|---:|---:|---:|
| Terrain Traversal | 47.426713 | 948.534258 | 948.534258 |
| Carrying Capacity | 504.587630 | 10091.752601 | 10091.752601 |
| Endurance | 694.077480 | 13881.549591 | 13881.549591 |
| KO Resistance | 274.826270 | 5496.525401 | 5496.525401 |

## Integer-backed stats

At `Multiplier=50.0`, the focused validation run produced:

| Category | Native gain | Requested | Stored |
|---|---:|---:|---:|
| Lung Capacity | 43 | 2150 | 2150 |
| Melee Combat | 2 | 100 | 100 |
| Assault Rifles | 2 | 100 | 100 |

The same run also re-confirmed exact `50.0x` storage on the active Double-backed categories.

## Visual result

The user confirmed that Lung Capacity advanced rapidly to level 5 and that the resulting levels were visible in Sam's Stats menu. Several other categories also reached or approached their visible maximum during testing.

No reliable gameplay-level notification appeared during this test. This does not indicate a failed write: the log proved that the boosted values were committed through the original mutators, and the Stats menu reflected the resulting levels. The mod does not patch notification or HUD code.

## Coverage interpretation

The Q32 test directly exercised Melee Combat and Assault Rifles. Strand Skills and the other firearm categories use the same validated Q32 producer, hook window, getter and mutator; only their mapped record IDs differ. Their record-ID mappings were recovered and statically validated, but not every individual category was deliberately triggered in the focused runtime session.

## Result

`PASS` for release v1.0.0.
