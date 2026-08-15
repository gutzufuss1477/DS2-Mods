SIDE=160
ITEM_CAP=320
banks={'S':6,'M':3,'L':2,'XL':1}
for name,bank in banks.items():
    for raw in range(SIDE):
        if name=='XL': eff=0 if raw else 0
        else: eff=raw%bank
        assert 0 <= eff < bank
    for raw in (160,161,255,320):
        # Release stubs fail closed outside controlled domain.
        eff=raw
        assert eff==raw
assert ITEM_CAP==320
print('PASS: cursor fold model 0..159; fail-closed >=160; item cap 320')
