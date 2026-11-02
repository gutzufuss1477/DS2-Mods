"""Offline runtime tests; maps DS2 without executing its entry point or imports."""
import argparse
import ctypes as C
from ctypes import wintypes as W
import hashlib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--game', required=True, type=Path)
p.add_argument('--build', default='build/rebuild')
p.add_argument('--dll', type=Path, help='Use the fixture built by the central suite validator.')
args = p.parse_args()
assert hashlib.sha256(args.game.read_bytes()).hexdigest() == 'bf3d1c665545930bc850d8f5df486f7395885bb729d4fd408fdb03390de0765b'
out = args.dll.resolve().parent if args.dll else ROOT / args.build
k = C.WinDLL('kernel32', use_last_error=True)
k.LoadLibraryExW.argtypes = [W.LPCWSTR, W.HANDLE, W.DWORD]
k.LoadLibraryExW.restype = W.HMODULE
k.FreeLibrary.argtypes = [W.HMODULE]
mapped = k.LoadLibraryExW(str(args.game.resolve()), None, 1)
if not mapped:
    raise C.WinError(C.get_last_error())
tests = C.WinDLL(str(args.dll.resolve() if args.dll else out / 'tests/apas_rebuild_tests.dll'))
f = tests.Configuration
f.argtypes = [W.LPCWSTR, W.DWORD]
f.restype = W.DWORD
cases = [('', 9), ('[APASMemoryCosts]\nGlobalCost=0\n', 1),
         ('[APASMemoryCosts]\nGlobalCost=25\n', 201),
         ('[APASMemoryCosts]\nGlobalCost=1000000\n', 8000001)]
for flags in range(8):
    cases.append((f'[APASMemoryCosts]\nEnabled={flags & 1}\n[APASUnlocks]\nUnlockAll={(flags >> 1) & 1}\nEarlyAccess={(flags >> 2) & 1}\n', 8 + flags))
for bad in ['-1', '1000001', '4294967296', '1garbage', '', '0' * 64 + '1']:
    cases.append(('[APASMemoryCosts]\nGlobalCost=' + bad + '\n', 0xffffffff))
for section, key in [('APASMemoryCosts', 'Enabled'), ('APASUnlocks', 'UnlockAll'), ('APASUnlocks', 'EarlyAccess')]:
    for bad in ['2', '-1', 'true', '']:
        cases.append((f'[{section}]\n{key}={bad}\n', 0xffffffff))
try:
    for index, (text, expected) in enumerate(cases):
        ini = out / 'test.ini'
        ini.write_text(text, encoding='utf-16')
        result = f(str(ini), expected)
        assert result == 0, ('Configuration', index, 'C++ line', result)
    assert f(str(ROOT / 'ds2_apas_memory_costs.ini'), 9) == 0
    print(f'Configuration: PASS ({len(cases)} cases + packaged defaults)', flush=True)
    for name in ['MemoryReadProtections', 'CostRules', 'MenuAndReload', 'Installation', 'FinalizationFailures', 'InactiveCallback', 'NativeAccounting', 'ConstructorAbi']:
        f = getattr(tests, name)
        f.argtypes = [W.HMODULE]
        f.restype = W.DWORD
        result = f(mapped)
        assert result == 0, (name, 'C++ line', result)
        print(name + ': PASS', flush=True)
    print('No game startup, save access, external imports or in-game validation performed.', flush=True)
finally:
    k.FreeLibrary(mapped)
