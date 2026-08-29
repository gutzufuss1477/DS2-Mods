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
EXPECTED_UI_CACHE_ROOT_RVA = 0x0623EBF8
EXPECTED_FIXED_RIDEABLE_SQUARED_RVA = 0x03462408
EXPECTED_FIXED_RIDEABLE_SQUARED = 123201.0
EXPECTED_TRAVEL_SPEED_RVA = 0x03461ED0
EXPECTED_TRAVEL_SPEED = 16.0


@dataclass(frozen=True)
class Signature:
    name: str
    rva: int
    data: bytes


SIGNATURES = (
    Signature(
        "RangeGetter",
        0x0124A8E0,
        bytes.fromhex(
            "48 8B 05 F1 41 FF 04 48 8B 40 28 48 85 C0 75 13 "
            "48 63 89 E0 01 00 00 33 C0 C5 FA 10 84 88 80 07 00 00 C3 "
            "48 8B 40 68 48 85 C0 74 E4 48 8B 40 30 48 63 89 E0 01 00 00 "
            "C5 FA 10 84 88 80 07 00 00 C3"
        ),
    ),
    Signature(
        "CandidateDiscovery",
        0x012CB0B8,
        bytes.fromhex(
            "48 8B 05 19 3A F7 04 48 8B 48 28 48 85 C9 75 06 33 C0 8B C8 "
            "EB 13 48 8B 49 68 48 85 C9 75 06 33 C0 8B C8 EB 04 48 8B 49 "
            "30 48 8B 84 24 28 02 00 00 48 63 40 18 C5 4A 5F 84 81 80 07 00 00"
        ),
    ),
    Signature(
        "FinalValidation",
        0x0124B0F0,
        bytes.fromhex(
            "48 8B 4D 28 48 85 C9 75 05 48 8B CE EB 12 48 8B 49 68 48 85 C9 "
            "75 05 48 8B CE EB 04 48 8B 49 30 49 63 47 18 C5 C2 5F BC 81 80 07 00 00"
        ),
    ),
    Signature(
        "ConstructorDefaults",
        0x012432B5,
        bytes.fromhex(
            "C7 83 84 07 00 00 00 00 96 43 C7 83 88 07 00 00 00 80 AF 43 "
            "C7 83 8C 07 00 00 00 80 AF 43 C7 83 90 07 00 00 00 80 AF 43 "
            "C7 83 94 07 00 00 00 80 AF 43"
        ),
    ),
    Signature(
        "RealizationUsage",
        0x01268031,
        bytes.fromhex(
            "48 8B 40 30 C5 FA 10 B0 D4 04 00 00 80 FA 0E 74 0A 48 8B 84 D1 38 01 00 00 "
            "EB 0A 41 0F B6 46 28 48 8B 44 C1 20 48 85 C0 75 05 49 8B C0 EB 04 48 8B 40 "
            "30 41 80 BD 50 07 00 00 00 C5 FA 10 B8 D8 04 00 00"
        ),
    ),
    Signature(
        "DistanceToOpenDefault",
        0x012435A6,
        bytes.fromhex("C7 83 A0 07 00 00 00 00 7A 44"),
    ),
    Signature(
        "ZiplineUiDistanceUsage",
        0x017CE1D5,
        bytes.fromhex(
            "48 8B 1D 1C 0A A7 04 4C 39 A3 D0 00 00 00 75 1A "
            "E8 D6 CE EA FE 48 8B 88 58 02 00 00 48 8B 05 00 0A A7 04 "
            "48 89 88 D0 00 00 00 48 8B 83 D0 00 00 00 48 8B 48 50 "
            "C5 CA 5F B1 84 00 00 00"
        ),
    ),
    Signature(
        "FixedRideableConnectionGate",
        0x0124C94D,
        bytes.fromhex(
            "45 84 F6 75 24 C5 F8 2F 3D AE 5A 21 02 "
            "0F 87 7C 05 00 00 C5 FA 10 85 28 01 00 00"
        ),
    ),
    Signature(
        "RideableFinalConfiguredGate",
        0x0124CEE0,
        bytes.fromhex(
            "45 84 F6 75 0A C4 C1 78 2F F9 0F 96 C0 EB 02 B0 01"
        ),
    ),
    Signature(
        "ZiplineConnectionObstacleResult",
        0x0124D1B5,
        bytes.fromhex(
            "85 C0 0F 94 C0 EB 02 32 C0 4C 8D 9C 24 58 01 00 00"
        ),
    ),
    Signature(
        "ZiplineTravelProgress",
        0x010389D1,
        bytes.fromhex(
            "48 8B 49 30 48 8B 96 D8 00 00 00 C5 FA 10 0D EC 94 42 02 "
            "80 7A 08 00 74 13 80 BA 87 01 00 00 00 74 0A "
            "C5 F2 59 89 DC 07 00 00 EB 28 49 83 B8 20 02 00 00 FF "
            "74 10 48 83 F8 FF 74 10 C5 F2 59 89 D8 07 00 00 EB 0E "
            "48 83 F8 FF 74 08 C5 F2 59 89 D4 07 00 00 "
            "C5 F2 59 C6 C5 FA 58 8E 90 01 00 00 C5 FA 11 8E 90 01 00 00"
        ),
    ),
    Signature(
        "ZiplineCarrierSpeedFactors",
        0x01243607,
        bytes.fromhex(
            "C7 83 D4 07 00 00 33 33 33 3F "
            "C7 83 D8 07 00 00 33 33 33 3F "
            "C7 83 DC 07 00 00 33 33 33 3F"
        ),
    ),
    Signature(
        "DSConstructionManagerConfigVTable",
        0x03280940,
        bytes.fromhex(
            "B0 26 24 41 01 00 00 00 80 42 27 41 01 00 00 00 "
            "E0 9D 10 40 01 00 00 00 B0 9E 10 40 01 00 00 00"
        ),
    ),
    Signature(
        "DSConstructionFacilityResourceVTable",
        0x032827C8,
        bytes.fromhex(
            "90 26 24 41 01 00 00 00 50 1C 29 40 01 00 00 00 "
            "E0 9D 10 40 01 00 00 00 B0 9E 10 40 01 00 00 00"
        ),
    ),
    Signature(
        "DSZiplineConstructionConfigVTable",
        0x032863A8,
        bytes.fromhex(
            "60 35 24 41 01 00 00 00 20 37 24 41 01 00 00 00 "
            "E0 9D 10 40 01 00 00 00 B0 9E 10 40 01 00 00 00"
        ),
    ),
    Signature(
        "EntityResourceVTable",
        0x0311B4F0,
        bytes.fromhex(
            "D0 B2 15 40 01 00 00 00 F0 4E 18 40 01 00 00 00 "
            "E0 9D 10 40 01 00 00 00 B0 9E 10 40 01 00 00 00"
        ),
    ),
    Signature(
        "DSZiplineImaginaryMeshComponentResourceVTable",
        0x03281BC0,
        bytes.fromhex(
            "20 34 24 41 01 00 00 00 40 34 24 41 01 00 00 00 "
            "E0 9D 10 40 01 00 00 00 B0 9E 10 40 01 00 00 00"
        ),
    ),
    Signature(
        "DSUIResidentResourceVTable",
        0x032BAE58,
        bytes.fromhex(
            "40 21 48 41 01 00 00 00 D0 9A 48 41 01 00 00 00 "
            "E0 9D 10 40 01 00 00 00 B0 9E 10 40 01 00 00 00"
        ),
    ),
    Signature(
        "DSUIGameDesignParametersVTable",
        0x032BAB60,
        bytes.fromhex(
            "30 21 48 41 01 00 00 00 00 92 48 41 01 00 00 00 "
            "E0 9D 10 40 01 00 00 00 B0 9E 10 40 01 00 00 00"
        ),
    ),
)


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
        self.sections: list[tuple[int, int, int]] = []
        for index in range(self.section_count):
            offset = section_table + index * 40
            _virtual_size, virtual_address, raw_size, raw_offset = struct.unpack_from(
                "<IIII", self.data, offset + 8
            )
            self.sections.append((virtual_address, raw_offset, raw_size))

    def read_rva(self, rva: int, size: int) -> bytes:
        for virtual_address, raw_offset, raw_size in self.sections:
            delta = rva - virtual_address
            if 0 <= delta and delta + size <= raw_size:
                offset = raw_offset + delta
                if offset + size > len(self.data):
                    raise ValueError(f"RVA 0x{rva:X} points past end of file")
                return self.data[offset : offset + size]
        raise ValueError(f"RVA 0x{rva:X} is outside mapped sections")


