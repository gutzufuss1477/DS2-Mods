from pathlib import Path
import hashlib
import json
import zipfile

root = Path(__file__).resolve().parents[1]
version = (root / "VERSION.txt").read_text(encoding="utf-8").strip()
build = root / "build"
release = root / "release"
release.mkdir(exist_ok=True)

asi = build / "ds2_beach_jump_with_cargo.asi"
ini = build / "ds2_beach_jump_with_cargo.ini"
for path in (asi, ini):
    if not path.is_file():
        raise SystemExit(f"Missing build artifact: {path}")

for old in release.glob("ds2_beach_jump_with_cargo.*"):
    old.unlink()
for old in release.glob("DS2_Beach_Jump_with_Cargo_v*.zip"):
    old.unlink()

release_asi = release / asi.name
release_ini = release / ini.name
release_asi.write_bytes(asi.read_bytes())
release_ini.write_bytes(ini.read_bytes())

zip_path = release / f"DS2_Beach_Jump_with_Cargo_v{version}.zip"
with zipfile.ZipFile(zip_path, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=9) as zf:
    for path in (release_asi, release_ini):
        info = zipfile.ZipInfo(path.name, (2026, 10, 4, 0, 0, 0))
        info.compress_type = zipfile.ZIP_DEFLATED
        info.external_attr = 0o644 << 16
        zf.writestr(info, path.read_bytes())

def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()

manifest = {
    "mod": "Beach Jump with Cargo",
    "version": version,
    "target": "DEATH STRANDING 2 PC Steam 1.10.89.0",
    "files": {
        release_asi.name: sha(release_asi),
        release_ini.name: sha(release_ini),
        zip_path.name: sha(zip_path),
    },
}
(release / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
(root / "SHA256SUMS.txt").write_text(
    "\n".join(f"{digest}  {name}" for name, digest in manifest["files"].items()) + "\n",
    encoding="utf-8",
)
print(zip_path)
