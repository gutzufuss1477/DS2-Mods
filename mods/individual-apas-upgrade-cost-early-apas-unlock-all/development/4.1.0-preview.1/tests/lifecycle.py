"""Exercise the real ASI export in a disposable non-game process."""
import argparse
from concurrent.futures import ThreadPoolExecutor
import ctypes as C
from ctypes import wintypes as W
from pathlib import Path
import shutil
import time

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--dll', type=Path, required=True)
p.add_argument('--output', type=Path, required=True)
args = p.parse_args()
args.output.mkdir(parents=True, exist_ok=False)
binary = args.output.resolve() / 'ds2_apas_memory_costs.asi'
shutil.copy2(args.dll, binary)
log = binary.with_suffix('.log')
k = C.WinDLL('kernel32', use_last_error=True)
k.LoadLibraryExW.argtypes = [W.LPCWSTR, W.HANDLE, W.DWORD]
k.LoadLibraryExW.restype = W.HMODULE
k.GetProcAddress.argtypes = [W.HMODULE, C.c_char_p]
k.GetProcAddress.restype = C.c_void_p
k.FreeLibrary.argtypes = [W.HMODULE]
k.GetModuleHandleW.argtypes = [W.LPCWSTR]
k.GetModuleHandleW.restype = W.HMODULE
handle = k.LoadLibraryExW(str(binary), None, 0)
assert handle, C.WinError(C.get_last_error())
version_address = k.GetProcAddress(handle, b'DS2ApasMemoryCostsVersion')
initialize_address = k.GetProcAddress(handle, b'InitializeASI')
assert version_address and initialize_address
version = C.CFUNCTYPE(C.c_char_p)(version_address)
initialize = C.CFUNCTYPE(None)(initialize_address)
assert version() == b'4.1.0-preview.1'
time.sleep(0.15)
assert not log.exists(), 'DllMain unexpectedly started initialization'
with ThreadPoolExecutor(max_workers=8) as pool:
    list(pool.map(lambda _: initialize(), range(32)))
assert k.FreeLibrary(handle)
assert k.GetModuleHandleW(str(binary)) == handle, 'Worker module was not pinned'
assert version() == b'4.1.0-preview.1'
deadline = time.monotonic() + 5
while time.monotonic() < deadline:
    text = log.read_text() if log.exists() else ''
    if 'REFUSED: process is not DS2.exe.' in text:
        break
    time.sleep(0.02)
else:
    raise AssertionError('Missing non-game refusal')
initialize()
time.sleep(0.15)
text = log.read_text()
assert text.count('--- DS2 APAS') == 1
assert text.count('REFUSED: process is not DS2.exe.') == 1
assert not binary.with_suffix('.ini').exists()
print('Lifecycle: PASS (explicit export, 32 concurrent calls, module pin, non-game refusal)')
print('No live game or save accessed. Pinned test copy is released at process exit.')
