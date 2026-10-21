#!/usr/bin/env python3
from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path

EXPECTED_TIMESTAMP = 0x6A3DAE46
EXPECTED_IMAGE_SIZE = 0x0B292000
EXPECTED_IMAGE_BASE = 0x140000000
EXPECTED_SHA256 = "BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B"

SIGNATURES = {
    "construction_init": (
        0x127B340,
        bytes.fromhex(
            "4C 8B DC 56 48 81 EC D0 02 00 00 48 8B 05 AE E4 "
            "73 02 48 33 C4 48 89 84 24 20 02 00 00"
        ),
    ),
    "init_dispatch_context_a": (
        0x1260FD0,
        bytes.fromhex(
            "E8 6B EC FF FF 48 8B F0 48 85 C0 74 7E 4C 8B 00 "
            "49 8B D6 48 8B C8 41 FF 90 E0 00 00 00 48 8D 55 "
            "E0 48 8B CF E8 17 1F 01 00"
        ),
    ),
    "init_dispatch_context_b": (
        0x1262044,
        bytes.fromhex(
            "E8 F7 DB FF FF 4C 8B F8 48 85 C0 0F 84 9C 00 00 "
            "00 4C 8B 00 48 8B D7 48 8B C8 41 FF 90 E0 00 00 "
            "00 48 8D 55 E0 48 8B CE E8 9F 0E 01 00"
        ),
    ),
    "construction_update": (
        0x1280950,
        bytes.fromhex(
            "48 8B C4 48 89 58 18 48 89 70 20 55 57 41 56 48 "
            "8D 68 A1 48 81 EC 00 01 00 00"
        ),
    ),
    "update_dispatch_context": (
        0x1281272,
        bytes.fromhex(
            "48 8B 46 38 41 B0 01 48 85 C0 48 8B CE C5 F8 28 CE "
            "4C 8D 78 E0 48 8B 06 4D 0F 44 FD FF 90 40 01 00 00"
        ),
    ),
    "update_dispatch_context_secondary": (
        0x12825E3,
        bytes.fromhex(
            "48 8B 03 45 33 C0 C5 F8 28 CE 48 8B CB FF 90 40 01 00 00 "
            "48 8B CB E8 22 02 00 00"
        ),
    ),
    "material_level_context": (
        0x12809EB,
        bytes.fromhex(
            "48 8B CF E8 DD D1 00 00 0F B6 8F F5 03 00 00 3B "
            "C1 74 1C 4C 8B 07 8B D0 48 8B CF 41 FF 90 38 02 "
            "00 00 EB 0B"
        ),
    ),
    "material_virtual_call": (0x1280A06, bytes.fromhex("41 FF 90 38 02 00 00")),
    "effective_max_getter": (
        0x12D81D0,
        bytes.fromhex(
            "40 53 48 83 EC 20 80 79 28 06 48 8B D9 75 19 B9 "
            "30 00 00 00 E8 97 E6 1A 00 84 C0 75 0B 8B 43 2C "
            "FF C8 48 83 C4 20 5B C3 8B 43 2C 48 83 C4 20 5B C3"
        ),
    ),
    "level_assignment": (
        0x12874F8,
        bytes.fromhex(
            "41 0F B6 86 F5 03 00 00 B9 10 00 00 00 44 3B E0 "
            "45 0F B6 CC 49 8B 46 38 45 88 8E F5 03 00 00"
        ),
    ),
    "completion_equality": (
        0x12875F5,
        bytes.fromhex("41 0F B6 86 F5 03 00 00 3B 81 E8 04 00 00 75 33"),
    ),
    "customizable_equality": (
        0x12876A8,
        bytes.fromhex("41 0F B6 86 F5 03 00 00 41 3B 80 EC 04 00 00 75"),
    ),
    "net_type_copy": (0x127B537, bytes.fromhex("41 0F B6 45 68 41 88 46 30")),
    "spawn_level_seed": (
        0x127B4F0,
        bytes.fromhex(
            "C4 C1 78 10 45 74 C4 C1 78 11 46 58 41 8B 45 6C "
            "41 89 46 70 41 0F B6 45 70 41 88 86 F6 03 00 00 "
            "41 88 86 F5 03 00 00 41 0F B7 45 72 66 41 89 46 "
            "32"
        ),
    ),
    "player_type_compare": (0x12876F3, bytes.fromhex("41 80 7E 30 01")),
    "net_load_compare": (0x127BF55, bytes.fromhex("41 80 7D 68 02")),
    "init_pending_copies": (
        0x127B6AA,
        bytes.fromhex(
            "41 0F B6 85 95 00 00 00 41 88 86 D1 03 00 00 "
            "41 0F B6 85 B2 00 00 00 41 88 86 EE 03 00 00"
        ),
    ),
    "pcc_foundation_pending_seed": (
        0x0C96356,
        bytes.fromhex("66 C7 86 94 00 00 00 01 01"),
    ),
    "foundation_state_entry": (
        0x1262F8F,
        bytes.fromhex(
            "48 8B 06 48 8B CE FF 90 48 01 00 00 B2 02 48 8B "
            "CE E8 EB D5 01 00"
        ),
    ),
    "player_foundation_state_select": (
        0x1266E44,
        bytes.fromhex(
            "48 8B 43 38 48 85 C0 48 8D 48 78 49 0F 44 CE 48 "
            "8B 01 48 C1 E8 30 A8 01 74 0B B2 02 44 38 A7 A4 "
            "00 00 00 74 02 B2 03 48 8B CB E8 1D 97 01 00"
        ),
    ),
    "native_foundation_dwell_gate": (
        0x12828BC,
        bytes.fromhex(
            "80 BF D1 03 00 00 00 74 16 C5 FA 10 05 FB E5 1D 02 "
            "C5 F8 2F 87 24 02 00 00 0F 87 F2 02 00 00"
        ),
    ),
    "foundation_dwell_seconds": (0x3460EC8, bytes.fromhex("CD CC CC 3D")),
    "streaming_active_attach": (
        0x1376219,
        bytes.fromhex(
            "48 8B 49 58 48 85 C9 74 5F 80 79 34 02 75 07 B2 "
            "03 E8 61 A3 F0 FF"
        ),
    ),
    "secondary_update_object_gate": (
        0x137648D,
        bytes.fromhex(
            "48 8B 49 58 80 B9 C8 03 00 00 00 0F 85 FF 00 00 00"
        ),
    ),
    "lifecycle_active_transition": (
        0x12805F3,
        bytes.fromhex(
            "48 8B 03 48 8B CB 40 3A F7 73 0C C6 43 34 03 FF 90 C8 01 "
            "00 00 EB 2D C6 43 34 01 FF 90 C0 01 00 00 EB 21"
        ),
    ),
    "update_level_dirty": (
        0x128098A,
        bytes.fromhex(
            "80 B9 E6 03 00 00 00 74 0E 33 D2 E8 E6 77 00 00 "
            "C6 87 E6 03 00 00"
        ),
    ),
    "update_material_dirty": (
        0x12809BE,
        bytes.fromhex("0F B6 B7 CB 03 00 00 C6 87 CB 03 00"),
    ),
    "material_dirty_rearm": (
        0x1280A0F,
        bytes.fromhex("3C 04 75 07 C6 87 CB 03 00 00 01"),
    ),
    "setlevel_pending_check": (
        0x12874BE,
        bytes.fromhex(
            "45 38 BE D1 03 00 00 75 19 49 8B CE E8 31 EE 97 FF 84 C0 74 0D"
        ),
    ),
    "setlevel_dirty_set": (
        0x12875B4,
        bytes.fromhex("41 C6 86 E6 03 00 00 01 48 8B 48 28 48 85 C9"),
    ),
    "level_terminal_ack": (
        0x1288D74,
        bytes.fromhex(
            "0F B6 86 F5 03 00 00 88 86 F6 03 00 00 C6 86 D1 03 00 00 00 "
            "C6 86 EE 03 00 00 00"
        ),
    ),
    "deferred_callback_queue": (
        0x1287954,
        bytes.fromhex(
            "48 8D 54 24 28 48 89 44 24 28 49 8D 8E E8 02 00 00 "
            "E8 F6 23 E9 FE"
        ),
    ),
    "deferred_callback_drain_check": (
        0x1281BF7,
        bytes.fromhex(
            "45 8B F5 44 39 AE E8 02 00 00 0F 8E 0B 01 00 00 "
            "41 BF 01 00 00 00"
        ),
    ),
    "deferred_callback_drain_remove": (
        0x1281CB8,
        bytes.fromhex(
            "8B 86 E8 02 00 00 48 8B 9E F0 02 00 00 41 2B C6 FF C8 4C 63 "
            "C0 49 63 C7 49 C1 E0 03 48 8D 14 C3 49 63 C6 48 8D 0C C3 E8 "
            "D6 86 84 01 FF 8E E8 02 00 00"
        ),
    ),
    "manager_callback_object_queue": (
        0x1287162,
        bytes.fromhex(
            "48 8B 01 4C 8B F1 48 89 44 24 20 48 8D A8 E8 02 00 00"
        ),
    ),
    "manager_callback_enqueue_primary": (
        0x12871E2,
        bytes.fromhex(
            "48 8D 54 24 50 48 89 44 24 50 48 8B CD E8 6C 2B E9 FE"
        ),
    ),
    "manager_callback_enqueue_secondary": (
        0x128727B,
        bytes.fromhex(
            "48 8D 54 24 50 48 89 44 24 50 48 8D 8B E8 02 00 00 "
            "E8 CF 2A E9 FE"
        ),
    ),
    "applied_ack_callback_primary": (
        0x1299B6D,
        bytes.fromhex(
            "48 8B 4B 10 0F B6 81 F5 03 00 00 88 81 F6 03 00 00"
        ),
    ),
    "applied_ack_callback_secondary": (
        0x1299D60,
        bytes.fromhex(
            "48 8B 4F 10 0F B6 81 F5 03 00 00 88 81 F6 03 00 00"
        ),
    ),
}

