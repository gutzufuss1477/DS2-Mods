"""Create transparent diagnostic/full archives. Never modifies source payloads."""
import argparse, hashlib, json, shutil, zipfile
from pathlib import Path

def digest(p): return hashlib.sha256(p.read_bytes()).hexdigest()
def main():
    parser=argparse.ArgumentParser()
    parser.add_argument('--payload-root',type=Path,required=True)
    parser.add_argument('--exe',type=Path,required=True)
    parser.add_argument('--out',type=Path,required=True)
    args=parser.parse_args()
    base=Path(__file__).resolve().parent
    version=json.loads((base/'package.json').read_text())['version']
    data=json.loads((base/'src-tauri/generated/data.json').read_text(encoding='utf-8'))
    args.out.mkdir(parents=True,exist_ok=True)
    common={'DS2ModManager.exe':args.exe,'README.md':base/'README.md'}
    for f in args.payload_root.iterdir():
        if f.is_file() and (f.name.startswith('LICENSE') or f.name.startswith('THIRD_PARTY')):common['licenses/native/'+f.name]=f
    deps=json.loads((base/'build-dependencies.json').read_text(encoding='utf-8-sig'))
    notices=['# Rust / Tauri dependencies','', 'Versions and original license notices shipped with this build.','']
    for p in sorted(deps['packages'],key=lambda p:(p['name'],p['version'])):
        if not p.get('source'):continue
        notices.append(f"- {p['name']} {p['version']} — {p.get('license','see included notices')}")
        root=Path(p['manifest_path']).parent
        for f in root.iterdir():
            if f.is_file() and any(f.name.lower().startswith(s) for s in ['license','licence','notice','copying','copyright']):common[f"licenses/rust/{p['name']}-{p['version']}/{f.name}"]=f
    notices_path=args.out/'THIRD_PARTY_NOTICES.md';notices_path.write_text('\n'.join(notices)+'\n',encoding='utf-8')
    common['THIRD_PARTY_NOTICES.md']=notices_path
    manifests={}
    for flavor in ['manager-only','complete']:
        files=dict(common)
        if flavor=='complete':
            specs=[data['catalog']['loader']]+[f for m in data['catalog']['mods'] for f in m['files']]
            for f in specs:
                src=args.payload_root/f['payload']
                assert digest(src).lower()==f['sha256'].lower(),f['payload']
                files[f['payload']]=src
        folder=args.out/flavor/'DS2-Mod-Suite';folder.mkdir(parents=True,exist_ok=False)
        for name,src in files.items():
            dest=folder/name;dest.parent.mkdir(parents=True,exist_ok=True);shutil.copyfile(src,dest)
        manifest={name:{'sha256':digest(src),'bytes':src.stat().st_size} for name,src in sorted(files.items())}
        (folder/'manifest.json').write_text(json.dumps({'variant':flavor,'version':version,'files':manifest},indent=2)+'\n',encoding='utf-8')
        archive=args.out/f'DS2_Rust_Manager_{version}_{flavor}.zip'
        with zipfile.ZipFile(archive,'w',compression=zipfile.ZIP_DEFLATED,compresslevel=9) as z:
            for path in sorted(folder.rglob('*')):
                if path.is_file():z.write(path,'DS2-Mod-Suite/'+path.relative_to(folder).as_posix())
        manifests[flavor]={'archive':str(archive),'sha256':digest(archive),'bytes':archive.stat().st_size,'exe_sha256':digest(args.exe),'file_count':len(manifest)+1}
    (args.out/'artifacts.json').write_text(json.dumps(manifests,indent=2)+'\n',encoding='utf-8')
    print(json.dumps(manifests,indent=2))
if __name__=='__main__':main()
