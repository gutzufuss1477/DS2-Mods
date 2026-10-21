#!/usr/bin/env python3
from __future__ import annotations

import argparse
import hashlib
import json
import struct
from pathlib import Path


EXPECTED_SHA256 = "BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B"
EXPECTED_TIMESTAMP = 0x6A3DAE46
EXPECTED_IMAGE_SIZE = 0x0B292000

SURVEY_FACTORY = 0x01D73FA0
SURVEY_CONSTRUCTOR = 0x01D89140
SURVEY_VTABLE = 0x0338E748
SURVEY_FACTORY_SLOT = 0x0449AF08
GAMEPLAY_SCAN_START = 0x00DE2420
GAMEPLAY_SCAN_SLOT = 0x0323B560
SENSOR_GATE_RADIUS_LOAD = 0x00DE6FDD
SENSOR_GATE_CALL_WINDOW = 0x01CE5AD0
BODY_SECONDARY_VTABLE = 0x0323B530
SENSOR_VTABLE = 0x0323B190
SENSOR_MANAGER_VTABLE = 0x0323AF90
EFFECT_INSTANCE_VTABLE = 0x0338E2C8
EFFECT_RESOURCE_VTABLE = 0x0338E5C8
UI_RESIDENT_VTABLE = 0x032BAE58
UI_GAME_PARAMS_VTABLE = 0x032BAB60
UI_CACHE_ROOT = 0x0623EBF8
UI_CACHE_ROOT_LOAD = 0x017C57E6
UI_CACHE_RESIDENT_FLOW = 0x017C57ED
UI_CACHE_ROOT_LAZY_LOAD = 0x017C5803
UI_SEARCHED_DISTANCE_LOAD = 0x017C5818
BAGGAGE_MANAGER_GLOBAL = 0x0623EA48
BAGGAGE_MANAGER_VTABLE = 0x0327CA78
BAGGAGE_MANAGER_SECONDARY_VTABLE = 0x0327C570
BAGGAGE_CONFIG_VTABLE = 0x0327CBE0
BAGGAGE_CONFIG_LOAD = 0x011C564E
BAGGAGE_CONFIG_CACHE_STORES = 0x011C58A1
BAGGAGE_MARKER_UNTOUCHED_LOAD = 0x011BC6A3
BAGGAGE_MARKER_TOUCHED_LOAD = 0x011BC692
BAGGAGE_MARKER_MISSION_LOAD = 0x011BC64B
BAGGAGE_FOUND_DISTANCE_LOAD = 0x011AF4BC
PLAYER_RESOURCE_ROOT = 0x0623DF40
PLAYER_RESOURCE_ROOT_LOAD = 0x00D845B6
PLAYER_RESOURCE_ENUMERATION = 0x00D845C5
PLAYER_ODRADEK_CONTEXT_PATH = 0x00D845F9
PLAYER_ODRADEK_CONFIG_GETTER = 0x00DA07A0
PLAYER_ODRADEK_CONFIG_VTABLE = 0x032459A8
DETECTED_CONSTANT_UPDATE_CORE = 0x01CE57C0
DETECTED_CONSTANT_UPDATE_THUNK = 0x01CF5A40
DETECTED_CONSTANT_UPDATE_MESSAGE = 0x041CEA30
DETECTED_CONSTANT_UPDATE_MESSAGE_SLOT = 0x04490180
DETECTED_CONSTANT_UPDATE_HANDLER_SLOT = 0x04490188
DETECTED_COMPONENT_VTABLE = 0x033855D8
ENTITY_MANAGER_GAME_UPDATE = 0x006FD510
ENTITY_MANAGER_GAME_VTABLE = 0x0318A498
ENTITY_MANAGER_GAME_UPDATE_SLOT = 0x0318A4A8
ENTITY_MANAGER_GAME_COL = 0x034FB058
ENTITY_MANAGER_GAME_TYPE_DESCRIPTOR = 0x060E80D0
ENTITY_REQUEST_REALIZE = 0x001312B0
REALIZE_COORDINATOR = 0x00171D90
REALIZE_COORDINATOR_CALL_WINDOW = 0x001826C8
FIND_ENTITY_COMPONENT = 0x0011FFA0
BAGGAGE_COMPONENT_VTABLE = 0x0327C440
REALIZED_BAGGAGE_VTABLE = 0x0327CF28
REALIZED_FOUND_BRIDGE = 0x011BC740
REALIZED_MARKER_BRIDGE = 0x011BC520
REALIZED_BAGGAGE_INIT_CORE = 0x011B4C90
REALIZED_BAGGAGE_INIT_THUNK = 0x01200190
REALIZED_BAGGAGE_INIT_MESSAGE = 0x041D5220
REALIZED_BAGGAGE_INIT_MESSAGE_SLOT = 0x0436BD90
REALIZED_BAGGAGE_INIT_HANDLER_SLOT = 0x0436BD98
REALIZED_BAGGAGE_UNLINK_CORE = 0x011B5460
REALIZED_BAGGAGE_UNLINK_THUNK = 0x01200160
REALIZED_BAGGAGE_UNLINK_MESSAGE = 0x041D4040
REALIZED_BAGGAGE_UNLINK_MESSAGE_SLOT = 0x0436BDC0
REALIZED_BAGGAGE_UNLINK_HANDLER_SLOT = 0x0436BDC8
REALIZED_BAGGAGE_UPDATE_CORE = 0x011BB340
REALIZED_BAGGAGE_UPDATE_THUNK = 0x01200170
REALIZED_BAGGAGE_UPDATE_MESSAGE = 0x041D45C0
REALIZED_BAGGAGE_UPDATE_MESSAGE_SLOT = 0x0436BDB0
REALIZED_BAGGAGE_UPDATE_HANDLER_SLOT = 0x0436BDB8

