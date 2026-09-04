#!/usr/bin/env python3
from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path

EXPECTED_TIMESTAMP = 0x6A3DAE46
EXPECTED_IMAGE_SIZE = 0x0B292000
EXPECTED_SHA256 = "BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B"
EXPECTED_STREAMING_SIGNATURE_RVA = 0x00693674
EXPECTED_STREAMING_GLOBAL_RVA = 0x06266938
EXPECTED_STREAMING_SYSTEM_VTABLE_RVA = 0x034532A0
EXPECTED_STREAMING_ADD_LISTENER_RVA = 0x026F6E40
EXPECTED_STREAMING_REMOVE_LISTENER_RVA = 0x026F6EE0
EXPECTED_DRIVE_HOOK_RVA = 0x0247A431
EXPECTED_SPEED_TELEMETRY_STORE_RVA = 0x0247A744
EXPECTED_STEERING_OUTPUT_HOOK_RVA = 0x01F46AF6
EXPECTED_COFFIN_VTABLE_RVA = 0x033AD0B0
EXPECTED_COFFIN_VFUNCS = (0x01F430B0, 0x01F4E160, 0x01F480A0, 0x01F49350)
EXPECTED_COFFIN_STEERING_VFUNC = 0x01F46AB0
EXPECTED_STEERING_DEGREE_LOAD_RVA = 0x01F46AD7
EXPECTED_STEERING_RAD_MULTIPLY_RVA = 0x01F46AEE
EXPECTED_DEGREES_TO_RADIANS_RVA = 0x03460D94
EXPECTED_WET_SIDE_GRIP_CONSUMER_RVA = 0x01F48C92
EXPECTED_COFFIN_LINKED_OBJECT_TYPE_LIMIT_RVA = 0x01007D8E
EXPECTED_COFFIN_PRESERVE_CARRIER_HOOK_RVA = 0x00F9A807
EXPECTED_CARRIER_DETACH_EVENT_GATE_RVA = 0x00E5C7AA
EXPECTED_CARRIER_WARNING_NOTIFICATION_GATE_RVA = 0x0121D839
EXPECTED_RIDE_VEHICLE_CARRIER_GATE_RVA = 0x01011B6F
EXPECTED_RIDE_VEHICLE_RESULT_GATE_RVA = 0x010049B0
EXPECTED_RIDE_VEHICLE_FALLBACK_GATE_RVA = 0x01004A8A

DRIVE_HOOK_SIGNATURE = bytes.fromhex(
    "C4 C1 4A 59 CC C5 FA 59 C1 C5 FA 11 47 18 C5 E2 5C C6"
)
SPEED_TELEMETRY_STORE = bytes.fromhex("C5 FA 11 83 20 01 00 00")
STEERING_DEGREE_LOAD = bytes.fromhex("C5 FA 10 50 40")
STEERING_RAD_MULTIPLY = bytes.fromhex("C5 EA 59 05 9E A2 51 01")
STEERING_OUTPUT_HOOK_SIGNATURE = bytes.fromhex(
    "C5 E2 59 E0 45 8B C1 4C 63 10 49 83 FA 04 7C 72"
)
WET_SIDE_GRIP_CONSUMER = bytes.fromhex(
    "48 8B 47 48 C5 FA 10 80 C8 00 00 00 48 8B 44 24 50 "
    "C5 FA 59 08 C5 FA 11 08"
)
COFFIN_LINKED_OBJECT_TYPE_LIMIT = bytes.fromhex(
    "41 8D 45 FF 83 F8 01 76 49"
)
COFFIN_PRESERVE_CARRIER_HOOK = bytes.fromhex(
    "48 8B 8F A8 00 00 00 E8 DD BA EA FF 48 85 C0 "
    "0F 84 4F 04 00 00"
)
CARRIER_DETACH_EVENT_GATE = bytes.fromhex(
    "8B 42 10 4C 8B EA 4C 8B 61 48 48 8B F1 "
    "3D E0 4F 75 13 0F 84 11 06 00 00"
)
CARRIER_WARNING_NOTIFICATION_GATE = bytes.fromhex(
    "C7 44 24 50 B0 9B 66 16 89 44 24 58 E8 16 34 F1 FE"
)
RIDE_VEHICLE_CARRIER_GATE = bytes.fromhex(
    "0F 85 87 00 00 00 48 8B 43 28 83 B8 48 75 00 00 01"
)
RIDE_VEHICLE_RESULT_GATE = bytes.fromhex(
    "8B 4C 24 24 83 F9 01 0F 85 93 00 00 00 "
    "81 BC 24 80 00 00 00 58 01 00 00"
)
RIDE_VEHICLE_FALLBACK_GATE = bytes.fromhex(
    "84 C0 0F 84 AC FE FF FF 48 8B 44 24 30 "
    "48 89 83 20 02 00 00"
)

