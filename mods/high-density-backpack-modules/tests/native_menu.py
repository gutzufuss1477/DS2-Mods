"""Execute the actual category builder in an isolated test DLL, never the game."""
import ctypes, hashlib, struct, sys
from pathlib import Path
data=Path(sys.argv[2]).read_bytes()
assert hashlib.sha256(data).hexdigest()=='bf3d1c665545930bc850d8f5df486f7395885bb729d4fd408fdb03390de0765b'
pe=struct.unpack_from('<I',data,0x3c)[0]
count=struct.unpack_from('<H',data,pe+6)[0]
optional=struct.unpack_from('<H',data,pe+20)[0]
for i in range(count):
    p=pe+24+optional+i*40
    size,rva,raw_size,raw=struct.unpack_from('<IIII',data,p+8)
    if rva<=0x1528e90 and 0x1529430<=rva+raw_size:
        code=data[raw+0x1528e90-rva:raw+0x1529430-rva]
        break
else:
    raise AssertionError('Category builder not found')
dll=ctypes.CDLL(sys.argv[1])
dll.test_native_menu.argtypes=[ctypes.c_void_p,ctypes.c_uint32]
buffer=ctypes.create_string_buffer(code)
result=dll.test_native_menu(buffer,len(code))
print('PASS: original DS2 category builder, 256 equipment masks, eight selectable rows and exact header counts' if result==0 else f'FAIL: runtime_tests.cpp line {result}')
sys.exit(bool(result))
