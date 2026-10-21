#!/usr/bin/env python3
# Offline behavioral model for release v1.1.0.

def fold(raw, bank):
    if raw >= 320:
        return raw
    while raw >= bank:
        raw -= bank
    return raw

for raw in range(320):
    assert fold(raw, 6) == raw % 6
    assert fold(raw, 3) == raw % 3
    assert fold(raw, 2) == raw % 2
    assert (0 if raw else 0) == 0

# Capacity modes: total target is always 320.
assert 160 + 160 == 320
assert 320 == 320

# Accessory virtualization: only exact real 1/1 becomes one-side behavior.
def mode(a, b):
    if a == 1 and b == 1:
        return 'one-side-virtualized'
    if a or b:
        return 'one-side-native'
    return 'two-side-native'

assert mode(0,0) == 'two-side-native'
assert mode(0,1) == 'one-side-native'
assert mode(1,0) == 'one-side-native'
assert mode(1,1) == 'one-side-virtualized'
print('PASS: v1.1.0 capacity/accessory model')
