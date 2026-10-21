"""Audit native evidence, exported resources, generated identities and the linked thunk."""
import argparse,configparser,hashlib,json,re,struct,subprocess,sys,zipfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tests/atlas'))
from native_scan import Image
ap=argparse.ArgumentParser()
ap.add_argument('--image',type=Path,required=True)
ap.add_argument('--exports',type=Path,required=True)
ap.add_argument('--types',type=Path,required=True)
ap.add_argument('--artifacts',default='build/pcc-dev1')
ap.add_argument('--reports',default='validation/pcc-dev1')
ap.add_argument('--version',default='1.7.1-pcc-dev1')
ap.add_argument('--default-pcc',type=int,choices=(0,1),default=0)
args=ap.parse_args()
im=Image(args.image)
schema=json.loads(args.types.read_text(encoding='utf-8'))
manifest=json.loads((ROOT/'research/pcc/resources.json').read_text())
assert hashlib.sha256(im.data).hexdigest()==manifest['executable_sha256']
def enum(name):return {x['name']:x['value'] for x in schema[name]['values']}
def resource(folder,type_,reference):
 return json.loads((args.exports/folder/(type_+'_'+reference.replace(':','_')+'.json')).read_text())
for type_,fields in {
 'DSAmmoParameter':{'Id':0x20,'AmmoType':0x24,'ConstructionPointCategory':0x248,'IsConstructionByChiralPrinter':0x24b,'CatalogueFact':0x378,'UsableContentTypes':0x380},
 'DSWeaponParameter':{'Id':0x20,'DetailCategory':0x3a,'Lv':0x72},
 'DSWeaponTrait':{'SelectableAmmoIds':0x58},
}.items():
 actual={a['name']:a['offset'] for a in schema[type_]['attrs'] if 'offset' in a}
 for name,offset in fields.items():assert actual[name]==offset,(type_,name)
for a in manifest['ammo']:
 actual=resource('ammo-json','DSAmmoParameter',a['resource'])
 fact=resource('weapon-json','BooleanFact',a['fact_resource'])
 assert actual['ObjectUUID']==a['uuid'] and actual['Id']==a['name']
 assert enum('EDSAmmoId')[actual['Id']]==a['id']
 assert actual['AmmoType']=='Builder'
 assert enum('EDSConstructionPointCategory')[actual['ConstructionPointCategory']]==a['category']
 assert actual['IsConstructionByChiralPrinter'] and not actual['UsableContentTypes']
 assert actual['CatalogueFact']=='<ref to '+a['fact_resource']+'>'
 assert fact['ObjectUUID']==a['fact_uuid'] and fact['DefaultValue'] is False
for t in manifest['tiers']:
 weapon=resource('weapon-json','DSWeaponParameter',t['resource'])
 trait=resource('weapon-json','DSWeaponTrait',t['trait_resource'])
 assert weapon['ObjectUUID']==t['uuid'] and weapon['Lv']==t['level']
 assert weapon['Id']==t['name'] and enum('EDSWeaponId')[weapon['Id']]==t['id']
 assert weapon['DetailCategory']=='Builder' and weapon['WeaponTraits']==['<ref to '+t['trait_resource']+'>']
 assert trait['ObjectUUID']==t['trait_uuid'] and trait['WeaponBehaviorType']=='Construction'
 assert trait['SelectableAmmoIds']==t['ammo_names']
 assert [enum('EDSAmmoId')[n] for n in trait['SelectableAmmoIds']]==t['ammo_ids']
assert len(manifest['ammo'])==13 and len({a['category'] for a in manifest['ammo']})==11
assert 353 not in {a['id'] for a in manifest['ammo']}
# Regeneration must produce the exact committed data used by the native fixture.
for script,file_,extra in [('generate_policy_data.py','pcc_identities.generated.hpp',[]),('generate_signatures.py','pcc_sites.generated.hpp',[str(args.image)])]:
 before=(ROOT/'src'/file_).read_bytes()
 subprocess.run([sys.executable,str(ROOT/'research/pcc'/script),*extra],check=True,capture_output=True)
 assert before==(ROOT/'src'/file_).read_bytes(),('stale generated data',file_)
