#!/usr/bin/env python3
"""Validate the v1.10.89.0 hook signatures against an RVA-mapped DS2 image."""
from pathlib import Path
import argparse
import hashlib
import sys

EXPECTED_SIZE = 0x0B292000
SIGNATURES = {
    0x012922F1: bytes.fromhex("48 8B 4C 24 48"),
    0x012923FB: bytes.fromhex("48 8B 4C 24 50"),
    0x015352D1: bytes.fromhex("E8 5A CF D5 FF"),
    0x015352DE: bytes.fromhex("E8 2D D0 D5 FF"),
    0x0175B7EC: bytes.fromhex("E8 3F 6A B3 FF"),
    0x0175B801: bytes.fromhex("E8 0A 6B B3 FF"),
    0x0129235E: bytes.fromhex("C5 F9 EF C0 33 C0 48 8D 9E 88 03 00 00"),
    0x0129238C: bytes.fromhex("0F B6 B6 F5 03 00 00"),
    0x0128D04F: bytes.fromhex("8B 06 01 87 80 02 00 00"),
    0x0128D074: bytes.fromhex("8B 46 0C 01 87 8C 02 00"),
    0x0128DAB2: bytes.fromhex("8B 87 80 02 00 00 01 03"),
    0x0128DACC: bytes.fromhex("8B 87 8C 02 00 00 01 43 0C"),
    0x012923E6: bytes.fromhex("8B 44 24 44 29 47 0C"),
}


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("image", type=Path, help="RVA-mapped DS2.exe image")
    args = ap.parse_args()
    data = args.image.read_bytes()
    if len(data) != EXPECTED_SIZE:
        print(f"FAIL: image size 0x{len(data):X}, expected 0x{EXPECTED_SIZE:X}")
        return 1
    failed = False
    for rva, expected in SIGNATURES.items():
        got = data[rva:rva + len(expected)]
        ok = got == expected
        print(f"{'PASS' if ok else 'FAIL'} DS2.exe+0x{rva:X}: {got.hex(' ').upper()}")
        failed |= not ok
    print("SHA-256:", hashlib.sha256(data).hexdigest().upper())
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
