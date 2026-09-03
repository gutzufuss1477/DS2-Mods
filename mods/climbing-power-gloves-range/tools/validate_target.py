#!/usr/bin/env python3
from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path


EXPECTED_SHA256 = "BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B"
EXPECTED_TIMESTAMP = 0x6A3DAE46
EXPECTED_IMAGE_SIZE = 0x0B292000
EXPECTED_IMAGE_BASE = 0x140000000

ITEM_SYSTEM_GLOBAL = 0x0623E5B0
ITEM_SYSTEM_VTABLE = 0x032036E0
ITEM_SYSTEM_DESCRIPTOR = 0x04332200
ITEM_SYSTEM_INIT = 0x01EC40A3
ITEM_SYSTEM_CONSTRUCTOR = 0x00BAC790
ITEM_SYSTEM_RTTI_GETTER = 0x00BAC240
ITEM_SYSTEM_GLOBAL_LOAD = 0x00BAD3E0
ITEM_LOOKUP = 0x00BAD380
ITEM_LOOKUP_CORE = 0x00BAD3A0
ITEM_PARAMETER_RTTI_GETTER = 0x00BAC170
ITEM_PARAMETER_FACTORY = 0x00BAC320
ITEM_PARAMETER_VTABLE = 0x03203920
ITEM_PARAMETER_TYPE_NAME = 0x04330A60
ITEM_PARAMETER_DESCRIPTOR = 0x043320A0

ITEM_SYSTEM_INIT_BYTES = bytes.fromhex(
    "48 8D 0D 56 E1 46 02 E8 31 FD 23 FE "
    "48 8B C8 48 8B D8 E8 D6 86 CE FE "
    "48 89 1D EF A4 37 04 E8 AA 87 CE FE"
)
ITEM_SYSTEM_CONSTRUCTOR_BYTES = bytes.fromhex(
    "40 53 48 83 EC 20 48 8B D9 48 8D 05 40 6F 65 02 "
    "48 89 01 BA 67 00 00 00 33 C0 48 83 C1 10 48 89"
)
ITEM_SYSTEM_RTTI_GETTER_BYTES = bytes.fromhex("48 8D 05 B9 5F 78 03 C3")
ITEM_SYSTEM_GLOBAL_LOAD_BYTES = bytes.fromhex(
    "48 89 5C 24 18 55 48 83 EC 70 48 8B 1D BF 11 69 05 "
    "45 33 D2 49 8B E8 48"
)
ITEM_LOOKUP_BYTES = bytes.fromhex(
    "48 89 5C 24 08 48 63 41 10 45 33 D2 48 8B D9 85 "
    "C0 7E 24 4C 8B 49 18 4C 8B D8 45 8B C2 0F 1F 00"
)
ITEM_LOOKUP_CORE_BYTES = bytes.fromhex(
    "49 8B 01 3A 50 20 74 17 41 FF C2 49 FF C0 49 83 C1 08 4D 3B C3"
)
RTTI_GETTER_BYTES = bytes.fromhex("48 8D 05 29 5F 78 03 C3")
FACTORY_BYTES = bytes.fromhex(
    "48 85 D2 74 6C 33 C9 48 8D 05 F2 75 65 02 "
    "48 89 4A 10 48 89 4A 18 48 89 4A 28 48 89 02 "
    "48 8D 05 5C 89 F8 02 48 89 4A 08 89 4A 20 "
    "48 89 4A 30 48 89 42 38 48 89 4A 40 48 89 4A 48 "
    "48 89 4A 50 48 89 4A 68 48 89 4A 70 48 89 4A 78 "
    "48 89 8A 80 00 00 00 48 89 8A 88 00 00 00 "
    "48 89 8A 90 00 00 00 C7 82 98 00 00 00 FF 00 00 00 "
    "66 89 8A 9C 00 00 00 C3"
)
EXPECTED_ITEM_SYSTEM_VFUNCS = (0x00BAC240, 0x00BAC7E0)
EXPECTED_ITEM_PARAMETER_VFUNCS = (
    0x00BAC170,
    0x00BAC180,
    0x00109DE0,
    0x00109EB0,
)
EXPECTED_TYPE_NAME = b"DSItemParameter\0"
DESCRIPTOR_BYTES = bytes.fromhex(
    "FF FF FF FF 04 00 01 00 00 FF 18 00 00 00 00 00 "
    "A0 00 00 00 08 00 00 00 20 C3 BA 40 01 00 00 00 "
    "A0 C3 BA 40 01 00 00 00 00 00 00 00 00 00 00 00 "
    "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
    "60 0A 33 44 01 00 00 00"
)


