#!/usr/bin/env python3
"""Static target validator for DS2 Sam Stats Booster v1.0.0."""
from __future__ import annotations

import hashlib
import re
import struct
import sys
from dataclasses import dataclass
from pathlib import Path

EXPECTED_SHA256 = "bf3d1c665545930bc850d8f5df486f7395885bb729d4fd408fdb03390de0765b"
EXPECTED_TIMESTAMP = 0x6A3DAE46
EXPECTED_IMAGE_SIZE = 0x0B292000
EXPECTED_IMAGE_BASE = 0x140000000

SIGNATURES = {
    "u32_producer": "40 57 48 83 EC 20 80 7A 02 01 48 8B FA 0F 85 ?? ?? ?? ?? 0F B7 02 48 89 5C 24 30 66 83 F8 3E",
    "q64_lung_producer": "40 53 48 83 EC 20 80 7A 02 01 48 8B DA 75 2E 66 83 3A 05 75 28 B2 0B E8 ?? ?? ?? ?? 84 C0 74 1D",
    "double_physical_producer": "40 57 48 83 EC 20 80 7A 02 01 48 8B FA 75 6C 0F B7 02 48 89 5C 24 30 66 83 F8 07 75 09 BB 08 00 00 00",
}
EXPECTED_ANCHORS = {
    "u32_producer": 0x0D92F30,
    "q64_lung_producer": 0x0D92FF0,
    "double_physical_producer": 0x0D93040,
}


@dataclass(frozen=True)
class Section:
    name: str
    virtual_size: int
    virtual_address: int
    raw_size: int
    raw_offset: int


class PE:
    def __init__(self, path: Path) -> None:
        self.path = path
        self.data = path.read_bytes()
        if self.data[:2] != b"MZ":
            raise ValueError("not an MZ executable")
        pe_offset = struct.unpack_from("<I", self.data, 0x3C)[0]
        if self.data[pe_offset : pe_offset + 4] != b"PE\0\0":
            raise ValueError("missing PE signature")
        coff = pe_offset + 4
        section_count = struct.unpack_from("<H", self.data, coff + 2)[0]
        self.timestamp = struct.unpack_from("<I", self.data, coff + 4)[0]
        optional_size = struct.unpack_from("<H", self.data, coff + 16)[0]
        optional = coff + 20
        if struct.unpack_from("<H", self.data, optional)[0] != 0x20B:
            raise ValueError("not PE32+")
        self.image_base = struct.unpack_from("<Q", self.data, optional + 24)[0]
        self.image_size = struct.unpack_from("<I", self.data, optional + 56)[0]
        section_table = optional + optional_size
        sections: list[Section] = []
        for index in range(section_count):
            offset = section_table + index * 40
            name = self.data[offset : offset + 8].split(b"\0", 1)[0].decode("ascii", "replace")
            virtual_size, virtual_address, raw_size, raw_offset = struct.unpack_from(
                "<IIII", self.data, offset + 8
            )
            sections.append(Section(name, virtual_size, virtual_address, raw_size, raw_offset))
        self.sections = sections

    def section(self, name: str) -> Section:
        return next(section for section in self.sections if section.name == name)

    def rva_to_offset(self, rva: int) -> int:
        for section in self.sections:
            size = max(section.virtual_size, section.raw_size)
            if section.virtual_address <= rva < section.virtual_address + size:
                return section.raw_offset + (rva - section.virtual_address)
        raise ValueError(f"RVA {rva:#x} is not file-backed")

    def read_rva(self, rva: int, size: int) -> bytes:
        offset = self.rva_to_offset(rva)
        return self.data[offset : offset + size]


def signature_regex(specification: str) -> re.Pattern[bytes]:
    pieces: list[bytes] = []
    for token in specification.split():
        pieces.append(b"." if token == "??" else re.escape(bytes([int(token, 16)])))
    return re.compile(b"".join(pieces), re.DOTALL)


