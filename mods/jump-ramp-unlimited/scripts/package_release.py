"""Package only verified local build output; never copy from a game installation."""
import ctypes
import hashlib
import json
from pathlib import Path
import zipfile

ROOT = Path(__file__).resolve().parents[1]
version = (ROOT / "VERSION.txt").read_text().strip()
report = json.loads((ROOT / "build/tests/verification.json").read_text())
asi = ROOT / "build/ds2_jump_ramp_unlimited.asi"
ini = ROOT / "config/ds2_jump_ramp_unlimited.ini"


def digest(data):
    return hashlib.sha256(data).hexdigest()


assert report["result"] == "PASS" and report["version"] == version
assert report["built_asi_sha256"] == digest(asi.read_bytes())
assert report["source_sha256"] == digest((ROOT / "src/jump_ramp_unlimited.cpp").read_bytes())
# Exercise the Windows loader on the actual release DLL. Its process-name guard
# keeps it inactive in this Python process; this is not an in-game startup test.
loaded = ctypes.WinDLL(str(asi))
assert loaded._handle
release = ROOT / "release"
release.mkdir(exist_ok=True)
payload = {asi.name: asi.read_bytes(), ini.name: ini.read_bytes()}
assert b"Enabled=1" in payload[ini.name]
archive = release / f"DS2_Jump_Ramp_Unlimited_v{version}.zip"
with zipfile.ZipFile(archive, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=9) as bundle:
    for name, data in sorted(payload.items()):
        (release / name).write_bytes(data)
        entry = zipfile.ZipInfo(name, date_time=(2026, 10, 3, 0, 0, 0))
        entry.compress_type = zipfile.ZIP_DEFLATED
        entry.create_system = 0
        entry.external_attr = 0x20
        bundle.writestr(entry, data, compress_type=zipfile.ZIP_DEFLATED, compresslevel=9)
with zipfile.ZipFile(archive) as bundle:
    assert bundle.testzip() is None
    assert sorted(bundle.namelist()) == sorted(payload)
    for name, data in payload.items():
        assert bundle.read(name) == data == (release / name).read_bytes()
hashes = {name: digest(data) for name, data in payload.items()}
hashes[archive.name] = digest(archive.read_bytes())
(ROOT / "SHA256SUMS.txt").write_text(
    "".join(f"{value}  release/{name}\n" for name, value in sorted(hashes.items())),
    encoding="utf-8", newline="\n",
)
manifest = {
    "name": "Jump Ramp Unlimited", "version": version,
    "supported_game": "DEATH STRANDING 2: ON THE BEACH, Steam 1.10.89.0 (x64)",
    "supported_exe_sha256": report["game_exe_sha256"],
    "archive": archive.name, "sha256": hashes,
    "requires_external_x64_asi_loader": True,
    "native_verification": "../docs/native-verification.json",
    "gameplay_validation": "../docs/VALIDATION.md",
}
(release / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8", newline="\n")
(ROOT / "docs/native-verification.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8", newline="\n")
print(json.dumps({"archive": archive.name, "payload_files": sorted(payload), "sha256": hashes}, indent=2))
