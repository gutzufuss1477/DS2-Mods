"""Package the tested local APAS build; does not publish or touch the game."""
import argparse
import hashlib
import json
from pathlib import Path
import zipfile

ROOT = Path(__file__).resolve().parents[1]
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--build', type=Path, required=True)
p.add_argument('--output', type=Path, required=True)
args = p.parse_args()
manifest = json.loads((args.build / 'apas/manifest.json').read_text())
binary = args.build / 'apas/ds2_apas_memory_costs.asi'
assert hashlib.sha256(binary.read_bytes()).hexdigest() == manifest['sha256'].lower()
for name, digest in manifest['sourceHashes'].items():
    assert hashlib.sha256((ROOT / name).read_bytes()).hexdigest() == digest.lower(), name
provenance = {key: manifest[key] for key in
              ['sha256', 'bytes', 'msvc', 'windowsSdk', 'sourceHashes']}
provenance['version'] = '4.1.0-preview.1'
provenance['status'] = 'Development test; no in-game or antivirus acceptance claim.'
files = {
    binary.name: binary.read_bytes(),
    'ds2_apas_memory_costs.ini': (ROOT / 'ds2_apas_memory_costs.ini').read_bytes(),
    'README.md': (ROOT / 'README.md').read_bytes(),
    'BUILD-PROVENANCE.json': (json.dumps(provenance, indent=2) + '\n').encode(),
}
args.output.mkdir(parents=True, exist_ok=True)
archive = args.output / 'DS2_APAS_4.1.0-preview.1.zip'
with zipfile.ZipFile(archive, 'x', compression=zipfile.ZIP_DEFLATED) as z:
    for name, data in files.items():
        info = zipfile.ZipInfo(name, (2026, 11, 2, 0, 0, 0))
        info.compress_type = zipfile.ZIP_DEFLATED
        z.writestr(info, data)
with zipfile.ZipFile(archive) as z:
    assert z.testzip() is None
    assert all(z.read(name) == data for name, data in files.items())
result = {
    'archive': str(archive.resolve()), 'bytes': archive.stat().st_size,
    'sha256': hashlib.sha256(archive.read_bytes()).hexdigest(),
    'files': {name: hashlib.sha256(data).hexdigest() for name, data in files.items()},
}
(args.output / 'artifact.json').write_text(json.dumps(result, indent=2) + '\n')
print(json.dumps(result, indent=2))
