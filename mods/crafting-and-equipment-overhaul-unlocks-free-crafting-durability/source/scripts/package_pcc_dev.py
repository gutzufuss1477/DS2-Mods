"""Package one locally validated PCC test artifact. Never installs or publishes."""
import configparser,hashlib,json,zipfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
BUILD=ROOT/'build/pcc-dev1';REPORTS=ROOT/'validation/pcc-dev1'
VERSION='1.7.1-pcc-dev1'
def sha(data):return hashlib.sha256(data).hexdigest()
asi=(BUILD/'ds2_crafting_unlocks.asi').read_bytes()
ini=(BUILD/'ds2_crafting_unlocks.ini').read_bytes()
with zipfile.ZipFile(ROOT/'release/DS2_Crafting_Equipment_Overhaul_v1.7.0.zip') as z:
    baseline=z.read('ds2_crafting_unlocks.ini')
assert ini.startswith(baseline)
config=configparser.ConfigParser(inline_comment_prefixes=(';',));config.read_string(ini.decode('ascii'))
assert config.getint('PCCStructures','UnlockAll')==0
proof=json.loads((REPORTS/'PCC_PROOF.json').read_text())
build=json.loads((REPORTS/'BUILD_VALIDATION.json').read_text())
integration=json.loads((REPORTS/f'integration-{VERSION}.json').read_text())
loader=json.loads((REPORTS/'FREECRAFTING_LOADER_SMOKE.json').read_text())
assert proof['status']=='PASS' and proof['asi_sha256']==build['sha256']==integration['sha256']==sha(asi)
assert integration['hook_count']==95 and not integration['overlapping_hooks']
assert len(loader)==6 and all(x['exit_code']==0 and 'PCC_ON:' not in x['stdout'] for x in loader)
assert all(VERSION in x['stdout'] for x in loader)
assert 'PASS 242 assertions' in (REPORTS/'pcc-native.txt').read_text()
native=(REPORTS/'native-suites.txt').read_text(encoding='utf-8')
assert all(x in native for x in ['ALL_WINDOWS_TESTS_PASSED','Ran 20 tests','Native branch cases executed: 110208','Ran 44 tests'])
assert not any(x in native for x in ['FAILED (','FAIL:','Traceback (most recent call last)'])
validation={'version':VERSION,'status':'OFFLINE_VALIDATED_FIRST_GAME_TEST_ONLY',
 'asi_sha256':sha(asi),'ini_sha256':sha(ini),'asi_bytes':len(asi),
 'required_exe_sha256':proof['exe_sha256'],'pcc_default':0,'pcc_native_assertions':242,
 'atlas_tests':20,'atlas_native_branch_cases':110208,'weapon_tests':44,
 'existing_regression_suites':'PASS','exact_patch_sites':95,'hook_overlap':False,
 'windows_loader_cases':6,'resource_schema_and_identities_verified':True,
 'game_tested':False,'installed':False,'published':False,
 'limits':['Hot Spring Digger excluded','Native tier/placement/network/bandwidth/material rules retained',
           'World loading, actual placement, persistence and use await the first game test']}
files={'ds2_crafting_unlocks.asi':asi,'ds2_crafting_unlocks.ini':ini,
 'README_TEST_DE.txt':(ROOT/'docs/PCC_DEV1_TEST_DE.txt').read_bytes(),
 'VALIDATION.json':(json.dumps(validation,indent=2)+'\n').encode('utf-8')}
files['SHA256SUMS.txt']=(''.join(f'{sha(data)}  {name}\n' for name,data in files.items())).encode('ascii')
out=ROOT/'dev-release'/f'DS2_Crafting_Equipment_Overhaul_v{VERSION}.zip'
out.parent.mkdir(exist_ok=True)
with zipfile.ZipFile(out,'w',compression=zipfile.ZIP_DEFLATED,compresslevel=9) as z:
 for name,data in files.items():
  item=zipfile.ZipInfo(name,(2026,10,15,0,0,0));item.compress_type=zipfile.ZIP_DEFLATED
  item.external_attr=0o100644<<16;z.writestr(item,data)
with zipfile.ZipFile(out) as z:
 assert z.testzip() is None and z.namelist()==list(files)
 assert sum(n.endswith('.asi') for n in z.namelist())==1
 assert sum(n.endswith('.ini') for n in z.namelist())==1
 for name,data in files.items():assert z.read(name)==data
package={'file':str(out.relative_to(ROOT)).replace('\\','/'),'sha256':sha(out.read_bytes()),
 'bytes':out.stat().st_size,'files':{n:{'bytes':len(b),'sha256':sha(b)} for n,b in files.items()},
 'verified_archive_bytes_match_tested_artifacts':True,**validation}
(REPORTS/'PACKAGE.json').write_text(json.dumps(package,indent=2)+'\n',newline='\n')
print(out)
print('SHA256 '+package['sha256'])
