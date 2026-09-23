"""Small read-only PE reader for the APAS offline checks (standard library only)."""
import struct
from pathlib import Path


class PEImage:
    def __init__(self, path):
        self.data = Path(path).read_bytes()
        self.pe = struct.unpack_from('<I', self.data, 0x3c)[0]
        assert self.data[self.pe:self.pe + 4] == b'PE\0\0'
        self.machine, count, self.timestamp = struct.unpack_from('<HHI', self.data, self.pe + 4)
        optional_size = struct.unpack_from('<H', self.data, self.pe + 20)[0]
        self.optional = self.pe + 24
        assert struct.unpack_from('<H', self.data, self.optional)[0] == 0x20b
        self.base = struct.unpack_from('<Q', self.data, self.optional + 24)[0]
        self.image_size = struct.unpack_from('<I', self.data, self.optional + 56)[0]
        self.sections = []
        for i in range(count):
            off = self.optional + optional_size + i * 40
            name = self.data[off:off + 8].rstrip(b'\0').decode()
            size, rva, raw_size, raw = struct.unpack_from('<IIII', self.data, off + 8)
            self.sections.append((name, rva, size, raw, raw_size))

    def offset(self, rva):
        for _, start, _, raw, size in self.sections:
            if start <= rva < start + size:
                return raw + rva - start
        raise ValueError(f'RVA {rva:X} has no file data')

    def read(self, rva, size):
        off = self.offset(rva)
        return self.data[off:off + size]

    def qword(self, rva):
        return struct.unpack('<Q', self.read(rva, 8))[0]

    def functions(self):
        rva, size = struct.unpack_from('<II', self.data, self.optional + 112 + 3 * 8)
        yield from struct.iter_unpack('<III', self.read(rva, size))


if __name__ == '__main__':
    import sys
    pe = PEImage(sys.argv[1])
    for arg in sys.argv[2:]:
        rva = int(arg, 16)
        print(f'{rva:08X}: {pe.read(rva, 64).hex(" ")}')
        print('qwords:', ' '.join(f'{pe.qword(rva + i * 8):X}' for i in range(16)))
        for start, end, _ in pe.functions():
            if start <= rva < end:
                print(f'function {start:X}..{end:X}')
