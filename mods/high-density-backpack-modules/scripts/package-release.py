"""Package the built release ASI without installers or user configuration."""
from pathlib import Path
import hashlib
import zipfile

root = Path(__file__).resolve().parents[1]
version = "1.1.0"
name = f"DS2_HighDensityBackpackModules_v{version}.asi"
data = (root / "dist" / name).read_bytes()
release = root / "release"
release.mkdir(exist_ok=True)
reference = root / "reference" / name
reference.write_bytes(data)
archive = release / f"DS2_High_Density_Backpack_Modules_v{version}.zip"
entry = zipfile.ZipInfo(name, date_time=(2026, 9, 24, 0, 0, 0))
entry.compress_type = zipfile.ZIP_DEFLATED
entry.external_attr = 0o100644 << 16
with zipfile.ZipFile(archive, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=9) as z:
    z.writestr(entry, data)
with zipfile.ZipFile(archive) as z:
    assert z.namelist() == [name]
    assert z.testzip() is None and z.read(name) == data

def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()

manifest = release / f"DS2_High_Density_Backpack_Modules_v{version}_SHA256.txt"
manifest.write_text(f"{sha(reference)}  {name}\n{sha(archive)}  {archive.name}\n", encoding="utf-8")
(root / "SHA256SUMS.txt").write_text(
    "".join(f"{sha(p)}  {p.relative_to(root).as_posix()}\n" for p in (reference, archive, manifest)),
    encoding="utf-8",
)
print(archive)
print("Verified: one ASI only; ZIP CRC, reference and extracted bytes match.")
