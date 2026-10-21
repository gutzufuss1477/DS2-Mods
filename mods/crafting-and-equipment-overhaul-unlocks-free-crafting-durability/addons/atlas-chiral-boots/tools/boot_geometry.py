"""Offline DS2 six-influence vertex streams; not a resource loader or installer.

Layout follows the exported native 31126:40597 vertex declaration. Four UVs
are retained in a wider UV stream. Its new declaration still needs native
resource serialization and shader validation. TangentBFlip stores a boolean,
not a signed fourth component; runtime material validation remains necessary.
"""
import math
import numpy as np


def require(ok, message):
    if not ok:
        raise ValueError(message)


def quantize_weights(weights):
    """Constrained nearest-byte weights: sum 255 and retain every positive slot."""
    w = np.asarray(weights, dtype=np.float64)
    require(w.ndim == 2 and 0 < w.shape[1] <= 8, 'Expected at most eight influences')
    require(np.isfinite(w).all() and (w >= 0).all(), 'Invalid weights')
    require(np.all(abs(w.sum(1) - 1) < 1e-5), 'Weights must sum to one')
    w = w / w.sum(1, keepdims=True)
    positive = w > 0
    desired = w * 255
    q = np.maximum(np.rint(desired).astype(np.int32), positive.astype(np.int32))
    for _ in range(16):
        difference = 255 - q.sum(1)
        if not difference.any():
            require(np.array_equal(q > 0, positive), 'Influence lost during quantization')
            return q.astype(np.uint8)
        for direction in (1, -1):
            rows = np.flatnonzero(difference * direction > 0)
            if not len(rows):
                continue
            eligible = positive[rows] if direction == 1 else q[rows] > positive[rows]
            cost = 2 * direction * (q[rows] - desired[rows]) + 1
            cost = np.where(eligible, cost, np.inf)
            columns = np.argmin(cost, axis=1)
            require(np.isfinite(cost[np.arange(len(rows)), columns]).all(), 'No quantization budget')
            q[rows, columns] += direction
    raise ValueError('Weight quantization did not converge')


def remap_skin(bones, weights, author_names, palette_names, slots=6):
    b, w = np.asarray(bones), np.asarray(weights)
    require(b.ndim == 2 and b.shape == w.shape, 'Bone/weight shape mismatch')
    require(b.dtype.kind in 'iu' and (b >= 0).all(), 'Invalid bone indices')
    require(len(set(author_names)) == len(author_names), 'Duplicate author joint name')
    require(len(set(palette_names)) == len(palette_names) <= 256, 'Invalid native palette')
    require(np.isfinite(w).all() and (w >= 0).all(), 'Invalid weights')
    positive = w > 0
    require(slots in (6, 8), 'Unsupported native skin declaration')
    require((positive.sum(1) <= slots).all(), 'Native skin slots exceeded; refusing truncation')
    require((b[positive] < len(author_names)).all(), 'Bone index outside author rig')
    palette = {name: i for i, name in enumerate(palette_names)}
    used = set(int(i) for i in b[positive])
    require(all(author_names[i] in palette for i in used), 'Weighted bone missing from native palette')
    out_b = np.zeros((len(b), slots), dtype=np.uint8)
    out_w = np.zeros((len(b), slots), dtype=np.float64)
    for i in range(len(b)):
        selected = np.flatnonzero(positive[i])
        require(len(set(b[i, selected])) == len(selected), 'Duplicate influence on a vertex')
        for slot, j in enumerate(selected):
            out_b[i, slot] = palette[author_names[int(b[i, j])]]
            out_w[i, slot] = w[i, j]
    return out_b, out_w


def pack_direction(vectors, flags):
    v = np.asarray(vectors, dtype=np.float64)
    lengths = np.linalg.norm(v, axis=1)
    require(np.isfinite(v).all() and (abs(lengths - 1) < 1e-3).all(), 'Non-unit normal/tangent')
    q = np.rint(np.clip(v / lengths[:, None], -1, 1) * 511).astype(np.int32)
    return ((q[:, 0] & 1023) | ((q[:, 1] & 1023) << 10) |
            ((q[:, 2] & 1023) << 20) | (np.asarray(flags, dtype=np.uint32) << 30)).astype('<u4')


def decode_direction(values):
    q = np.stack([((values >> shift) & 1023).astype(np.int32) for shift in (0, 10, 20)], axis=1)
    q = np.where(q >= 512, q - 1024, q).astype(np.float64)
    length = np.linalg.norm(q, axis=1)
    require((length > 0).all(), 'Zero packed direction')
    return q / length[:, None]


