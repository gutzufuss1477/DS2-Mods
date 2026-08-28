#!/usr/bin/env python3
from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path

EXPECTED_TIMESTAMP = 0x6A3DAE46
EXPECTED_IMAGE_SIZE = 0x0B292000
EXPECTED_SHA256 = "BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B"
SIGNATURE_RVA = 0x00FA4247
PATCH_RVA = 0x00FA4255
EXPECTED_SIGNATURE = bytes.fromhex(
    "49 0F 44 C0 48 8B 00 "
    "44 38 88 E7 01 00 00 74 06 "
    "B8 06 00 00 00 C3"
)
ORIGINAL_PATCH = bytes.fromhex("74")


class PEImage:
    def __init__(self, path: Path) -> None:
        self.path = path
        self.data = path.read_bytes()
        if len(self.data) < 0x40 or self.data[:2] != b"MZ":
            raise ValueError("not an MZ executable")
        pe = struct.unpack_from("<I", self.data, 0x3C)[0]
        if pe + 24 > len(self.data):
            raise ValueError("truncated PE header")
        if self.data[pe : pe + 4] != b"PE\0\0":
            raise ValueError("not a PE image")
        coff = pe + 4
        self.section_count = struct.unpack_from("<H", self.data, coff + 2)[0]
        self.timestamp = struct.unpack_from("<I", self.data, coff + 4)[0]
        optional_size = struct.unpack_from("<H", self.data, coff + 16)[0]
        optional = coff + 20
        if optional_size < 60:
            raise ValueError("optional header is too small")
        if optional + optional_size > len(self.data):
            raise ValueError("truncated optional header")
        if struct.unpack_from("<H", self.data, optional)[0] != 0x20B:
            raise ValueError("not a PE32+ image")
        self.image_size = struct.unpack_from("<I", self.data, optional + 56)[0]
        section_table = optional + optional_size
        if section_table + self.section_count * 40 > len(self.data):
            raise ValueError("truncated section table")
        self.sections: list[tuple[int, int, int, int]] = []
        for index in range(self.section_count):
            offset = section_table + index * 40
            virtual_size, virtual_address, raw_size, raw_offset = struct.unpack_from(
                "<IIII", self.data, offset + 8
            )
            self.sections.append((virtual_address, virtual_size, raw_offset, raw_size))

    def read_rva(self, rva: int, size: int) -> bytes:
        for virtual_address, _virtual_size, raw_offset, raw_size in self.sections:
            delta = rva - virtual_address
            if 0 <= delta and delta + size <= raw_size:
                offset = raw_offset + delta
                if offset + size > len(self.data):
                    raise ValueError(f"RVA 0x{rva:X} points past end of file")
                return self.data[offset : offset + size]
        raise ValueError(f"RVA 0x{rva:X} is outside mapped sections")


def main() -> int:
    parser = argparse.ArgumentParser(description="Validate the DS2 Weapons Anywhere target")
    parser.add_argument("exe", type=Path, help="path to DS2.exe")
    args = parser.parse_args()

    try:
        pe = PEImage(args.exe)
    except (OSError, ValueError, struct.error) as error:
        print(f"ERROR: {error}")
        return 1
    digest = hashlib.sha256(pe.data).hexdigest().upper()
    try:
        signature = pe.read_rva(SIGNATURE_RVA, len(EXPECTED_SIGNATURE))
        patch = pe.read_rva(PATCH_RVA, len(ORIGINAL_PATCH))
    except ValueError as error:
        print(f"ERROR: {error}")
        return 1
    signature_count = pe.data.count(EXPECTED_SIGNATURE)

    checks = {
        "timestamp": pe.timestamp == EXPECTED_TIMESTAMP,
        "image_size": pe.image_size == EXPECTED_IMAGE_SIZE,
        "sha256": digest == EXPECTED_SHA256,
        "signature": signature == EXPECTED_SIGNATURE,
        "signature_count": signature_count == 1,
        "patch_site": patch == ORIGINAL_PATCH,
    }

    print(f"Target: {args.exe}")
    print(f"PE timestamp: 0x{pe.timestamp:08X}")
    print(f"Image size: 0x{pe.image_size:08X}")
    print(f"SHA-256: {digest}")
    print(f"Signature @ 0x{SIGNATURE_RVA:X}: {signature.hex(' ').upper()}")
    print(f"Signature occurrences in file: {signature_count}")
    print(f"Patch site @ 0x{PATCH_RVA:X}: {patch.hex(' ').upper()}")
    for name, ok in checks.items():
        print(f"{name}: {'OK' if ok else 'FAIL'}")
    return 0 if all(checks.values()) else 1


if __name__ == "__main__":
    raise SystemExit(main())
