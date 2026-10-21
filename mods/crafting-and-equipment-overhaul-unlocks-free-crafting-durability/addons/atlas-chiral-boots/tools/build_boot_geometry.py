"""Build and verify offline Chiral boot streams outside the game installation.

These buffers are NOT a serialized DS2 resource package or an installable mod.
This entry point has fixed input/output roots, refuses overwrite and never opens
game archives or saves. The accepted authoring export is kept byte-identical.
"""
from datetime import datetime, timezone
import hashlib
import json
from pathlib import Path
import shutil
import tempfile

import numpy as np
from PIL import Image

from boot_geometry import build_stream, read_stream, remap_skin, require
from overlay_cache import digest, plain_path, relative_file

ROOT = Path(__file__).resolve().parents[1]
SOURCE_SHA = '6ec1bcb1d467f8915cf922129d6c63ea1a3f1589b32d05a2c856d580c5b57cb1'
BOOT_NAMES = {'left-sole', 'right-sole', 'left-upper', 'right-upper'}


def native_matrix(value):
    return np.array([[value['Col' + str(c)][r] for c in range(4)] for r in 'XYZW'])


def angular_error(a, b):
    a = np.asarray(a, dtype=np.float64)
    a /= np.linalg.norm(a, axis=1, keepdims=True)
    return float(np.degrees(np.arccos(np.clip(np.sum(a * b, axis=1), -1, 1))).max())


def synthetic_pose_audit(positions, bones, source_weights, decoded_weights, rest, inverse_bind):
    """Compare quantization under artificial joint deltas, NOT game animation."""
    source_weights = source_weights / source_weights.sum(1, keepdims=True)
    homogeneous = np.column_stack((positions, np.ones(len(positions))))
    rows = []
    for label, angles in [('rest', [0] * 6),
                          ('ankle-flex', [0, 35, 35, 35, 35, 0]),
                          ('toe-flex', [0, 0, 50, 0, 0, 0]),
                          ('mixed-stress', [-10, 25, 55, 15, 30, -5])]:
        matrices = []
        for j, degrees in enumerate(angles):
            theta = np.radians(degrees)
            c, s = np.cos(theta), np.sin(theta)
            rotation = np.array([[1, 0, 0, 0], [0, c, -s, 0], [0, s, c, 0], [0, 0, 0, 1.]])
            pivot = np.eye(4)
            pivot[:3, 3] = rest[j, :3, 3]
            # World-space delta about each joint's rest pivot; no hierarchy claim.
            matrices.append(pivot @ rotation @ np.linalg.inv(pivot) @ rest[j] @ inverse_bind[j])
        transformed = np.einsum('jab,vb->jva', np.array(matrices), homogeneous)[:, :, :3]
        a, b = np.zeros_like(positions, dtype=np.float64), np.zeros_like(positions, dtype=np.float64)
        for slot in range(6):
            points = transformed[bones[:, slot], np.arange(len(positions))]
            a += source_weights[:, slot, None] * points
            b += decoded_weights[:, slot, None] * points
        errors = np.linalg.norm(a - b, axis=1)
        maximum = float(errors.max())
        require(maximum <= .002, 'Synthetic pose quantization exceeds 2 mm review limit')
        rows.append({'pose': label, 'maxPositionErrorMeters': maximum,
                     'p99PositionErrorMeters': float(np.quantile(errors, .99))})
    return rows


def verify_stream(data, blob, layout, author_names, palette_names, rest, inverse_bind, binding_indices):
    read = read_stream(blob, layout)
    bones, weights = remap_skin(data['bones'], data['weights'], author_names, palette_names)
    require(np.array_equal(data['positions'], read['positions']), 'Position roundtrip changed geometry')
    require(np.array_equal(data['triangles'], read['triangles']), 'Index roundtrip changed topology')
    require(np.array_equal(bones, read['bones']), 'Joint palette roundtrip mismatch')
    require(np.array_equal(weights > 0, read['weights'] > 0), 'Positive influence lost')
    require(np.all(np.rint(read['weights'] * 255).sum(1) == 255), 'Weight byte sum mismatch')
    require(np.array_equal(data['tangents'][:, 3], read['bitangentSign']), 'Handedness roundtrip mismatch')
    require(np.array_equal(data['uv'].astype(np.float16).astype(np.float32), read['uv']), 'Half UV roundtrip mismatch')
    normal_error = angular_error(data['normals'], read['normals'])
    tangent_error = angular_error(data['tangents'][:, :3], read['tangents'])
    require(max(normal_error, tangent_error) <= .12, 'Direction quantization exceeds review limit')
    require(np.isin(bones[weights > 0], binding_indices).all(), 'Vertex bone lacks inverse bind')
    # Synthetic matrix array is compact; convert only the audit's lookup indices.
    binding_lookup = np.zeros(len(palette_names), dtype='u1')
    for j, index in enumerate(binding_indices):
        binding_lookup[index] = j
    return {'positionsExact': True, 'triangleIndicesExact': True, 'jointsMappedByName': True,
            'jointIndexDomain': 'complete native skeleton (not compact binding-list position)',
            'positiveInfluencesRetained': True, 'weightBytesSum': 255,
            'maxWeightAbsoluteError': float(abs(weights - read['weights']).max()),
            'maxNormalAngleDegrees': normal_error, 'maxTangentAngleDegrees': tangent_error,
            'uvLayersRetained': 4, 'maxUvAbsoluteError': float(abs(data['uv'] - read['uv']).max()),
            'handednessRoundtripExact': True,
            'syntheticPoses': synthetic_pose_audit(data['positions'], binding_lookup[bones], weights, read['weights'], rest, inverse_bind)}