def build_stream(data, author_names, palette_names):
    p, n, t, uv, tri = (np.asarray(data[k]) for k in ('positions', 'normals', 'tangents', 'uv', 'triangles'))
    count = len(p)
    require(0 < count < 65536 and p.shape == (count, 3), 'Unsupported position/index count')
    require(n.shape == (count, 3) and t.shape == (count, 4), 'Invalid NBT shape')
    require(uv.shape == (count, 4, 2), 'Four original UV layers required')
    require(all(np.isfinite(x).all() for x in (p, n, t, uv)), 'Non-finite vertex attribute')
    require((abs(uv) <= 65504).all(), 'UV outside finite half-float range')
    require(np.isin(t[:, 3], (-1, 1)).all(), 'Invalid tangent handedness')
    require(tri.ndim == 2 and tri.shape[1] == 3 and tri.size > 0, 'Invalid triangle shape')
    require(tri.dtype.kind in 'iu' and tri.min() >= 0 and tri.max() < count, 'Triangle outside vertex array')
    b, w = remap_skin(data['bones'], data['weights'], author_names, palette_names)
    require(len(b) == count, 'Skin vertex count mismatch')
    q = quantize_weights(w)
    position_skin = np.zeros((count, 28), dtype=np.uint8)
    position_skin[:, :12] = p.astype('<f4').view(np.uint8).reshape(count, 12)
    position_skin[:, 12:16], position_skin[:, 16:18] = b[:, :4], b[:, 4:]
    position_skin[:, 20:24], position_skin[:, 24:26] = q[:, :4], q[:, 4:]
    nbt = np.column_stack((pack_direction(n, 1), pack_direction(t[:, :3], t[:, 3] < 0))).astype('<u4')
    payload = bytearray()
    sections = {}
    for name, stride, array in [('positionSkin', 28, position_skin), ('normalTangent', 8, nbt),
                                ('uv', 16, uv.astype('<f2')), ('index', 2, tri.astype('<u2'))]:
        alignment = math.lcm(stride, 16)
        offset = math.ceil(len(payload) / alignment) * alignment
        payload.extend(bytes(offset - len(payload)))
        raw = array.tobytes()
        sections[name] = {'offset': offset, 'length': len(raw), 'stride': stride}
        payload.extend(raw)
    descriptor = {'vertexCount': count, 'indexCount': tri.size, 'sections': sections,
                  'skinType': 'SKVTXTYPE_6x8', 'deformer': 'DeformPosAndNormalsMax8Weights',
                  'jointPalette': list(palette_names), 'uvLayers': 4,
                  'nativeResourceSerialized': False, 'runtimeValidated': False}
    return bytes(payload), descriptor


def read_stream(payload, descriptor):
    n = descriptor['vertexCount']
    index_count = descriptor['indexCount']
    require(type(n) is int and 0 < n < 65536, 'Invalid vertex count')
    require(type(index_count) is int and index_count > 0 and index_count % 3 == 0, 'Invalid index count')
    sections = descriptor['sections']
    expected = {'positionSkin': (28, n), 'normalTangent': (8, n), 'uv': (16, n), 'index': (2, index_count)}
    require(set(sections) == set(expected), 'Invalid section list')
    end = 0
    for name, (stride, count) in expected.items():
        s = sections[name]
        require(s['stride'] == stride and s['length'] == stride * count, 'Invalid section size')
        require(s['offset'] % math.lcm(s['stride'], 16) == 0, 'Unaligned section')
        require(0 <= s['offset'] <= s['offset'] + s['length'] <= len(payload), 'Section outside payload')
        require(s['offset'] >= end, 'Overlapping sections')
        end = s['offset'] + s['length']
    def view(name, dtype, shape, strides=None, extra=0):
        return np.ndarray(shape, dtype=dtype, buffer=payload,
                          offset=sections[name]['offset'] + extra, strides=strides).copy()
    p = view('positionSkin', '<f4', (n, 3), (28, 4))
    b = view('positionSkin', 'u1', (n, 6), (28, 1), 12)
    w = view('positionSkin', 'u1', (n, 6), (28, 1), 20)
    nbt = view('normalTangent', '<u4', (n, 2))
    return {'positions': p, 'bones': b, 'weights': w.astype(np.float64) / 255,
            'normals': decode_direction(nbt[:, 0]), 'tangents': decode_direction(nbt[:, 1]),
            'bitangentSign': np.where((nbt[:, 1] >> 30) == 0, 1, -1),
            'uv': view('uv', '<f2', (n, 4, 2)).astype(np.float32),
            'triangles': view('index', '<u2', (descriptor['indexCount'] // 3, 3))}
