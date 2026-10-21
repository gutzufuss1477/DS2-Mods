"""Rebuild native dynamic-array reservation counts from a read-only object census.

DS2 RVA 0x26fb4f0 consumes one count + (type, allocations, elements) table per
group. Each entry contributes (max(16,alignment)-1)*allocations + size*elements.
RVA 0x26f87d0 allocates that budget once for all of a group's dynamic arrays.
Fixed inline arrays and HashMaps must not be counted as dynamic Array entries.
"""
from pathlib import Path
import hashlib
import json
import shutil
import struct
from native_bundle import ROOT, Graph
from serialize_native_geometry import PACKAGE, GAME
from overlay_cache import build_cache, digest


def array_tables(graph):
    data=graph.parts['arrayTable']
    size=struct.unpack_from('<I',data)[0]
    assert len(data)==4+size*4
    cursor=4
    rows={}
    for group in graph.groups:
        count=struct.unpack_from('<I',data,cursor)[0];cursor+=4
        entries=[list(struct.unpack_from('<III',data,cursor+i*12)) for i in range(count)]
        cursor+=count*12
        assert len({e[0]&0xffff for e in entries})==count
        rows[group['id']]=entries
    assert cursor==len(data)
    return rows


def validate(graph,census,names):
    rows=array_tables(graph)
    for gid,counts in census.items():
        actual={names[t&0xffff]:[instances,elements] for t,instances,elements in rows[int(gid)]}
        assert actual==counts['actual'], f'Group {gid}: dynamic-array reservations differ from serialized resources'
    return rows


def graph_segments(original, graph, bundle):
    payload=bytearray();segments=[]
    def changed(data):
        if data:
            at=len(payload);payload.extend(data)
            segments.append(dict(payload='graph-delta.bin',offset=at,length=len(data)))
    def unchanged(at,size):
        if size:segments.append(dict(original=at,length=size))
    raw=graph.bytes()
    changed(raw[:12]);at=12
    for key in original.order:
        before,after=original.parts[key],graph.parts[key]
        if before==after:unchanged(at,len(before))
        elif len(before)>4 and after[4:len(before)]==before[4:]:
            changed(after[:4]);unchanged(at+4,len(before)-4);changed(after[len(before):])
        else:changed(after)
        at+=len(before)
    unchanged(at,4)
    (bundle/'graph-delta.bin').write_bytes(payload)
    sha=hashlib.sha256(payload).hexdigest()
    for s in segments:
        if 'payload' in s:s['sha256']=sha
    return segments


def reservation_proof(graph, manifest):
    return dict(archiveSha256=manifest['files'][0]['resultSha256'],
                linksSha256=manifest['files'][2]['resultSha256'],
                typeHashesSha256=hashlib.sha256(graph.parts['typeHashes']).hexdigest(),
                typeTableSha256=hashlib.sha256(graph.parts['typeTable']).hexdigest(),
                groupsSha256=hashlib.sha256(graph.parts['groups']).hexdigest(),
                census='evidence/test07-dynamic-array-census.json',
                names='evidence/test07-array-table-baseline.json')


def validate_cache_reservations(stage):
    """Release gate: a resource-changing build needs matching allocation evidence."""
    report=json.loads((stage/'report.json').read_text())
    manifest=json.loads((stage/'bundle/overlay.json').read_text())
    graph=Graph(Path(report['cache'])/'streaming_graph.core')
    proof=report.get('arrayReservationProof')
    assert proof is not None, 'Missing native array reservation audit; do not compile a runtime cache'
    expected=reservation_proof(graph,manifest)
    for key in ['archiveSha256','linksSha256','typeHashesSha256','typeTableSha256','groupsSha256']:
        assert proof[key]==expected[key], 'Array census no longer matches resources: '+key
    census=json.loads((ROOT/proof['census']).read_text())
    names=json.loads((ROOT/proof['names']).read_text())['names']
    validate(graph,census,names)


