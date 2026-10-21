"""Preserve full native upper-body streams; graft only accepted subdivided cuffs."""
from pathlib import Path
import base64
import json
import math
import re
import sys
import numpy as np
from boot_geometry import remap_skin, quantize_weights, pack_direction
from overlay_cache import digest, require
from map_pants_cuff import load_mesh

ROOT = Path(__file__).resolve().parents[1]
LAB = ROOT.parents[1]
GAME = Path(r'C:\Program Files (x86)\Steam\steamapps\common\DEATH STRANDING 2 - ON THE BEACH\LocalCacheWinGame\package')
sys.path.insert(0, str(LAB / 'scripts'))
from dsar_reader import DSAR
from atlas_graph import Graph


def alignment(offset, stride):
    n = math.lcm(16, stride)
    return (offset + n - 1) // n * n


def assemble_sections(arrays, triangles):
    payload = bytearray()
    sections = []
    require(triangles.max() < 65536, 'Native UInt16 index range exceeded')
    for name, a in [*arrays, ('indices', triangles.astype('<u2').reshape(-1, 1))]:
        a = np.ascontiguousarray(a)
        stride = a.dtype.itemsize * math.prod(a.shape[1:])
        offset = alignment(len(payload), stride)
        payload.extend(bytes(offset-len(payload)))
        raw = a.tobytes()
        payload.extend(raw)
        sections.append(dict(name=name, offset=offset, stride=stride, length=len(raw)))
    return bytes(payload), sections


def corner_remap(accepted_indices, corner_uv, corner_paint=None):
    """Split only differing native half-float UV/paint values, never cross source seams."""
    uv = np.asarray(corner_uv, dtype='<f2')
    paint = np.asarray(corner_paint, dtype='<f2') if corner_paint is not None else None
    keys, source_ids, unique_uv, unique_paint, remap = {}, [], [], [], []
    for i, v in enumerate(np.asarray(accepted_indices).ravel()):
        u = uv.reshape((-1, *uv.shape[2:]))[i]
        p = paint.reshape(-1, 2)[i] if paint is not None else None
        key = (int(v), u.tobytes(), None if p is None else p.tobytes())
        if key not in keys:
            keys[key] = len(source_ids)
            source_ids.append(int(v))
            unique_uv.append(u)
            if p is not None:
                unique_paint.append(p)
        remap.append(keys[key])
    return (np.array(source_ids, dtype='u4'), np.array(unique_uv, dtype='<f2'),
            np.array(unique_paint, dtype='<f2') if paint is not None else None,
            np.array(remap, dtype='u4').reshape(-1, 3))


