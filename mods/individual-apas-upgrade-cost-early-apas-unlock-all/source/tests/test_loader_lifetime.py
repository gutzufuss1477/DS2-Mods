"""Exercise the exact release ASI in an isolated, unsupported host process."""
import ctypes as C
from ctypes import wintypes as W
from pathlib import Path
import shutil
import sys
import time

root = Path(__file__).resolve().parents[1]

stage = root / 'build' / ('loader-lifetime-' + str(time.time_ns()))
stage.mkdir()
asi = stage / 'ds2_apas_memory_costs.asi'
shutil.copy2(root / 'build' / asi.name, asi)
shutil.copy2(root / 'ds2_apas_memory_costs.ini', stage / 'ds2_apas_memory_costs.ini')
log = stage / 'ds2_apas_memory_costs.log'
k = C.WinDLL('kernel32', use_last_error=True)
k.LoadLibraryW.argtypes = [W.LPCWSTR]
k.LoadLibraryW.restype = W.HMODULE
k.GetModuleHandleW.argtypes = [W.LPCWSTR]
k.GetModuleHandleW.restype = W.HMODULE
k.FreeLibrary.argtypes = [W.HMODULE]
module = k.LoadLibraryW(str(asi))
assert module, C.get_last_error()
deadline = time.monotonic() + 5
while time.monotonic() < deadline:
    if log.exists() and b'UNSUPPORTED_OR_CONFLICT' in log.read_bytes():
        break
    time.sleep(0.02)
else:
    raise AssertionError('Worker did not reject unsupported host within five seconds')
before, stamp = log.read_bytes(), log.stat().st_mtime_ns
# Exercise normal loader references. The PE entry now belongs to the CRT and
# must not be called manually as if it were the application's DllMain.
for _ in range(20):
    repeated = k.LoadLibraryW(str(asi))
    assert repeated == module
    assert k.FreeLibrary(repeated)
time.sleep(0.1)
assert log.read_bytes() == before and log.stat().st_mtime_ns == stamp, 'Duplicate initialization rewrote log'
assert k.FreeLibrary(module)
assert k.GetModuleHandleW(str(asi)) == module, 'Callback module was not retained for process lifetime'
assert b'UnlockAll=0' in before and b'EarlyAccess=0' in before
print('Actual ASI: unsupported-host refusal, default UnlockAll=0/EarlyAccess=0, repeated load/unload references, module pin: PASS')
