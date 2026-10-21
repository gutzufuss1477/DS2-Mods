"""Rebase compiled child-controller offsets in the private Sam resource group.

Only seven uint32 fields change. Original package blocks, all geometry, texture
headers/pixels, object IDs, links and array counts are preserved. New spans
substitute the patched records from appended private data.
"""
import hashlib
import json
from pathlib import Path
import shutil
import struct
from native_bundle import ROOT, Graph, DSAR, archive_segments
from native_hierarchy import rebase_cover_record
from serialize_native_geometry import PACKAGE, GAME, result_hash
from overlay_cache import build_cache, digest
from fix_array_reservations import graph_segments, reservation_proof, validate_cache_reservations

COMPONENTS = [116, 88, 125, 138, 74, 79, 96]


def metadata(index):
    paths = list((ROOT/'evidence/test08-cover-metadata').glob(f'*_39987_{index}.json'))
    assert len(paths) == 1
    return json.loads(paths[0].read_text())


def rows():
    return {int(p[0]): (p[1], *map(int, p[2:]))
            for line in (ROOT/'evidence/test07-arrays-test07/39987.tsv').read_text().splitlines()
            if (p := line.split('\t'))}


def sliced_spans(spans, start, end):
    cursor = 0
    result = []
    for file_id, size, position in spans:
        low, high = max(start, cursor), min(end, cursor+size)
        if low < high:
            result.append((file_id, high-low, position+low-cursor))
        cursor += size
    assert sum(s[1] for s in result) == end-start
    return result


def main():
    previous = ROOT/'staging/test08-array-reservations'
    target = ROOT/'staging/test09-controller-offsets'
    assert not target.exists()
    before = json.loads((previous/'report.json').read_text())
    cache_before = Path(before['cache'])
    graph = Graph(cache_before/'streaming_graph.core')
    original = Graph(PACKAGE/'streaming_graph.core')
    manifest = json.loads((previous/'bundle/overlay.json').read_text())
    group = next(g for g in graph.groups if g['id'] == 39987)
    spans = [struct.unpack_from('<Iii', graph.parts['spans'], 4+12*i)
             for i in range(group['spanStart'], group['spanStart']+group['spanCount'])]
    data = (ROOT/'evidence/test07-arrays-test07/39987.bin').read_bytes()
    assert len(data) == group['groupSize']
    # Verify the entire audit blob against the exact installed cache generation.
    readers = {}
    def read_span(span):
        file_id, length, at = span
        name = graph.files[file_id].split('/')[-1]
        if file_id not in readers:
            readers[file_id] = DSAR(cache_before/name if (cache_before/name).exists() else PACKAGE/name)
        return readers[file_id].read(at, length)
    assert b''.join(map(read_span, spans)) == data
    indexed = rows()
    patches = []
    appended = bytearray()
    source = PACKAGE/'package.42.02.core.stream'
    archive_before, archive_native = DSAR(cache_before/source.name), DSAR(source)
    for index in sorted(COMPONENTS):
        typ, at, size, *_ = indexed[index]
        raw = bytearray(data[at:at+size])
        info = rebase_cover_record(raw, metadata(index), 53, 2)
        differences = [i for i, (a, b) in enumerate(zip(data[at:at+size], raw)) if a != b]
        assert differences == [info['fieldOffset']], (index, differences)
        patches.append(dict(resource=index, type=typ, groupOffset=at, size=size,
                            virtualOffset=archive_before.total+len(appended),
                            sha256=hashlib.sha256(raw).hexdigest(), **info))
        appended.extend(raw)
    assert len(patches) == 7
    new_spans, cursor = [], 0
    file_id = graph.files.index('cache:package/package.42.02.core.stream')
    for p in patches:
        new_spans += sliced_spans(spans, cursor, p['groupOffset'])
        new_spans.append((file_id, p['size'], p['virtualOffset']))
        cursor = p['groupOffset']+p['size']
    new_spans += sliced_spans(spans, cursor, len(data))
    assert sum(s[1] for s in new_spans) == len(data)
    count = struct.unpack_from('<I', graph.parts['spans'])[0]
    graph.parts['spans'] = (struct.pack('<I', count+len(new_spans))+graph.parts['spans'][4:] +
                           b''.join(struct.pack('<Iii', *s) for s in new_spans))
    groups = bytearray(graph.parts['groups'])
    struct.pack_into('<II', groups, group['relative']+32, count, len(new_spans))
    graph.parts['groups'] = groups
    target.mkdir()
    bundle = target/'bundle'
    bundle.mkdir()
    extra_chunks = archive_before.chunks[len(archive_native.chunks):]
    payloads = [archive_before.read(c[0], c[2]) for c in extra_chunks]+[bytes(appended)]
    starts, segments, size = archive_segments(source, payloads, bundle, 'geometry')
    assert starts[:-1] == [c[0] for c in extra_chunks] and starts[-1] == archive_before.total
    manifest['files'][0].update(segments=segments, resultSize=size,
                               resultSha256=result_hash(source, bundle, segments))
    manifest['files'][1].update(segments=graph_segments(original, graph, bundle),
                               resultSize=len(graph.bytes()),
                               resultSha256=hashlib.sha256(graph.bytes()).hexdigest())
    shutil.copyfile(previous/'bundle/links-delta.bin', bundle/'links-delta.bin')
    (bundle/'overlay.json').write_text(json.dumps(manifest, indent=2))
    cache = build_cache(GAME, bundle, manifest, target/'cache')
    final = DSAR(cache/source.name)
    # All previously appended resources/geometry remain byte-identical.
    extra = archive_before.read(archive_native.total, archive_before.total-archive_native.total)
    assert final.read(archive_native.total, len(extra)) == extra
    assert final.read(archive_before.total, len(appended)) == appended
    assert digest(cache/'streaming_links.stream') == digest(cache_before/'streaming_links.stream')
    prior_graph = Graph(cache_before/'streaming_graph.core')
    assert {k for k in graph.parts if graph.parts[k] != prior_graph.parts[k]} == {'groups', 'spans'}
    with source.open('rb') as a, (cache/source.name).open('rb') as b:
        for old, new in zip(archive_native.chunks, final.chunks):
            assert old[0] == new[0] and old[2:] == new[2:]
            a.seek(old[1]); b.seek(new[1])
            assert a.read(old[3]) == b.read(new[3])
    report = dict(cache=str(cache), status='Test09 controller-offset correction; independent readback pending',
                  previousCache=str(cache_before), patches=patches, changedResourceBytes=7,
                  appendedRecordBytes=len(appended), nativeCompressedBlocksPreserved=len(archive_native.chunks),
                  previousPrivatePayloadUnchanged=True, linksUnchanged=True, arrayTableUnchanged=True,
                  geometryAndTexturesUnchanged=True,
                  arrayReservationProof=reservation_proof(graph, manifest))
    (target/'report.json').write_text(json.dumps(report, indent=2))
    validate_cache_reservations(target)
    print(json.dumps(report, indent=2))


if __name__ == '__main__':
    main()
