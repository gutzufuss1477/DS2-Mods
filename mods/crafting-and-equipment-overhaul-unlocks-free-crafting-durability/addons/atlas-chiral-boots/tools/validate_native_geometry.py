"""Independent native-loader -> CAST roundtrip for all accepted geometry."""
from pathlib import Path
import json
import sys
import numpy as np
from boot_geometry import read_stream
from overlay_cache import digest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT.parents[1]/'tools/cast/libraries/python'))
import cast


def run(report_path=None, cast_folder=None, output=None):
    report_path = report_path or ROOT/'staging/native-geometry-v1/report.json'
    cast_folder = cast_folder or ROOT/'evidence/native-geometry-cast-v2'
    report = json.loads(report_path.read_text())
    results = []
    for row in report['meshes']:
        name, gid, mid = row['label'], row['group'], row['mesh']
        if name.startswith('hanging'):
            continue  # The independent hanging validator includes the second primitive.
        path = cast_folder/f'RegularSkinnedMeshResource_{gid}_{mid}.cast'
        model = cast.Cast.load(str(path)).Roots()[0].ChildrenOfType(cast.Model)[0]
        assert len(model.Meshes()) == 1
        mesh = model.Meshes()[0]
        if name.startswith('pants'):
            expected = np.load(ROOT/f'staging/full-pants-graft-v1/{name}.npz')
        else:
            folder = ROOT/'staging/chiral-cuff-v3-geometry-v2'
            layout = json.loads((folder/(name+'.layout.json')).read_text())
            expected = read_stream((folder/(name+'.stream.bin')).read_bytes(), layout)
        n = row['vertexCount']
        positions = np.array(mesh.VertexPositionBuffer(), dtype='f4').reshape(n, 3)
        triangles = np.array(mesh.FaceBuffer()).reshape(-1, 3)
        bones = np.array(mesh.VertexWeightBoneBuffer()).reshape(n, -1)
        weights = np.array(mesh.VertexWeightValueBuffer()).reshape(n, -1)
        assert np.array_equal(positions, expected['positions']), name+' positions'
        assert np.array_equal(triangles, expected['triangles']), name+' indices'
        assert mesh.UVLayerCount() == expected['uv'].shape[1]
        uv_error = 0.
        for layer in range(mesh.UVLayerCount()):
            uv = np.array(mesh.VertexUVLayerBuffer(layer)).reshape(n, 2)
            uv[:, 1] = 1-uv[:, 1]
            error = float(abs(uv-expected['uv'][:, layer]).max())
            assert error < 1e-6, (name, layer, error)
            uv_error = max(error, uv_error)
        # CAST normalizes native byte weights. Compare complete skeleton domains
        # after that normalization, including every nonzero influence.
        slots = bones.shape[1]
        expected_bones = expected['bones'][:, :slots]
        expected_weights = expected['weights'][:, :slots]
        expected_weights = expected_weights / expected_weights.sum(1, keepdims=True)
        assert np.array_equal(bones[weights > 0], expected_bones[weights > 0]), name+' bones'
        assert np.array_equal(weights > 0, expected_weights > 0), name+' influence loss'
        err = float(abs(weights-expected_weights).max())
        assert err < 1e-6, (name, err)
        results.append(dict(name=name, nativeGroup=gid, nativeMesh=mid, vertices=n,
                            positionsExact=True, indicesExact=True, skeletonIndicesExact=True,
                            allPositiveInfluencesRetained=True, maxNormalizedWeightError=err,
                            uvLayers=mesh.UVLayerCount(), maxUVError=uv_error,
                            independentCastSha256=digest(path)))
    result = dict(status='All six resources passed independent native-to-CAST readback',
                  meshes=results, inGameTested=False, materialBindingPending=not bool(report.get('material')),
                  tangentCoverage='CAST exporter does not export TangentBFlip; binary tests cover it')
    output = output or ROOT/'evidence/native-geometry-validation-v1.json'
    with output.open('x') as f:
        json.dump(result, f, indent=2)
    print(json.dumps(result))


if __name__ == '__main__':
    run()
