#!/usr/bin/env python3
from __future__ import annotations

import argparse
import struct
from pathlib import Path

from capstone import CS_ARCH_X86, CS_MODE_64, CS_OP_MEM, Cs
from capstone.x86_const import X86_REG_RIP


class PEImage:
    def __init__(self, path: Path) -> None:
        self.data = path.read_bytes()
        pe = struct.unpack_from("<I", self.data, 0x3C)[0]
        coff = pe + 4
        count = struct.unpack_from("<H", self.data, coff + 2)[0]
        optional_size = struct.unpack_from("<H", self.data, coff + 16)[0]
        optional = coff + 20
        self.base = struct.unpack_from("<Q", self.data, optional + 24)[0]
        table = optional + optional_size
        self.sections: list[tuple[str, int, int, int, int]] = []
        for index in range(count):
            entry = table + index * 40
            name = self.data[entry : entry + 8].rstrip(b"\0").decode("ascii")
            virtual_size, rva, raw_size, raw = struct.unpack_from(
                "<IIII", self.data, entry + 8
            )
            self.sections.append((name, rva, raw, raw_size, virtual_size))

    def offset_to_rva(self, offset: int) -> int:
        for _name, rva, raw, raw_size, _virtual_size in self.sections:
            if raw <= offset < raw + raw_size:
                return rva + offset - raw
        raise ValueError(f"file offset 0x{offset:X} is outside sections")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("exe", type=Path)
    parser.add_argument("text")
    parser.add_argument("--utf16", action="store_true")
    args = parser.parse_args()
    image = PEImage(args.exe)
    needle = args.text.encode("utf-16-le" if args.utf16 else "utf-8")
    positions: list[int] = []
    start = 0
    while True:
        found = image.data.find(needle, start)
        if found < 0:
            break
        positions.append(found)
        start = found + 1
    if not positions:
        print("No string occurrence found")
        return 1
    targets = {image.base + image.offset_to_rva(position) for position in positions}
    for target in sorted(targets):
        print(f"String VA 0x{target:X} (RVA 0x{target - image.base:X})")

    pointer_targets: set[int] = set()
    for target in targets:
        encoded = struct.pack("<Q", target)
        start = 0
        while True:
            found = image.data.find(encoded, start)
            if found < 0:
                break
            pointer_rva = image.offset_to_rva(found)
            pointer_targets.add(image.base + pointer_rva)
            print(f"pointer RVA 0x{pointer_rva:X} -> 0x{target - image.base:X}")
            start = found + 1

    code_targets = targets | pointer_targets

    decoder = Cs(CS_ARCH_X86, CS_MODE_64)
    decoder.detail = True
    for name, rva, raw, raw_size, _virtual_size in image.sections:
        if name != ".text":
            continue
        code = image.data[raw : raw + raw_size]
        for instruction in decoder.disasm(code, image.base + rva):
            for operand in instruction.operands:
                if operand.type != CS_OP_MEM or operand.mem.base != X86_REG_RIP:
                    continue
                resolved = instruction.address + instruction.size + operand.mem.disp
                if resolved in code_targets:
                    print(
                        f"xref RVA 0x{instruction.address - image.base:X}: "
                        f"{instruction.mnemonic} {instruction.op_str}"
                    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
