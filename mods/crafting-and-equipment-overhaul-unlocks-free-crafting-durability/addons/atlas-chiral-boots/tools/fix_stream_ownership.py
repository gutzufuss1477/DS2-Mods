"""Test10: independent clip buffer resources, preserving geometry and runtime."""
import hashlib
import json
from pathlib import Path
import shutil
import struct
from native_bundle import ROOT, Graph, Group, DSAR, encode_link, archive_segments
from native_hanging import isolate_clip
from serialize_native_geometry import PACKAGE, GAME, result_hash
from overlay_cache import build_cache, digest
from fix_array_reservations import graph_segments


def audited_group(graph, links, gid, audit):
    """Read a multispan PRIVATE group using an independently exported census."""
    g = Group.__new__(Group)
    g.graph, g.all_links = graph, links
    g.info = next(x for x in graph.groups if x['id'] == gid)
    g.data = (audit/f'{gid}.bin').read_bytes()
    g.rows = [(int(p[0]), p[1], *map(int, p[2:]))
              for line in (audit/f'{gid}.tsv').read_text().splitlines()
              if (p := line.split('\t'))]
    assert len(g.rows) == g.info['numObjects'] == g.info['typeCount']
    assert len(g.data) == g.info['groupSize']
    from native_bundle import decode_link
    at, g.links = g.info['linkStart'], []
    while at < g.info['linkStart']+g.info['linkSize']:
        link, at = decode_link(links, at)
        g.links.append(link)
    assert at == g.info['linkStart']+g.info['linkSize']
    assert len(g.links) == g.rows[-1][5]
    g.locators = list(struct.unpack_from('<'+'Q'*g.info['locatorCount'],
                     graph.parts['locators'], 4+8*g.info['locatorStart']))
    g.spans = [struct.unpack_from('<Iii', graph.parts['spans'], 4+12*i)
               for i in range(g.info['spanStart'],g.info['spanStart']+g.info['spanCount'])]
    g.added, g.edits = [], {}
    return g


