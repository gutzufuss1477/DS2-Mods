#!/usr/bin/env python3
from __future__ import annotations

import argparse
import hashlib
import struct
from dataclasses import dataclass
from pathlib import Path

EXPECTED_TIMESTAMP = 0x6A3DAE46
EXPECTED_IMAGE_SIZE = 0x0B292000
EXPECTED_SHA256 = "BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B"
EXPECTED_GLOBAL_ROOT_RVA = 0x0623EAD8
EXPECTED_MANAGER_VTABLE_RVA = 0x03280940
QPID_COST_DISPLACEMENT = 0x4E4

EXPECTED_MANAGER_VTABLE = bytes.fromhex(
    "B0 26 24 41 01 00 00 00 80 42 27 41 01 00 00 00 "
    "E0 9D 10 40 01 00 00 00 B0 9E 10 40 01 00 00 00"
)

# Common integer operations whose r/m32 operand can read QpidCost.
MEMORY_READ_OPCODES = {
    0x03: "add",
    0x2B: "sub",
    0x39: "cmp",
    0x3B: "cmp",
    0x63: "movsxd",
    0x8B: "mov",
}


@dataclass(frozen=True)
class CostSite:
    name: str
    rva: int
    expected: bytes


QPID_COST_SITES = (
    CostSite(
        "CapacityCheck",
        0x0133755C,
        bytes.fromhex("8B 8B E4 04 00 00 03 4C 24 58 3B 4C 24 68"),
    ),
    CostSite(
        "AccountingAdd",
        0x0127D859,
        bytes.fromhex("8B 88 E4 04 00 00 43 01 4C 88 04"),
    ),
    CostSite(
        "AccountingValue",
        0x0127D87C,
        bytes.fromhex("8B 90 E4 04 00 00 48 8D 44 24 60"),
    ),
    CostSite(
        "AccountingSubtract",
        0x0129CCC5,
        bytes.fromhex("8B 90 E4 04 00 00 41 29 51 04"),
    ),
    CostSite(
        "UiCost",
        0x016BCFF8,
        bytes.fromhex("8B 80 E4 04 00 00 89 45 88"),
    ),
)


@dataclass(frozen=True)
class Section:
    virtual_address: int
    virtual_size: int
    raw_offset: int
    raw_size: int
    characteristics: int


class PEImage:
    def __init__(self, path: Path) -> None:
        self.path = path
        self.data = path.read_bytes()
        if len(self.data) < 0x40 or self.data[:2] != b"MZ":
            raise ValueError("not an MZ executable")

        pe = struct.unpack_from("<I", self.data, 0x3C)[0]
        if pe + 24 > len(self.data) or self.data[pe : pe + 4] != b"PE\0\0":
            raise ValueError("not a valid PE image")

        coff = pe + 4
        self.machine = struct.unpack_from("<H", self.data, coff)[0]
        self.section_count = struct.unpack_from("<H", self.data, coff + 2)[0]
        self.timestamp = struct.unpack_from("<I", self.data, coff + 4)[0]
        optional_size = struct.unpack_from("<H", self.data, coff + 16)[0]
        optional = coff + 20
        if optional + optional_size > len(self.data):
            raise ValueError("truncated optional header")
        if struct.unpack_from("<H", self.data, optional)[0] != 0x20B:
            raise ValueError("not a PE32+ image")
        self.image_size = struct.unpack_from("<I", self.data, optional + 56)[0]

        section_table = optional + optional_size
        self.sections: list[Section] = []
        for index in range(self.section_count):
            offset = section_table + index * 40
            if offset + 40 > len(self.data):
                raise ValueError("truncated section table")
            virtual_size, virtual_address, raw_size, raw_offset = struct.unpack_from(
                "<IIII", self.data, offset + 8
            )
            characteristics = struct.unpack_from("<I", self.data, offset + 36)[0]
            self.sections.append(
                Section(
                    virtual_address,
                    virtual_size,
                    raw_offset,
                    raw_size,
                    characteristics,
                )
            )

    def section_for_rva(self, rva: int, size: int) -> Section:
        if rva < 0 or size < 0 or rva + size < rva:
            raise ValueError("invalid RVA range")
        for section in self.sections:
            mapped_size = max(section.virtual_size, section.raw_size)
            delta = rva - section.virtual_address
            if 0 <= delta and delta + size <= mapped_size:
                return section
        raise ValueError(f"RVA 0x{rva:X} is outside mapped sections")

    def read_rva(self, rva: int, size: int) -> bytes:
        section = self.section_for_rva(rva, size)
        delta = rva - section.virtual_address
        if delta + size > section.raw_size:
            raise ValueError(f"RVA 0x{rva:X} has no complete raw-file backing")
        offset = section.raw_offset + delta
        if offset + size > len(self.data):
            raise ValueError(f"RVA 0x{rva:X} points past end of file")
        return self.data[offset : offset + size]


