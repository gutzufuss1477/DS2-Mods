"""Read-only exact-build and hook-overlap checks for all combined features."""
from pathlib import Path
import hashlib,json,re,struct,sys
sys.path.insert(0,str(Path(__file__).resolve().parent/'atlas'))
from native_scan import Image

ROOT=Path(__file__).resolve().parents[1]
def source(name):return (ROOT/'src'/name).read_text(encoding='utf-8')
def byte_list(text):return bytes(int(x.strip(),0) for x in text.split(',') if x.strip())
sites=[]
def add(rva,expected,group):sites.append((rva,bytes(expected),group))
# Parse the production branch allowlist, rather than relying on the evidence copy.
for rva,length,expected in re.findall(r'\{(0x[0-9A-F]+),0x[0-9A-F]+,(\d+),\d+,\d+,\d+,\{([^}]+)\}',source('atlas/sites.generated.hpp')):
 raw=byte_list(expected);assert len(raw)==int(length);add(int(rva,16),raw,'atlas')
assert len(sites)==27
for rva,expected in [(0xE6E4AE,'e81d050000'),(0x10B8928,'498d809a1a0000'),(0xEA0681,'0f94442460')]:add(rva,bytes.fromhex(expected),'atlas')
branch=source('atlas/branch.hpp')
for rva,length,expected in re.findall(r'\{(0x[0-9A-F]+),(\d+),\d+,(?:true|false),\{([^}]+)\}',branch):
 raw=byte_list(expected);assert len(raw)==int(length);add(int(rva,16),raw,'atlas')
for rva,expected in [(0x1EC3DB7,'e8648dcafe'),(0xF430E0,'48895c2420'),(0xF43E30,'405355574881ecd0000000'),(0xEC70F6,'0fb6c083c0ea')]:add(rva,bytes.fromhex(expected),'atlas')
assert len(sites)==36,len(sites)
for rva,length,expected in re.findall(r'\{(0x[0-9A-F]+),(\d+),\{([^}]+)\}',source('freecrafting_sites.hpp')):
 raw=byte_list(expected);assert len(raw)==int(length);add(int(rva,16),raw,'freecrafting')
assert len(sites)==85
for rva,expected in re.findall(r'\{(0x[0-9A-F]+),\{([^}]+)\},\(void\*\)',source('durability_runtime.inl')):add(int(rva,16),byte_list(expected),'durability')
assert len(sites)==88
for rva,expected in [(0x171DB1D,'e89e1e45ff'),(0x1529896,'e8856364ff'),(0x171C124,'0f84b2000000')]:add(rva,bytes.fromhex(expected),'menus')
for rva,expected in [(0xB6CB20,'48895c2408'),(0x1FAD390,'e9bbc4ffff'),(0x17BE390,'48895c2408')]:add(rva,bytes.fromhex(expected),'weapons')
im=Image();sites.sort()
for i,(rva,expected,group) in enumerate(sites):
 assert im.read(rva,len(expected))==expected,(hex(rva),group,'native bytes differ')
 if i:assert sites[i-1][0]+len(sites[i-1][1])<=rva,('overlap',sites[i-1],sites[i])
binary=(ROOT/'release/ds2_crafting_unlocks.asi').read_bytes()
pe=struct.unpack_from('<I',binary,0x3c)[0];machine,count,_,_,_,opt_size,characteristics=struct.unpack_from('<HHIIIHH',binary,pe+4)
assert machine==0x8664 and characteristics&0x2000
opt=pe+24;assert struct.unpack_from('<H',binary,opt)[0]==0x20b
flags=struct.unpack_from('<H',binary,opt+70)[0];assert flags&0x140==0x140
sections=[]
for i in range(count):
 off=opt+opt_size+40*i;name=binary[off:off+8].split(b'\0')[0].decode()
 chars=struct.unpack_from('<I',binary,off+36)[0]
 assert not(chars&0x20000000 and chars&0x80000000),('writable executable section',name)
 sections.append(name)
assert b'DS2 Crafting & Equipment Overhaul 1.7.0' in binary and b'ATLAS_ON:' in binary and b'ATLAS_OFF:' in binary
report={'version':'1.7.0','sha256':hashlib.sha256(binary).hexdigest(),'bytes':len(binary),'hook_count':len(sites),
 'groups':{g:sum(s[2]==g for s in sites) for g in ['atlas','freecrafting','durability','menus','weapons']},
 'exact_exe_signatures':True,'overlapping_hooks':False,'amd64_dll':True,'aslr_nx':True,'writable_executable_sections':False,
 'sites':[{'rva':hex(a),'length':len(b),'group':g} for a,b,g in sites]}
(ROOT/'validation/integration-1.7.0.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
print(f"PASS {len(sites)} exact native signatures; no overlaps; AMD64 DLL, ASLR/NX, no RWX sections.")