def main() -> int:
    if len(sys.argv) != 2:
        print("Usage: validate_target.py <DS2.exe>")
        return 2
    path = Path(sys.argv[1])
    if not path.is_file():
        print(f"ERROR: file not found: {path}")
        return 2

    pe = PE(path)
    sha256 = hashlib.sha256(pe.data).hexdigest()
    failures: list[str] = []

    print(f"File: {path}")
    print(f"SHA-256: {sha256}")
    print(f"PE timestamp: {pe.timestamp:#010x}")
    print(f"Image base: {pe.image_base:#x}")
    print(f"Image size: {pe.image_size:#010x}")

    if sha256 != EXPECTED_SHA256:
        failures.append("SHA-256 does not match the analysed DS2.exe")
    if pe.timestamp != EXPECTED_TIMESTAMP:
        failures.append("PE timestamp mismatch")
    if pe.image_base != EXPECTED_IMAGE_BASE:
        failures.append("image base mismatch")
    if pe.image_size != EXPECTED_IMAGE_SIZE:
        failures.append("image size mismatch")

    text = pe.section(".text")
    text_bytes = pe.data[text.raw_offset : text.raw_offset + text.raw_size]
    resolved: dict[str, int] = {}
    for name, specification in SIGNATURES.items():
        matches = [text.virtual_address + match.start() for match in signature_regex(specification).finditer(text_bytes)]
        print(f"{name}: {len(matches)} match(es): {', '.join(f'{rva:#x}' for rva in matches) or '-'}")
        if len(matches) != 1:
            failures.append(f"{name} is not unique")
            continue
        resolved[name] = matches[0]
        if matches[0] != EXPECTED_ANCHORS[name]:
            failures.append(f"{name} moved from expected RVA {EXPECTED_ANCHORS[name]:#x}")

    if len(resolved) == 3:
        u32_site = resolved["u32_producer"] + 0x9A
        q64_site = resolved["q64_lung_producer"] + 0x20
        double_site = resolved["double_physical_producer"] + 0x5A
        manager_disp = struct.unpack("<i", pe.read_rva(u32_site + 10, 4))[0]
        manager_slot = u32_site + 14 + manager_disp
        cap_disp = struct.unpack("<i", pe.read_rva(double_site + 4, 4))[0]
        cap_rva = double_site + 8 + cap_disp
        cap_value = struct.unpack("<d", pe.read_rva(cap_rva, 8))[0]
        mutator_disp = struct.unpack("<i", pe.read_rva(double_site + 24, 4))[0]
        double_mutator = double_site + 28 + mutator_disp
        u32_getter = 0x00C01120
        u32_mutator = 0x00C00D70
        q64_getter = 0x00C011D0
        q64_mutator = 0x00C00E60
        double_getter = 0x00C012B0

        print(f"U32 patch site: {u32_site:#x}; continuation after native call: {u32_site + 19:#x}")
        print(f"Q64 patch site: {q64_site:#x}; continuation at ordinary epilogue: {q64_site + 29:#x}")
        print(f"Double patch site: {double_site:#x}; continuation after native call: {double_site + 28:#x}")
        print(f"DSPlayRecordManager slot: {manager_slot:#x}")
        print(f"Double-record cap: RVA {cap_rva:#x}, value {cap_value:.2f}")
        print(f"U32 getter/mutator: {u32_getter:#x} / {u32_mutator:#x}")
        print(f"Q64 getter/mutator: {q64_getter:#x} / {q64_mutator:#x}")
        print(f"Double getter: {double_getter:#x}")
        print(f"Double mutator decoded from producer call: {double_mutator:#x}")

        if manager_slot != 0x0623E5E8:
            failures.append("decoded DSPlayRecordManager slot mismatch")
        if cap_rva != 0x03462020:
            failures.append("decoded double cap RVA mismatch")
        if not (999_999_999_999.0 < cap_value < 1_000_000_000_001.0):
            failures.append("decoded double cap value is implausible")
        if double_mutator != 0x00C00FA0:
            failures.append("decoded double mutator mismatch")
        if pe.read_rva(u32_site, 19)[:14] != bytes.fromhex("44 8B 47 04 0F B7 D3 48 8B 0D 10 B6 4A 05"):
            failures.append("U32 extended patch window mismatch")
        if pe.read_rva(q64_site, 29) != bytes.fromhex("4C 8B 43 08 BA 06 00 00 00 49 B9 FF 0F A5 D4 E8 00 00 00 48 83 C4 20 5B E9 33 DE E6 FF"):
            failures.append("Q64 extended patch window mismatch")
        if pe.read_rva(u32_getter, 13) != bytes.fromhex("40 57 48 83 EC 20 48 8B F9 66 83 FA 79"):
            failures.append("U32 getter signature mismatch")
        if pe.read_rva(u32_mutator, 15) != bytes.fromhex("48 89 6C 24 18 48 89 74 24 20 57 48 83 EC 20"):
            failures.append("U32 mutator signature mismatch")
        if pe.read_rva(q64_getter, 15) != bytes.fromhex("40 57 48 83 EC 20 33 C0 48 8B F9 66 83 FA 09"):
            failures.append("Q64 getter signature mismatch")
        if pe.read_rva(q64_mutator, 13) != bytes.fromhex("48 89 6C 24 18 56 57 41 56 48 83 EC 20"):
            failures.append("Q64 mutator signature mismatch")
        if pe.read_rva(double_getter, 13) != bytes.fromhex("40 57 48 83 EC 20 48 8B F9 66 83 FA 2F"):
            failures.append("double getter signature mismatch")

    if failures:
        print("\nRESULT: FAILED")
        for failure in failures:
            print(f"- {failure}")
        return 1

    print("\nRESULT: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
