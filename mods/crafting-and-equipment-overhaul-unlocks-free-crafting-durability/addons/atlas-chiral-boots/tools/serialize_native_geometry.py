"""Serialize accepted boot/pants geometry as private, appended native resources.

This is an offline parser-validation build, NOT a game-test installer. Materials,
conditional visibility, and hanging geometry are integrated in a subsequent build.
"""
from pathlib import Path
import base64
import hashlib
import json
import re
import struct
import sys
import numpy as np
from native_bundle import ROOT, Graph, DSAR, Group, archive_segments, murmur128
from overlay_cache import build_cache, digest

GAME = Path(r'C:\Program Files (x86)\Steam\steamapps\common\DEATH STRANDING 2 - ON THE BEACH')
PACKAGE = GAME / 'LocalCacheWinGame/package'
U = lambda n: struct.pack('<I', n)


def ref(value):
    m = re.search(r'(\d+):(\d+)', value)
    return tuple(map(int, m.groups()))


def metadata(group, index):
    folders = ['native-layouts', 'native-resource-audit-v1', 'native-resource-deps-v1',
               'native-resource-deps-v2', 'native-parts-v1', 'native-body-parts-v1',
               'native-serialize-deps-v1', 'native-part-parent-v1', 'native-hanging-v1',
               'native-hanging-deps-v1', 'native-hanging-deps-v2',
               'test08-cover-metadata']
    for folder in folders:
        paths = list((ROOT/'evidence'/folder).glob(f'*_{group}_{index}.json'))
        if paths:
            return json.loads(paths[0].read_text())
    raise FileNotFoundError((group, index))


def mesh_bounds(resource, points):
    struct.pack_into('<6f', resource.raw, 16, *(points.min(0)-.002), *(points.max(0)+.002))


def bone_bounds(group, source, binding, positions, bones, weights, label):
    result = group.clone(source, label)
    native = metadata(group.info['id'], source)
    count = struct.unpack_from('<I', result.raw, 16)[0]
    assert count == len(native['BoneBoundingBoxes'])
    for bid, columns in zip(binding['JointIndexList'], binding['InverseBindMatrices']):
        selected = np.any((bones == bid) & (weights > 0), axis=1)
        if not selected.any():
            continue
        box_index = native['Indices'][bid]
        # Native full-body resources intentionally omit boxes for many weighted
        # cloth/helper joints. Index zero is the original empty-box sentinel.
        # Keep that native policy, and expand every existing nonempty box.
        if box_index == 0:
            continue
        assert 0 < box_index < count
        matrix = np.array([[columns[f'Col{j}'][a] for a in 'XYZW'] for j in range(4)])
        p = np.column_stack((positions[selected], np.ones(selected.sum()))) @ matrix
        old = native['BoneBoundingBoxes'][box_index]
        low = np.minimum([old['Min'][a] for a in 'XYZ'], p[:, :3].min(0)-.002)
        high = np.maximum([old['Max'][a] for a in 'XYZ'], p[:, :3].max(0)+.002)
        struct.pack_into('<8f', result.raw, 20+32*box_index, *low, 1., *high, 1.)
    return result


def vertex_array(group, source, sections, blob, vertex_count, uv_layers, label):
    d = group.clone(source, label)
    native = metadata(group.info['id'], source)
    raw, cursor = bytearray(d.raw[:25]), 25
    struct.pack_into('<I', raw, 16, vertex_count)
    for i, (stream, section) in enumerate(zip(native['Streams'], sections)):
        flags, stride, count = struct.unpack_from('<III', d.raw, cursor)
        cursor += 12
        elements = bytes(d.raw[cursor:cursor+count*4])
        cursor += count*4+16
        if i == len(native['Streams'])-1 and uv_layers != len(stream['Elements']):
            assert stream['Elements'][0]['Element'] == 'UV0'
            assert uv_layers == 4 and count == 3
            # Native four-byte declaration: byte offset, storage, slots, semantic.
            elements += bytes([12, elements[1], 2, elements[3]+3])
            count += 1
            stride = 16
        assert stride == section['stride']
        part = blob[section['offset']:section['offset']+section['length']]
        assert len(part) == vertex_count*stride
        raw += struct.pack('<III', flags, stride, count)+elements+murmur128(part)
    assert cursor == len(d.raw)
    d.raw = raw
    return d


