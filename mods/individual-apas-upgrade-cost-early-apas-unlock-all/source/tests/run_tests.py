"""Tests run in a separate Python process; DS2 itself is never launched."""
import ctypes as C
from ctypes import wintypes as W
import hashlib
from pathlib import Path
import sys

root = Path(__file__).resolve().parents[1]
exe = Path(sys.argv[1]).resolve()
expected_sha = 'BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B'
assert hashlib.sha256(exe.read_bytes()).hexdigest().upper() == expected_sha
k32 = C.WinDLL('kernel32', use_last_error=True)
k32.LoadLibraryExW.argtypes = [W.LPCWSTR, W.HANDLE, W.DWORD]
k32.LoadLibraryExW.restype = W.HMODULE
k32.FreeLibrary.argtypes = [W.HMODULE]
module = k32.LoadLibraryExW(str(exe), None, 1)  # DONT_RESOLVE_DLL_REFERENCES
if not module:
    raise C.WinError(C.get_last_error())
tests = C.WinDLL(str(root / 'build' / 'apas_tests.dll'))
config_test = tests.TestConfiguration
config_test.argtypes = [W.LPCWSTR, W.DWORD]
config_test.restype = W.DWORD
ini = root / 'build' / 'test-settings.ini'
cases = [
    ('', 9),
    ('[APASMemoryCosts]\nEnabled=0\n[APASUnlocks]\nUnlockAll=0\nEarlyAccess=0\n', 8),
    ('[APASMemoryCosts]\nEnabled=0\n[APASUnlocks]\nUnlockAll=1\n', 10),
    ('[APASMemoryCosts]\nGlobalCost=0\n[APASUnlocks]\nUnlockAll=1\nEarlyAccess=1\n', 7),
    ('[APASMemoryCosts]\nGlobalCost=25\n', 201),
    ('[APASMemoryCosts]\nGlobalCost=1000000\n', 8000001),
    ('[APASMemoryCosts]\nGlobalCost=-1\n', 0xffffffff),
    ('[APASMemoryCosts]\nGlobalCost=1000001\n', 0xffffffff),
    ('[APASMemoryCosts]\nGlobalCost=4294967296\n', 0xffffffff),
    ('[APASMemoryCosts]\nGlobalCost=1garbage\n', 0xffffffff),
    ('[APASMemoryCosts]\nEnabled=2\n', 0xffffffff),
    ('[APASUnlocks]\nUnlockAll=-1\n', 0xffffffff),
    ('[APASUnlocks]\nUnlockAll=true\n', 0xffffffff),
    ('[APASUnlocks]\nEarlyAccess=-1\n', 0xffffffff),
    ('[APASUnlocks]\nEarlyAccess=true\n', 0xffffffff),
    ('[APASUnlocks]\nUnlockAll=\n', 0xffffffff),
    ('[APASMemoryCosts]\nGlobalCost=' + '0' * 64 + '1\n', 0xffffffff),
]
try:
    for i, (text, expected) in enumerate(cases):
        ini.write_text(text, encoding='utf-16')
        result = config_test(str(ini), expected)
        assert result == 0, (i, result)
    print(f'INI parsing/defaults/limits/invalid settings: PASS ({len(cases)} cases)', flush=True)
    for path in [root / 'ds2_apas_memory_costs.ini', root / 'release/ds2_apas_memory_costs.ini']:
        assert config_test(str(path), 9) == 0, f'Packaged defaults differ: {path}'
    print('Source and packaged INIs: Enabled=1, GlobalCost=1, UnlockAll=0, EarlyAccess=0: PASS', flush=True)
    for name in ['TestCostRules', 'TestEarlyAccessState', 'TestRelay', 'TestRingRelay', 'TestMappedInstall', 'TestNativeCosts']:
        function = getattr(tests, name)
        function.argtypes = [W.HMODULE]
        function.restype = W.DWORD
        result = function(module)
        assert result == 0, (name, result)
        print(f'{name}: PASS', flush=True)
    print('All offline/runtime harness checks passed. In-game new-save/UI/FPS validation remains separate.', flush=True)
finally:
    k32.FreeLibrary(module)