def main():
    previous=ROOT/'staging/test07-resident-readonly-textures'
    target=ROOT/'staging/test08-array-reservations'
    assert not target.exists()
    previous_report=json.loads((previous/'report.json').read_text())
    previous_cache=Path(previous_report['cache'])
    graph=Graph(previous_cache/'streaming_graph.core')
    original=Graph(PACKAGE/'streaming_graph.core')
    census=json.loads((ROOT/'evidence/test07-dynamic-array-census.json').read_text())
    names=json.loads((ROOT/'evidence/test07-array-table-baseline.json').read_text())['names']
    assert graph.parts['typeHashes']==original.parts['typeHashes']
    assert graph.parts['arrayTable']==original.parts['arrayTable']
    baseline=array_tables(original)
    for gid,counts in census.items():
        observed={names[t&0xffff]:[a,n] for t,a,n in baseline[int(gid)]}
        assert observed==counts['original']
        assert counts['originalCensusExactlyMatchesNativeArrayTable']
    try:validate(graph,census,names)
    except AssertionError as err:old_rejection=str(err)
    else:raise AssertionError('Test07 should fail reservations check')
    tables=array_tables(graph)
    changes=[]
    for gid,counts in census.items():
        entries=tables[int(gid)]
        assert set(counts['actual'])=={names[t&0xffff] for t,_,_ in entries}
        for entry in entries:
            wanted=counts['actual'][names[entry[0]&0xffff]]
            if entry[1:]!=wanted:
                # Only positive growth in this patch; retain native slack if a
                # future resource shrinks rather than reducing it speculatively.
                assert all(a<=b for a,b in zip(entry[1:],wanted))
                changes.append(dict(group=int(gid),type=names[entry[0]&0xffff],
                                    old=entry[1:],new=wanted))
                entry[1:]=wanted
    raw=b''.join(struct.pack('<I',len(tables[g['id']]))+
                 b''.join(struct.pack('<III',*x) for x in tables[g['id']])
                 for g in graph.groups)
    graph.parts['arrayTable']=struct.pack('<I',len(raw)//4)+raw
    assert len(graph.parts['arrayTable'])==len(original.parts['arrayTable'])
    checked=validate(graph,census,names)
    for gid in baseline:
        if str(gid) not in census:assert checked[gid]==baseline[gid]
    prior=Graph(previous_cache/'streaming_graph.core')
    assert [k for k in graph.parts if graph.parts[k]!=prior.parts[k]]==['arrayTable']
    different=sum(a!=b for a,b in zip(graph.bytes(),prior.bytes()))
    target.mkdir();bundle=target/'bundle';bundle.mkdir()
    manifest=json.loads((previous/'bundle/overlay.json').read_text())
    for name in ['geometry-table.bin','geometry-append.bin','links-delta.bin']:
        shutil.copyfile(previous/'bundle'/name,bundle/name)
    segments=graph_segments(original,graph,bundle)
    manifest['files'][1].update(resultSize=len(graph.bytes()),
                               resultSha256=hashlib.sha256(graph.bytes()).hexdigest(),
                               segments=segments)
    (bundle/'overlay.json').write_text(json.dumps(manifest,indent=2))
    cache=build_cache(GAME,bundle,manifest,target/'cache')
    assert (cache/'streaming_graph.core').read_bytes()==graph.bytes()
    validate(Graph(cache/'streaming_graph.core'),census,names)
    for name in ['package.42.02.core.stream','streaming_links.stream']:
        assert digest(cache/name)==digest(previous_cache/name)
    report=dict(previous_report,cache=str(cache),status='Test08 corrected native array reservations; game test pending',
                previousCache=previous_report['cache'],arrayReservationChanges=changes,
                graphChangedBytes=different,onlyArrayTableChanged=True,
                oldReservationRegressionRejected=old_rejection,
                missingArrayInstances=sum(x['new'][0]-x['old'][0] for x in changes),
                missingArrayElements=sum(x['new'][1]-x['old'][1] for x in changes),
                originalArrayCensusMatchesNative=True,serializedResourcesAndLinksByteIdentical=True,
                arrayCensusEvidence='evidence/test07-dynamic-array-census.json',
                arrayReservationProof=reservation_proof(graph,manifest),
                bundleBytes=sum(p.stat().st_size for p in bundle.iterdir()),
                diagnosisEvidence=['evidence/test07-minidump-triage.json',
                                   'evidence/test07-graph-memory-layout.txt',
                                   'evidence/test07-resource-allocation.txt'])
    (target/'report.json').write_text(json.dumps(report,indent=2))
    print(json.dumps({k:report[k] for k in ['cache','graphChangedBytes','missingArrayInstances',
                                         'missingArrayElements','bundleBytes']},indent=2))


if __name__=='__main__':
    main()
