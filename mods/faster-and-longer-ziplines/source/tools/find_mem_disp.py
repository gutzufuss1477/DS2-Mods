#!/usr/bin/env python3
from __future__ import annotations

import argparse
import struct
from pathlib import Path

from capstone import CS_ARCH_X86, CS_MODE_64, CS_OP_MEM, Cs


def integer(value: str) -> int:
    return int(value, 0)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("exe", type=Path)
    parser.add_argument("displacement", type=integer)
    args = parser.parse_args()
    data = args.exe.read_bytes()
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    coff = pe + 4
    count = struct.unpack_from("<H", data, coff + 2)[0]
    optional_size = struct.unpack_from("<H", data, coff + 16)[0]
    optional = coff + 20
    base = struct.unpack_from("<Q", data, optional + 24)[0]
    table = optional + optional_size
    decoder = Cs(CS_ARCH_X86, CS_MODE_64)
    decoder.detail = True
    decoder.skipdata = True
    for index in range(count):
        entry = table + index * 40
        name = data[entry : entry + 8].rstrip(b"\0")
        _virtual_size, rva, raw_size, raw = struct.unpack_from(
            "<IIII", data, entry + 8
        )
        if name != b".text":
            continue
        section = data[raw : raw + raw_size]
        needle = struct.pack("<i", args.displacement)
        positions: list[int] = []
        cursor = 0
        while True:
            found = section.find(needle, cursor)
            if found < 0:
                break
            positions.append(found)
            cursor = found + 1
        emitted: set[int] = set()
        for position in positions:
            for backtrack in range(1, 16):
                start = max(0, position - backtrack)
                window = section[start : position + 20]
                for instruction in decoder.disasm(window, base + rva + start):
                    begin = instruction.address - (base + rva)
                    end = begin + instruction.size
                    if not (begin <= position < end):
                        continue
                    if instruction.address in emitted:
                        continue
                    if not any(
                        operand.type == CS_OP_MEM
                        and operand.mem.disp == args.displacement
                        for operand in instruction.operands
                    ):
                        continue
                    emitted.add(instruction.address)
                    print(
                        f"{instruction.address - base:08X}  "
                        f"{instruction.bytes.hex(' '):<30}  "
                        f"{instruction.mnemonic:<8} {instruction.op_str}"
                    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
