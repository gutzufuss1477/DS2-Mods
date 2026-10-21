#!/usr/bin/env python3
from __future__ import annotations

import argparse
import struct
from pathlib import Path

from capstone import CS_ARCH_X86, CS_MODE_64, Cs


class PEImage:
    def __init__(self, path: Path) -> None:
        self.data = path.read_bytes()
        pe = struct.unpack_from("<I", self.data, 0x3C)[0]
        coff = pe + 4
        section_count = struct.unpack_from("<H", self.data, coff + 2)[0]
        optional_size = struct.unpack_from("<H", self.data, coff + 16)[0]
        optional = coff + 20
        self.image_base = struct.unpack_from("<Q", self.data, optional + 24)[0]
        table = optional + optional_size
        self.sections: list[tuple[int, int, int]] = []
        for index in range(section_count):
            entry = table + index * 40
            virtual_size, virtual_address, raw_size, raw_offset = struct.unpack_from(
                "<IIII", self.data, entry + 8
            )
            self.sections.append(
                (virtual_address, raw_offset, max(virtual_size, raw_size))
            )

    def read_rva(self, rva: int, size: int) -> bytes:
        for virtual_address, raw_offset, mapped_size in self.sections:
            delta = rva - virtual_address
            if 0 <= delta < mapped_size:
                available = min(size, len(self.data) - (raw_offset + delta))
                return self.data[raw_offset + delta : raw_offset + delta + available]
        raise ValueError(f"RVA 0x{rva:X} is outside mapped sections")


def integer(value: str) -> int:
    return int(value, 0)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("exe", type=Path)
    parser.add_argument("rva", type=integer)
    parser.add_argument("size", type=integer)
    args = parser.parse_args()
    image = PEImage(args.exe)
    decoder = Cs(CS_ARCH_X86, CS_MODE_64)
    decoder.skipdata = True
    start = image.image_base + args.rva
    for instruction in decoder.disasm(image.read_rva(args.rva, args.size), start):
        print(
            f"{instruction.address - image.image_base:08X}  "
            f"{instruction.bytes.hex(' '):<30}  "
            f"{instruction.mnemonic:<8} {instruction.op_str}"
        )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
