#!/usr/bin/env python3
"""Compare two PE files while ignoring only the 4-byte COFF TimeDateStamp."""
from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def normalize_pe_timestamp(data: bytes) -> bytes:
    out = bytearray(data)
    if len(out) < 0x40:
        raise ValueError("file is too small to be a PE image")
    pe_offset = struct.unpack_from("<I", out, 0x3C)[0]
    if pe_offset + 12 > len(out) or out[pe_offset : pe_offset + 4] != b"PE\0\0":
        raise ValueError("PE signature not found")
    timestamp_offset = pe_offset + 8
    out[timestamp_offset : timestamp_offset + 4] = b"\0\0\0\0"
    return bytes(out)


def main() -> int:
    if len(sys.argv) != 3:
        print("Usage: py tools\\verify_pe_equivalence.py <built.asi> <reference.asi>")
        return 2

    left_path = Path(sys.argv[1])
    right_path = Path(sys.argv[2])
    left = left_path.read_bytes()
    right = right_path.read_bytes()

    print(f"{left_path}: raw SHA-256 {sha256(left)}")
    print(f"{right_path}: raw SHA-256 {sha256(right)}")

    left_normalized = normalize_pe_timestamp(left)
    right_normalized = normalize_pe_timestamp(right)
    normalized_hash = sha256(left_normalized)

    if left_normalized == right_normalized:
        print("MATCH: files are byte-identical after zeroing only the PE COFF timestamp.")
        print(f"Normalized SHA-256: {normalized_hash}")
        return 0

    print("NO MATCH: files differ beyond the PE COFF timestamp.")
    print(f"Left normalized SHA-256:  {normalized_hash}")
    print(f"Right normalized SHA-256: {sha256(right_normalized)}")
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
