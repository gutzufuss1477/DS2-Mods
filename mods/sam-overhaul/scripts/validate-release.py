"""Validate the stable Sam Overhaul Nexus ZIP, binary, metadata and INI.
No DS2 process or game files are modified. Run AFTER package-release.ps1.
"""
from __future__ import annotations
import configparser
import hashlib
from pathlib import Path
import re
import struct
import zipfile

root=Path(__file__).resolve().parents[1]
version=(root/'VERSION.txt').read_text(encoding='utf-8-sig').strip()
assert re.fullmatch(r"\d+\.\d+\.\d+",version),"stable release version required"
name=f"DS2_Sam_Overhaul_v{version}"
directory=root/'release'/name
zip_path=root/'release'/f"{name}.zip"
assert directory.is_dir() and zip_path.is_file()
expected={
 f"{name}.asi","ds2_sam_overhaul.ini","README.md","README_DE.md",
 "CHANGELOG.md","THIRD_PARTY_NOTICES.md","LICENSE_MINHOOK.txt",
}
actual={f.name for f in directory.iterdir() if f.is_file()}
assert actual==expected, f"Unexpected package file set: {actual^expected}"
assert not any(f.is_dir() for f in directory.iterdir()),"No subfolders in user package"
with zipfile.ZipFile(zip_path) as zip_file:
 zip_names=set(zip_file.namelist())
 assert zip_names==expected, f"Unexpected ZIP entries: {zip_names^expected}"
 assert zip_file.testzip() is None,"Corrupt ZIP archive"
 for entry in zip_file.infolist():
  assert not entry.is_dir() and '/' not in entry.filename and '\\' not in entry.filename, "No nested or unsafe ZIP paths"
  data=(directory/entry.filename).read_bytes()
  assert zip_file.read(entry.filename)==data, f"Archive mismatch: {entry.filename}"
package_asi=(directory/f"{name}.asi").read_bytes()
build_asi=(root/'build'/'public'/f"{name}.asi").read_bytes()
assert package_asi==build_asi
assert b"1.1.0-dev." not in package_asi, "Development version accidentally in stable binary"
assert b"1.1.0\x00" in package_asi, "Stable exported version string missing"
assert package_asi[:2]==b"MZ"
pe=struct.unpack_from('<I',package_asi,0x3c)[0]
assert package_asi[pe:pe+4]==b"PE\x00\x00"
machine,num_sections=struct.unpack_from("<HH",package_asi,pe+4)
assert machine==0x8664 and 1<=num_sections<=10,"Unsupported ASI architecture"
opt=pe+24
assert struct.unpack_from("<H",package_asi,opt)[0]==0x20b,"Expected PE32+"
assert struct.unpack_from("<II",package_asi,opt+112+9*8)==(0,0),"No TLS callbacks"
optional_size=struct.unpack_from("<H",package_asi,pe+20)[0]
sections=[]
for idx in range(num_sections):
 p=opt+optional_size+idx*40
 sec_name=package_asi[p:p+8].split(b'\x00',1)[0].decode('ascii')
 vs,va,raw_size,raw=struct.unpack_from('<IIII',package_asi,p+8)
 sections.append((sec_name,va,raw_size,raw))
assert not any(n.startswith(('.CRT','.tls')) for n,_,_,_ in sections)
def offset(rva):
 for _,addr,size,file_at in sections:
  if addr<=rva<addr+size:return file_at+rva-addr
 raise ValueError(f"Bad RVA: {rva:x}")
def cstr(rva):
 p=offset(rva)
 return package_asi[p:package_asi.index(b"\0",p)].decode("ascii")
imp=struct.unpack_from("<I",package_asi,opt+112+8)[0]
import_libraries=[]
p=offset(imp)
while True:
 fields=struct.unpack_from("<IIIII",package_asi,p)
 if not any(fields):break
 import_libraries.append(cstr(fields[3]).lower())
 p+=20