class PEImage:
    def __init__(self, path: Path) -> None:
        self.path = path
        with path.open("rb") as stream:
            dos = stream.read(0x1000)
            if dos[:2] != b"MZ":
                raise ValueError("missing MZ header")
            pe = struct.unpack_from("<I", dos, 0x3C)[0]
            stream.seek(pe)
            headers = stream.read(24)
            if headers[:4] != b"PE\0\0":
                raise ValueError("missing PE signature")
            coff = headers[4:]
            if struct.unpack_from("<H", coff, 0)[0] != 0x8664:
                raise ValueError("target is not x64")
            section_count = struct.unpack_from("<H", coff, 2)[0]
            self.timestamp = struct.unpack_from("<I", coff, 4)[0]
            optional_size = struct.unpack_from("<H", coff, 16)[0]
            optional = stream.read(optional_size)
            if len(optional) != optional_size or struct.unpack_from("<H", optional, 0)[0] != 0x20B:
                raise ValueError("target is not PE32+")
            self.image_base = struct.unpack_from("<Q", optional, 24)[0]
            self.image_size = struct.unpack_from("<I", optional, 56)[0]
            self.sections: list[tuple[int, int, int]] = []
            for _ in range(section_count):
                entry = stream.read(40)
                if len(entry) != 40:
                    raise ValueError("truncated section table")
                virtual_size, rva, raw_size, raw = struct.unpack_from("<IIII", entry, 8)
                self.sections.append((rva, raw, min(virtual_size or raw_size, raw_size)))

    def read_rva(self, rva: int, size: int) -> bytes:
        for section_rva, raw, readable_size in self.sections:
            delta = rva - section_rva
            if 0 <= delta and delta + size <= readable_size:
                with self.path.open("rb") as stream:
                    stream.seek(raw + delta)
                    data = stream.read(size)
                if len(data) != size:
                    raise ValueError(f"RVA 0x{rva:X} points past end of file")
                return data
        raise ValueError(f"RVA 0x{rva:X} is outside mapped sections")


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        while chunk := stream.read(8 * 1024 * 1024):
            digest.update(chunk)
    return digest.hexdigest().upper()


def rip_relative_target(rva: int, instruction: bytes, displacement_offset: int) -> int:
    displacement = struct.unpack_from("<i", instruction, displacement_offset)[0]
    return rva + len(instruction) + displacement


def check(label: str, actual: object, expected: object, results: list[bool]) -> None:
    ok = actual == expected
    results.append(ok)
    print(f"{label}: {'OK' if ok else 'FAIL'}")
    if not ok:
        print(f"  got      {actual!r}")
        print(f"  expected {expected!r}")


