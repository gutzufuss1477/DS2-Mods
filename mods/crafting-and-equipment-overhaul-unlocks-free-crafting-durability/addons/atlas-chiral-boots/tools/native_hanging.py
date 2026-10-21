"""Rigid accepted Chiral pairs with the native clip and flexible root straps."""
import json
import math
import struct
import numpy as np
from scipy.spatial import cKDTree
from native_bundle import ROOT, DSAR, murmur128
from serialize_native_geometry import metadata, ref, vertex_array, bone_bounds, mesh_bounds, U, PACKAGE
from boot_geometry import read_stream, pack_direction, decode_direction
from native_hierarchy import new_part, extend_model, extend_controller


def sections(graph, mesh):
    d = metadata(31126, mesh)
    stream = d['StreamingDataSource']
    locator = stream['Locator']
    blob = DSAR(PACKAGE/graph.files[locator & 0xffffff].split('/')[-1]).read(
        (locator >> 24)+stream['Offset'], stream['Length'])
    at, result = 0, []
    for primitive in d['Primitives']:
        pr = metadata(31126, ref(primitive)[1])
        va, ia = [metadata(31126, ref(pr[k])[1]) for k in ['VertexArray', 'IndexArray']]
        assert ia['Format'] == 'Index16'
        arrays, strides = [], [s['Stride'] for s in va['Streams']]+[2]
        for i, stride in enumerate(strides):
            at += (-at) % math.lcm(16, stride)
            length = stride*(va['Count'] if i < len(strides)-1 else ia['Count'])
            arrays.append(blob[at:at+length])
            at += length
        result.append(dict(arrays=arrays, strides=strides, count=va['Count'], indexCount=ia['Count']))
    assert at == len(blob)
    return result


def assemble(parts):
    result, layouts = bytearray(), []
    for part in parts:
        layout = []
        for raw, stride in zip(part['arrays'], part['strides']):
            result.extend(bytes((-len(result)) % math.lcm(16, stride)))
            layout.append(dict(offset=len(result), length=len(raw), stride=stride))
            result.extend(raw)
        layouts.append(layout)
    return bytes(result), layouts