def decode_memory_operation_before_disp(window: bytes, displacement_offset: int) -> str | None:
    """Recognize a small integer r/m32 instruction ending in a disp32 operand."""

    earliest = max(0, displacement_offset - 5)
    for start in range(earliest, displacement_offset):
        cursor = start
        if cursor < displacement_offset and 0x40 <= window[cursor] <= 0x4F:
            cursor += 1
        if cursor >= displacement_offset:
            continue
        opcode = window[cursor]
        mnemonic = MEMORY_READ_OPCODES.get(opcode)
        if mnemonic is None:
            continue
        cursor += 1
        if cursor >= displacement_offset:
            continue
        modrm = window[cursor]
        cursor += 1
        if modrm >> 6 != 0b10:
            continue
        if modrm & 0x07 == 0x04:
            if cursor >= displacement_offset:
                continue
            cursor += 1  # SIB byte
        if cursor == displacement_offset:
            return mnemonic
    return None


def validate_qpid_cost_site(pe: PEImage, rva: int) -> tuple[bool, str, bytes]:
    window = pe.read_rva(rva, 24)
    wanted = struct.pack("<I", QPID_COST_DISPLACEMENT)
    matches: list[str] = []
    position = window.find(wanted)
    while position >= 0:
        mnemonic = decode_memory_operation_before_disp(window, position)
        if mnemonic is not None:
            matches.append(f"{mnemonic} disp32 at +0x{position:X}")
        position = window.find(wanted, position + 1)
    return bool(matches), ", ".join(matches) if matches else "no compatible +0x4E4 operand", window


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Validate the DS2 Chiral Bandwidth Costs v1.0.0 target"
    )
    parser.add_argument("exe", type=Path, help="path to DS2.exe")
    args = parser.parse_args()

    try:
        pe = PEImage(args.exe)
    except (OSError, ValueError, struct.error) as error:
        print(f"ERROR: {error}")
        return 1

    digest = hashlib.sha256(pe.data).hexdigest().upper()
    checks: dict[str, bool] = {
        "machine_x64": pe.machine == 0x8664,
        "timestamp": pe.timestamp == EXPECTED_TIMESTAMP,
        "image_size": pe.image_size == EXPECTED_IMAGE_SIZE,
        "sha256_hard_identity": digest == EXPECTED_SHA256,
    }

    print(f"Target: {args.exe}")
    print("Validation mode: full-file SHA/PE identity plus semantic QpidCost-site checks")
    print(f"Machine: 0x{pe.machine:04X}")
    print(f"PE timestamp: 0x{pe.timestamp:08X}")
    print(f"Image size: 0x{pe.image_size:08X}")
    print(f"SHA-256: {digest}")

    try:
        root_section = pe.section_for_rva(EXPECTED_GLOBAL_ROOT_RVA, 8)
        root_delta = EXPECTED_GLOBAL_ROOT_RVA - root_section.virtual_address
        root_has_raw_backing = root_delta + 8 <= root_section.raw_size
        root_description = (
            pe.read_rva(EXPECTED_GLOBAL_ROOT_RVA, 8).hex(" ").upper()
            if root_has_raw_backing
            else "zero-filled virtual storage"
        )
        checks["construction_root_rva_mapped"] = True
        checks["construction_root_rva_aligned"] = EXPECTED_GLOBAL_ROOT_RVA % 8 == 0
        checks["construction_root_section_writable"] = bool(
            root_section.characteristics & 0x80000000
        )
        print(
            f"Construction root slot @ 0x{EXPECTED_GLOBAL_ROOT_RVA:X}: "
            f"mapped, backing={root_description}, "
            f"section_characteristics=0x{root_section.characteristics:08X}"
        )

        manager_vtable = pe.read_rva(
            EXPECTED_MANAGER_VTABLE_RVA, len(EXPECTED_MANAGER_VTABLE)
        )
        manager_occurrences = pe.data.count(EXPECTED_MANAGER_VTABLE)
        checks["manager_vtable_signature"] = manager_vtable == EXPECTED_MANAGER_VTABLE
        checks["manager_vtable_unique"] = manager_occurrences == 1
        print(
            f"Manager VTable @ 0x{EXPECTED_MANAGER_VTABLE_RVA:X}: "
            f"{'OK' if manager_vtable == EXPECTED_MANAGER_VTABLE else 'FAIL'}"
        )
        print(f"Manager VTable signature occurrences: {manager_occurrences}")

        for site in QPID_COST_SITES:
            actual = pe.read_rva(site.rva, len(site.expected))
            occurrences = pe.data.count(site.expected)
            exact = actual == site.expected
            semantic, detail, context = validate_qpid_cost_site(pe, site.rva)
            checks[f"{site.name}_exact_signature"] = exact
            checks[f"{site.name}_signature_unique"] = occurrences == 1
            checks[f"{site.name}_semantic_qpid_cost"] = semantic
            print(
                f"QpidCost {site.name} @ 0x{site.rva:X}: "
                f"exact={'OK' if exact else 'FAIL'}, "
                f"semantic={'OK' if semantic else 'FAIL'} ({detail}), "
                f"occurrences={occurrences}"
            )
            print(f"  context: {context.hex(' ').upper()}")
    except (ValueError, struct.error) as error:
        print(f"ERROR: {error}")
        return 1

    print("\nCheck summary:")
    for name, ok in checks.items():
        print(f"{name}: {'OK' if ok else 'FAIL'}")
    return 0 if all(checks.values()) else 1


if __name__ == "__main__":
    raise SystemExit(main())
