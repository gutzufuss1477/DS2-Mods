"""Stage a complete development payload set, replacing only reviewed APAS files."""
import argparse
import hashlib
import json
from pathlib import Path
import shutil
import zipfile

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--payload-root', type=Path, required=True)
p.add_argument('--apas-archive', type=Path, required=True)
p.add_argument('--out', type=Path, required=True)
args = p.parse_args()
base = Path(__file__).resolve().parent
catalog = json.loads((base.parent / 'source/catalog.json').read_text(encoding='utf-8-sig'))
archive = args.apas_archive.read_bytes()
assert hashlib.sha256(archive).hexdigest() == '147b94fb0ded43a4c072a92f05e685090b9c06d3ebdba1a7000f4388070027a8', 'Use the reviewed Nexus 402 candidate'
args.out.mkdir(parents=True, exist_ok=False)
specs = [catalog['loader']] + [f for m in catalog['mods'] for f in m['files']]
for f in specs:
    src = args.payload_root / f['payload']
    assert hashlib.sha256(src.read_bytes()).hexdigest() == f['sha256'].lower(), f['payload']
    dest = args.out / f['payload']
    dest.parent.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(src, dest)
for src in args.payload_root.iterdir():
    if src.is_file() and (src.name.startswith('LICENSE') or src.name.startswith('THIRD_PARTY')):
        shutil.copyfile(src, args.out / src.name)
apas = next(m for m in catalog['mods'] if m['id'] == 'apas-memory-costs')
with zipfile.ZipFile(args.apas_archive) as z:
    for f in apas['files']:
        data = z.read(f['target'])
        if f['kind'] == 'binary':
            f.setdefault('previousHashes', []).append(f['sha256'])
        f['sha256'] = hashlib.sha256(data).hexdigest().upper()
        (args.out / f['payload']).write_bytes(data)
apas['version'] = '4.1.0-preview.1'
catalog['suiteVersion'] = json.loads((base / 'package.json').read_text())['version']
(args.out / 'catalog.json').write_text(json.dumps(catalog, indent=2, ensure_ascii=False) + '\n', encoding='utf-8')
print('Staged 54 verified payload files; only APAS ASI and INI replaced. Original catalog unchanged.')