def main():
    previous = ROOT/'staging/test09-controller-offsets'
    target = ROOT/'staging/test10-private-clip-buffers'
    assert not target.exists()
    before = json.loads((previous/'report.json').read_text())
    cache_before = Path(before['cache'])
    graph = Graph(cache_before/'streaming_graph.core')
    original = Graph(PACKAGE/'streaming_graph.core')
    links_before = (cache_before/'streaming_links.stream').read_bytes()
    group = audited_group(graph, links_before, 31126, ROOT/'evidence/test09-resource-readback/raw')
    source = PACKAGE/'package.42.02.core.stream'
    prior, native = DSAR(cache_before/source.name), DSAR(source)
    # Exact audit provenance before using any record offsets.
    readers = {}
    captured = bytearray()
    for file_id, size, at in group.spans:
        name = graph.files[file_id].split('/')[-1]
        if file_id not in readers:
            readers[file_id] = DSAR(cache_before/name if (cache_before/name).exists() else PACKAGE/name)
        captured.extend(readers[file_id].read(at, size))
    assert captured == group.data
    fixes = []
    for slot, donor, mesh_id in [(0,15474,48122),(1,32583,48132)]:
        mesh = group.edit_private_copy(mesh_id)
        fix = isolate_clip(group, donor, mesh, f'hanging-{slot}')
        assert mesh.raw == group.original(mesh_id).raw
        assert mesh.locators == group.original(mesh_id).locators
        fixes.append(dict(mesh=mesh_id, donor=donor, **fix))
    assert len(group.added) == 6 and sum(f['streamedBuffers'] for f in fixes) == 8
    for r in group.added:
        assert r.raw[16:] == group.original(r.source).raw[16:]
        assert not r.locators
    appended = b''.join(r.raw for r in group.added)
    resources = [group.edits.get(i) or group.original(i) for i in range(len(group.rows))]+group.added
    new_links = b''.join(encode_link(l) for r in resources for l in r.links)
    links = links_before+new_links
    p, g = graph.parts, group.info
    type_data = bytearray(p['typeTable'][4:])
    count, capacity = struct.unpack_from('<II',type_data,8)
    assert count == capacity
    old_types = type_data[20+2*g['typeStart']:20+2*(g['typeStart']+g['typeCount'])]
    group_types = old_types+b''.join(old_types[2*r.source:2*r.source+2] for r in group.added)
    type_data.extend(group_types)
    struct.pack_into('<II',type_data,8,count+len(group_types)//2,count+len(group_types)//2)
    p['typeTable'] = struct.pack('<I',len(type_data))+type_data
    file_id = graph.files.index('cache:package/package.42.02.core.stream')
    spans = group.spans+[(file_id,len(appended),prior.total)]
    span_start = struct.unpack_from('<I',p['spans'])[0]
    p['spans'] = (struct.pack('<I',span_start+len(spans))+p['spans'][4:] +
                  b''.join(struct.pack('<Iii',*s) for s in spans))
    groups = bytearray(p['groups'])
    struct.pack_into('<iq',groups,g['relative']+4,len(resources),len(group.data)+len(appended))
    struct.pack_into('<6I',groups,g['relative']+32,span_start,len(spans),count,len(resources),
                     len(links_before),len(new_links))
    p['groups'] = groups
    p['linkSize'] = struct.pack('<I',len(links))
    target.mkdir()
    bundle = target/'bundle'
    bundle.mkdir()
    chunks = prior.chunks[len(native.chunks):]
    payloads = [prior.read(c[0],c[2]) for c in chunks]+[appended]
    starts,segments,size = archive_segments(source,payloads,bundle,'geometry')
    assert starts[:-1] == [c[0] for c in chunks] and starts[-1] == prior.total
    manifest = json.loads((previous/'bundle/overlay.json').read_text())
    manifest['files'][0].update(segments=segments,resultSize=size,
                              resultSha256=result_hash(source,bundle,segments))
    manifest['files'][1].update(segments=graph_segments(original,graph,bundle),
                               resultSize=len(graph.bytes()),resultSha256=hashlib.sha256(graph.bytes()).hexdigest())
    # Preserve the complete existing link overlay, then append one new group table.
    delta = (cache_before/'streaming_links.stream').read_bytes()[(PACKAGE/'streaming_links.stream').stat().st_size:]+new_links
    (bundle/'links-delta.bin').write_bytes(delta)
    segments = [dict(original=0,length=(PACKAGE/'streaming_links.stream').stat().st_size),
                dict(payload='links-delta.bin',offset=0,length=len(delta),sha256=hashlib.sha256(delta).hexdigest())]
    manifest['files'][2].update(segments=segments,resultSize=len(links),resultSha256=hashlib.sha256(links).hexdigest())
    (bundle/'overlay.json').write_text(json.dumps(manifest,indent=2))
    cache = build_cache(GAME,bundle,manifest,target/'cache')
    final = DSAR(cache/source.name)
    extra = prior.read(native.total,prior.total-native.total)
    assert final.read(native.total,len(extra)) == extra
    assert final.read(prior.total,len(appended)) == appended
    assert final.total < 2**31
    assert (cache/'streaming_links.stream').read_bytes() == links
    report = dict(cache=str(cache),previousCache=str(cache_before),
                  status='Test10 independent clip identities; array/controller/readback gates pending',
                  clipOwnershipFixes=fixes,addedResources=6,appendedRecordBytes=len(appended),
                  previousPrivatePayloadUnchanged=True,allExistingResourceBytesUnchanged=True,
                  geometryAndTexturesUnchanged=True,
                  changedOriginalFiles=False,
                  nativeCodeEvidence=['evidence/test09-buffer-collection-builder.txt',
                                      'evidence/test09-buffer-collection-xrefs.txt'],
                  limitation='Known ownership defect corrected; dump lacks heap to identify exact faulting buffer.')
    (target/'report.json').write_text(json.dumps(report,indent=2))
    print(json.dumps(report,indent=2))


if __name__ == '__main__':
    main()