STREAM_SIGNATURE = bytes.fromhex(
    "48 89 05 00 00 00 00 E8 00 00 00 00 33 D2 41 B8 "
    "F8 0A 00 00 48 8B C8 48 8B D8 E8 00"
)
STREAM_MASK = bytes(
    (1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0)
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
        self.image_base = struct.unpack_from("<Q", self.data, optional + 24)[0]
        self.image_size = struct.unpack_from("<I", self.data, optional + 56)[0]
        section_table = optional + optional_size
        self.sections: dict[bytes, tuple[int, int, int, int]] = {}
        for index in range(self.section_count):
            offset = section_table + index * 40
            if offset + 40 > len(self.data):
                raise ValueError("truncated section table")
            name = self.data[offset : offset + 8].split(b"\0", 1)[0]
            virtual_size, virtual_address, raw_size, raw_offset = struct.unpack_from(
                "<IIII", self.data, offset + 8
            )
            self.sections[name] = (virtual_address, virtual_size, raw_offset, raw_size)

    def read_rva(self, rva: int, size: int) -> bytes:
        matches = []
        for virtual_address, virtual_size, raw_offset, raw_size in self.sections.values():
            if virtual_address <= rva and rva + size <= virtual_address + max(virtual_size, raw_size):
                offset = raw_offset + rva - virtual_address
                matches.append(self.data[offset : offset + size])
        if len(matches) != 1 or len(matches[0]) != size:
            raise ValueError(f"RVA 0x{rva:X} does not map uniquely")
        return matches[0]


def masked_matches(data: bytes, pattern: bytes, mask: bytes) -> list[int]:
    matches: list[int] = []
    size = len(pattern)
    anchor = next(index for index, required in enumerate(mask) if required)
    anchor_value = pattern[anchor]
    start = 0
    while True:
        candidate = data.find(bytes((anchor_value,)), start)
        if candidate < 0:
            return matches
        offset = candidate - anchor
        start = candidate + 1
        if offset < 0 or offset + size > len(data):
            continue
        if all(not mask[index] or data[offset + index] == pattern[index] for index in range(size)):
            matches.append(offset)


def main() -> int:
    parser = argparse.ArgumentParser(description="Validate the DS2 Coffin Board mod target")
    parser.add_argument("exe", type=Path, help="path to DS2.exe")
    args = parser.parse_args()
    try:
        pe = PEImage(args.exe)
    except (OSError, ValueError, struct.error) as error:
        print(f"ERROR: {error}")
        return 1

    digest = hashlib.sha256(pe.data).hexdigest().upper()
    checks: dict[str, bool] = {
        "timestamp": pe.timestamp == EXPECTED_TIMESTAMP,
        "image_size": pe.image_size == EXPECTED_IMAGE_SIZE,
        "sha256": digest == EXPECTED_SHA256,
    }
    print(f"Target: {args.exe}")
    print(f"PE timestamp: 0x{pe.timestamp:08X}")
    print(f"Image size: 0x{pe.image_size:08X}")
    print(f"SHA-256: {digest}")

    text_section = pe.sections.get(b".text")
    if not text_section:
        print("ERROR: .text section not found")
        return 1
    text_rva, _virtual_size, text_offset, text_raw_size = text_section
    text = pe.data[text_offset : text_offset + text_raw_size]
    matches = masked_matches(text, STREAM_SIGNATURE, STREAM_MASK)
    checks["unique_streaming_signature"] = len(matches) == 1
    print(f"StreamingManager signature matches: {len(matches)}")
    if len(matches) == 1:
        file_offset = text_offset + matches[0]
        instruction_rva = text_rva + matches[0]
        displacement = struct.unpack_from("<i", pe.data, file_offset + 3)[0]
        global_rva = instruction_rva + 7 + displacement
        checks["exact_streaming_signature_rva"] = (
            instruction_rva == EXPECTED_STREAMING_SIGNATURE_RVA
        )
        checks["exact_streaming_global_rva"] = global_rva == EXPECTED_STREAMING_GLOBAL_RVA
        print(f"StreamingManager signature RVA: 0x{instruction_rva:X}")
        print(f"StreamingManager global RVA: 0x{global_rva:X}")

    streaming_listener_slots = struct.unpack(
        "<2Q", pe.read_rva(EXPECTED_STREAMING_SYSTEM_VTABLE_RVA + 3 * 8, 16)
    )
    expected_streaming_listener_slots = (
        pe.image_base + EXPECTED_STREAMING_ADD_LISTENER_RVA,
        pe.image_base + EXPECTED_STREAMING_REMOVE_LISTENER_RVA,
    )
    checks["exact_streaming_add_remove_slots"] = (
        streaming_listener_slots == expected_streaming_listener_slots
    )
    print(
        "StreamingSystem listener slots 3/4: " +
        ", ".join(f"0x{address:X}" for address in streaming_listener_slots)
    )

    drive_matches = [
        text_rva + offset
        for offset in range(len(text))
        if text.startswith(DRIVE_HOOK_SIGNATURE, offset)
    ]
    checks["unique_drive_hook_signature"] = drive_matches == [EXPECTED_DRIVE_HOOK_RVA]
    print(
        "Coffin drive-moment hook RVA(s): " +
        (", ".join(f"0x{rva:X}" for rva in drive_matches) if drive_matches else "none")
    )
    telemetry_offset = EXPECTED_SPEED_TELEMETRY_STORE_RVA - text_rva
    checks["exact_speed_telemetry_store"] = (
        telemetry_offset >= 0 and
        text.startswith(SPEED_TELEMETRY_STORE, telemetry_offset)
    )
    print(f"Vehicle km/h telemetry store RVA: 0x{EXPECTED_SPEED_TELEMETRY_STORE_RVA:X}")
    checks["exact_steering_degree_load"] = (
        pe.read_rva(EXPECTED_STEERING_DEGREE_LOAD_RVA, len(STEERING_DEGREE_LOAD)) ==
        STEERING_DEGREE_LOAD
    )
    checks["exact_steering_radian_multiply"] = (
        pe.read_rva(EXPECTED_STEERING_RAD_MULTIPLY_RVA, len(STEERING_RAD_MULTIPLY)) ==
        STEERING_RAD_MULTIPLY
    )
    checks["exact_steering_output_hook"] = (
        pe.read_rva(
            EXPECTED_STEERING_OUTPUT_HOOK_RVA,
            len(STEERING_OUTPUT_HOOK_SIGNATURE),
        ) == STEERING_OUTPUT_HOOK_SIGNATURE
    )
    checks["exact_wet_side_grip_consumer"] = (
        pe.read_rva(
            EXPECTED_WET_SIDE_GRIP_CONSUMER_RVA,
            len(WET_SIDE_GRIP_CONSUMER),
        ) == WET_SIDE_GRIP_CONSUMER
    )
    checks["exact_coffin_linked_object_type_limit"] = (
        pe.read_rva(
            EXPECTED_COFFIN_LINKED_OBJECT_TYPE_LIMIT_RVA,
            len(COFFIN_LINKED_OBJECT_TYPE_LIMIT),
        ) == COFFIN_LINKED_OBJECT_TYPE_LIMIT
    )
    checks["exact_coffin_preserve_carrier_hook"] = (
        pe.read_rva(
            EXPECTED_COFFIN_PRESERVE_CARRIER_HOOK_RVA,
            len(COFFIN_PRESERVE_CARRIER_HOOK),
        ) == COFFIN_PRESERVE_CARRIER_HOOK
    )
    checks["exact_carrier_detach_event_gate"] = (
        pe.read_rva(
            EXPECTED_CARRIER_DETACH_EVENT_GATE_RVA,
            len(CARRIER_DETACH_EVENT_GATE),
        ) == CARRIER_DETACH_EVENT_GATE
    )
    checks["exact_carrier_warning_notification_gate"] = (
        pe.read_rva(
            EXPECTED_CARRIER_WARNING_NOTIFICATION_GATE_RVA,
            len(CARRIER_WARNING_NOTIFICATION_GATE),
        ) == CARRIER_WARNING_NOTIFICATION_GATE
    )
    checks["exact_ride_vehicle_carrier_gate"] = (
        pe.read_rva(
            EXPECTED_RIDE_VEHICLE_CARRIER_GATE_RVA,
            len(RIDE_VEHICLE_CARRIER_GATE),
        ) == RIDE_VEHICLE_CARRIER_GATE
    )
    checks["exact_ride_vehicle_result_gate"] = (
        pe.read_rva(
            EXPECTED_RIDE_VEHICLE_RESULT_GATE_RVA,
            len(RIDE_VEHICLE_RESULT_GATE),
        ) == RIDE_VEHICLE_RESULT_GATE
    )
    checks["exact_ride_vehicle_fallback_gate"] = (
        pe.read_rva(
            EXPECTED_RIDE_VEHICLE_FALLBACK_GATE_RVA,
            len(RIDE_VEHICLE_FALLBACK_GATE),
        ) == RIDE_VEHICLE_FALLBACK_GATE
    )
    degrees_to_radians = struct.unpack(
        "<f", pe.read_rva(EXPECTED_DEGREES_TO_RADIANS_RVA, 4)
    )[0]
    checks["exact_degrees_to_radians_constant"] = abs(
        degrees_to_radians - 0.0174532923847
    ) < 1e-9
    print(
        f"SteeringDegree consumer RVA: 0x{EXPECTED_STEERING_DEGREE_LOAD_RVA:X}; "
        f"degrees-to-radians={degrees_to_radians:.12f}"
    )
    expected_vtable = tuple(pe.image_base + rva for rva in EXPECTED_COFFIN_VFUNCS)
    actual_vtable = struct.unpack("<4Q", pe.read_rva(EXPECTED_COFFIN_VTABLE_RVA, 32))
    checks["exact_coffin_vtable"] = actual_vtable == expected_vtable
    steering_vfunc = struct.unpack(
        "<Q", pe.read_rva(EXPECTED_COFFIN_VTABLE_RVA + 6 * 8, 8)
    )[0]
    checks["exact_coffin_steering_vfunc"] = (
        steering_vfunc == pe.image_base + EXPECTED_COFFIN_STEERING_VFUNC
    )
    print(
        f"Coffin component vtable RVA 0x{EXPECTED_COFFIN_VTABLE_RVA:X}: " +
        ", ".join(f"0x{address:X}" for address in actual_vtable)
    )
    print(
        f"Coffin steering-output hook RVA: 0x{EXPECTED_STEERING_OUTPUT_HOOK_RVA:X}; "
        f"vtable slot 6=0x{steering_vfunc:X}"
    )
    print(
        f"Coffin wet side-grip +0xC8 consumer RVA: "
        f"0x{EXPECTED_WET_SIDE_GRIP_CONSUMER_RVA:X}"
    )
    print(
        f"RideVehicle Floating Carrier gate RVA: "
        f"0x{EXPECTED_RIDE_VEHICLE_CARRIER_GATE_RVA:X}"
    )
    print(
        f"Coffin linked-object gate / carrier-preserve hook RVAs: "
        f"0x{EXPECTED_COFFIN_LINKED_OBJECT_TYPE_LIMIT_RVA:X}, "
        f"0x{EXPECTED_COFFIN_PRESERVE_CARRIER_HOOK_RVA:X}"
    )
    print(
        f"Floating Carrier detach-event gate RVA: "
        f"0x{EXPECTED_CARRIER_DETACH_EVENT_GATE_RVA:X}"
    )
    print(
        f"Floating Carrier overextension-notification gate RVA: "
        f"0x{EXPECTED_CARRIER_WARNING_NOTIFICATION_GATE_RVA:X}"
    )
    print(
        f"RideVehicle result/fallback gates: "
        f"0x{EXPECTED_RIDE_VEHICLE_RESULT_GATE_RVA:X}, "
        f"0x{EXPECTED_RIDE_VEHICLE_FALLBACK_GATE_RVA:X}"
    )

    type_names = (
        b"DSPhysicsCoffinResource\0",
        b"DSPlayerRideCoffinConfig\0",
    )
    for type_name in type_names:
        count = pe.data.count(type_name)
        key = type_name[:-1].decode("ascii")
        checks[f"rtti_{key}"] = count >= 1
        print(f"RTTI name {key}: {count} occurrence(s)")

    print(
        "Odradek resource baseline: "
        "land=40, landBoost=60, water=60, waterBoost=80, "
        "finalGearRatio=15, SteeringDegree=50, wetSideGrip=0.09, slip=39, "
        "rider StandardSpeed=40 km/h; "
        "physics group/index=499/71288 (group size >= 87603)."
    )
    for name, passed in checks.items():
        print(f"{name}: {'OK' if passed else 'FAIL'}")
    return 0 if all(checks.values()) else 1


if __name__ == "__main__":
    raise SystemExit(main())
