"""Stage the production catalog from verified existing payloads and reviewed add-ons."""
import argparse, hashlib, json, shutil, zipfile
from pathlib import Path

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--baseline',type=Path,required=True)
    p.add_argument('--apas-archive',type=Path,required=True)
    p.add_argument('--free-mining-dir',type=Path,required=True)
    p.add_argument('--out',type=Path,required=True)
    a=p.parse_args()
    base=Path(__file__).resolve().parent
    catalog=json.loads((base/'catalog.json').read_text(encoding='utf-8'))
    a.out.mkdir(parents=True,exist_ok=False)
    specs=[(None,catalog['loader'])]+[(m['id'],f) for m in catalog['mods'] for f in m['files']]
    with zipfile.ZipFile(a.apas_archive) as apas:
        for mod,f in specs:
            if mod=='apas-memory-costs':raw=apas.read(f['target'])
            elif mod=='infrastructure-free-mining':raw=(a.free_mining_dir/f['target']).read_bytes()
            else:raw=(a.baseline/f['payload']).read_bytes()
            assert hashlib.sha256(raw).hexdigest().lower()==f['sha256'].lower(),f['payload']
            dest=a.out/f['payload'];dest.parent.mkdir(parents=True,exist_ok=True);dest.write_bytes(raw)
    for f in a.baseline.iterdir():
        if f.is_file() and (f.name.startswith('LICENSE') or f.name.startswith('THIRD_PARTY')):
            shutil.copyfile(f,a.out/f.name)
    print(f'Staged {len(specs)} hash-verified payload files for Suite {catalog["suiteVersion"]}.')

if __name__=='__main__':main()
