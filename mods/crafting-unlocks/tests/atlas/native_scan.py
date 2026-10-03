"""Read-only PE disassembly and reference survey for the supported DS2 binary."""
from __future__ import annotations
import argparse
import bisect
import hashlib
import json
from pathlib import Path
import struct
from capstone import Cs, CS_ARCH_X86, CS_MODE_64

ROOT = Path(__file__).resolve().parents[5]
EXPECTED = 'bf3d1c665545930bc850d8f5df486f7395885bb729d4fd408fdb03390de0765b'

class Image:
    def __init__(self, path=ROOT/'analysis/DS2.exe'):
        self.data = path.read_bytes()
        if hashlib.sha256(self.data).hexdigest() != EXPECTED:
            raise ValueError('Unsupported executable SHA256')
        pe = struct.unpack_from('<I', self.data, 0x3c)[0]
        opt = pe+24
        self.base = struct.unpack_from('<Q', self.data, opt+24)[0]
        ns = struct.unpack_from('<H', self.data, pe+6)[0]
        st = opt+struct.unpack_from('<H', self.data, pe+20)[0]
        self.sections = []
        for i in range(ns):
            off = st+i*40
            vs, va, rs, ro = struct.unpack_from('<IIII', self.data, off+8)
            self.sections.append((va, rs, ro))
        prva, psz = struct.unpack_from('<II', self.data, opt+112+3*8)
        self.functions = list(struct.iter_unpack('<III', self.read(prva, psz)))
        self.starts = [x[0] for x in self.functions]
        self.cs = Cs(CS_ARCH_X86, CS_MODE_64)
        self.cs.skipdata = True

    def read(self, rva, size):
        for va, rs, ro in self.sections:
            if va <= rva and rva+size <= va+rs:
                return self.data[ro+rva-va:ro+rva-va+size]
        raise ValueError(f'Unmapped span {rva:x}+{size:x}')

    def function(self, rva):
        i = bisect.bisect_right(self.starts, rva)-1
        if i >= 0 and self.functions[i][0] <= rva < self.functions[i][1]:
            return self.functions[i][:2]
        return rva, rva+128

    def disasm(self, start, end):
        return self.cs.disasm_lite(self.read(start, end-start), self.base+start)

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('mode', choices=['fields','function','calls'])
    ap.add_argument('values', nargs='+', type=lambda x:int(x,0))
    ap.add_argument('--start', type=lambda x:int(x,0), default=0xD00000)
    ap.add_argument('--end', type=lambda x:int(x,0), default=0x1100000)
    ap.add_argument('--output', type=Path)
    args = ap.parse_args()
    im = Image()
    lines = []
    if args.mode == 'function':
        for target in args.values:
            start,end = im.function(target)
            lines.append(f'=== FUNCTION 0x{start:X}..0x{end:X} ===')
            for addr,size,op,operands in im.disasm(start,end):
                lines.append(f'{addr-im.base:08X} {im.read(addr-im.base,size).hex(" "):32s} {op:9s} {operands}')
    else:
        for addr,size,op,operands in im.disasm(args.start,args.end):
            if args.mode == 'fields':
                match = any(f'+ 0x{x:x}]' in operands for x in args.values)
            else:
                match = op in ('call','jmp') and operands in {hex(im.base+x) for x in args.values}
            if match:
                start,end = im.function(addr-im.base)
                lines.append(f'FN={start:08X} SIZE={end-start:X} RVA={addr-im.base:08X} {op} {operands}')
    text = '\n'.join(lines)+'\n'
    if args.output:
        args.output.write_text(text, encoding='utf-8')
        print(f'{len(lines)} lines -> {args.output}')
    else:
        print(text)

if __name__ == '__main__':
    main()
