#!/usr/bin/env python3
"""Offline PE checks for the packaged Infrastructure One Unit ASI."""
from __future__ import annotations

import hashlib
import struct
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
ASI = ROOT / "reference" / "ds2_infrastructure_one_unit.asi"
EXPECTED_SHA256 = "235556A9F4FCF8D45707D16598190FD62CF9DB2C5BEB4BC566050F87B85F64EE"
EXPECTED_IMPORT_DLL = "KERNEL32.dll"
EXPECTED_EXPORT = "DllMain"


def u16(data: bytes, off: int) -> int:
    return struct.unpack_from("<H", data, off)[0]


def u32(data: bytes, off: int) -> int:
    return struct.unpack_from("<I", data, off)[0]


def u64(data: bytes, off: int) -> int:
    return struct.unpack_from("<Q", data, off)[0]


def cstr(data: bytes, off: int) -> str:
    end = data.index(0, off)
    return data[off:end].decode("ascii")


def main() -> int:
    data = ASI.read_bytes()
    assert hashlib.sha256(data).hexdigest().upper() == EXPECTED_SHA256
    assert data[:2] == b"MZ"

    pe = u32(data, 0x3C)
    assert data[pe:pe + 4] == b"PE\0\0"
    coff = pe + 4
    machine = u16(data, coff)
    section_count = u16(data, coff + 2)
    optional_size = u16(data, coff + 16)
    characteristics = u16(data, coff + 18)
    assert machine == 0x8664, hex(machine)
    assert characteristics & 0x2000, "DLL characteristic missing"

    opt = coff + 20
    assert u16(data, opt) == 0x20B
    dll_characteristics = u16(data, opt + 0x46)
    assert dll_characteristics & 0x20, "HIGH_ENTROPY_VA missing"
    assert dll_characteristics & 0x40, "DYNAMIC_BASE missing"
    assert dll_characteristics & 0x100, "NX_COMPAT missing"

    section_table = opt + optional_size
    sections: list[tuple[int, int, int, int]] = []
    for i in range(section_count):
        s = section_table + i * 40
        virtual_size = u32(data, s + 8)
        virtual_address = u32(data, s + 12)
        raw_size = u32(data, s + 16)
        raw_address = u32(data, s + 20)
        sections.append((virtual_address, max(virtual_size, raw_size), raw_address, raw_size))

    def rva_to_off(rva: int) -> int:
        if rva < u32(data, opt + 0x3C):
            return rva
        for va, span, raw, raw_size in sections:
            if va <= rva < va + span:
                delta = rva - va
                assert delta < raw_size, f"RVA 0x{rva:X} has no raw bytes"
                return raw + delta
        raise AssertionError(f"RVA not mapped: 0x{rva:X}")

    data_dir = opt + 0x70
    export_rva = u32(data, data_dir)
    import_rva = u32(data, data_dir + 8)

    import_dlls: list[str] = []
    imp = rva_to_off(import_rva)
    while True:
        original_first_thunk = u32(data, imp)
        name_rva = u32(data, imp + 12)
        first_thunk = u32(data, imp + 16)
        if not (original_first_thunk or name_rva or first_thunk):
            break
        import_dlls.append(cstr(data, rva_to_off(name_rva)))
        imp += 20
    assert import_dlls == [EXPECTED_IMPORT_DLL], import_dlls

    exp = rva_to_off(export_rva)
    name_count = u32(data, exp + 24)
    names_rva = u32(data, exp + 32)
    names_off = rva_to_off(names_rva)
    exports = [cstr(data, rva_to_off(u32(data, names_off + i * 4))) for i in range(name_count)]
    assert EXPECTED_EXPORT in exports, exports

    assert b"DS2 Infrastructure One Unit v1.0.0-release" in data
    assert b"ACTIVE: exact Need/Current caller-pair hooks armed. Known caller pairs: 16." in data

    print("PASS: PE32+ x64 DLL/ASI")
    print("PASS: ASLR, high-entropy ASLR and DEP flags")
    print("PASS: only KERNEL32.dll imported")
    print("PASS: DllMain exported")
    print(f"PASS: SHA-256 {EXPECTED_SHA256}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
