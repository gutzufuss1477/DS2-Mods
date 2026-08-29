#!/usr/bin/env python3
from __future__ import annotations

import argparse
from pathlib import Path

from validate_target import PEImage


def integer(value: str) -> int:
    return int(value, 0)


def main() -> int:
    parser = argparse.ArgumentParser(description="Hex-dump bytes from a PE RVA")
    parser.add_argument("exe", type=Path)
    parser.add_argument("rva", type=integer)
    parser.add_argument("size", type=integer)
    args = parser.parse_args()
    data = PEImage(args.exe).read_rva(args.rva, args.size)
    for offset in range(0, len(data), 16):
        row = data[offset : offset + 16]
        hex_bytes = " ".join(f"{value:02X}" for value in row)
        text = "".join(chr(value) if 32 <= value < 127 else "." for value in row)
        print(f"{args.rva + offset:08X}  {hex_bytes:<47}  {text}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
