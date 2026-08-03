#!/usr/bin/env python3
"""Create the tested no-log release from the validated working baseline ASI.

This is a deterministic data-only patch. It does not alter the PE code section,
entry point, imports, exports, DS2 addresses, overlay logic, or exception data.
"""
from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path

BASELINE_SHA256 = "A325FEC6CAD46F478FA1812DF93BE75D8EC63EE7AFC1618C9B3272F28E8CBD25"
OUTPUT_SHA256 = "696D05285C23F2D73EE27C23827A27BA347C76D79DF0A84F7DB29E164DCB695C"

LOG_NAMES = [
    "DS2_Remote_Orders_Intermediate_Overlay.log",
    "DS2_Remote_Orders_Intermediate_Overlay_STARTUP.log",
    "DS2_Remote_Orders_Intermediate_Overlay_WORKER_START.log",
    "DS2_Remote_Orders_Intermediate_Overlay_STARTMOD_ENTER.log",
    "DS2_Remote_Orders_Intermediate_Overlay_STARTMOD_ALREADY.log",
    "DS2_Remote_Orders_Intermediate_Overlay_VERIFY_FAIL.log",
    "DS2_Remote_Orders_Intermediate_Overlay_VERIFY_OK.log",
    "DS2_Remote_Orders_Intermediate_Overlay_AFTER_CREATE.log",
    "DS2_Remote_Orders_Intermediate_Overlay_AFTER_IMAGE.log",
    "DS2_Remote_Orders_Intermediate_Overlay_AFTER_ATOMIC.log",
]


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest().upper()


def pe_sections(blob: bytes) -> dict[str, bytes]:
    pe = struct.unpack_from("<I", blob, 0x3C)[0]
    count = struct.unpack_from("<H", blob, pe + 6)[0]
    optional_size = struct.unpack_from("<H", blob, pe + 20)[0]
    section_table = pe + 24 + optional_size
    result: dict[str, bytes] = {}
    for index in range(count):
        offset = section_table + index * 40
        name = blob[offset : offset + 8].rstrip(b"\0").decode("ascii")
        raw_size, raw_pointer = struct.unpack_from("<II", blob, offset + 16)
        result[name] = blob[raw_pointer : raw_pointer + raw_size]
    return result


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("baseline", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()

    original = args.baseline.read_bytes()
    if sha256(original) != BASELINE_SHA256:
        raise SystemExit("Baseline SHA-256 does not match the validated ASI.")

    patched = bytearray(original)
    for name in LOG_NAMES:
        needle = (name + "\0").encode("utf-16le")
        positions: list[int] = []
        start = 0
        while True:
            position = patched.find(needle, start)
            if position < 0:
                break
            positions.append(position)
            start = position + 2
        if len(positions) != 1:
            raise SystemExit(f"Expected exactly one occurrence of {name!r}; found {len(positions)}")
        position = positions[0]
        replacement = ("NUL\0").encode("utf-16le")
        patched[position : position + len(needle)] = replacement + b"\0" * (len(needle) - len(replacement))

    output = bytes(patched)
    if sha256(output) != OUTPUT_SHA256:
        raise SystemExit("Output SHA-256 differs from the runtime-tested no-log release.")

    before = pe_sections(original)
    after = pe_sections(output)
    for section in (".text", ".data", ".pdata"):
        if before[section] != after[section]:
            raise SystemExit(f"Unexpected change in PE section {section}")

    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_bytes(output)
    print(f"Created: {args.output}")
    print(f"SHA-256: {OUTPUT_SHA256}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
