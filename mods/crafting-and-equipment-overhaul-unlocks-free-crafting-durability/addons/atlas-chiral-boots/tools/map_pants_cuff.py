"""Match accepted subdivided cuffs to current native faces; restore native UVs.

Produces an offline graft map, not a native resource or an installed modification.
Requires the existing geometry-venv (NumPy/SciPy). Does not open Blender or the game.
"""
import json
from pathlib import Path
import sys
import numpy as np
from scipy.spatial import cKDTree
from overlay_cache import digest, plain_path, require

ROOT = Path(__file__).resolve().parents[1]
LAB = ROOT.parents[1]
sys.path.insert(0, str(LAB / 'tools/cast/libraries/python'))
import cast


def match_faces(original, face_uv, native, native_uv, k=64):
    """Match whole triangles by surface containment AND UV0, preserving seams."""
    tree = cKDTree(native.mean(1))
    parents, barycentric, residuals = [], [], []
    for begin in range(0, len(original), 1024):
        q = original[begin:begin + 1024]
        uv = face_uv[begin:begin + 1024]
        _, near = tree.query(q.mean(1), k=min(k, len(native)))
        if near.ndim == 1:
            near = near[:, None]
        candidates = native[near]
        a = candidates[:, :, 0]
        v0, v1 = candidates[:, :, 1] - a, candidates[:, :, 2] - a
        v2 = q[:, None] - a[:, :, None]
        d00, d01, d11 = (v0*v0).sum(2), (v0*v1).sum(2), (v1*v1).sum(2)
        den = d00*d11 - d01*d01
        safe = np.where(abs(den) > 1e-22, den, 1)
        d20, d21 = (v2*v0[:, :, None]).sum(3), (v2*v1[:, :, None]).sum(3)
        v = (d11[:, :, None]*d20 - d01[:, :, None]*d21)/safe[:, :, None]
        w = (d00[:, :, None]*d21 - d01[:, :, None]*d20)/safe[:, :, None]
        bary = np.stack((1-v-w, v, w), axis=3)
        prediction = np.einsum('nkij,nkjd->nkid', bary, candidates)
        distance = np.linalg.norm(prediction-q[:, None], axis=3).max(2)
        uv_prediction = np.einsum('nkij,nkjd->nkid', bary, native_uv[near])
        uv_error = abs(uv_prediction-uv[:, None]).max((2, 3))
        valid = ((bary.min((2, 3)) >= -2e-4) & (bary.max((2, 3)) <= 1.0002)
                 & (distance < 3e-7) & (uv_error < 3e-6) & (abs(den) > 1e-22))
        scores = np.where(valid, distance + uv_error*.01, np.inf)
        chosen = scores.argmin(1)
        ok = np.isfinite(scores[np.arange(len(q)), chosen])
        picked_parent = near[np.arange(len(q)), chosen]
        picked_bary = bary[np.arange(len(q)), chosen]
        picked_error = distance[np.arange(len(q)), chosen]
        if not ok.all():
            require(k < len(native), f'Unmatched cuff faces near {begin}: {np.flatnonzero(~ok)[:10].tolist()}')
            # Long source triangles can have distant centroids; widen only those
            # searches. Geometric and UV acceptance thresholds stay unchanged.
            retry_parent, retry_bary, retry_error = match_faces(
                q[~ok], uv[~ok], native, native_uv, min(k*4, len(native)))
            picked_parent[~ok], picked_bary[~ok] = retry_parent, retry_bary
            picked_error[~ok] = retry_error
        parents.append(picked_parent)
        barycentric.append(picked_bary)
        residuals.extend(picked_error.tolist())
    return np.concatenate(parents), np.concatenate(barycentric), max(residuals)


def load_mesh(path):
    model = cast.Cast.load(str(path)).Roots()[0].ChildrenOfType(cast.Model)[0]
    require(len(model.Meshes()) == 1, 'Unexpected native primitive count')
    return model.Meshes()[0]


