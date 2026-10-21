"""Pin the user-confirmed public 1.9.8 release and copy its source for addon API work."""
import hashlib
import json
from pathlib import Path
import shutil
import zipfile

ROOT = Path(__file__).resolve().parents[1]
SOURCE = Path('D:/DS2-Asset-Lab/analysis/warrior-guitar-v198-nexus')
TARGET = ROOT/'development/crafting-v198-addon-api'
EVIDENCE = ROOT/'evidence/published-v198-baseline'
ZIP_SHA = '31ad45156822377f7d8d0e08dd0e9fc9a034bdb89919f3a5fd1d6f772dbcc971'
ASI_SHA = '7d158c1d8698a36143f55a60b3004e09b20e4fedef48ec9e8be097f7426ef35a'
INI_SHA = 'c028993823f2e23de94c75adc0ab75037dc17f2306ead60e2e381da6028a4773'
sha = lambda p: hashlib.sha256(p.read_bytes()).hexdigest()

def main():
    assert not TARGET.exists() and not EVIDENCE.exists(), 'Keep existing baseline evidence'
    public = SOURCE/'release'
    archive = public/'DS2_Crafting_Equipment_Overhaul_v1.9.8.zip'
    assert sha(archive)==ZIP_SHA
    assert sha(public/'ds2_crafting_unlocks.asi')==ASI_SHA
    with zipfile.ZipFile(archive) as z:
        assert z.testzip() is None
        for name,want in [('ds2_crafting_unlocks.asi',ASI_SHA),('ds2_crafting_unlocks.ini',INI_SHA)]:
            names=[p for p in z.namelist() if Path(p).name==name]
            assert len(names)==1
            assert hashlib.sha256(z.read(names[0])).hexdigest()==want
    EVIDENCE.mkdir(parents=True)
    TARGET.mkdir(parents=True)
    inventory = {}
    for folder in ['src','config','assets','scripts','tests','tools','docs']:
        for original in sorted((SOURCE/folder).rglob('*')):
            if not original.is_file() or '__pycache__' in original.parts:
                continue
            relative = original.relative_to(SOURCE)
            destination = TARGET/relative
            destination.parent.mkdir(parents=True,exist_ok=True)
            shutil.copy2(original,destination)
            h=sha(original)
            assert sha(destination)==h
            inventory[relative.as_posix()]=dict(bytes=original.stat().st_size,sha256=h)
    shutil.copy2(SOURCE/'build.ps1',TARGET/'build.ps1')
    inventory['build.ps1']=dict(bytes=(SOURCE/'build.ps1').stat().st_size,sha256=sha(SOURCE/'build.ps1'))
    for name in [archive.name,'RELEASE_MANIFEST.json']:
        shutil.copy2(public/name,EVIDENCE/name)
    # The uploaded ZIP is authoritative; loose staging files may have other endings.
    with zipfile.ZipFile(archive) as z:
        for name in ['ds2_crafting_unlocks.asi','ds2_crafting_unlocks.ini','SHA256SUMS.txt']:
            (EVIDENCE/name).write_bytes(z.read(name))
        (TARGET/'config/public-v198.ini').write_bytes(z.read('ds2_crafting_unlocks.ini'))
    with zipfile.ZipFile(EVIDENCE/'source-unmodified.zip','x',zipfile.ZIP_DEFLATED) as z:
        for name in inventory:
            z.write(TARGET/name,name)
    report=dict(publicVersion='1.9.8',publicStatusConfirmedByUser=True,
                confirmationDateLocal='2026-10-21',
                publicStatusEvidence='User reply: Ja, veröffentlicht ist 1.9.8',
                note='Historical package manifest says nexus_uploaded=false; public status is confirmed by user, not inferred from that field.',
                publicZipSha256=ZIP_SHA,publicAsiSha256=ASI_SHA,publicIniSha256=INI_SHA,
                sourceDirectory=str(SOURCE),workingDirectory=str(TARGET),
                sourceSnapshotSha256=sha(EVIDENCE/'source-unmodified.zip'),
                sourceInventory=inventory,sourceFiles=len(inventory),
                sourceBytes=sum(x['bytes'] for x in inventory.values()),
                privateV1922UsedAsBaseline=False,gameFilesWritten=False,
                originalPublishedSourceChanged=False,
                unmodifiedRebuildValidation='Pending')
    (EVIDENCE/'baseline.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
    print(json.dumps({k:v for k,v in report.items() if k!='sourceInventory'},indent=2))

if __name__=='__main__':
    main()
