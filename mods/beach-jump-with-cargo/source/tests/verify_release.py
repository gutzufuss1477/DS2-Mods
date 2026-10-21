from pathlib import Path
import hashlib
import json
import os
import struct
import sys

root = Path(__file__).resolve().parents[1]
exe = Path(os.environ.get(
    "DS2_EXE",
    r"C:\Program Files (x86)\Steam\steamapps\common\DEATH STRANDING 2 - ON THE BEACH\DS2.exe"
))
expected_hash = "BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B"
target_rva = 0x11E34A0
expected_prologue = bytes.fromhex("40 55 41 55 41 56 41 57")

data = exe.read_bytes()
actual_hash = hashlib.sha256(data).hexdigest().upper()
if actual_hash != expected_hash:
    raise SystemExit(f"FAIL: unsupported DS2.exe SHA256 {actual_hash}")

pe = struct.unpack_from("<I", data, 0x3C)[0]
timestamp = struct.unpack_from("<I", data, pe + 8)[0]
optional = pe + 24
size_of_image = struct.unpack_from("<I", data, optional + 56)[0]
if timestamp != 0x6A3DAE46 or size_of_image != 0x0B292000:
    raise SystemExit("FAIL: PE identity mismatch")

section_count = struct.unpack_from("<H", data, pe + 6)[0]
optional_size = struct.unpack_from("<H", data, pe + 20)[0]
section_table = pe + 24 + optional_size
raw = None
for index in range(section_count):
    off = section_table + index * 40
    virtual_size, va, raw_size, raw_ptr = struct.unpack_from("<IIII", data, off + 8)
    if va <= target_rva and target_rva + len(expected_prologue) <= va + max(virtual_size, raw_size):
        file_off = raw_ptr + target_rva - va
        raw = data[file_off:file_off + len(expected_prologue)]
        break
if raw != expected_prologue:
    raise SystemExit(f"FAIL: fast-travel baggage prologue mismatch: {raw!r}")

source = (root / "src" / "beach_jump_with_cargo.cpp").read_text(encoding="utf-8")
required = [
    "DSBaggageManager::HandlingBaggagesOnFastTravel(bool)",
    "BAGGAGE_FAST_TRAVEL_RVA=0x011E34A0u",
    'MOD_VERSION "1.0.0"',
]
for token in required:
    if token not in source:
        raise SystemExit(f"FAIL: release source missing {token}")

result = {
    "result": "PASS",
    "ds2_sha256": actual_hash,
    "timestamp": f"0x{timestamp:08X}",
    "size_of_image": f"0x{size_of_image:08X}",
    "target_rva": f"0x{target_rva:08X}",
    "target_prologue": raw.hex(" ").upper(),
}
(root / "docs" / "release-verification.json").write_text(
    json.dumps(result, indent=2) + "\n", encoding="utf-8"
)
print("PASS: supported DS2.exe identity and fast-travel baggage hook target verified")