def run():
    output = plain_path(ROOT / 'staging/pants-cuff-map-v1')
    require(not output.exists(), 'Refusing existing output')
    assets = ROOT / 'assets/chiral-cuff-v3'
    manifest = json.loads((assets / 'manifest.json').read_text('utf-8'))
    inputs, results, pending = {}, [], []
    for mid, name in [(270, 'pants-body'), (638, 'pants-strips')]:
        current = ROOT / f'evidence/pants-current-cast/RegularSkinnedMeshResource_39987_{mid}.cast'
        previous = LAB / f'analysis/atlas-test17-fit/suit/cast/RegularSkinnedMeshResource_39987_{mid}.cast'
        source = assets / (name + '.npz')
        provenance = LAB / f'analysis/ds1-ds2-boot-fit-20261019/cuff-v2/cuff-{mid}.npz'
        for path in (current, previous, source, provenance):
            inputs[str(path)] = digest(plain_path(path))
        entry = next(x for x in manifest['meshes'] if x['name'] == name)
        require(inputs[str(source)] == entry['sha256'], 'Accepted data hash mismatch')
        native, old = load_mesh(current), load_mesh(previous)
        require(set(native.properties) == set(old.properties), 'Native property list changed')
        for key in native.properties:
            require(np.array_equal(native.properties[key].values, old.properties[key].values),
                    'Current game/reference mismatch in ' + key)
        p = np.array(native.VertexPositionBuffer()).reshape(-1, 3)
        f = np.array(native.FaceBuffer(), dtype=np.uint32).reshape(-1, 3)
        uv = np.stack([np.array(native.VertexUVLayerBuffer(i)).reshape(-1, 2)
                       for i in range(native.UVLayerCount())], axis=1)
        uv[:, :, 1] = 1 - uv[:, :, 1]  # CAST exporter flips V; authoring uses native V.
        with np.load(source, allow_pickle=False) as a, np.load(provenance, allow_pickle=False) as b:
            original = b['original'][a['sourceVertex']]
            source_faces = a['triangles']
            take = original[source_faces, 2].min(1) < .365
            replace = p[f, 2].min(1) < .365
            selected, selected_ids = f[replace], np.flatnonzero(replace)
            cuff_faces = source_faces[take]
            parent, bary, error = match_faces(original[cuff_faces], a['uv'][cuff_faces, 0],
                                              p[selected], uv[selected, 0])
            parent = selected_ids[parent]
            restored_uv = np.einsum('fij,fjld->fild', bary, uv[f[parent]])
            require(abs(restored_uv[:, :, 0] - a['uv'][cuff_faces, 0]).max() < 3e-6,
                    'Native UV0 disagrees with accepted UVs')
            # Total pre-deformation area per native parent must be covered once.
            tri = original[cuff_faces]
            areas = np.linalg.norm(np.cross(tri[:, 1]-tri[:, 0], tri[:, 2]-tri[:, 0]), axis=1)/2
            native_tri = p[f]
            native_area = np.linalg.norm(np.cross(native_tri[:, 1]-native_tri[:, 0],
                                                  native_tri[:, 2]-native_tri[:, 0]), axis=1)/2
            covered = np.bincount(parent, weights=areas, minlength=len(f))
            area_error = abs(covered[replace] - native_area[replace])
            require((area_error < 1e-9 + native_area[replace]*1e-4).all(), 'Parent surface coverage differs')
            # Save corner UVs so separate UV seams never get silently welded.
            data = dict(nativeKeepFaces=np.flatnonzero(~replace).astype('u4'),
                        acceptedCuffFaces=np.flatnonzero(take).astype('u4'),
                        nativeParentFace=parent.astype('u4'), barycentric=bary,
                        restoredCornerUV=restored_uv.astype('f4'))
            pending.append((name, data))
            results.append({'name': name, 'currentNativeReferenceExact': True,
                            'keptNativeFaces': int((~replace).sum()), 'replacedNativeFaces': int(replace.sum()),
                            'acceptedCuffFaces': len(cuff_faces), 'maxSurfaceResidualMeters': error,
                            'maxParentAreaErrorSquareMeters': float(area_error.max()),
                            'restoredUvLayers': native.UVLayerCount(), 'nativeResourceSerialized': False})
    require(all(digest(Path(p)) == h for p, h in inputs.items()), 'Source changed')
    output.mkdir()
    for name, data in pending:
        dest = output / (name + '.npz')
        np.savez_compressed(dest, **data)
        with np.load(dest, allow_pickle=False) as stored:
            require(all(np.array_equal(stored[k], v) for k, v in data.items()), 'Map roundtrip differs')
    report = {'status': 'Verified cuff surface correspondence and restored UVs; assembly/encoding pending',
              'inputSha256': inputs, 'inputsUnchanged': True, 'meshes': results,
              'gameFilesWritten': False, 'gameTested': False}
    (output / 'report.json').write_text(json.dumps(report, indent=2), encoding='utf-8')
    print(json.dumps(report['meshes'], indent=2))


if __name__ == '__main__':
    run()