def build():
    root = plain_path(ROOT)
    assets = plain_path(root / 'assets/chiral-cuff-v3')
    staging_root = plain_path(root / 'staging')
    require(staging_root.parent == root, 'Output must be the private addon staging folder')
    target = plain_path(staging_root / 'chiral-cuff-v3-geometry-v2')
    require(not target.exists(), 'Output exists; refusing overwrite: ' + str(target))
    tracked = {}

    def track(path, expected=None):
        path = plain_path(path)
        actual = digest(path)
        require(expected is None or actual == expected, 'Input checksum mismatch: ' + str(path))
        tracked[path] = actual
        return path

    manifest = json.loads(track(assets / 'manifest.json').read_text('utf-8'))
    require(manifest['sourceSha256'] == SOURCE_SHA, 'Unexpected accepted art revision')
    track(Path(manifest['source']), SOURCE_SHA)
    for entry in [*manifest['meshes'], *manifest['images'].values()]:
        track(relative_file(assets, entry['file']), entry['sha256'])
    for name in ('boot_geometry.py', 'build_boot_geometry.py', 'overlay_cache.py'):
        track(root / 'tools' / name)

    def metadata(name):
        path = track(root / 'evidence/native-layouts' / (name + '.json'))
        return json.loads(path.read_text('utf-8'))

    skeleton = metadata('Skeleton_31126_301')
    boot_parts = [x for x in manifest['meshes'] if x['name'] in BOOT_NAMES]
    require(len(boot_parts) == 4 and {x['name'] for x in boot_parts} == BOOT_NAMES, 'Incomplete boot parts')
    native = {}
    for side, bindings_id, vertex_id, skin_id in [('left', 23085, 40597, 6527), ('right', 22123, 25583, 12189)]:
        binding = metadata(f'SkinnedMeshIndexedJointBindings_31126_{bindings_id}')
        vertex = metadata(f'VertexArrayResource_31126_{vertex_id}')
        skin = metadata(f'RegularSkinnedMeshResourceSkinInfo_31126_{skin_id}')
        require([s['Stride'] for s in vertex['Streams']] == [28, 8, 12], 'Unexpected donor strides')
        expected_elements = [
            [('Pos', 0, 'Float', 3), ('BlendIndices', 12, 'UnsignedByte', 4),
             ('BlendIndices2', 16, 'UnsignedByte', 2), ('BlendWeights', 20, 'UnsignedByteNormalized', 4),
             ('BlendWeights2', 24, 'UnsignedByteNormalized', 2)],
            [('Normal', 0, 'X10Y10Z10W2Normalized', 3), ('TangentBFlip', 4, 'X10Y10Z10W2Normalized', 4)],
            [(f'UV{i}', i * 4, 'HalfFloat', 2) for i in range(3)]]
        for stream, expected in zip(vertex['Streams'], expected_elements):
            actual = [(e['Element'], e['Offset'], e['StorageType'], e['SlotsUsed']) for e in stream['Elements']]
            require(actual == expected, 'Unexpected donor vertex declaration')
        require(len(skin['Parts']) == 1 and skin['Parts'][0]['SkinVtxTypes'] == 'SKVTXTYPE_6x8', 'Unsupported donor skin type')
        palette = [j['Name'] for j in skeleton['Joints']]
        require(len(binding['JointIndexList']) == 6, 'Unexpected native joint bindings')
        native[side] = (palette, binding['JointIndexList'],
                        np.array([native_matrix(x) for x in binding['InverseBindMatrices']]))

    prepared = []
    for part in boot_parts:
        with np.load(assets / part['file'], allow_pickle=False) as source:
            data = {key: source[key] for key in source.files}
        rig = manifest['rigs'][part['rig']]
        palette, binding_indices, inverse = native[part['name'].split('-')[0]]
        rest = np.array([rig['restMatrices'][palette[i]] for i in binding_indices])
        bind_error = float(abs(rest @ inverse - np.eye(4)).max())
        require(bind_error <= 2e-5, 'Author rig and native inverse bind disagree')
        blob, layout = build_stream(data, rig['jointNames'], palette)
        checks = verify_stream(data, blob, layout, rig['jointNames'], palette, rest, inverse, binding_indices)
        layout.update(jointIndexDomain='complete native skeleton', bindingIndices=binding_indices)
        layout.update({'name': part['name'], 'materialSource': part['material'],
                       'sourceSha256': part['sha256'], 'maxBindMatrixIdentityError': bind_error,
                       'bounds': [data['positions'].min(0).tolist(), data['positions'].max(0).tolist()]})
        prepared.append((part, data, rig, palette, rest, inverse, binding_indices, blob, layout, checks))

    pants = []
    for name, native_id in [('pants-body', 270), ('pants-strips', 638)]:
        part = next(x for x in manifest['meshes'] if x['name'] == name)
        full = metadata(f'RegularSkinnedMeshResource_39987_{native_id}')
        full_bounds = full['MeshHierarchyInfo']['LocalBounds']
        pants.append({'name': name, 'exportedBounds': part['bounds'], 'nativeBounds': full_bounds,
                      'directReplacementAllowed': False,
                      'reason': 'Export is cropped lower legs; native resource also contains upper-body geometry. Transfer cuff deformation onto complete source mesh first.'})
    textures = []
    for name, entry in manifest['images'].items():
        with Image.open(assets / entry['file']) as im:
            textures.append({'name': name, 'file': entry['file'], 'size': list(im.size), 'mode': im.mode,
                             'sha256': entry['sha256'], 'nativeMaterialBound': False})

    report = {'schemaVersion': 1, 'createdUtc': datetime.now(timezone.utc).isoformat(),
              'status': 'Offline geometry only; not a serialized DS2 resource package',
              'sourceSha256': SOURCE_SHA, 'nativeResourceSerialized': False, 'runtimeValidated': False,
              'gameFilesWritten': False, 'saveFilesAccessed': False,
              'validationLimits': {'normalTangentAngleDegrees': .12, 'syntheticPoseErrorMeters': .002,
                                   'bindMatrixIdentityError': 2e-5},
              'pending': ['Native resource serialization, hashes and references',
                          'Four-UV declaration and TangentBFlip material/shader validation in DS2',
                          'Transfer accepted cuff deformation to full native trouser/body resources',
                          'Conditional ATLAS binding, all LODs and hanging/menu meshes',
                          'Runtime overlay cooperation with Crafting 1.9.8',
                          'User-run animation and addon-removal tests'],
              'pants': pants, 'textures': textures, 'meshes': [],
              'inputSha256': {str(p): h for p, h in tracked.items()}}
    staging_root.mkdir(exist_ok=True)
    temp = Path(tempfile.mkdtemp(prefix='.building-boot-', dir=staging_root))
    try:
        for part, data, rig, palette, rest, inverse, binding_indices, blob, layout, checks in prepared:
            filename = part['name'] + '.stream.bin'
            binary = temp / filename
            with binary.open('xb') as out:
                out.write(blob)
            reread = binary.read_bytes()
            require(reread == blob, 'Stored binary mismatch')
            verify_stream(data, reread, layout, rig['jointNames'], palette, rest, inverse, binding_indices)
            with (temp / (part['name'] + '.layout.json')).open('x', encoding='utf-8') as out:
                json.dump(layout, out, indent=2)
            report['meshes'].append({'name': part['name'], 'file': filename, 'bytes': len(blob),
                                     'sha256': hashlib.sha256(blob).hexdigest(), 'vertices': layout['vertexCount'],
                                     'triangles': layout['indexCount'] // 3, 'checks': checks})
        require(all(digest(p) == h for p, h in tracked.items()), 'Input changed during build')
        report['inputsUnchanged'] = True
        report['totalStreamBytes'] = sum(x['bytes'] for x in report['meshes'])
        with (temp / 'report.json').open('x', encoding='utf-8') as out:
            json.dump(report, out, indent=2)
        require(not target.exists(), 'Concurrent output detected')
        temp.rename(target)
    finally:
        if temp.exists():
            checked = plain_path(temp)
            require(checked.parent == staging_root and checked.name.startswith('.building-boot-'), 'Unsafe cleanup path')
            shutil.rmtree(checked)
    return target, report


if __name__ == '__main__':
    output, result = build()
    print(json.dumps({'output': str(output), 'meshes': len(result['meshes']),
                      'streamBytes': result['totalStreamBytes'], 'inputsUnchanged': result['inputsUnchanged'],
                      'nativeResourceSerialized': False, 'runtimeValidated': False}, indent=2))