VTABLES: dict[str, tuple[int, tuple[int, ...]]] = {
    "manager": (0x3280940, (0x12426B0, 0x1274280, 0x109DE0, 0x109EB0)),
    "facility_common": (0x32827C8, (0x1242690, 0x291C50, 0x109DE0, 0x109EB0)),
    "Postbox": (0x3291028, (0x1305660, 0x1305670, 0x109DE0, 0x109EB0)),
    "SafeHouse": (0x32913C0, (0x1307B90, 0x1307C60, 0x109DE0, 0x109EB0)),
    "Watchtower": (0x327F8F8, (0x1245220, 0x1245230, 0x109DE0, 0x109EB0)),
    "CargoCatapult": (0x3293488, (0x1311E70, 0x1311FF0, 0x109DE0, 0x109EB0)),
    "Generator": (0x32938E8, (0x131A0F0, 0x131A220, 0x109DE0, 0x109EB0)),
    "RainShelter": (0x3291EC0, (0x1306030, 0x13060D0, 0x109DE0, 0x109EB0)),
    "Zipline": (0x32863A8, (0x1243560, 0x1243720, 0x109DE0, 0x109EB0)),
    "Bridge": (0x3292F68, (0x130D8F0, 0x130DA00, 0x109DE0, 0x109EB0)),
    "ChiralBridge": (0x3292870, (0x131CEC0, 0x131CED0, 0x109DE0, 0x109EB0)),
    "JumpRamp": (0x3291CF0, (0x12EE520, 0x12EE610, 0x109DE0, 0x109EB0)),
    "FastTravelStation": (0x3291068, (0x12EABB0, 0x1247010, 0x109DE0, 0x109EB0)),
}