def author(graph, slot, mesh):
    clip, native = sections(graph, mesh)
    old = np.frombuffer(native['arrays'][0], 'u1').reshape(-1, 20)
    faces = np.frombuffer(native['arrays'][-1], '<u2').reshape(-1, 3)
    root = np.where(old[:, 12:14] == 1, old[:, 16:18], 0).sum(1)/np.maximum(old[:, 16:18].sum(1), 1)
    strap = faces[np.all(root[faces] > .5, axis=1)]
    si, inv = np.unique(strap, return_inverse=True)
    legacy = ROOT.parents[1]/'analysis/atlas-test20-hanging'
    transforms = json.loads((legacy/'native-hanging-transforms.json').read_text())[slot]['parts']
    native_pair = np.load(legacy/f'native-pair-{mesh}.npz')
    arrays, all_faces, placements = [], [], []
    count = 0
    for t in transforms:
        side = t['side']
        native_worn = np.load(ROOT.parents[1]/f'analysis/atlas-test19-fit/native-{side}.npz')
        r, base = np.array(t['rotation']), np.array(t['translation'])
        assert np.max(abs(r.T@r-np.eye(3))) < 1e-5 and np.linalg.det(r) > .999
        dist, idx = cKDTree(native_pair['uv']).query(native_worn['uv'])
        select = (native_worn['positions'][:, 2] > .225) & (dist < 1e-5)
        assert select.sum() > 30
        residual = np.median(native_pair['positions'][idx[select]] -
                             (native_worn['positions'][select]@r+base), axis=0)
        data_parts = []
        for kind in ['sole', 'upper']:
            folder = ROOT/'staging/chiral-cuff-v3-geometry-v2'
            desc = json.loads((folder/f'{side}-{kind}.layout.json').read_text())
            data_parts.append(read_stream((folder/f'{side}-{kind}.stream.bin').read_bytes(), desc))
        max_z = max(d['positions'][:, 2].max() for d in data_parts)
        shift = base+residual+(native_worn['positions'][:, 2].max()-max_z)*r[2]
        placements.append(dict(side=side, rotation=r.tolist(), translation=shift.tolist()))
        for data in data_parts:
            p = data['positions']
            q = (p@r+shift).astype('<f4')
            f = data['triangles']
            error = np.max(abs(np.linalg.norm(p[f[:, 0]]-p[f[:, 1]], axis=1) -
                               np.linalg.norm(q[f[:, 0]]-q[f[:, 1]], axis=1)))
            assert error < 1e-6
            v = np.zeros((len(p), 20), 'u1')
            v[:, :12] = q.view('u1').reshape(-1, 12)
            v[:, 12], v[:, 16] = slot+2, 255
            nbt = np.column_stack((pack_direction(data['normals']@r, 1),
                                  pack_direction(data['tangents']@r, data['bitangentSign'] < 0)))
            arrays.append((v, nbt, data['uv'].astype('<f2')))
            all_faces.append(f.astype('i4')+count)
            count += len(p)
    v = old[si].copy()
    w = v[:, 16:18].astype(float)
    first = np.rint(w[:, 0]/w.sum(1)*255).astype('u1')
    v[:, 14:16], v[:, 18:20] = 0, 0
    v[:, 16], v[:, 17] = first, 255-first
    arrays.append((v, np.frombuffer(native['arrays'][1], '<u4').reshape(-1, 2)[si],
                   np.full((len(si), 4, 2), .001, '<f2')))
    all_faces.append(inv.reshape(-1, 3)+count)
    v, nbt, uv = [np.concatenate([a[i] for a in arrays]) for i in range(3)]
    f = np.concatenate(all_faces)
    assert len(v) < 65536 and f.max() < len(v)
    main = dict(arrays=[v.tobytes(), nbt.astype('<u4').tobytes(), uv.tobytes(), f.astype('<u2').tobytes()],
                strides=[20, 8, 16, 2], count=len(v), indexCount=f.size)
    blob, layout = assemble([clip, main])
    p = v[:, :12].copy().view('<f4').reshape(-1, 3)
    cp = np.frombuffer(clip['arrays'][0], 'u1').reshape(-1, 16)[:, :12].copy().view('<f4').reshape(-1, 3)
    return dict(blob=blob, layout=layout, count=len(v), indexCount=f.size,
                positions=p, bones=v[:, 12:14], weights=v[:, 16:18]/255.,
                allPositions=np.concatenate((cp, p)), triangles=f, uv=uv, normals=decode_direction(nbt[:, 0]),
                clip=clip, originalPairCount=native['count'], straps=len(si), placements=placements)


