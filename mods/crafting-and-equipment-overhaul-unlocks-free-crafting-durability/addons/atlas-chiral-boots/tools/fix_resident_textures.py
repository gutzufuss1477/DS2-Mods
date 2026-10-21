"""Repair only private Texture headers; retain every original and all pixel data."""
import hashlib
import json
import shutil
import struct
import uuid
from pathlib import Path
from native_bundle import ROOT, DSAR, archive_segments
from serialize_native_geometry import PACKAGE, GAME, result_hash
from overlay_cache import build_cache


def private_texture_rows(payload):
    """Locate deterministic resource UUIDs; validate full serialized records."""
    recipes = json.loads((ROOT/'staging/native-materials-v2/report.json').read_text())['textures']
    for row in recipes:
        key = uuid.uuid5(uuid.NAMESPACE_URL,
                        'ds2-chiral-cuff-v3/67696/chiral/'+row['name']).bytes_le
        assert payload.count(key) == 1, row['name']
        at = payload.index(key)
        total, embedded, streamed, streamed_mips = struct.unpack_from('<4I', payload, at+94)
        assert total == embedded+12 and embedded == row['bytes']
        assert (streamed, streamed_mips) == (0, 0)
        assert struct.unpack_from('<I', payload, at+17)[0] == 8
        assert payload[at+21:at+53] == b'\xff'*32
        assert struct.unpack_from('<HH', payload, at+64) == (row['width'], row['height'])
        assert payload[at+70] == row['mips'] and payload[at+71] == row['pixelFormat']
        pixels = payload[at+110:at+110+embedded]
        assert hashlib.sha256(pixels).hexdigest() == row['sha256']
        yield row, at, pixels


def validate_resident_headers(payload):
    checked = []
    for row, at, pixels in private_texture_rows(payload):
        assert payload[at+75] & 8, row['name']+': compressed texture would request unordered GPU writes'
        assert payload[at+75] == 0xa9, row['name']+': changed native usage flags'
        assert payload[at+76] == 0, row['name']+': automatic streamed mip selection'
        block = 16 if row['pixelFormat'] == 75 else 8
        sizes = [max(1,(max(1,row['width'] >> m)+3)//4) *
                 max(1,(max(1,row['height'] >> m)+3)//4)*block
                 for m in range(row['mips'])]
        assert sum(sizes) == len(pixels), row['name']
        checked.append(dict(name=row['name'], mips=row['mips'], consumedPixelBytes=sum(sizes),
                            pixelSha256=hashlib.sha256(pixels).hexdigest(),
                            flags=hex(payload[at+75]), streamingSelection=payload[at+76]))
    assert len(checked) == 7
    return checked


def main():
    old_stage=ROOT/'staging/test03-final-v2'
    target=ROOT/'staging/test07-resident-readonly-textures'
    assert not target.exists()
    old_report=json.loads((old_stage/'report.json').read_text())
    manifest=json.loads((old_stage/'bundle/overlay.json').read_text())
    source=PACKAGE/'package.42.02.core.stream'
    native=DSAR(source)
    old=DSAR(Path(old_report['cache'])/source.name)
    extra=old.read(native.total,old.total-native.total)
    try:
        validate_resident_headers(extra)
    except AssertionError as err:
        old_rejection=str(err)
    else:
        raise AssertionError('Regression fixture unexpectedly passed')
    test06_report=json.loads((ROOT/'staging/test06-resident-textures/report.json').read_text())
    test06=DSAR(Path(test06_report['cache'])/source.name)
    test06_extra=test06.read(native.total,test06.total-native.total)
    try:
        validate_resident_headers(test06_extra)
    except AssertionError as err:
        test06_rejection=str(err)
    else:
        raise AssertionError('Test06 regression fixture unexpectedly passed')
    repaired=bytearray(extra)
    changed=[]
    for row, at, pixels in private_texture_rows(extra):
        assert extra[at+75:at+77] == bytes([0xa9,0xff])
        repaired[at+76] = 0
        changed.append(dict(texture=row['name'], virtualOffset=native.total+at+75,
                            old='a9ff', test06='a000', new='a900', pixelSha256=row['sha256']))
    check=validate_resident_headers(repaired)
    differences=[i for i,(a,b) in enumerate(zip(extra,repaired)) if a!=b]
    assert differences==sorted([c['virtualOffset']-native.total+1 for c in changed])
    assert len(differences)==7 and len(extra)==len(repaired)
    assert len(test06_extra)==len(repaired)
    test06_differences=[i for i,(a,b) in enumerate(zip(test06_extra,repaired)) if a!=b]
    assert test06_differences==sorted([c['virtualOffset']-native.total for c in changed])
    target.mkdir(); bundle=target/'bundle'; bundle.mkdir()
    # Preserve all existing virtual chunk starts, including boundaries between
    # independently appended geometry and metadata payloads.
    chunks=old.chunks[len(native.chunks):]
    payloads=[repaired[c[0]-native.total:c[0]-native.total+c[2]] for c in chunks]
    starts,segments,size=archive_segments(source,payloads,bundle,'geometry')
    assert starts==[c[0] for c in chunks]
    manifest['files'][0].update(segments=segments,resultSize=size,
                               resultSha256=result_hash(source,bundle,segments))
    for name in ['graph-delta.bin','links-delta.bin']:
        shutil.copyfile(old_stage/'bundle'/name,bundle/name)
    (bundle/'overlay.json').write_text(json.dumps(manifest,indent=2))
    cache=build_cache(GAME,bundle,manifest,target/'cache')
    final=DSAR(cache/source.name)
    assert final.read(native.total,len(repaired))==repaired
    for name in ['streaming_graph.core','streaming_links.stream']:
        assert (cache/name).read_bytes()==(Path(old_report['cache'])/name).read_bytes()
    # Original compressed blocks stay byte-identical, with only physical offsets adjusted.
    with source.open('rb') as f, (cache/source.name).open('rb') as g:
        for a,b in zip(native.chunks,final.chunks):
            assert a[0]==b[0] and a[2:]==b[2:]
            f.seek(a[1]);g.seek(b[1])
            assert f.read(a[3])==g.read(b[3])
    report=dict(old_report,cache=str(cache),status='Test07 resident textures with original read-only usage; game test pending',
                previousCache=old_report['cache'],changedBytes=7,changes=changed,
                test06ChangedBytes=len(test06_differences),test06HeaderRegressionRejected=test06_rejection,
                oldHeaderRegressionRejected=old_rejection,residentTextureChecks=check,
                archiveOriginalCompressedBlocksIdentical=len(native.chunks),
                geometryAndPixelDataUnchanged=True,graphAndLinksUnchanged=True,
                bundleBytes=sum(p.stat().st_size for p in bundle.iterdir()),
                diagnosisEvidence=['evidence/test06-minidump-triage.json',
                                   'evidence/test06-native-texture-desc-builder.txt',
                                   'evidence/test05-texture-native-functions.txt'])
    (target/'report.json').write_text(json.dumps(report,indent=2))
    print(json.dumps({k:report[k] for k in ['cache','changedBytes','oldHeaderRegressionRejected',
                                        'archiveOriginalCompressedBlocksIdentical','bundleBytes']},indent=2))


if __name__=='__main__':
    main()