def main() -> int:
    parser = argparse.ArgumentParser(description="Validate the DS2 Zipline Range target")
    parser.add_argument("exe", type=Path, help="path to DS2.exe")
    args = parser.parse_args()

    try:
        pe = PEImage(args.exe)
    except (OSError, ValueError, struct.error) as error:
        print(f"ERROR: {error}")
        return 1

    digest = hashlib.sha256(pe.data).hexdigest().upper()
    checks = {
        "timestamp": pe.timestamp == EXPECTED_TIMESTAMP,
        "image_size": pe.image_size == EXPECTED_IMAGE_SIZE,
        "sha256": digest == EXPECTED_SHA256,
    }

    print(f"Target: {args.exe}")
    print(f"PE timestamp: 0x{pe.timestamp:08X}")
    print(f"Image size: 0x{pe.image_size:08X}")
    print(f"SHA-256: {digest}")
    try:
        for signature in SIGNATURES:
            actual = pe.read_rva(signature.rva, len(signature.data))
            occurrences = pe.data.count(signature.data)
            checks[f"{signature.name}_signature"] = actual == signature.data
            checks[f"{signature.name}_unique"] = occurrences == 1
            print(f"{signature.name} @ 0x{signature.rva:X}: {'OK' if actual == signature.data else 'FAIL'}")
            print(f"{signature.name} occurrences: {occurrences}")

        getter = pe.read_rva(0x0124A8E0, 7)
        displacement = struct.unpack_from("<i", getter, 3)[0]
        global_root_rva = 0x0124A8E0 + 7 + displacement
        checks["global_root_rva"] = global_root_rva == EXPECTED_GLOBAL_ROOT_RVA
        print(f"Resolved construction root global: 0x{global_root_rva:X}")

        ui_usage = pe.read_rva(0x017CE1D5, 35)
        ui_displacement_1 = struct.unpack_from("<i", ui_usage, 3)[0]
        ui_root_rva_1 = 0x017CE1D5 + 7 + ui_displacement_1
        ui_displacement_2 = struct.unpack_from("<i", ui_usage, 31)[0]
        ui_root_rva_2 = 0x017CE1D5 + 35 + ui_displacement_2
        checks["ui_cache_root_rva_first"] = ui_root_rva_1 == EXPECTED_UI_CACHE_ROOT_RVA
        checks["ui_cache_root_rva_second"] = ui_root_rva_2 == EXPECTED_UI_CACHE_ROOT_RVA
        print(f"Resolved UI cache global (first load): 0x{ui_root_rva_1:X}")
        print(f"Resolved UI cache global (second load): 0x{ui_root_rva_2:X}")

        fixed_compare = pe.read_rva(0x0124C952, 8)
        fixed_displacement = struct.unpack_from("<i", fixed_compare, 4)[0]
        fixed_squared_rva = 0x0124C952 + len(fixed_compare) + fixed_displacement
        fixed_squared = struct.unpack("<f", pe.read_rva(fixed_squared_rva, 4))[0]
        checks["fixed_rideable_constant_rva"] = (
            fixed_squared_rva == EXPECTED_FIXED_RIDEABLE_SQUARED_RVA
        )
        checks["fixed_rideable_constant_value"] = (
            fixed_squared == EXPECTED_FIXED_RIDEABLE_SQUARED
        )
        checks["atomic_patch_block_alignment"] = 0x0124C950 % 16 == 0
        print(f"Resolved fixed rideable squared-distance constant: 0x{fixed_squared_rva:X}")
        print(f"Fixed rideable squared-distance value: {fixed_squared:.1f} (= 351^2)")
        print("Atomic replacement block @ 0x124C950: 16-byte aligned")

        travel_load = pe.read_rva(0x010389DC, 8)
        travel_displacement = struct.unpack_from("<i", travel_load, 4)[0]
        travel_speed_rva = 0x010389DC + len(travel_load) + travel_displacement
        travel_speed = struct.unpack("<f", pe.read_rva(travel_speed_rva, 4))[0]
        checks["travel_speed_constant_rva"] = travel_speed_rva == EXPECTED_TRAVEL_SPEED_RVA
        checks["travel_speed_constant_value"] = travel_speed == EXPECTED_TRAVEL_SPEED
        checks["atomic_travel_displacement_alignment"] = 0x010389E0 % 4 == 0
        print(f"Resolved Zipline base travel speed constant: 0x{travel_speed_rva:X}")
        print(f"Zipline base travel speed: {travel_speed:.1f} m/s")
        print("Atomic travel-speed displacement @ 0x10389E0: 4-byte aligned")
    except (ValueError, struct.error) as error:
        print(f"ERROR: {error}")
        return 1

    print(
        "Documented resource baselines (not read by this validator): "
        "MaxLevel=3; PassableRadius=301/351/351/351/351; "
        "MarkerHUDZiplineInZiplineModeVisibleDistance=400; "
        "Cart1Speed/Cart2Speed/ADSSpeed=0.7."
    )
    for name, ok in checks.items():
        print(f"{name}: {'OK' if ok else 'FAIL'}")
    return 0 if all(checks.values()) else 1


if __name__ == "__main__":
    raise SystemExit(main())
