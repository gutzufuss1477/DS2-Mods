"""Read-only diagnostic of the documented APAS executable anchors, never save data."""
import ctypes as C
from ctypes import wintypes as W
from pathlib import Path
import struct
import sys
from pe_image import PEImage

pe = PEImage(sys.argv[1])
pid, base = int(sys.argv[2]), int(sys.argv[3], 0)
k = C.WinDLL('kernel32', use_last_error=True)
k.OpenProcess.argtypes = [W.DWORD, W.BOOL, W.DWORD]
k.OpenProcess.restype = W.HANDLE
k.ReadProcessMemory.argtypes = [W.HANDLE, C.c_void_p, C.c_void_p, C.c_size_t, C.POINTER(C.c_size_t)]
k.CloseHandle.argtypes = [W.HANDLE]
handle = k.OpenProcess(0x10, False, pid)
if not handle:
    raise C.WinError(C.get_last_error())

def read(rva, count):
    buf = C.create_string_buffer(count)
    actual = C.c_size_t()
    if not k.ReadProcessMemory(handle, base + rva, buf, count, C.byref(actual)) or actual.value != count:
        raise C.WinError(C.get_last_error())
    return buf.raw

try:
    offset = struct.unpack('<I', read(0x3c, 4))[0]
    print('PE offset:', hex(offset))
    print('Timestamp:', read(offset + 8, 4).hex(), 'expected', pe.read(pe.pe + 8, 4).hex() if pe.pe >= 0x1000 else pe.data[pe.pe + 8:pe.pe + 12].hex())
    for rva, count in [(0xBE0270, 32), (0xBE04D2, 13), (0xBE12E0, 32), (0xBE1326, 32), (0xBE3D00, 41), (0xBE39A0, 96)]:
        actual, expected = read(rva, count), pe.read(rva, count)
        print(f'{rva:X}:', 'MATCH' if actual == expected else f'DIFFERENT {actual.hex(" ")}')
    values = struct.unpack('<QQQQ', read(0x32088b8, 32))
    print('Resource vtable target RVAs:', [hex(v - base) for v in values])
    print('Manager present:', any(read(0x623e5d0, 8)))
finally:
    k.CloseHandle(handle)