assert set(import_libraries)=={"kernel32.dll","bcrypt.dll"},import_libraries
exp_rva=struct.unpack_from("<I",package_asi,opt+112)[0]
exp=offset(exp_rva)
num_names=struct.unpack_from("<I",package_asi,exp+24)[0]
names=offset(struct.unpack_from("<I",package_asi,exp+32)[0])
exports=[cstr(struct.unpack_from("<I",package_asi,names+i*4)[0]) for i in range(num_names)]
required_exports={"InitializeASI","SamOverhaulVersion","SamOverhaulFootprintsState",
                  "SamOverhaulGeneratorRangeChanges","SamOverhaulShelterRangeChanges"}
assert set(exports)==required_exports,exports

cfg=configparser.ConfigParser(interpolation=None)
cfg.read(root/'ds2_sam_overhaul.ini',encoding='utf-8-sig')
assert cfg['Footprints']['hidefootprints']=='0'
assert cfg['GeneratorRange']['enabled']=='0'
assert cfg['GeneratorRange']['rangepercent']=='200'
assert cfg['TimefallShelterRange']['enabled']=='0'
assert cfg['TimefallShelterRange']['rangepercent']=='200'
assert cfg['TimefallShelterRange']['repairradiuspercent']=='215'
assert cfg['TimefallShelterRange']['fixrestprompt']=='1'
assert cfg['TimefallShelterRange']['spatialdiagnostics']=='0'
assert cfg['CargoVisibility']['hideshouldercargo']=='1'
assert cfg['Movement']['landingrollwithbackpack']=='1'
assert cfg['AutoDrive']['activationseconds']=='2.0'
original=configparser.ConfigParser(interpolation=None)
original.read(root/'release'/'DS2_Sam_Overhaul_v1.0.0'/'ds2_sam_overhaul.ini',encoding='utf-8-sig')
for section in original.sections():
 assert section in cfg,section
 for key,value in original[section].items():
  assert cfg[section][key]==value,f"Original setting changed: {section}.{key}"

release_manifest=(root/'SHA256SUMS.txt').read_text(encoding='ascii').splitlines()
checks={}
for line in release_manifest:
 match=re.fullmatch(r'([0-9A-F]{64})\s{2}(.+)',line)
 assert match,f"Bad SHA256SUMS line: {line}"
 checks[match.group(2)]=match.group(1)
assert set(checks)==expected|{zip_path.name}
for fname in expected:
 assert hashlib.sha256((directory/fname).read_bytes()).hexdigest().upper()==checks[fname],fname
assert hashlib.sha256(zip_path.read_bytes()).hexdigest().upper()==checks[zip_path.name]
for n in ('README.md','README_DE.md','NEXUS_DESCRIPTION_BBCODE.txt','NEXUS_SHORT_DESCRIPTION.txt'):
 doc=(root/n).read_text(encoding='utf-8')
 assert '1.0.0.asi' not in doc,n
 assert '1.1.0-dev.' not in doc,n
 assert ('footprints' in doc.lower() or 'fussabdr' in doc.lower()),n
 assert 'generator' in doc.lower(),n
 assert 'shelter' in doc.lower() or 'zeitregenunterstand' in doc.lower(),n
short=(root/'NEXUS_SHORT_DESCRIPTION.txt').read_text(encoding='utf8').strip()
assert len(short)<=255, f"Nexus short description too long ({len(short)})"
nexus=(root/'NEXUS_DESCRIPTION_BBCODE.txt').read_text(encoding='utf8')
for tag in ('center','size','b','list','code'):
 if tag!='size':
  assert len(re.findall(r'\['+tag+r'(?:=[^\]]+)?\]',nexus))==nexus.count('[/'+tag+']'),f"Unbalanced BBCODE [{tag}]"
print('PASS_STABLE_NEXUS_RELEASE_ARCHIVE')
print('VERSION='+version)
print('FILES='+', '.join(sorted(expected)))
print('ASI_SHA256='+checks[f"{name}.asi"])
print('ZIP_SHA256='+checks[zip_path.name])
print('SHORT_DESCRIPTION_LENGTH='+str(len(short)))
print('EXPORTS='+', '.join(sorted(exports)))
print('ORIGINAL_INI_OPTIONS_PRESERVED=True')
print('NO_EXTRA_RUNTIME_DEPENDENCIES=True')