OBJECT_HOOK_SLOTS: dict[str, tuple[int, int, int]] = {
    "DSPost": (0x327F920, 0x12EACC0, 0x1287400),
    "DSSafetyHouse": (0x327F460, 0x1307E10, 0x13084A0),
    "DSWatchTower": (0x32820F8, 0x12455B0, 0x1287400),
    "DSCatapult": (0x3283E08, 0x13123F0, 0x1287400),
    "DSCharger": (0x3284108, 0x12EACC0, 0x1287400),
    "DSRainShelter": (0x32851C8, 0x12EACC0, 0x1287400),
    "DSZipline": (0x32824A8, 0x124AAC0, 0x1287400),
    "DSBridge": (0x32838E8, 0x130F3F0, 0x130E3C0),
    "DSChiralBridge": (0x3285978, 0x131D460, 0x1287400),
    "DSJumpStand": (0x3285580, 0x12EEB10, 0x12EEE10),
    "DSFastTravelStation": (0x32816D8, 0x12EACC0, 0x1287400),
}

UNIQUE_TEXT_SIGNATURES = (
    "construction_init",
    "init_dispatch_context_a",
    "init_dispatch_context_b",
    "update_dispatch_context_secondary",
    "material_virtual_call",
    "spawn_level_seed",
    "pcc_foundation_pending_seed",
    "foundation_state_entry",
    "player_foundation_state_select",
    "native_foundation_dwell_gate",
    "streaming_active_attach",
    "secondary_update_object_gate",
    "level_terminal_ack",
    "deferred_callback_queue",
    "manager_callback_object_queue",
    "manager_callback_enqueue_primary",
    "manager_callback_enqueue_secondary",
    "applied_ack_callback_primary",
    "applied_ack_callback_secondary",
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
        self.machine = struct.unpack_from("<H", self.data, coff)[0]
        section_count = struct.unpack_from("<H", self.data, coff + 2)[0]
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
        for index in range(section_count):
            offset = section_table + index * 40
            if offset + 40 > len(self.data):
                raise ValueError("truncated section table")
            name = self.data[offset : offset + 8].split(b"\0", 1)[0]
            virtual_size, virtual_address, raw_size, raw_offset = struct.unpack_from(
                "<IIII", self.data, offset + 8
            )
            self.sections[name] = (virtual_address, virtual_size, raw_offset, raw_size)

    def read_rva(self, rva: int, size: int) -> bytes:
        for virtual_address, virtual_size, raw_offset, raw_size in self.sections.values():
            span = max(virtual_size, raw_size)
            relative = rva - virtual_address
            if 0 <= relative and relative + size <= span and relative + size <= raw_size:
                start = raw_offset + relative
                return self.data[start : start + size]
        raise ValueError(f"RVA 0x{rva:X} is not backed by file data")


def main() -> int:
    parser = argparse.ArgumentParser(description="Validate the DS2 construction mod target")
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
        "image_base": pe.image_base == EXPECTED_IMAGE_BASE,
        "sha256": digest == EXPECTED_SHA256,
    }
    print(f"Target: {args.exe}")
    print(f"PE timestamp: 0x{pe.timestamp:08X}")
    print(f"Image size: 0x{pe.image_size:08X}")
    print(f"Image base: 0x{pe.image_base:X}")
    print(f"SHA-256: {digest}")

    for name, (rva, expected) in SIGNATURES.items():
        try:
            actual = pe.read_rva(rva, len(expected))
            checks[f"signature_{name}"] = actual == expected
        except ValueError:
            checks[f"signature_{name}"] = False
        print(f"Signature {name} at RVA 0x{rva:X}: {'OK' if checks[f'signature_{name}'] else 'FAIL'}")

    for name, (table_rva, function_rvas) in VTABLES.items():
        try:
            actual = struct.unpack("<4Q", pe.read_rva(table_rva, 32))
            expected = tuple(pe.image_base + rva for rva in function_rvas)
            checks[f"vtable_{name}"] = actual == expected
        except (ValueError, struct.error):
            checks[f"vtable_{name}"] = False
        print(f"VTable {name} at RVA 0x{table_rva:X}: {'OK' if checks[f'vtable_{name}'] else 'FAIL'}")

    for name, (table_rva, init_rva, setter_rva) in OBJECT_HOOK_SLOTS.items():
        init_slot_rva = table_rva + 0xE0
        update_slot_rva = table_rva + 0x140
        setter_slot_rva = table_rva + 0x238
        try:
            init_actual = struct.unpack("<Q", pe.read_rva(init_slot_rva, 8))[0]
            update_actual = struct.unpack("<Q", pe.read_rva(update_slot_rva, 8))[0]
            setter_actual = struct.unpack("<Q", pe.read_rva(setter_slot_rva, 8))[0]
            checks[f"init_{name}"] = init_actual == pe.image_base + init_rva
            checks[f"update_{name}"] = update_actual == pe.image_base + 0x1280950
            checks[f"setlevel_{name}"] = setter_actual == pe.image_base + setter_rva
        except (ValueError, struct.error):
            checks[f"init_{name}"] = False
            checks[f"update_{name}"] = False
            checks[f"setlevel_{name}"] = False
        print(
            f"Object Init slot {name} at RVA 0x{init_slot_rva:X}: "
            f"{'OK' if checks[f'init_{name}'] else 'FAIL'}"
        )
        print(
            f"Object Update slot {name} at RVA 0x{update_slot_rva:X}: "
            f"{'OK' if checks[f'update_{name}'] else 'FAIL'}"
        )
        print(
            f"Object SetLevel slot {name} at RVA 0x{setter_slot_rva:X}: "
            f"{'OK' if checks[f'setlevel_{name}'] else 'FAIL'}"
        )

    text_section = pe.sections.get(b".text")
    if text_section:
        _rva, _virtual_size, raw_offset, raw_size = text_section
        text_data = pe.data[raw_offset : raw_offset + raw_size]
        for signature_name in UNIQUE_TEXT_SIGNATURES:
            signature_bytes = SIGNATURES[signature_name][1]
            occurrence_count = text_data.count(signature_bytes)
            checks[f"unique_{signature_name}"] = occurrence_count == 1
            print(
                f"Exact {signature_name} byte sequence in .text: "
                f"{occurrence_count} occurrence(s)"
            )
    else:
        for signature_name in UNIQUE_TEXT_SIGNATURES:
            checks[f"unique_{signature_name}"] = False
        print("Exact byte-sequence uniqueness checks: .text section missing")

    print("Canonical player target configs: 13 (including three bridge variants and Fast Travel).")
    print("Native level triples validated at runtime: ordinary 3/1/2; SafeHouse/bridges 4/2/3.")
    for name, passed in checks.items():
        print(f"{name}: {'OK' if passed else 'FAIL'}")
    return 0 if all(checks.values()) else 1


if __name__ == "__main__":
    raise SystemExit(main())