SURVEY_FACTORY_BYTES = bytes.fromhex(
    "48 85 D2 74 08 48 8B CA E9 93 51 01 00 C3 CC CC"
)
SURVEY_CONSTRUCTOR_BYTES = bytes.fromhex(
    "33 D2 48 8D 05 FF 55 60 01 48 89 51 10 48 89 51 18 48 89 51 50"
)
GAMEPLAY_SCAN_START_BYTES = bytes.fromhex(
    "48 83 EC 48 48 8B 81 A0 00 00 00 48 8B 89 A8 00 00 00 "
    "C5 FB 10 80 80 02 00 00"
)
SENSOR_GATE_RADIUS_LOAD_BYTES = bytes.fromhex("C5 FA 10 83 98 00 00 00")
SENSOR_GATE_CALL_WINDOW_BYTES = bytes.fromhex("00 E8 6A 13 10 FF 84 C0")
UI_CACHE_ROOT_LOAD_OPCODE = bytes.fromhex("48 8B 1D")
UI_CACHE_ROOT_LAZY_LOAD_OPCODE = bytes.fromhex("48 8B 05")
UI_CACHE_RESIDENT_FLOW_BYTES = bytes.fromhex(
    "48 83 BB D0 00 00 00 00 75 1A E8 C4 58 EB FE "
    "48 8B 88 58 02 00 00 48 8B 05 EE 93 A7 04 "
    "48 89 88 D0 00 00 00 48 8B 83 D0 00 00 00"
)
UI_SEARCHED_DISTANCE_LOAD_BYTES = bytes.fromhex(
    "48 8B 48 50 C5 FA 10 41 30"
)
BAGGAGE_CONFIG_LOAD_BYTES = bytes.fromhex("48 8B 3D F3 93 07 05")
BAGGAGE_CONFIG_CACHE_STORES_BYTES = bytes.fromhex(
    "C5 FA 10 46 30 C5 FA 59 C8 C5 FA 11 8F 38 66 03 00 "
    "C5 FA 10 56 34 C5 EA 59 C2 C5 FA 11 87 3C 66 03 00 "
    "C5 FA 10 4E 38 C5 F2 59 D1 C5 FA 11 97 40 66 03 00"
)
BAGGAGE_MARKER_UNTOUCHED_LOAD_BYTES = bytes.fromhex(
    "C5 FA 10 80 38 66 03 00 C5 F8 2F 86 F8 01 00 00 "
    "0F 93 C0 84 C0 74 6C 8B"
)
BAGGAGE_MARKER_TOUCHED_LOAD_BYTES = bytes.fromhex(
    "C5 FA 10 80 3C 66 03 00"
)
BAGGAGE_MARKER_MISSION_LOAD_BYTES = bytes.fromhex(
    "C5 FA 10 80 40 66 03 00"
)
BAGGAGE_FOUND_DISTANCE_LOAD_BYTES = bytes.fromhex("C5 FA 10 41 48")
PLAYER_RESOURCE_ROOT_LOAD_OPCODE = bytes.fromhex("48 8B 05")
PLAYER_RESOURCE_ENUMERATION_BYTES = bytes.fromhex(
    "48 8B 50 40 48 63 40 38 4C 8D 04 C2 49 3B D0 74 12 "
    "48 8B 02 83 78 70 00 74 1A 48 83 C2 08 49 3B D0 75 EE"
)
PLAYER_ODRADEK_CONTEXT_PATH_BYTES = bytes.fromhex(
    "48 85 C0 74 EA 48 8B 48 48 48 85 C9 74 E1 "
    "48 8B 89 C8 56 00 00 E8 8D C1 01 00 8B 48 68"
)
PLAYER_ODRADEK_CONFIG_GETTER_BYTES = bytes.fromhex(
    "48 83 EC 28 48 8B 41 30 48 8B 40 28 48 85 C0 75"
)
DETECTED_CONSTANT_UPDATE_THUNK_BYTES = bytes.fromhex(
    "E9 7B FD FE FF CC CC CC"
)
DETECTED_CONSTANT_UPDATE_CORE_BYTES = bytes.fromhex(
    "40 55 57 41 54 41 56 41 57 48 8D 6C 24 C9 48 81"
)
ENTITY_MANAGER_GAME_UPDATE_BYTES = bytes.fromhex(
    "48 89 5C 24 08 57 48 83 EC 20 48 8B FA 48 8B D9 "
    "E8 DB B0 A7 FF 80 3F 02"
)
ENTITY_REQUEST_REALIZE_BYTES = bytes.fromhex(
    "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20"
)
REALIZE_COORDINATOR_BYTES = bytes.fromhex(
    "40 56 57 B8 78 00 08 00 E8 63 6D 95 02 48 2B E0"
)
REALIZE_COORDINATOR_CALL_WINDOW_BYTES = bytes.fromhex(
    "8B CB E8 C1 F6 FE FF 48"
)
ENTITY_MANAGER_GAME_TYPE_NAME = b".?AVEntityManagerGame@@\0"
FIND_ENTITY_COMPONENT_BYTES = bytes.fromhex(
    "48 89 5C 24 08 4C 8B 49 18 4C 8B D9 4C 63 41 10"
)
REALIZED_FOUND_BRIDGE_BYTES = bytes.fromhex(
    "4C 8B DC 49 89 5B 18 49 89 73 20 41 56 48 81 EC"
)
REALIZED_MARKER_BRIDGE_BYTES = bytes.fromhex(
    "48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 8B"
)
REALIZED_BAGGAGE_INIT_THUNK_BYTES = bytes.fromhex(
    "E9 FB 4A FB FF CC CC CC"
)
REALIZED_BAGGAGE_INIT_CORE_BYTES = bytes.fromhex(
    "48 89 5C 24 10 48 89 74 24 18 48 89 7C 24 20 41"
)
REALIZED_BAGGAGE_UNLINK_THUNK_BYTES = bytes.fromhex(
    "E9 FB 52 FB FF CC CC CC"
)
REALIZED_BAGGAGE_UNLINK_CORE_BYTES = bytes.fromhex(
    "48 89 5C 24 18 48 89 74 24 20 41 56 48 83 EC 20"
)
REALIZED_BAGGAGE_UPDATE_THUNK_BYTES = bytes.fromhex(
    "E9 CB B1 FB FF CC CC CC"
)
REALIZED_BAGGAGE_UPDATE_CORE_BYTES = bytes.fromhex(
    "4C 8B DC 57 41 56 48 81 EC 38 02 00 00 4C 8B 71"
)
SURVEY_VFUNCS = (0x01D73F20, 0x01D89730, 0x00109DE0, 0x00109EB0)
BODY_SECONDARY_VFUNCS = (0x00DEBB10, 0x00DD7AE0, 0x00DCFD10, 0x00DCFDF0)
SENSOR_VFUNCS = (0x00DD7BF0, 0x00DD7C00, 0x00109DE0, 0x00109EB0)
SENSOR_MANAGER_VFUNCS = (0x00DD7BD0, 0x00DE6550, 0x03596660, 0x00DE7A10)
EFFECT_INSTANCE_VFUNCS = (0x01D70110, 0x01D70400, 0x00109DE0, 0x00109EB0)
EFFECT_RESOURCE_VFUNCS = (0x01D70120, 0x01D896E0, 0x00109DE0, 0x00109EB0)
UI_RESIDENT_VFUNCS = (0x01482140, 0x01489AD0, 0x00109DE0, 0x00109EB0)
UI_GAME_PARAMS_VFUNCS = (0x01482130, 0x01489200, 0x00109DE0, 0x00109EB0)
BAGGAGE_MANAGER_VFUNCS = (0x011AEE20, 0x011C2C90, 0x035B7250, 0x00DCCB80)
BAGGAGE_MANAGER_SECONDARY_VFUNCS = (
    0x0120B9EC,
    0x000A10E0,
    0x000A10E0,
    0x000A10E0,
)
BAGGAGE_CONFIG_VFUNCS = (0x011AE5F0, 0x011AEBC0, 0x00109DE0, 0x00109EB0)
PLAYER_ODRADEK_CONFIG_VFUNCS = (
    0x00E3EE20,
    0x00E60430,
    0x00109DE0,
    0x00109EB0,
)
DETECTED_COMPONENT_VFUNCS = (
    0x01CE53E0,
    0x01CE53F0,
    0x00109DE0,
    0x00109EB0,
)
ENTITY_MANAGER_GAME_VFUNCS = (
    0x00174B20,
    0x00174FB0,
    0x006FD510,
    0x006FD570,
)
BAGGAGE_COMPONENT_VFUNCS = (
    0x011AF150,
    0x011AF1C0,
    0x00109DE0,
    0x00109EB0,
)
REALIZED_BAGGAGE_VFUNCS = (
    0x011AF220,
    0x011AF340,
    0x00109DE0,
    0x00109EB0,
)