def add_hanging(group, material, cursor, stream_index, target):
    payloads, records, root_ids = [], [], []
    for slot, source, source_part, lod_source in [(0, 15474, 39928, 17683), (1, 32583, 46358, 36882)]:
        label = f'hanging-{slot}'
        data = author(group.graph, slot, source)
        native = metadata(31126, source)
        prim_id = ref(native['Primitives'][1])[1]
        prim = metadata(31126, prim_id)
        va_id, ia_id = [ref(prim[k])[1] for k in ['VertexArray', 'IndexArray']]
        va = vertex_array(group, va_id, data['layout'][1][:-1], data['blob'], data['count'], 4, label+'/vertices')
        ia = group.clone(ia_id, label+'/indices')
        struct.pack_into('<I', ia.raw, 16, data['indexCount'])
        s = data['layout'][1][-1]
        ia.raw[32:48] = murmur128(data['blob'][s['offset']:s['offset']+s['length']])
        pr = group.clone(prim_id, label+'/primitive')
        struct.pack_into('<I', pr.raw, 30, data['indexCount'])
        old_box = struct.pack('<8f', *[prim['BoundingBox'][k][a] for k in ['Min', 'Max'] for a in 'XYZW'])
        p = data['positions']
        pr.replace(old_box, struct.pack('<8f', *(p.min(0)-.002), 1., *(p.max(0)+.002), 1.))
        pr.remap({(None, va_id):(None, va.id), (None, ia_id):(None, ia.id)})
        skin_id = ref(native['SkinInfo'])[1]
        skin = group.clone(skin_id, label+'/skin')
        skin.replace(U(data['originalPairCount']), U(data['count']))
        paint_id = ref(native['AdjustPaintUVBuffer'])[1]
        paint = group.clone(paint_id, label+'/paint')
        # Native hanging paint covers only the pair primitive, not the clip.
        assert metadata(31126, paint_id)['Count'] == data['originalPairCount']*2
        assert len(paint.raw) == 45+data['originalPairCount']*4
        paint.raw = paint.raw[:45]+bytes(data['count']*4)
        struct.pack_into('<I', paint.raw, 25, data['count']*2)
        binding = metadata(31126, ref(native['SkinnedMeshJointBindings'])[1])
        bb_id = ref(native['SkinnedMeshBoneBoundingBoxes'])[1]
        bb = bone_bounds(group, bb_id, binding, p, data['bones'], data['weights'], label+'/bounds')
        mesh = group.clone(source, label+'/mesh')
        mesh_bounds(mesh, data['allPositions'])
        struct.pack_into('<II', mesh.raw, len(mesh.raw)-8, 0, len(data['blob']))
        mesh.locators = [stream_index | (cursor << 24)]
        mesh.remap({(None, prim_id):(None, pr.id), (None, skin_id):(None, skin.id),
                    (None, paint_id):(None, paint.id), (None, bb_id):(None, bb.id),
                    (None, ref(native['ShadingGroups'][1])[1]):(None, material['shadingGroup'])})
        lod = group.clone(lod_source, label+'/lod')
        lod.links = [(None, mesh.id)]*3
        mesh_bounds(lod, data['allPositions'])
        part_bb = bone_bounds(group, ref(metadata(31126, source_part)['BoneBoundingBoxes'])[1],
                              binding, p, data['bones'], data['weights'], label+'/part-bounds')
        part = new_part(group, source_part, lod.id, part_bb.id, label)
        root_ids.append(part.id)
        np.savez(target/(label+'.npz'), **{k:data[k] for k in ['positions', 'bones', 'weights', 'triangles', 'uv', 'normals']})
        records.append(dict(label=label, group=31126, mesh=mesh.id, vertexCount=data['count'],
                            indexCount=data['indexCount'], source=source, part=part.id,
                            originalClipPreserved=True, nativeStrapVertices=data['straps'], placements=data['placements']))
        cursor += len(data['blob'])
        payloads.append(data['blob'])
    extend_model(group, 20086, root_ids, root_ids)
    extend_controller(group, 3575, ['hanging-0', 'hanging-1'])
    # Keep existing private indices stable. Clone clip buffers only after all
    # earlier resources: each mesh stream must own its streamed GPU buffers.
    for row in records:
        mesh = next(r for r in group.added if r.id == row['mesh'])
        row['clipOwnership'] = isolate_clip(group, row['source'], mesh, row['label'])
    # The worn VS consumes BLENDINDICES1/BLENDWEIGHT1 and six influences;
    # the hanging pair supplies only two. Preserve native hanging shader IDs.
    from build_native_materials import add_hanging_material
    hanging_material = add_hanging_material(group, material)
    for row in records:
        mesh = next(r for r in group.added if r.id == row['mesh'])
        mesh.remap({(None, material['shadingGroup']): (None, hanging_material['shadingGroup'])})
        row['hangingMaterial'] = hanging_material
    return payloads, records


def isolate_clip(group, source, mesh, label):
    """Copy unchanged clip geometry but give its stream independent resources.

    Native 0x22aee00 deduplicates GPU buffers only inside ONE mesh collection.
    Native 0x253f860 unbinds every buffer when that collection is unloaded.
    Reusing the donor's streamed VertexArray/IndexArray in another collection
    therefore shares a mutable allocation owner across independent lifetimes.
    """
    prim_id = ref(metadata(31126, source)['Primitives'][0])[1]
    primitive = metadata(31126, prim_id)
    va_id, ia_id = [ref(primitive[k])[1] for k in ['VertexArray', 'IndexArray']]
    assert metadata(31126, va_id)['IsStreaming']
    assert metadata(31126, ia_id)['IsStreaming']
    va = group.clone(va_id, label+'/clip/vertices')
    ia = group.clone(ia_id, label+'/clip/indices')
    pr = group.clone(prim_id, label+'/clip/primitive')
    pr.remap({(None, va_id): (None, va.id), (None, ia_id): (None, ia.id)})
    assert (None, prim_id) in mesh.links
    mesh.remap({(None, prim_id): (None, pr.id)})
    return dict(originalPrimitive=prim_id, primitive=pr.id,
                originalVertices=va_id, vertices=va.id,
                originalIndices=ia_id, indices=ia.id,
                streamedBuffers=len(metadata(31126, va_id)['Streams'])+1)
