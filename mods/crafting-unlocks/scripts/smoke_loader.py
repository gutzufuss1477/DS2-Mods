"""Load the mod only in disposable Python test processes, never in DS2."""
from pathlib import Path
import ctypes
import json
import shutil
import subprocess
import sys
import time
ROOT=Path(__file__).resolve().parents[1]
if len(sys.argv)>1 and sys.argv[1]=='--worker':
    dll=Path(sys.argv[2]); marker=sys.argv[3]
    loaded=ctypes.WinDLL(str(dll))
    log=dll.parent/'ds2_crafting_unlocks.log'
    for _ in range(100):
        text=log.read_text(encoding='utf-8',errors='replace') if log.exists() else ''
        if marker in text:
            print(text.strip()); sys.exit(0)
        time.sleep(0.05)
    print('Timed out waiting for '+marker);sys.exit(1)
base=(ROOT/'release/ds2_crafting_unlocks.ini').read_text(encoding='utf-8-sig')
results=[]
for name,config,marker in [('disabled',base.replace('Enabled=1','Enabled=0'),'DISABLED: no patch installed.'),('wrong_executable',base.replace('FreeCrafting=0','FreeCrafting=1'),'VERSION_BLOCKED:'),('invalid_config',base.replace('FreeCrafting=0','FreeCrafting=2'),'CONFIG_ERROR')]:
    folder=ROOT/'build'/('loader_smoke_'+name)
    folder.mkdir(exist_ok=True)
    dll=folder/'ds2_crafting_unlocks.asi';shutil.copy2(ROOT/'release/ds2_crafting_unlocks.asi',dll)
    (folder/'ds2_crafting_unlocks.ini').write_text(config,encoding='utf-8')
    log=folder/'ds2_crafting_unlocks.log'
    if log.exists():log.unlink()
    run=subprocess.run([sys.executable,__file__,'--worker',str(dll),marker],capture_output=True,text=True,timeout=15)
    result=dict(case=name,exit_code=run.returncode,stdout=run.stdout,stderr=run.stderr)
    results.append(result)
    assert run.returncode==0,(name,run.stdout,run.stderr)
    assert 'HOOK_INSTALLED:' not in run.stdout
    assert 'FREECRAFTING_ON:' not in run.stdout
    print(name+': PASS')
(ROOT/'docs/FREECRAFTING_LOADER_SMOKE.json').write_text(json.dumps(results,indent=2),encoding='utf-8')
print('PASS: DLL loads in Windows; disabled, exact-build gate and invalid-config handling verified. No game process used.')