def serialize_mesh(group, source, label, blob, sections, points, bones, weights,
                   index_count, uv_layers, locator, paint_bytes=None):
    gid = group.info['id']
    native = metadata(gid, source)
    prim_id = ref(native['Primitives'][0])[1]
    prim = metadata(gid, prim_id)
    va_id, ia_id = ref(prim['VertexArray'])[1], ref(prim['IndexArray'])[1]
    n = len(points)
    va = vertex_array(group, va_id, sections[:-1], blob, n, uv_layers, label+'/vertices')
    ia = group.clone(ia_id, label+'/indices')
    assert len(ia.raw) == 48
    struct.pack_into('<I', ia.raw, 16, index_count)
    ia.raw[32:48] = murmur128(blob[sections[-1]['offset']:])
    pr = group.clone(prim_id, label+'/primitive')
    assert struct.unpack_from('<I', pr.raw, 30)[0] == prim['EndIndex']
    struct.pack_into('<I', pr.raw, 30, index_count)
    old_box = struct.pack('<8f', *[prim['BoundingBox'][k][a] for k in ['Min', 'Max'] for a in 'XYZW'])
    low, high = points.min(0)-.002, points.max(0)+.002
    pr.replace(old_box, struct.pack('<8f', *low, low[2], *high, high[2]))
    pr.remap({(None, va_id):(None, va.id), (None, ia_id):(None, ia.id)})
    skin_id = ref(native['SkinInfo'])[1]
    skin = group.clone(skin_id, label+'/skin')
    assert len(skin.raw) == 66 and struct.unpack_from('<I', skin.raw, 16)[0] == 1
    assert struct.unpack_from('<I', skin.raw, 44)[0] == metadata(gid, skin_id)['Parts'][0]['VertexCount']
    struct.pack_into('<I', skin.raw, 44, n)
    bind = metadata(gid, ref(native['SkinnedMeshJointBindings'])[1])
    bb_id = ref(native['SkinnedMeshBoneBoundingBoxes'])[1]
    bb = bone_bounds(group, bb_id, bind, points, bones, weights, label+'/bounds')
    replacements = {(None, prim_id):(None, pr.id), (None, skin_id):(None, skin.id),
                    (None, bb_id):(None, bb.id)}
    if 'AdjustPaintUVBuffer' in native:
        paint_id = ref(native['AdjustPaintUVBuffer'])[1]
        paint = group.clone(paint_id, label+'/paint')
        before = metadata(gid, paint_id)
        assert before['Count']*2 == len(paint.raw)-45
        payload = bytes(n*4) if paint_bytes is None else paint_bytes
        assert len(payload) == n*4
        paint.raw = paint.raw[:45]+payload
        struct.pack_into('<I', paint.raw, 25, n*2)
        replacements[(None, paint_id)] = (None, paint.id)
    mesh = group.clone(source, label+'/mesh')
    mesh_bounds(mesh, points)
    # StreamingDataSource is the final field: Channel byte, Offset u32, Length u32.
    assert struct.unpack_from('<II', mesh.raw, len(mesh.raw)-8) == (
        native['StreamingDataSource']['Offset'], native['StreamingDataSource']['Length'])
    struct.pack_into('<II', mesh.raw, len(mesh.raw)-8, 0, len(blob))
    mesh.locators = [locator]
    mesh.remap(replacements)
    return mesh


def result_hash(source, bundle, segments):
    h = hashlib.sha256()
    for s in segments:
        file = source if 'original' in s else bundle/s['payload']
        offset = s.get('original', s.get('offset', 0))
        with file.open('rb') as stream:
            stream.seek(offset)
            left = s['length']
            while left:
                b = stream.read(min(left, 8 << 20))
                assert b
                h.update(b)
                left -= len(b)
    return h.hexdigest()


