from pathlib import Path
import hashlib
import zipfile

ROOT=Path(__file__).resolve().parents[1]
RELEASE=ROOT/"release"
OUT=RELEASE/"DS2_Crafting_Overhaul_v1.3.0.zip"
FILES=[
    (RELEASE/"ds2_crafting_unlocks.asi","ds2_crafting_unlocks.asi"),
    (RELEASE/"ds2_crafting_unlocks.ini","ds2_crafting_unlocks.ini"),
    (RELEASE/"README_EN.txt","README_EN.txt"),
    (RELEASE/"README_DE.txt","README_DE.txt"),
    (RELEASE/"CHANGELOG.txt","CHANGELOG.txt"),
]
sums=[f"{hashlib.sha256(src.read_bytes()).hexdigest()}  {arc}" for src,arc in FILES]
(RELEASE/"SHA256SUMS.txt").write_text("\r\n".join(sums)+"\r\n",encoding="ascii")
FILES.append((RELEASE/"SHA256SUMS.txt","SHA256SUMS.txt"))
if OUT.exists():
    OUT.unlink()
with zipfile.ZipFile(OUT,"w",zipfile.ZIP_DEFLATED,compresslevel=9) as z:
    for src,arc in FILES:
        z.write(src,arc)
with zipfile.ZipFile(OUT) as z:
    assert z.testzip() is None
print(OUT)
print("SHA256",hashlib.sha256(OUT.read_bytes()).hexdigest())
