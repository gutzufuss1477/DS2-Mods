"""Package the already-built test ASI; intentionally include no installer."""
from pathlib import Path
import hashlib, zipfile
root=Path(__file__).resolve().parents[1]
asi=root/'dist/DS2_HighDensityBackpackModules_v1.1.0-test5.asi'
archive=root/'dist/DS2_High_Density_Backpack_Modules_v1.1.0-test5.zip'
data=asi.read_bytes()
entry=zipfile.ZipInfo(asi.name, date_time=(2026,9,23,0,0,0))
entry.compress_type=zipfile.ZIP_DEFLATED
entry.external_attr=0o100644<<16
with zipfile.ZipFile(archive,'w',compression=zipfile.ZIP_DEFLATED,compresslevel=9) as z:z.writestr(entry,data)
with zipfile.ZipFile(archive) as z:
    assert z.namelist()==[asi.name]
    assert z.testzip() is None and z.read(asi.name)==data
(root/'dist/SHA256SUMS.txt').write_text('\n'.join(hashlib.sha256(p.read_bytes()).hexdigest().upper()+'  '+p.name for p in (asi,archive))+'\n')
print(archive)
print('Verified: exactly one ASI; ZIP CRC and extracted bytes match the build.')
