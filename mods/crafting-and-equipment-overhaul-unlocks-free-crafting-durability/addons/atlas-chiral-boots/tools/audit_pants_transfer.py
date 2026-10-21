"""Read-only provenance audit; writes a report, never alters or merges meshes."""
import json
from pathlib import Path
import sys
import numpy as np
from overlay_cache import digest, plain_path, relative_file, require

ROOT = Path(__file__).resolve().parents[1]
LAB = ROOT.parents[1]
sys.path.insert(0, str(LAB / 'tools/cast/libraries/python'))
import cast


def run():
    output = plain_path(ROOT / 'evidence/pants-transfer-audit.json')
    require(not output.exists(), 'Existing audit will not be overwritten')
    assets = plain_path(ROOT / 'assets/chiral-cuff-v3')
    manifest_path = assets / 'manifest.json'
    manifest = json.loads(manifest_path.read_text('utf-8'))
    inputs = {str(manifest_path): digest(manifest_path)}
    rows = []
    for native_id, name in [(270, 'pants-body'), (638, 'pants-strips')]:
        entry = next(x for x in manifest['meshes'] if x['name'] == name)
        accepted = relative_file(assets, entry['file'])
        require(digest(accepted) == entry['sha256'], 'Accepted export hash differs')
        full = LAB / f'analysis/atlas-test17-fit/suit/cast/RegularSkinnedMeshResource_39987_{native_id}.cast'
        base = LAB / 'analysis/ds1-ds2-boot-fit-20261019'
        v2 = base / f'cuff-v2/cuff-{native_id}.npz'
        v3 = base / f'cuff-v3/cuff-{native_id}.npz'
        for path in (accepted, full, v2, v3):
            inputs[str(path)] = digest(plain_path(path))
        model = cast.Cast.load(str(full)).Roots()[0].ChildrenOfType(cast.Model)[0]
        require(len(model.Meshes()) == 1, 'Unexpected full reference mesh count')
        mesh = model.Meshes()[0]
        positions = np.array(mesh.VertexPositionBuffer()).reshape(-1, 3)
        triangles = np.array(mesh.FaceBuffer()).reshape(-1, 3)
        with np.load(accepted, allow_pickle=False) as a, np.load(v2, allow_pickle=False) as b, np.load(v3, allow_pickle=False) as c:
            ids = a['sourceVertex']
            require(np.array_equal(a['positions'], c['after'][ids].astype('f4')), 'Accepted/v3 vertex bridge differs')
            require(np.array_equal(c['before'], b['positions'].astype('f4')), 'v2/v3 bridge differs')
            require(np.array_equal(c['triangles'], b['triangles']), 'v3 changed topology')
            require(np.array_equal(ids[a['triangles']], c['triangles']), 'Accepted export triangle/source IDs differ')
            original = b['original'][ids]
            protected = original[:, 2] >= .365
            require(np.array_equal(a['positions'][protected], original[protected].astype('f4')), 'Upper region unexpectedly modified')
            rows.append({'name': name, 'resource': f'39987:{native_id}',
                         'fullReferenceVertices': len(positions), 'fullReferenceTriangles': len(triangles),
                         'fullReferenceUvLayers': mesh.UVLayerCount(), 'acceptedUvLayers': a['uv'].shape[1],
                         'acceptedTriangles': len(a['triangles']), 'acceptedMaxInfluences': int((a['weights'] > 0).sum(1).max()),
                         'originalTrianglesEntirelyAbove365mm': int((positions[triangles, 2].min(1) >= .365).sum()),
                         'originalTrianglesTouchingBelow365mm': int((positions[triangles, 2].min(1) < .365).sum()),
                         'acceptedTrianglesTouchingBelow365mm': int((original[a['triangles'], 2].min(1) < .365).sum()),
                         'sourceVertexAndTriangleProvenanceVerified': True, 'acceptedVerticesAbove365mmUnchanged': True,
                         'wholeResourceReplacementAllowed': False})
    require(all(digest(Path(p)) == h for p, h in inputs.items()), 'Inputs changed during audit')
    report = {'schemaVersion': 1, 'status': 'Provenance and scope verified; full-mesh graft NOT implemented',
              'inputSha256': inputs, 'inputsUnchanged': True, 'meshes': rows,
              'referenceCastNeedsCurrentNativeStreamVerification': True,
              'requiredNextSteps': [
                  'Verify full reference CAST data against current native resource streams',
                  'Use v2 original positions and sourceVertex IDs to map subdivided cuff faces onto original faces',
                  'Replace only the lower face region; verify boundary continuity and preserve all remaining faces',
                  'Restore native UV1/UV2 on subdivided body vertices via proven original-face barycentric mapping',
                  'Preserve all seven positive influences with a verified native eight-slot skin declaration',
                  'Validate normals/tangents, bind matrices, LODs and conditional ATLAS-only binding'],
              'nativeResourceSerialized': False, 'gameTested': False, 'gameFilesWritten': False}
    with output.open('x', encoding='utf-8') as out:
        json.dump(report, out, indent=2)
    print(json.dumps({'report': str(output), 'inputsUnchanged': True, 'meshes': rows}, indent=2))


if __name__ == '__main__':
    run()
