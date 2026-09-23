"""Extract documented anchors only from the exact supported, unmodified executable."""
import hashlib
import sys
from pathlib import Path
from pe_image import PEImage

EXPECTED = 'BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B'
pe = PEImage(sys.argv[1])
assert hashlib.sha256(pe.data).hexdigest().upper() == EXPECTED, 'Unsupported executable'
anchors = {
    'Constructor': (0xBE0270, 32),
    'CostCopy': (0xBE04D2, 13),
    'Activate': (0xBE12E0, 32),
    'Capacity': (0xBE1326, 32),
    'Accounting': (0xBE3D00, 41),
    'Unlock': (0xBE39A0, 96),
    'ResourceVtable': (0x32088B8, 32),
}
out = ['// Steam DS2.exe 1.10.89.0. Generated once from the SHA-256-verified research target.',
       f'// SHA-256: {EXPECTED}', '#pragma once',
       f'constexpr u32 kTimestamp = 0x{pe.timestamp:X}u;',
       f'constexpr u32 kImageSize = 0x{pe.image_size:X}u;',
       'constexpr u32 kManagerRva = 0x623E5D0u;',
       'struct Anchor { u32 rva; u32 length; const u8* bytes; bool relocatedPointers; };']
for name, (rva, size) in anchors.items():
    values = ','.join(f'0x{x:02X}' for x in pe.read(rva, size))
    out.append(f'constexpr u8 k{name}Bytes[] = {{{values}}};')
out.append('constexpr Anchor kAnchors[] = {')
for name, (rva, size) in anchors.items():
    out.append(f'    {{0x{rva:X}u, {size}u, k{name}Bytes, {str(name == "ResourceVtable").lower()}}},')
out.append('};\n')
Path(sys.argv[2]).write_text('\n'.join(out), encoding='utf-8')
