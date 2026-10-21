"""Package the CURRENT published Crafting & Equipment Overhaul release (v1.8.0).
Never modifies DS2.exe, game files, Nexus, or a previous release ZIP.
"""
from pathlib import Path
import configparser
import hashlib
import zipfile
import json

ROOT=Path(__file__).resolve().parents[1]
RELEASE=ROOT/"release"
VERSION="1.8.0"
OUT=RELEASE/f"DS2_Crafting_Equipment_Overhaul_v{VERSION}.zip"
FILES=[
    (RELEASE/"ds2_crafting_unlocks.asi","ds2_crafting_unlocks.asi"),
    (RELEASE/"ds2_crafting_unlocks.ini","ds2_crafting_unlocks.ini"),
    (RELEASE/"README_EN.txt","README_EN.txt"),
    (RELEASE/"README_DE.txt","README_DE.txt"),
    (RELEASE/"CHANGELOG.txt","CHANGELOG.txt"),
]
data={arc:src.read_bytes() for src,arc in FILES}
asi=data["ds2_crafting_unlocks.asi"]
config=configparser.ConfigParser(inline_comment_prefixes=(";",))
config.read_string(data["ds2_crafting_unlocks.ini"].decode("ascii"))
assert ("DS2 Crafting & Equipment Overhaul "+VERSION).encode() in asi
assert config.getint("PCCStructures","UnlockAll")==1
assert config.getint("CraftingUnlocks","Enabled")==1
assert b"PCC_ON:" in asi and b"PCC_OVERRIDE:" in asi
assert data["README_EN.txt"].startswith(("# Crafting & Equipment Overhaul "+VERSION).encode())
assert data["README_DE.txt"].startswith(("# Crafting & Equipment Overhaul "+VERSION).encode())
assert b"1.8.0" in data["CHANGELOG.txt"]
assert (RELEASE/"DS2_Crafting_Equipment_Overhaul_v1.7.0.zip").is_file()
sums="".join(f"{hashlib.sha256(blob).hexdigest()}  {name}\n" for name,blob in data.items()).encode("ascii")
(RELEASE/"SHA256SUMS.txt").write_bytes(sums)
data["SHA256SUMS.txt"]=sums
with zipfile.ZipFile(OUT,"w",compression=zipfile.ZIP_DEFLATED,compresslevel=9) as z:
    for name,blob in data.items():
        item=zipfile.ZipInfo(name,(2026,10,15,0,0,0))
        item.compress_type=zipfile.ZIP_DEFLATED
        item.external_attr=0o100644<<16
        z.writestr(item,blob)
with zipfile.ZipFile(OUT) as z:
    assert z.testzip() is None
    assert z.namelist()==list(data)
    for name,blob in data.items(): assert z.read(name)==blob
report={
 "version":VERSION,
 "archive":str(OUT.relative_to(ROOT)).replace("\\", "/"),
 "sha256":hashlib.sha256(OUT.read_bytes()).hexdigest(),
 "bytes":OUT.stat().st_size,
 "files":{name:{"sha256":hashlib.sha256(blob).hexdigest(),"bytes":len(blob)} for name,blob in data.items()},
 "game_test":"Episode 2: early PCC choices visible, Safehouse placed; remaining structures not individually verified",
 "github_push":"separate action",
 "nexus_uploaded":False,
}
reports=ROOT/"validation/release-1.8.0"
reports.mkdir(parents=True,exist_ok=True)
(reports/"PACKAGE.json").write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8")
print(OUT)
print("SHA256",report["sha256"])