class PEImage:
    def __init__(self, path: Path) -> None:
        self.path = path
        self.data = path.read_bytes()
        if self.data[:2] != b"MZ":
            raise ValueError("missing MZ header")
        pe = struct.unpack_from("<I", self.data, 0x3C)[0]
        if self.data[pe : pe + 4] != b"PE\0\0":
            raise ValueError("missing PE signature")
        coff = pe + 4
        if struct.unpack_from("<H", self.data, coff)[0] != 0x8664:
            raise ValueError("target is not x64")
        section_count = struct.unpack_from("<H", self.data, coff + 2)[0]
        self.timestamp = struct.unpack_from("<I", self.data, coff + 4)[0]
        optional_size = struct.unpack_from("<H", self.data, coff + 16)[0]
        optional = coff + 20
        if struct.unpack_from("<H", self.data, optional)[0] != 0x20B:
            raise ValueError("target is not PE32+")
        self.image_base = struct.unpack_from("<Q", self.data, optional + 24)[0]
        self.image_size = struct.unpack_from("<I", self.data, optional + 56)[0]
        table = optional + optional_size
        self.sections: list[tuple[int, int, int]] = []
        for index in range(section_count):
            entry = table + index * 40
            virtual_size, rva, raw_size, raw = struct.unpack_from(
                "<IIII", self.data, entry + 8
            )
            self.sections.append((rva, raw, max(virtual_size, raw_size)))

    def read_rva(self, rva: int, size: int) -> bytes:
        for section_rva, raw, mapped_size in self.sections:
            delta = rva - section_rva
            if 0 <= delta and delta + size <= mapped_size:
                start = raw + delta
                return self.data[start : start + size]
        raise ValueError(f"RVA 0x{rva:X} is outside mapped sections")