# Verify the three native parameters and the two live contextual registers independently.
instructions={a:(op,operands) for a,_,op,operands in im.disasm(0x2000a00,0x2000b99)}
for rva,expected in {
 0x2000a0d:('mov','r15, rcx'),0x2000aa0:('mov','rax, qword ptr [rbx - 0x18]'),
 0x2000aa4:('mov','r8, qword ptr [rax + 0x378]'),
 0x2000abe:('call','0x140182510'),0x2000ac3:('mov','byte ptr [rbx], al'),
}.items():assert instructions[im.base+rva]==expected,(hex(rva),instructions.get(im.base+rva))
assert struct.unpack('<Q',im.read(0x33c98b8+0x208,8))[0]==0x142000a00
# Inspect the linked artifact (not just the source or its object file).
binary=(ROOT/args.artifacts/'ds2_crafting_unlocks.asi').read_bytes()
pe=struct.unpack_from('<I',binary,0x3c)[0];opt=pe+24
sections=[]
for i in range(struct.unpack_from('<H',binary,pe+6)[0]):
 off=opt+struct.unpack_from('<H',binary,pe+20)[0]+40*i
 size,rva,rawsize,raw=struct.unpack_from('<IIII',binary,off+8);sections.append((rva,rawsize,raw))
def rawoff(rva):
 return next(raw+rva-va for va,size,raw in sections if va<=rva<va+size)
def rvaof(off):
 return next(va+off-raw for va,size,raw in sections if raw<=off<raw+size)
prefix=bytes.fromhex('48 83 ec 28 49 89 c1 4c 89 7c 24 20 e8')
hits=[m.start() for m in re.finditer(re.escape(prefix),binary)]
assert len(hits)==1
start=rvaof(hits[0]);assert binary[hits[0]+17:hits[0]+22]==bytes.fromhex('48 83 c4 28 c3')
pdata,size=struct.unpack_from('<II',binary,opt+112+3*8)
entries=list(struct.iter_unpack('<III',binary[rawoff(pdata):rawoff(pdata)+size]))
matches=[e for e in entries if e[0]==start and e[1]==start+22];assert len(matches)==1
unwind=rawoff(matches[0][2]);assert binary[unwind:unwind+6]==bytes.fromhex('01 04 01 00 04 42')
# Original 1.7.0 settings remain byte-for-byte as the INI prefix.
ini=(ROOT/args.artifacts/'ds2_crafting_unlocks.ini').read_bytes()
with zipfile.ZipFile(ROOT/'release/DS2_Crafting_Equipment_Overhaul_v1.7.0.zip') as z:
 baseline_ini=z.read('ds2_crafting_unlocks.ini')
assert ini.startswith(baseline_ini)
config=configparser.ConfigParser(inline_comment_prefixes=(';',));config.read_string(ini.decode('ascii'))
assert config.getint('PCCStructures','UnlockAll')==args.default_pcc
result={'status':'PASS','version':args.version,'asi_sha256':hashlib.sha256(binary).hexdigest(),
 'exe_sha256':manifest['executable_sha256'],'resource_variants':13,'structure_categories':11,
 'native_tier_counts':[5,12,12],'overridden_tier_counts':[5,11,11],
 'resource_schema_and_export_identities':True,'generated_data_current':True,
 'native_callsite_abi_verified':True,'linked_thunk_rva':hex(start),'linked_thunk_unwind_verified':True,
 'legacy_ini_prefix_unchanged':True,'default_unlock_all':args.default_pcc,'game_test':False}
reports=ROOT/args.reports;reports.mkdir(parents=True,exist_ok=True)
(reports/'PCC_PROOF.json').write_text(json.dumps(result,indent=2)+'\n',newline='\n')
print(json.dumps(result,indent=2))
