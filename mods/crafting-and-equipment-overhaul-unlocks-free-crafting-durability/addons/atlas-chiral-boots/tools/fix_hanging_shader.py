"""Append a private two-influence material, retaining the working worn material."""
import hashlib
import json
from pathlib import Path
import struct
from native_bundle import ROOT, Graph, DSAR, encode_link, archive_segments
from fix_stream_ownership import audited_group
from build_native_materials import add_hanging_material
from serialize_native_geometry import PACKAGE, GAME, result_hash
from overlay_cache import build_cache
from fix_array_reservations import array_tables, graph_segments, reservation_proof, validate_cache_reservations

STAGE = ROOT/'staging/test11-hanging-shader'


def main():
    previous = ROOT/'staging/test10-private-clip-buffers'
    assert not STAGE.exists()
    prior_report = json.loads((previous/'report.json').read_text())
    before = Path(prior_report['cache'])
    graph = Graph(before/'streaming_graph.core')
    original = Graph(PACKAGE/'streaming_graph.core')
    old_links = (before/'streaming_links.stream').read_bytes()
    g = audited_group(graph,old_links,31126,ROOT/'evidence/test10-resource-readback/raw')
    source = PACKAGE/'package.42.02.core.stream'
    old,native = DSAR(before/source.name),DSAR(source)
    readers = {}
    captured = bytearray()
    for fid,n,at in g.spans:
        name=graph.files[fid].split('/')[-1]
        if fid not in readers:readers[fid]=DSAR(before/name if (before/name).exists() else PACKAGE/name)
        captured.extend(readers[fid].read(at,n))
    assert captured == g.data
    full = json.loads((ROOT/'staging/native-complete-v1/report.json').read_text())
    material = full['material']
    material['textures'] = {int(k):v for k,v in material['textures'].items()}
    added = add_hanging_material(g,material)
    for mesh in [48122,48132]:
        r = g.edit_private_copy(mesh)
        assert r.links.count((None,material['shadingGroup'])) == 1
        r.remap({(None,material['shadingGroup']):(None,added['shadingGroup'])})
    assert len(g.added)==2 and all(not r.locators for r in g.added)
    payload = b''.join(r.raw for r in g.added)
    resources = [g.edits.get(i) or g.original(i) for i in range(len(g.rows))]+g.added
    group_links = b''.join(encode_link(x) for r in resources for x in r.links)
    links = old_links+group_links
    p,info=graph.parts,g.info
    types = bytearray(p['typeTable'][4:])
    count,capacity=struct.unpack_from('<II',types,8)
    assert count==capacity
    old_types=types[20+2*info['typeStart']:20+2*(info['typeStart']+info['typeCount'])]
    new_types=old_types+b''.join(old_types[2*r.source:2*r.source+2] for r in g.added)
    types.extend(new_types)
    struct.pack_into('<II',types,8,count+len(new_types)//2,count+len(new_types)//2)
    p['typeTable']=struct.pack('<I',len(types))+types
    spans=g.spans+[(graph.files.index('cache:package/package.42.02.core.stream'),len(payload),old.total)]
    span_at=struct.unpack_from('<I',p['spans'])[0]
    p['spans']=struct.pack('<I',span_at+len(spans))+p['spans'][4:]+b''.join(struct.pack('<Iii',*s) for s in spans)
    groups=bytearray(p['groups'])
    struct.pack_into('<iq',groups,info['relative']+4,len(resources),len(g.data)+len(payload))
    struct.pack_into('<6I',groups,info['relative']+32,span_at,len(spans),count,len(resources),len(old_links),len(group_links))
    p['groups']=groups
    p['linkSize']=struct.pack('<I',len(links))
    # New material arrays need real allocation reservations, even though no
    # geometry/controller arrays change. Derive growth from original readback.
    census=json.loads((ROOT/'evidence/test07-dynamic-array-census.json').read_text())
    names=json.loads((ROOT/'evidence/test07-array-table-baseline.json').read_text())['names']
    growth={}
    donor_ids={r.source for r in g.added}
    for line in (ROOT/'evidence/test10-resource-readback/raw/31126.arrays.tsv').read_text().splitlines():
        owner,typ,elements,kind=line.split('\t')
        if int(owner) in donor_ids and kind=='Array':
            value=growth.setdefault(typ,[0,0]);value[0]+=1;value[1]+=int(elements)
    for typ,(a,n) in growth.items():
        value=census['31126']['actual'][typ];value[0]+=a;value[1]+=n
    census['31126']['test11MaterialGrowth']=growth
    census_file=ROOT/'evidence/test11-dynamic-array-census.json'
    assert not census_file.exists()
    census_file.write_text(json.dumps(census,indent=2))
    tables=array_tables(graph)
    for row in tables[31126]:
        typ=names[row[0]&0xffff]
        if typ in growth:row[1:]=census['31126']['actual'][typ]
    raw=b''.join(struct.pack('<I',len(tables[x['id']]))+
                 b''.join(struct.pack('<III',*e) for e in tables[x['id']]) for x in graph.groups)
    p['arrayTable']=struct.pack('<I',len(raw)//4)+raw
    STAGE.mkdir();bundle=STAGE/'bundle';bundle.mkdir()
    chunks=old.chunks[len(native.chunks):]
    payloads=[old.read(c[0],c[2]) for c in chunks]+[payload]
    starts,segments,size=archive_segments(source,payloads,bundle,'geometry')
    assert starts[:-1]==[c[0] for c in chunks] and starts[-1]==old.total
    manifest=json.loads((previous/'bundle/overlay.json').read_text())
    manifest['files'][0].update(segments=segments,resultSize=size,resultSha256=result_hash(source,bundle,segments))
    manifest['files'][1].update(segments=graph_segments(original,graph,bundle),resultSize=len(graph.bytes()),
                                resultSha256=hashlib.sha256(graph.bytes()).hexdigest())
    baseline=(PACKAGE/'streaming_links.stream').stat().st_size
    delta=old_links[baseline:]+group_links
    (bundle/'links-delta.bin').write_bytes(delta)
    manifest['files'][2].update(resultSize=len(links),resultSha256=hashlib.sha256(links).hexdigest(),
        segments=[dict(original=0,length=baseline),dict(payload='links-delta.bin',offset=0,length=len(delta),
        sha256=hashlib.sha256(delta).hexdigest())])
    (bundle/'overlay.json').write_text(json.dumps(manifest,indent=2))
    cache=build_cache(GAME,bundle,manifest,STAGE/'cache')
    final=DSAR(cache/source.name)
    raw=old.read(native.total,old.total-native.total)
    assert final.read(native.total,len(raw))==raw
    assert final.total==old.total+len(payload)<2**31
    proof=reservation_proof(graph,manifest)
    proof['census']='evidence/test11-dynamic-array-census.json'
    report=dict(cache=str(cache),previousCache=str(before),status='Test11 hanging shader; independent readback pending',
                hangingMaterial=added,modifiedMeshLinks=[48122,48132],addedResources=2,appendedRecordBytes=len(payload),
                previousPrivatePayloadUnchanged=True,geometryAndTexturesUnchanged=True,
                clipOwnershipFixes=prior_report['clipOwnershipFixes'],arrayReservationProof=proof,
                materialArrayGrowth=growth,workingWornMaterialPreserved=True)
    (STAGE/'report.json').write_text(json.dumps(report,indent=2))
    validate_cache_reservations(STAGE)
    print(json.dumps(report,indent=2))


if __name__=='__main__':
    main()