def run():
    output = ROOT / 'staging/full-pants-graft-v1'
    require(not output.exists(), 'Existing output refused')
    assets = ROOT / 'assets/chiral-cuff-v3'
    manifest = json.loads((assets / 'manifest.json').read_text('utf-8'))
    graph = Graph(GAME / 'streaming_graph.core')
    metadata_dirs = [ROOT / 'evidence/native-resource-audit-v1', ROOT / 'evidence/native-resource-deps-v1',
                     ROOT / 'evidence/native-layouts', LAB / 'analysis/atlas-test17-fit/suit-layout']
    tracked = {}

    def track(path):
        tracked[str(path)] = digest(path)
        return path

    def obj(kind, index):
        paths = [d / f'{kind}_39987_{index}.json' for d in metadata_dirs]
        p = next(p for p in paths if p.exists())
        return json.loads(track(p).read_text('utf-8'))

    ref = lambda s: int(re.search(r':(\d+)', s)[1])
    results, files = [], {}
    for mid, name in [(270, 'pants-body'), (638, 'pants-strips')]:
        mesh_meta = obj('RegularSkinnedMeshResource', mid)
        bindings = obj('SkinnedMeshIndexedJointBindings', ref(mesh_meta['SkinnedMeshJointBindings']))
        prim_id = ref(mesh_meta['Primitives'][0])
        primitive = obj('PrimitiveResource', prim_id)
        va = obj('VertexArrayResource', ref(primitive['VertexArray']))
        native_cast = track(ROOT / f'evidence/pants-current-cast/RegularSkinnedMeshResource_39987_{mid}.cast')
        native = load_mesh(native_cast)
        # Both CAST and the native byte stream address the complete skeleton.
        # JointIndexList selects inverse binds; it does not renumber vertex indices.
        import cast
        model = cast.Cast.load(str(native_cast)).Roots()[0].ChildrenOfType(cast.Model)[0]
        native_names = [b.Name() for b in model.Skeleton().Bones()]
        palette = native_names
        entry = next(x for x in manifest['meshes'] if x['name'] == name)
        source = track(assets / entry['file'])
        require(digest(source) == entry['sha256'], 'Accepted geometry changed')
        a = np.load(source, allow_pickle=False)
        mapping = np.load(track(ROOT / f'staging/pants-cuff-map-v1/{name}.npz'), allow_pickle=False)
        p = np.array(native.VertexPositionBuffer(), dtype='<f4').reshape(-1, 3)
        faces = np.array(native.FaceBuffer(), dtype='u4').reshape(-1, 3)
        n = len(p)
        stream = mesh_meta['StreamingDataSource']
        index = stream['Locator'] & 0xffffff
        archive_path = GAME / graph.files[index].split('/')[-1]
        raw = DSAR(archive_path).read((stream['Locator'] >> 24) + stream['Offset'], stream['Length'])
        cursor, native_arrays = 0, []
        for st in va['Streams']:
            cursor = alignment(cursor, st['Stride'])
            native_arrays.append(np.frombuffer(raw, 'u1', n*st['Stride'], cursor).reshape(n, st['Stride']).copy())
            cursor += n * st['Stride']
        index_offset = alignment(cursor, 2)
        require(index_offset + faces.size*2 == len(raw), 'Native stream length mismatch')
        require(np.array_equal(np.frombuffer(raw, '<u2', faces.size, index_offset).reshape(-1, 3), faces),
                'Native triangles do not match current CAST')
        require(np.array_equal(native_arrays[0][:, :12].copy().view('<f4'), p), 'Native positions differ')
        require(va['Streams'][0]['Stride'] == 28, 'Unexpected native skin stride')
        native_bones = native_arrays[0][:, 12:20]
        native_weights = native_arrays[0][:, 20:28]
        cast_b = np.array(native.VertexWeightBoneBuffer()).reshape(n, -1)
        cast_w = np.array(native.VertexWeightValueBuffer()).reshape(n, -1)
        require(np.array_equal(native_bones[native_weights > 0], cast_b[native_weights > 0]),
                'Native/CAST joint mapping differs')
        require(np.isin(native_bones[native_weights > 0], bindings['JointIndexList']).all(),
                'Native weighted joint lacks inverse bind')
        require((native_weights.sum(1) > 0).all(), 'Unweighted native vertex')
        require(np.allclose(native_weights/native_weights.sum(1, keepdims=True), cast_w, atol=4e-7, rtol=0),
                'Native/CAST normalized weight proportions differ')
        native_uv = native_arrays[-1].copy().view('<f2').reshape(n, -1, 2)
        keep = mapping['nativeKeepFaces']
        cuff_faces = a['triangles'][mapping['acceptedCuffFaces']]
        restored = mapping['restoredCornerUV'].copy()
        # The explicitly approved UV0 wins; additional layers come from native face mapping.
        restored[:, :, 0] = a['uv'][cuff_faces, 0]
        paint, corner_paint = None, None
        if 'AdjustPaintUVBuffer' in mesh_meta:
            paint_meta = obj('DataBufferResource', ref(mesh_meta['AdjustPaintUVBuffer']))
            require(not paint_meta['IsStreaming'] and paint_meta['Format'] == 'R_FLOAT_16', 'Unknown paint format')
            paint = np.frombuffer(base64.b64decode(paint_meta['Data']), '<f2').reshape(n, 2)
            corner_paint = np.einsum('fij,fjd->fid', mapping['barycentric'],
                                    paint[faces[mapping['nativeParentFace']]].astype('f8'))
        used, uv, new_paint, new_faces = corner_remap(cuff_faces, restored, corner_paint)
        require(n + len(used) < 65536, 'UInt16 overflow')
        author_names = manifest['rigs'][entry['rig']]['jointNames']
        bones, weights = remap_skin(a['bones'][used], a['weights'][used], author_names, palette, slots=8)
        q = quantize_weights(weights)
        require(np.isin(bones[q > 0], bindings['JointIndexList']).all(), 'Cuff joint lacks inverse bind')
        skin = np.zeros((len(used), 28), 'u1')
        skin[:, :12] = a['positions'][used].astype('<f4').view('u1').reshape(-1, 12)
        skin[:, 12:20], skin[:, 20:28] = bones, q
        arrays = [('positionSkin', np.concatenate((native_arrays[0], skin)))]
        if len(native_arrays) == 3:
            normals, tangents = a['normals'][used], a['tangents'][used]
            nbt = np.column_stack((pack_direction(normals, 1),
                                   pack_direction(tangents[:, :3], tangents[:, 3] < 0))).astype('<u4')
            arrays.append(('normalTangent', np.concatenate((native_arrays[1], nbt.view('u1').reshape(-1, 8)))))
        arrays.append(('uv', np.concatenate((native_uv, uv))))
        indices = np.concatenate((faces[keep], new_faces+n))
        payload, sections = assemble_sections(arrays, indices)
        # Every native attribute remains byte-identical, including all upper geometry.
        for (key, array), original in zip(arrays, native_arrays):
            require(array[:n].tobytes() == original.tobytes(), 'Native attributes changed: ' + key)
        require(np.array_equal(indices[:len(keep)], faces[keep]), 'Upper/rest triangles changed')
        require(np.array_equal(a['positions'][used][new_faces], a['positions'][cuff_faces]), 'Cuff shape changed')
        positions = np.concatenate((p, a['positions'][used]))
        unique_cuff_bones = (q > 0).sum(1)
        for section, (_, arr) in zip(sections, arrays):
            require(payload[section['offset']:section['offset']+section['length']] == arr.tobytes(),
                    'Serialized stream readback differs')
        if paint is not None:
            full_paint = np.concatenate((paint, new_paint)).astype('<f2')
            require(full_paint[:n].tobytes() == paint.tobytes(), 'Original paint data changed')
            files[name + '.paint.bin'] = full_paint.tobytes()
        files[name + '.stream.bin'] = payload
        files[name + '.npz'] = dict(positions=positions, triangles=indices,
            bones=np.concatenate((native_bones, bones)), weights=np.concatenate((native_weights, q)),
            uv=arrays[-1][1].astype('f4'), acceptedSourceVertex=used,
            nativeKeepFaces=keep, jointNames=np.array(palette))
        results.append(dict(name=name, sourceModel=mid, vertexCount=len(positions), nativeVertexCount=n,
            addedCuffVertices=len(used), indexCount=indices.size, triangleCount=len(indices),
            keptNativeFaces=len(keep), addedCuffFaces=len(new_faces), uvLayers=uv.shape[1],
            maxCuffInfluences=int(unique_cuff_bones.max()), nativeVertexBytesPreserved=True,
            nativeKeepIndicesPreserved=True, acceptedCuffPositionsExact=True,
            positiveCuffInfluencesPreserved=bool(np.array_equal(q > 0, weights > 0)),
            sections=sections, jointPalette=palette, jointIndexDomain='complete skeleton',
            bindingIndices=bindings['JointIndexList'],
            bounds=[positions.min(0).tolist(), positions.max(0).tolist()],
            skinType='SKVTXTYPE_8x8', hasNormalTangent=len(native_arrays)==3,
            byteLength=len(payload), paintBytes=0 if paint is None else full_paint.nbytes))
    require(all(digest(Path(p)) == h for p, h in tracked.items()), 'Input changed')
    output.mkdir()
    for name, value in files.items():
        if isinstance(value, dict):
            np.savez_compressed(output / name, **value)
        else:
            (output / name).write_bytes(value)
    report = dict(status='Full native stream graft built; native resource serialization/runtime pending',
                  meshes=results, inputsUnchanged=True, inputSha256=tracked,
                  files={name: digest(output/name) for name in files}, gameFilesWritten=False)
    (output/'report.json').write_text(json.dumps(report, indent=2), encoding='utf-8')
    print(json.dumps(results, indent=2))


if __name__ == '__main__':
    run()