def run(with_material=False, with_hierarchy=False, with_hanging=False):
    with_hierarchy = with_hierarchy or with_hanging
    with_material = with_material or with_hierarchy
    variant = ('native-complete-v1' if with_hanging else 'native-hierarchy-v1' if with_hierarchy else
               'native-geometry-material-v1' if with_material else 'native-geometry-v1')
    target = ROOT/'staging'/variant
    assert not target.exists()
    target.mkdir()
    bundle = target/'bundle'
    bundle.mkdir()
    baseline = json.loads((ROOT/'evidence/native-graph-proof/result.json').read_text())
    graph_path = Path(baseline['cache'])/'streaming_graph.core'
    assert digest(graph_path) == baseline['cacheSha256']
    graph = Graph(graph_path)
    links = (PACKAGE/'streaming_links.stream').read_bytes()
    groups = {gid:Group(graph, links, gid, ROOT/'evidence/native-resource-audit-v1/raw')
              for gid in [31126, 39987]}
    material = None
    if with_material:
        from build_native_materials import add_material
        groups[67696] = Group(graph, links, 67696, ROOT/'evidence/native-boot-texture-v1/raw')
        material = add_material(groups[31126], groups[67696])
    stream_path = PACKAGE/'package.42.02.core.stream'
    stream_index = graph.files.index('cache:package/package.42.02.core.stream')
    cursor = DSAR(stream_path).total
    payloads, records = [], []
    from boot_geometry import read_stream
    for side, source in [('left', 33600), ('right', 29579)]:
        for part in ['sole', 'upper']:
            label = side+'-'+part
            folder = ROOT/'staging/chiral-cuff-v3-geometry-v2'
            d = json.loads((folder/(label+'.layout.json')).read_text())
            assert d['jointIndexDomain'] == 'complete native skeleton'
            blob = (folder/(label+'.stream.bin')).read_bytes()
            data = read_stream(blob, d)
            sections = list(d['sections'].values())
            r = serialize_mesh(groups[31126], source, label, blob, sections, data['positions'],
                               data['bones'], data['weights'], d['indexCount'], 4,
                               stream_index | (cursor << 24))
            if material:
                shades = metadata(31126, source)['ShadingGroups']
                assert len(shades) == 1
                r.remap({(None, ref(shades[0])[1]):(None, material['shadingGroup'])})
            records.append(dict(label=label, group=31126, mesh=r.id, vertexCount=d['vertexCount'],
                                indexCount=d['indexCount'], source=source))
            cursor += len(blob)
            payloads.append(blob)
    graft = ROOT/'staging/full-pants-graft-v1'
    for d in json.loads((graft/'report.json').read_text())['meshes']:
        label = d['name']
        data = np.load(graft/(label+'.npz'))
        blob = (graft/(label+'.stream.bin')).read_bytes()
        paint_path = graft/(label+'.paint.bin')
        paint = paint_path.read_bytes() if paint_path.exists() else None
        r = serialize_mesh(groups[39987], d['sourceModel'], label, blob, d['sections'],
                           data['positions'], data['bones'], data['weights'], d['indexCount'],
                           d['uvLayers'], stream_index | (cursor << 24), paint)
        records.append(dict(label=label, group=39987, mesh=r.id, vertexCount=d['vertexCount'],
                            indexCount=d['indexCount'], source=d['sourceModel']))
        cursor += len(blob)
        payloads.append(blob)
    hierarchy = None
    if with_hierarchy:
        from native_hierarchy import extend_worn_and_body
        hierarchy = extend_worn_and_body(groups, records)
    if with_hanging:
        from native_hanging import add_hanging
        extra, rows = add_hanging(groups[31126], material, cursor, stream_index, target)
        payloads.extend(extra)
        cursor += sum(map(len, extra))
        records.extend(rows)
    extended_links = links
    for group in groups.values():
        payload, extra_links = group.emit(cursor, len(extended_links), stream_index)
        cursor += len(payload)
        payloads.append(payload)
        extended_links += extra_links
    graph.parts['linkSize'] = U(len(extended_links))
    starts, segments, result_size = archive_segments(stream_path, payloads, bundle, 'geometry')
    assert starts[-1]+len(payloads[-1]) == cursor
    files = [dict(gamePath='LocalCacheWinGame/package/'+stream_path.name,
                  originalSha256=digest(stream_path), resultSha256=result_hash(stream_path, bundle, segments),
                  resultSize=result_size, segments=segments)]
    for name, raw in [('streaming_graph.core', graph.bytes()), ('streaming_links.stream', extended_links)]:
        (bundle/name).write_bytes(raw)
        h = hashlib.sha256(raw).hexdigest()
        files.append(dict(gamePath='LocalCacheWinGame/package/'+name,
                          originalSha256=digest(PACKAGE/name), resultSha256=h, resultSize=len(raw),
                          segments=[dict(payload=name, sha256=h, offset=0, length=len(raw))]))
    manifest = dict(schemaVersion=1, files=files)
    (bundle/'overlay.json').write_text(json.dumps(manifest, indent=2))
    cache = build_cache(GAME, bundle, manifest, target/'cache')
    report = dict(status='Native geometry serialized; offline parser validation pending; NOT installable',
                  cache=str(cache), meshes=records, originalFilesWritten=False,
                  nativeGeometryMaterialsNotYetRetargeted=not with_material,
                  material=material,
                  hierarchy=hierarchy,
                  conditionalSelectionNotYetBound=True,
                  archiveMetadataAndStreamsSharePrivate201=True,
                  nativeCoreArchiveNotOverlaid=True,
                  fingerprint='Murmur3 x64 seed42 for new streams; original compiler hash derivation unconfirmed')
    (target/'report.json').write_text(json.dumps(report, indent=2))
    print(json.dumps(report))


if __name__ == '__main__':
    run(with_material='--material' in sys.argv, with_hierarchy='--hierarchy' in sys.argv,
        with_hanging='--hanging' in sys.argv)
