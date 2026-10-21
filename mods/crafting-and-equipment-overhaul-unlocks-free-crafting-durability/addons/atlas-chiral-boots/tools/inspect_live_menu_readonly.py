"""Bounded read-only DS2 catalogue snapshot; no injected code, writes, UI or saves."""
import ctypes as C
from ctypes import wintypes as W
import struct, json, sys
from pathlib import Path

K = C.WinDLL('kernel32', use_last_error=True)
K.OpenProcess.argtypes = [W.DWORD, W.BOOL, W.DWORD]; K.OpenProcess.restype = W.HANDLE
K.ReadProcessMemory.argtypes = [W.HANDLE, C.c_void_p, C.c_void_p, C.c_size_t, C.POINTER(C.c_size_t)]
K.ReadProcessMemory.restype = W.BOOL
K.CloseHandle.argtypes = [W.HANDLE]
pid, base = int(sys.argv[1]), int(sys.argv[2], 16)
h = K.OpenProcess(0x410, False, pid)  # QUERY_INFORMATION | VM_READ only
if not h: raise OSError(C.get_last_error())


def read(p, n):
    if p < 0x10000 or n > 2_000_000: return b''
    buf = C.create_string_buffer(n); got = C.c_size_t()
    if not K.ReadProcessMemory(h, p, buf, n, C.byref(got)) or got.value != n: return b''
    return buf.raw


def num(p, fmt='<Q'):
    b = read(p, struct.calcsize(fmt))
    return struct.unpack(fmt, b)[0] if b else 0


def rtype(p):
    fn = num(num(p)); code = read(fn, 16)
    if code[:3] == b'\x48\x8d\x05' and code[7:8] == b'\xc3':
        return fn + 7 + struct.unpack_from('<i', code, 3)[0] - base
    return None


def string(p):
    return dict(pointer=hex(p), header=read(p-16, 16).hex(),
                text=read(p, 128).split(b'\0')[0].decode('ascii', errors='replace'))


result = dict(pid=pid, base=hex(base), accessMask='0x410 READ ONLY', gameStarted=False,
              memoryWritten=False, saveFilesRead=False, depots=[], atlasCandidates=[], samples=[])
seen = set()


def scan(p, depth=0):
    if p in seen: return
    if depth > 32 or len(seen) > 8192: raise ValueError('Traversal bound')
    seen.add(p)
    n = num(p+0x38, '<i'); rows = num(p+0x40)
    result['depots'].append(dict(pointer=hex(p), type=hex(rtype(p) or 0), count=n))
    if n < 0 or n > 200000: raise ValueError('Invalid depot')
    b = read(rows, n*8)
    if len(b) != n*8: raise ValueError('Unreadable rows')
    for (q,) in struct.iter_unpack('<Q', b):
        if not q: continue
        t = rtype(q)
        if len(result['samples']) < 12:
            result['samples'].append(dict(pointer=hex(q), type=hex(t or 0),
                                          getterBytes=read(num(num(q)), 16).hex()))
        if t == 0x605f3a0: scan(q, depth+1)
        head = read(q, 0x60)
        if not head: continue
        guid = struct.unpack_from('<QQ', head, 16)
        listid = struct.unpack_from('<I', head, 0x40)[0]
        if guid[0] == 0x4a634cbdce790267 or (t == 0x4333a00 and listid == 1000103):
            result['atlasCandidates'].append(dict(pointer=hex(q), type=hex(t or 0),
                header=head.hex(), icon=string(num(q+0x50)), listId=listid,
                listCode=hex(num(q+0x44, '<I')), guid=[hex(x) for x in guid]))


try:
    game = num(base+0x623e310)
    result['game'] = hex(game)
    result['items'] = hex(num(game+0x280)); result['catalogue'] = hex(num(game+0x270))
    scan(num(game+0x270))
    scan(num(game+0x280))
    out = Path(__file__).resolve().parents[1]/'evidence/test12-live-menu-readonly.json'
    with out.open('x') as f: json.dump(result, f, indent=2)
    print(json.dumps({k: result[k] for k in ['pid','game','items','catalogue','atlasCandidates']}, indent=2))
    print('DEPOTS', len(result['depots']), 'samples', result['samples'][:3])
finally:
    K.CloseHandle(h)
