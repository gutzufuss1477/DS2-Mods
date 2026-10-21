"""Repack only private append payloads into native-size aligned archive blocks."""
from pathlib import Path
import json
import shutil
from native_bundle import ROOT, DSAR, archive_segments
from serialize_native_geometry import PACKAGE, GAME, result_hash
from overlay_cache import build_cache

previous=ROOT/'staging/test03-final-v1'
target=ROOT/'staging/test03-final-v2'
assert not target.exists()
target.mkdir();bundle=target/'bundle';bundle.mkdir()
report=json.loads((previous/'report.json').read_text())
manifest=json.loads((previous/'bundle/overlay.json').read_text())
source=PACKAGE/'package.42.02.core.stream'
native=DSAR(source)
old=DSAR(Path(report['cache'])/source.name)
extra=old.chunks[len(native.chunks):]
assert extra[0][0]==native.total
payloads=[old.read(c[0],c[2]) for c in extra]
starts,segments,size=archive_segments(source,payloads,bundle,'geometry')
assert starts==[c[0] for c in extra]
manifest['files'][0].update(segments=segments,resultSize=size,resultSha256=result_hash(source,bundle,segments))
for name in ['graph-delta.bin','links-delta.bin']:shutil.copyfile(previous/'bundle'/name,bundle/name)
(bundle/'overlay.json').write_text(json.dumps(manifest,indent=2))
cache=build_cache(GAME,bundle,manifest,target/'cache')
new=DSAR(cache/source.name)
for at,data in zip(starts,payloads):assert new.read(at,len(data))==data
assert all(c[1]%16==0 and c[2]<=262144 for c in new.chunks[len(native.chunks):])
assert all(a[0]==b[0] and a[2:]==b[2:] for a,b in zip(native.chunks,new.chunks))
report.update(cache=str(cache),bundleBytes=sum(p.stat().st_size for p in bundle.iterdir()),
              nativeBlockAlignment=16,maximumNewUncompressedBlock=262144,
              virtualContentIdenticalToValidatedV1=True)
(target/'report.json').write_text(json.dumps(report,indent=2))
print(json.dumps(dict(cache=str(cache),newChunks=len(new.chunks)-len(native.chunks),bundleBytes=report['bundleBytes'])))