def require_equal(label: str, actual: object, expected: object) -> None:
    if actual != expected:
        raise ValueError(f"{label} mismatch: got {actual!r}, expected {expected!r}")


def validate_vtable(image: PEImage, label: str, rva: int, functions: tuple[int, ...]) -> None:
    require_equal(
        label,
        struct.unpack(f"<{len(functions)}Q", image.read_rva(rva, len(functions) * 8)),
        tuple(image.image_base + function for function in functions),
    )


def validate_rip_relative_target(
    image: PEImage, label: str, rva: int, opcode: bytes, target_rva: int
) -> None:
    instruction = image.read_rva(rva, len(opcode) + 4)
    require_equal(f"{label} opcode", instruction[: len(opcode)], opcode)
    displacement = struct.unpack_from("<i", instruction, len(opcode))[0]
    require_equal(
        f"{label} target",
        rva + len(instruction) + displacement,
        target_rva,
    )


def validate(path: Path) -> None:
    image = PEImage(path)
    require_equal("SHA-256", hashlib.sha256(image.data).hexdigest().upper(), EXPECTED_SHA256)
    require_equal("PE timestamp", image.timestamp, EXPECTED_TIMESTAMP)
    require_equal("image size", image.image_size, EXPECTED_IMAGE_SIZE)
    require_equal(
        "survey factory", image.read_rva(SURVEY_FACTORY, len(SURVEY_FACTORY_BYTES)), SURVEY_FACTORY_BYTES
    )
    require_equal(
        "survey constructor",
        image.read_rva(SURVEY_CONSTRUCTOR, len(SURVEY_CONSTRUCTOR_BYTES)),
        SURVEY_CONSTRUCTOR_BYTES,
    )
    require_equal(
        "gameplay scan wrapper",
        image.read_rva(GAMEPLAY_SCAN_START, len(GAMEPLAY_SCAN_START_BYTES)),
        GAMEPLAY_SCAN_START_BYTES,
    )
    require_equal(
        "sensor acquisition-gate radius load",
        image.read_rva(SENSOR_GATE_RADIUS_LOAD, len(SENSOR_GATE_RADIUS_LOAD_BYTES)),
        SENSOR_GATE_RADIUS_LOAD_BYTES,
    )
    require_equal(
        "sensor acquisition-gate call window",
        image.read_rva(SENSOR_GATE_CALL_WINDOW, len(SENSOR_GATE_CALL_WINDOW_BYTES)),
        SENSOR_GATE_CALL_WINDOW_BYTES,
    )
    validate_rip_relative_target(
        image,
        "UI cache-root load",
        UI_CACHE_ROOT_LOAD,
        UI_CACHE_ROOT_LOAD_OPCODE,
        UI_CACHE_ROOT,
    )
    validate_rip_relative_target(
        image,
        "UI cache-root lazy load",
        UI_CACHE_ROOT_LAZY_LOAD,
        UI_CACHE_ROOT_LAZY_LOAD_OPCODE,
        UI_CACHE_ROOT,
    )
    require_equal(
        "UI cache resident flow",
        image.read_rva(UI_CACHE_RESIDENT_FLOW, len(UI_CACHE_RESIDENT_FLOW_BYTES)),
        UI_CACHE_RESIDENT_FLOW_BYTES,
    )
    require_equal(
        "UI searched-distance load",
        image.read_rva(
            UI_SEARCHED_DISTANCE_LOAD, len(UI_SEARCHED_DISTANCE_LOAD_BYTES)
        ),
        UI_SEARCHED_DISTANCE_LOAD_BYTES,
    )
    require_equal(
        "baggage-manager root load",
        image.read_rva(BAGGAGE_CONFIG_LOAD, len(BAGGAGE_CONFIG_LOAD_BYTES)),
        BAGGAGE_CONFIG_LOAD_BYTES,
    )
    validate_rip_relative_target(
        image,
        "baggage-manager canonical root",
        BAGGAGE_CONFIG_LOAD,
        bytes.fromhex("48 8B 3D"),
        BAGGAGE_MANAGER_GLOBAL,
    )
    require_equal(
        "baggage marker cache builder",
        image.read_rva(
            BAGGAGE_CONFIG_CACHE_STORES, len(BAGGAGE_CONFIG_CACHE_STORES_BYTES)
        ),
        BAGGAGE_CONFIG_CACHE_STORES_BYTES,
    )
    require_equal(
        "untouched-baggage marker gate",
        image.read_rva(
            BAGGAGE_MARKER_UNTOUCHED_LOAD,
            len(BAGGAGE_MARKER_UNTOUCHED_LOAD_BYTES),
        ),
        BAGGAGE_MARKER_UNTOUCHED_LOAD_BYTES,
    )
    require_equal(
        "touched-baggage marker gate",
        image.read_rva(
            BAGGAGE_MARKER_TOUCHED_LOAD, len(BAGGAGE_MARKER_TOUCHED_LOAD_BYTES)
        ),
        BAGGAGE_MARKER_TOUCHED_LOAD_BYTES,
    )
    require_equal(
        "mission-baggage marker gate",
        image.read_rva(
            BAGGAGE_MARKER_MISSION_LOAD, len(BAGGAGE_MARKER_MISSION_LOAD_BYTES)
        ),
        BAGGAGE_MARKER_MISSION_LOAD_BYTES,
    )
    require_equal(
        "baggage-found HUD distance load",
        image.read_rva(
            BAGGAGE_FOUND_DISTANCE_LOAD, len(BAGGAGE_FOUND_DISTANCE_LOAD_BYTES)
        ),
        BAGGAGE_FOUND_DISTANCE_LOAD_BYTES,
    )
    validate_rip_relative_target(
        image,
        "player-resource canonical root",
        PLAYER_RESOURCE_ROOT_LOAD,
        PLAYER_RESOURCE_ROOT_LOAD_OPCODE,
        PLAYER_RESOURCE_ROOT,
    )
    require_equal(
        "player-resource kind-zero enumeration",
        image.read_rva(
            PLAYER_RESOURCE_ENUMERATION,
            len(PLAYER_RESOURCE_ENUMERATION_BYTES),
        ),
        PLAYER_RESOURCE_ENUMERATION_BYTES,
    )
    require_equal(
        "player Odradek context path",
        image.read_rva(
            PLAYER_ODRADEK_CONTEXT_PATH,
            len(PLAYER_ODRADEK_CONTEXT_PATH_BYTES),
        ),
        PLAYER_ODRADEK_CONTEXT_PATH_BYTES,
    )
    require_equal(
        "DSPlayerOdradekConfig getter",
        image.read_rva(
            PLAYER_ODRADEK_CONFIG_GETTER,
            len(PLAYER_ODRADEK_CONFIG_GETTER_BYTES),
        ),
        PLAYER_ODRADEK_CONFIG_GETTER_BYTES,
    )
    require_equal(
        "detected-constant update thunk",
        image.read_rva(
            DETECTED_CONSTANT_UPDATE_THUNK,
            len(DETECTED_CONSTANT_UPDATE_THUNK_BYTES),
        ),
        DETECTED_CONSTANT_UPDATE_THUNK_BYTES,
    )
    require_equal(
        "detected-constant update core",
        image.read_rva(
            DETECTED_CONSTANT_UPDATE_CORE,
            len(DETECTED_CONSTANT_UPDATE_CORE_BYTES),
        ),
        DETECTED_CONSTANT_UPDATE_CORE_BYTES,
    )
    require_equal(
        "EntityManagerGame update",
        image.read_rva(
            ENTITY_MANAGER_GAME_UPDATE,
            len(ENTITY_MANAGER_GAME_UPDATE_BYTES),
        ),
        ENTITY_MANAGER_GAME_UPDATE_BYTES,
    )
    require_equal(
        "selective entity-realize request",
        image.read_rva(
            ENTITY_REQUEST_REALIZE,
            len(ENTITY_REQUEST_REALIZE_BYTES),
        ),
        ENTITY_REQUEST_REALIZE_BYTES,
    )
    require_equal(
        "realization coordinator",
        image.read_rva(
            REALIZE_COORDINATOR,
            len(REALIZE_COORDINATOR_BYTES),
        ),
        REALIZE_COORDINATOR_BYTES,
    )
    require_equal(
        "realization coordinator call window",
        image.read_rva(
            REALIZE_COORDINATOR_CALL_WINDOW,
            len(REALIZE_COORDINATOR_CALL_WINDOW_BYTES),
        ),
        REALIZE_COORDINATOR_CALL_WINDOW_BYTES,
    )
    require_equal(
        "entity-component finder",
        image.read_rva(
            FIND_ENTITY_COMPONENT,
            len(FIND_ENTITY_COMPONENT_BYTES),
        ),
        FIND_ENTITY_COMPONENT_BYTES,
    )
    require_equal(
        "realized-baggage found bridge",
        image.read_rva(
            REALIZED_FOUND_BRIDGE,
            len(REALIZED_FOUND_BRIDGE_BYTES),
        ),
        REALIZED_FOUND_BRIDGE_BYTES,
    )
    require_equal(
        "realized-baggage marker bridge",
        image.read_rva(
            REALIZED_MARKER_BRIDGE,
            len(REALIZED_MARKER_BRIDGE_BYTES),
        ),
        REALIZED_MARKER_BRIDGE_BYTES,
    )
    require_equal(
        "realized-baggage init thunk",
        image.read_rva(
            REALIZED_BAGGAGE_INIT_THUNK,
            len(REALIZED_BAGGAGE_INIT_THUNK_BYTES),
        ),
        REALIZED_BAGGAGE_INIT_THUNK_BYTES,
    )
    require_equal(
        "realized-baggage init core",
        image.read_rva(
            REALIZED_BAGGAGE_INIT_CORE,
            len(REALIZED_BAGGAGE_INIT_CORE_BYTES),
        ),
        REALIZED_BAGGAGE_INIT_CORE_BYTES,
    )
    require_equal(
        "realized-baggage unlink thunk",
        image.read_rva(
            REALIZED_BAGGAGE_UNLINK_THUNK,
            len(REALIZED_BAGGAGE_UNLINK_THUNK_BYTES),
        ),
        REALIZED_BAGGAGE_UNLINK_THUNK_BYTES,
    )
    require_equal(
        "realized-baggage unlink core",
        image.read_rva(
            REALIZED_BAGGAGE_UNLINK_CORE,
            len(REALIZED_BAGGAGE_UNLINK_CORE_BYTES),
        ),
        REALIZED_BAGGAGE_UNLINK_CORE_BYTES,
    )
    require_equal(
        "realized-baggage update thunk",
        image.read_rva(
            REALIZED_BAGGAGE_UPDATE_THUNK,
            len(REALIZED_BAGGAGE_UPDATE_THUNK_BYTES),
        ),
        REALIZED_BAGGAGE_UPDATE_THUNK_BYTES,
    )
    require_equal(
        "realized-baggage update core",
        image.read_rva(
            REALIZED_BAGGAGE_UPDATE_CORE,
            len(REALIZED_BAGGAGE_UPDATE_CORE_BYTES),
        ),
        REALIZED_BAGGAGE_UPDATE_CORE_BYTES,
    )
    require_equal(
        "survey factory slot",
        struct.unpack("<Q", image.read_rva(SURVEY_FACTORY_SLOT, 8))[0],
        image.image_base + SURVEY_FACTORY,
    )
    require_equal(
        "gameplay scan slot",
        struct.unpack("<Q", image.read_rva(GAMEPLAY_SCAN_SLOT, 8))[0],
        image.image_base + GAMEPLAY_SCAN_START,
    )
    require_equal(
        "detected-constant update message slot",
        struct.unpack(
            "<Q", image.read_rva(DETECTED_CONSTANT_UPDATE_MESSAGE_SLOT, 8)
        )[0],
        image.image_base + DETECTED_CONSTANT_UPDATE_MESSAGE,
    )
    require_equal(
        "detected-constant update handler slot",
        struct.unpack(
            "<Q", image.read_rva(DETECTED_CONSTANT_UPDATE_HANDLER_SLOT, 8)
        )[0],
        image.image_base + DETECTED_CONSTANT_UPDATE_THUNK,
    )
    require_equal(
        "realized-baggage init message/handler pair",
        struct.unpack(
            "<2Q",
            image.read_rva(
                REALIZED_BAGGAGE_INIT_MESSAGE_SLOT,
                REALIZED_BAGGAGE_INIT_HANDLER_SLOT
                - REALIZED_BAGGAGE_INIT_MESSAGE_SLOT
                + 8,
            ),
        ),
        (
            image.image_base + REALIZED_BAGGAGE_INIT_MESSAGE,
            image.image_base + REALIZED_BAGGAGE_INIT_THUNK,
        ),
    )
    require_equal(
        "realized-baggage unlink message/handler pair",
        struct.unpack(
            "<2Q",
            image.read_rva(
                REALIZED_BAGGAGE_UNLINK_MESSAGE_SLOT,
                REALIZED_BAGGAGE_UNLINK_HANDLER_SLOT
                - REALIZED_BAGGAGE_UNLINK_MESSAGE_SLOT
                + 8,
            ),
        ),
        (
            image.image_base + REALIZED_BAGGAGE_UNLINK_MESSAGE,
            image.image_base + REALIZED_BAGGAGE_UNLINK_THUNK,
        ),
    )
    require_equal(
        "realized-baggage update message/handler pair",
        struct.unpack(
            "<2Q",
            image.read_rva(
                REALIZED_BAGGAGE_UPDATE_MESSAGE_SLOT,
                REALIZED_BAGGAGE_UPDATE_HANDLER_SLOT
                - REALIZED_BAGGAGE_UPDATE_MESSAGE_SLOT
                + 8,
            ),
        ),
        (
            image.image_base + REALIZED_BAGGAGE_UPDATE_MESSAGE,
            image.image_base + REALIZED_BAGGAGE_UPDATE_THUNK,
        ),
    )
    require_equal(
        "EntityManagerGame update slot",
        struct.unpack("<Q", image.read_rva(ENTITY_MANAGER_GAME_UPDATE_SLOT, 8))[0],
        image.image_base + ENTITY_MANAGER_GAME_UPDATE,
    )
    validate_vtable(image, "survey VTable", SURVEY_VTABLE, SURVEY_VFUNCS)
    validate_vtable(
        image, "body secondary VTable", BODY_SECONDARY_VTABLE, BODY_SECONDARY_VFUNCS
    )
    validate_vtable(image, "sensor VTable", SENSOR_VTABLE, SENSOR_VFUNCS)
    validate_vtable(
        image, "sensor manager VTable", SENSOR_MANAGER_VTABLE, SENSOR_MANAGER_VFUNCS
    )
    validate_vtable(
        image, "effect instance VTable", EFFECT_INSTANCE_VTABLE, EFFECT_INSTANCE_VFUNCS
    )
    validate_vtable(
        image, "effect resource VTable", EFFECT_RESOURCE_VTABLE, EFFECT_RESOURCE_VFUNCS
    )
    validate_vtable(image, "UI resident VTable", UI_RESIDENT_VTABLE, UI_RESIDENT_VFUNCS)
    validate_vtable(
        image, "UI game-params VTable", UI_GAME_PARAMS_VTABLE, UI_GAME_PARAMS_VFUNCS
    )
    validate_vtable(
        image,
        "baggage-manager VTable",
        BAGGAGE_MANAGER_VTABLE,
        BAGGAGE_MANAGER_VFUNCS,
    )
    validate_vtable(
        image,
        "baggage-manager secondary VTable",
        BAGGAGE_MANAGER_SECONDARY_VTABLE,
        BAGGAGE_MANAGER_SECONDARY_VFUNCS,
    )
    validate_vtable(
        image,
        "baggage-config VTable",
        BAGGAGE_CONFIG_VTABLE,
        BAGGAGE_CONFIG_VFUNCS,
    )
    validate_vtable(
        image,
        "DSPlayerOdradekConfig VTable",
        PLAYER_ODRADEK_CONFIG_VTABLE,
        PLAYER_ODRADEK_CONFIG_VFUNCS,
    )
    validate_vtable(
        image,
        "detected-component VTable",
        DETECTED_COMPONENT_VTABLE,
        DETECTED_COMPONENT_VFUNCS,
    )
    validate_vtable(
        image,
        "EntityManagerGame VTable",
        ENTITY_MANAGER_GAME_VTABLE,
        ENTITY_MANAGER_GAME_VFUNCS,
    )
    require_equal(
        "EntityManagerGame VTable COL",
        struct.unpack("<Q", image.read_rva(ENTITY_MANAGER_GAME_VTABLE - 8, 8))[0],
        image.image_base + ENTITY_MANAGER_GAME_COL,
    )
    require_equal(
        "EntityManagerGame COL type descriptor",
        struct.unpack(
            "<I", image.read_rva(ENTITY_MANAGER_GAME_COL + 0x0C, 4)
        )[0],
        ENTITY_MANAGER_GAME_TYPE_DESCRIPTOR,
    )
    require_equal(
        "EntityManagerGame RTTI name",
        image.read_rva(
            ENTITY_MANAGER_GAME_TYPE_DESCRIPTOR + 0x10,
            len(ENTITY_MANAGER_GAME_TYPE_NAME),
        ),
        ENTITY_MANAGER_GAME_TYPE_NAME,
    )
    validate_vtable(
        image,
        "baggage-component VTable",
        BAGGAGE_COMPONENT_VTABLE,
        BAGGAGE_COMPONENT_VFUNCS,
    )
    validate_vtable(
        image,
        "realized-baggage VTable",
        REALIZED_BAGGAGE_VTABLE,
        REALIZED_BAGGAGE_VFUNCS,
    )

    # New common-list call only; both other candidate sorts remain untouched.
    for label, rva, signature in (
        ('scanned HUD call context', 0x11D04F2, '48 63 84 24 10 c1 06 00 48 8d 94 24 10 01 00 00 48 8d 0c 40 48 c1 e1 04 48 8d 84 24 10 01 00 00 48 03 d1 48 3b c2 74 0d 48 8d 8c 24 10 01 00 00 e8 49 93 03 00'),
        ('scanned HUD atomic window', 0x11D0520, '00 00 e8 49 93 03 00 48'),
        ('scanned HUD sort', 0x1209870, '44 88 4c 24 20 4c 89 44 24 18 48 89 4c 24 08 53 56 57 48 81 ec b0 00 00'),
        ('scanned HUD recent predicate', 0x1CE6300, '83 79 68 00 7e 29 48 8b 05 e3 86 55 04 48 8b 50 08 48 85 d2 75 0a c5 fa 10 05 aa bc 77 01 eb 05 c5 fa 10 42 24 c5 f8 2f 41 7c 76 03 b0 01 c3 32 c0 c3'),
    ):
        expected = bytes.fromhex(signature)
        require_equal(label, image.read_rva(rva, len(expected)), expected)

    anchors_path = Path(__file__).resolve().with_name('hud-distance-anchors.json')
    for anchor in json.loads(anchors_path.read_text(encoding='utf-8')):
        expected = bytes.fromhex(anchor['hex'])
        require_equal(anchor['name'], image.read_rva(anchor['rva'], len(expected)), expected)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("exe", type=Path)
    args = parser.parse_args()
    validate(args.exe)
    print(f"Offline DS2 target validation: PASS ({args.exe})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