def validate(path: Path) -> bool:
    image = PEImage(path)
    results: list[bool] = []
    digest = sha256_file(path)

    print(f"Target: {path}")
    print(f"PE timestamp: 0x{image.timestamp:08X}")
    print(f"Image size: 0x{image.image_size:08X}")
    print(f"Image base: 0x{image.image_base:X}")
    print(f"SHA-256: {digest}")

    check("SHA-256", digest, EXPECTED_SHA256, results)
    check("PE timestamp", image.timestamp, EXPECTED_TIMESTAMP, results)
    check("image size", image.image_size, EXPECTED_IMAGE_SIZE, results)
    check("image base", image.image_base, EXPECTED_IMAGE_BASE, results)

    system_init = image.read_rva(ITEM_SYSTEM_INIT, len(ITEM_SYSTEM_INIT_BYTES))
    system_constructor = image.read_rva(
        ITEM_SYSTEM_CONSTRUCTOR, len(ITEM_SYSTEM_CONSTRUCTOR_BYTES)
    )
    system_rtti_getter = image.read_rva(
        ITEM_SYSTEM_RTTI_GETTER, len(ITEM_SYSTEM_RTTI_GETTER_BYTES)
    )
    system_global_load = image.read_rva(
        ITEM_SYSTEM_GLOBAL_LOAD, len(ITEM_SYSTEM_GLOBAL_LOAD_BYTES)
    )
    item_lookup = image.read_rva(ITEM_LOOKUP, len(ITEM_LOOKUP_BYTES))
    item_lookup_core = image.read_rva(ITEM_LOOKUP_CORE, len(ITEM_LOOKUP_CORE_BYTES))
    getter = image.read_rva(ITEM_PARAMETER_RTTI_GETTER, len(RTTI_GETTER_BYTES))
    factory = image.read_rva(ITEM_PARAMETER_FACTORY, len(FACTORY_BYTES))
    system_vtable = struct.unpack("<2Q", image.read_rva(ITEM_SYSTEM_VTABLE, 16))
    parameter_vtable = struct.unpack("<4Q", image.read_rva(ITEM_PARAMETER_VTABLE, 32))
    expected_system_vtable = tuple(
        image.image_base + rva for rva in EXPECTED_ITEM_SYSTEM_VFUNCS
    )
    expected_parameter_vtable = tuple(
        image.image_base + rva for rva in EXPECTED_ITEM_PARAMETER_VFUNCS
    )

    check("DSItemSystem initialization signature", system_init, ITEM_SYSTEM_INIT_BYTES, results)
    check(
        "DSItemSystem initialization descriptor target",
        rip_relative_target(ITEM_SYSTEM_INIT, system_init[:7], 3),
        ITEM_SYSTEM_DESCRIPTOR,
        results,
    )
    init_constructor_displacement = struct.unpack_from("<i", system_init, 19)[0]
    check(
        "DSItemSystem initialization constructor call",
        ITEM_SYSTEM_INIT + 23 + init_constructor_displacement,
        ITEM_SYSTEM_CONSTRUCTOR,
        results,
    )
    check(
        "DSItemSystem initialization global target",
        rip_relative_target(ITEM_SYSTEM_INIT + 23, system_init[23:30], 3),
        ITEM_SYSTEM_GLOBAL,
        results,
    )
    check(
        "DSItemSystem constructor signature",
        system_constructor,
        ITEM_SYSTEM_CONSTRUCTOR_BYTES,
        results,
    )
    check(
        "DSItemSystem constructor VTable target",
        rip_relative_target(
            ITEM_SYSTEM_CONSTRUCTOR + 9, system_constructor[9:16], 3
        ),
        ITEM_SYSTEM_VTABLE,
        results,
    )
    check(
        "DSItemSystem RTTI getter signature",
        system_rtti_getter,
        ITEM_SYSTEM_RTTI_GETTER_BYTES,
        results,
    )
    check(
        "DSItemSystem RTTI getter target",
        rip_relative_target(
            ITEM_SYSTEM_RTTI_GETTER, system_rtti_getter[:7], 3
        ),
        ITEM_SYSTEM_DESCRIPTOR,
        results,
    )
    check(
        "DSItemSystem canonical global-load signature",
        system_global_load,
        ITEM_SYSTEM_GLOBAL_LOAD_BYTES,
        results,
    )
    check(
        "DSItemSystem canonical global-load target",
        rip_relative_target(
            ITEM_SYSTEM_GLOBAL_LOAD + 10, system_global_load[10:17], 3
        ),
        ITEM_SYSTEM_GLOBAL,
        results,
    )
    check("DSItemSystem lookup signature", item_lookup, ITEM_LOOKUP_BYTES, results)
    check(
        "DSItemSystem lookup ID loop",
        item_lookup_core,
        ITEM_LOOKUP_CORE_BYTES,
        results,
    )
    check("DSItemSystem VTable", system_vtable, expected_system_vtable, results)

    check("DSItemParameter RTTI getter signature", getter, RTTI_GETTER_BYTES, results)
    check(
        "DSItemParameter RTTI getter target",
        rip_relative_target(ITEM_PARAMETER_RTTI_GETTER, getter[:7], 3),
        ITEM_PARAMETER_DESCRIPTOR,
        results,
    )
    check("DSItemParameter factory signature", factory, FACTORY_BYTES, results)
    check(
        "DSItemParameter factory VTable target",
        rip_relative_target(ITEM_PARAMETER_FACTORY + 7, factory[7:14], 3),
        ITEM_PARAMETER_VTABLE,
        results,
    )
    check(
        "DSItemParameter VTable",
        parameter_vtable,
        expected_parameter_vtable,
        results,
    )
    check(
        "DSItemParameter type name",
        image.read_rva(ITEM_PARAMETER_TYPE_NAME, len(EXPECTED_TYPE_NAME)),
        EXPECTED_TYPE_NAME,
        results,
    )
    check(
        "DSItemParameter descriptor",
        image.read_rva(ITEM_PARAMETER_DESCRIPTOR, len(DESCRIPTOR_BYTES)),
        DESCRIPTOR_BYTES,
        results,
    )

    print("Documented resource baselines: Lv1 ID=53/level=0/Params[6]=8.0; "
          "Lv2 ID=54/level=1/Params[6]=10.0.")
    return all(results)


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Validate the exact DS2 Climbing Power Gloves Range target"
    )
    parser.add_argument("exe", type=Path, help="path to DS2.exe")
    args = parser.parse_args()
    try:
        ok = validate(args.exe)
    except (OSError, ValueError, struct.error) as error:
        print(f"ERROR: {error}")
        return 1
    print(f"Overall: {'OK - supported target' if ok else 'FAIL - unsupported target'}")
    return 0 if ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
