#!/usr/bin/env python3
from __future__ import annotations

import argparse
import struct
from pathlib import Path

from capstone import CS_ARCH_X86, CS_ERR_SKIPDATA, CS_MODE_64, CS_OP_MEM, Cs, CsError
from capstone.x86_const import X86_REG_RIP


def integer(value: str) -> int:
    return int(value, 0)


def main() -> int:
    parser = argparse.ArgumentParser(description="Find .text RIP-relative references to an RVA")
    parser.add_argument("exe", type=Path)
    parser.add_argument("target_rva", type=integer)
    args = parser.parse_args()

    data = args.exe.read_bytes()
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    coff = pe + 4
    count = struct.unpack_from("<H", data, coff + 2)[0]
    optional_size = struct.unpack_from("<H", data, coff + 16)[0]
    optional = coff + 20
    image_base = struct.unpack_from("<Q", data, optional + 24)[0]
    table = optional + optional_size

    decoder = Cs(CS_ARCH_X86, CS_MODE_64)
    decoder.detail = True
    decoder.skipdata = True
    matches = 0
    for index in range(count):
        entry = table + index * 40
        name = data[entry : entry + 8].rstrip(b"\0")
        virtual_size, rva, raw_size, raw = struct.unpack_from("<IIII", data, entry + 8)
        if name != b".text":
            continue
        section = data[raw : raw + min(virtual_size, raw_size)]
        for instruction in decoder.disasm(section, image_base + rva):
            if instruction.id == 0:
                continue
            try:
                operands = instruction.operands
            except CsError as error:
                if error.errno == CS_ERR_SKIPDATA:
                    continue
                raise
            for operand in operands:
                if operand.type != CS_OP_MEM or operand.mem.base != X86_REG_RIP:
                    continue
                resolved = instruction.address + instruction.size + operand.mem.disp
                if resolved - image_base != args.target_rva:
                    continue
                matches += 1
                print(
                    f"{instruction.address - image_base:08X}  "
                    f"{instruction.bytes.hex(' '):<30}  "
                    f"{instruction.mnemonic:<8} {instruction.op_str}"
                )
    print(f"references: {matches}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
