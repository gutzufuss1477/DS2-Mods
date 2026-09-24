"""Produce and verify the single unified Nexus candidate, using only stdlib."""
import hashlib
from pathlib import Path
import re
import zipfile

root = Path(__file__).resolve().parents[1]
version = re.search(r'#define APAS_VERSION "([^"]+)"', (root / 'src/apas.cpp').read_text()).group(1)
release = root / 'release'
release.mkdir(exist_ok=True)
archive = release / f'DS2_APAS_Memory_Costs_v{version}_NEXUS.zip'
# Earlier versioned Nexus archives and dedicated test archives are historical
# artifacts. The archive named above is the only current package this run writes.
sources = {
    'ds2_apas_memory_costs.asi': root / 'build/ds2_apas_memory_costs.asi',
    'ds2_apas_memory_costs.ini': root / 'ds2_apas_memory_costs.ini',
    'README.txt': root / 'docs/RELEASE_README.txt',
}
payload = {name: path.read_bytes() for name, path in sources.items()}
for name in ['ds2_apas_memory_costs.ini', 'README.txt']:
    payload[name] = payload[name].replace(b'\r\n', b'\n').replace(b'\n', b'\r\n')
    assert version.encode() in payload[name], f'Stale version in {name}'
assert b'UnlockAll=0' in payload['ds2_apas_memory_costs.ini']
assert version.encode() in payload['ds2_apas_memory_costs.asi']
with zipfile.ZipFile(archive, 'w', compression=zipfile.ZIP_DEFLATED) as z:
    for name, data in payload.items():
        (release / name).write_bytes(data)
        info = zipfile.ZipInfo(name, (2026, 9, 23, 0, 0, 0))
        info.compress_type = zipfile.ZIP_DEFLATED
        info.external_attr = 0o100644 << 16
        z.writestr(info, data)
with zipfile.ZipFile(archive) as z:
    assert z.namelist() == list(payload)
    assert z.testzip() is None
    for name, data in payload.items():
        assert z.read(name) == data
hashes = []
for path in [*(release / name for name in payload), archive]:
    sha = hashlib.sha256(path.read_bytes()).hexdigest().upper()
    hashes.append(f'{sha}  {path.relative_to(root).as_posix()}')
(root / 'SHA256SUMS.txt').write_text('\n'.join(hashes) + '\n', encoding='ascii')
print(archive)
print('\n'.join(hashes))
print('Verified: one ZIP, one ASI, one INI, one README; source/binary/config versions match.')
