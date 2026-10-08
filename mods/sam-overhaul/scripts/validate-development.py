"""Validate the combined development binary and backward-compatible default INI."""
from pathlib import Path
import configparser
import hashlib
import json
import re
import struct
import sys

def require(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)

root = Path(__file__).resolve().parents[1]
version = (root / "VERSION.txt").read_text(encoding="ascii").strip()
require(re.fullmatch(r"\d+\.\d+\.\d+-dev\.\d+", version), "Expected a development version")
binary = root / "build" / "public" / f"DS2_Sam_Overhaul_v{version}.asi"
data = binary.read_bytes()
require(data[:2] == b"MZ", "Missing DOS header")
pe = struct.unpack_from("<I", data, 0x3C)[0]
require(data[pe:pe+4] == b"PE\0\0", "Missing PE signature")
machine, count = struct.unpack_from("<HH", data, pe+4)
require(machine == 0x8664, "Expected x64")
opt = pe + 24
require(struct.unpack_from("<H", data, opt)[0] == 0x20B, "Expected PE32+")
opt_size = struct.unpack_from("<H", data, pe+20)[0]
characteristics = struct.unpack_from("<H", data, opt+70)[0]
require(characteristics & 0x160 == 0x160, "ASLR/NX/high-entropy flags missing")
sections = []
for i in range(count):
    p = opt+opt_size+i*40
    name = data[p:p+8].split(b"\0",1)[0].decode("ascii")
    vs, va, rs, raw = struct.unpack_from("<IIII",data,p+8)
    sections.append((name,va,rs,raw))
require(not any(n.startswith((".CRT", ".tls")) for n,_,_,_ in sections), "Unexpected runtime initialization section")
require(struct.unpack_from("<II",data,opt+112+9*8)==(0,0), "Unexpected TLS directory")
def offset(rva):
    for _,va,rs,raw in sections:
        if va <= rva < va+rs:
            return raw+rva-va
    raise ValueError(f"Unmapped RVA {rva:#x}")
def string(rva):
    start=offset(rva)
    return data[start:data.index(b"\0",start)].decode("ascii")
imports=[]
imp=struct.unpack_from("<I",data,opt+112+8)[0]
p=offset(imp)
while True:
    desc=struct.unpack_from("<IIIII",data,p)
    if not any(desc):break
    imports.append(string(desc[3]))
    p+=20
require(set(n.lower() for n in imports)=={"kernel32.dll","bcrypt.dll"}, "Unexpected runtime dependency")
exp=offset(struct.unpack_from("<I",data,opt+112)[0])
number=struct.unpack_from("<I",data,exp+24)[0]
names=offset(struct.unpack_from("<I",data,exp+32)[0])
exports=[string(struct.unpack_from("<I",data,names+4*i)[0]) for i in range(number)]
require(set(exports)=={"InitializeASI","SamOverhaulVersion","SamOverhaulFootprintsState"}, "Unexpected/missing exports")
core=(root/"src/footprints/footprint_core.h").read_text(encoding="utf-8-sig")
require(hashlib.sha256(core.encode()).hexdigest()=="e054c990d66126e04bc0b21068d4f1877e4adffb4b73580cc029cd2c1cd16e63", "Visually validated core changed")
def ini(path):
    cfg=configparser.ConfigParser(interpolation=None)
    cfg.read_string(path.read_text(encoding="utf-8-sig"))
    return cfg
old=ini(root/"release/DS2_Sam_Overhaul_v1.0.0/ds2_sam_overhaul.ini")
new=ini(root/"ds2_sam_overhaul.ini")
legacy_options=0
for section in old.sections():
    require(section in new, f"Missing existing section {section}")
    for key,value in old[section].items():
        require(new[section].get(key)==value, f"Existing default changed: {section}.{key}")
        legacy_options+=1
require(new["Footprints"]["HideFootprints"]=="0", "New option must default off")
require(set(new.sections())-set(old.sections())=={"Footprints"}, "Unexpected new configuration section")
require(set(new["Footprints"])=={"hidefootprints"}, "Unexpected footprint option")
result={"version":version,"binary":binary.name,"size_bytes":len(data),
    "sha256":hashlib.sha256(data).hexdigest(),"imports":imports,"exports":exports,
    "validated_core_normalized_sha256":"e054c990d66126e04bc0b21068d4f1877e4adffb4b73580cc029cd2c1cd16e63","legacy_defaults_preserved":legacy_options,
    "footprints_default":0,"gameplay_test_of_combined_build":"pending"}
print(json.dumps(result,indent=2))
print("PASS binary structure, system-only imports, original defaults, validated core and opt-in setting")
