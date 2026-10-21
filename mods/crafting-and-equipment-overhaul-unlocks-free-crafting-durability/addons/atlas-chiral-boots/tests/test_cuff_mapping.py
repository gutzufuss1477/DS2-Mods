import importlib.util
import sys
import unittest
from pathlib import Path
import numpy as np

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
HAS_SCIPY = importlib.util.find_spec('scipy') is not None
if HAS_SCIPY:
    from map_pants_cuff import match_faces


@unittest.skipUnless(HAS_SCIPY, 'Run with the existing geometry-venv for cuff mapping tests')
class CuffMappingTests(unittest.TestCase):
    def data(self):
        tri = np.array([[[0., 0, 0], [1, 0, 0], [0, 1, 0]]])
        uv = tri[:, :, :2].copy()
        sub = np.array([[[.1, .1, 0], [.2, .1, 0], [.1, .2, 0]]])
        return tri, uv, sub

    def test_subdivision_recovers_parent_and_barycentrics(self):
        tri, uv, sub = self.data()
        parent, bary, error = match_faces(sub, sub[:, :, :2], tri, uv)
        self.assertEqual(parent.tolist(), [0])
        np.testing.assert_allclose(np.einsum('fij,fjd->fid', bary, tri[parent]), sub)
        self.assertLess(error, 1e-14)

    def test_coplanar_uv_seams_choose_matching_surface(self):
        tri, uv, sub = self.data()
        doubled = np.concatenate((tri, tri))
        split_uv = np.concatenate((uv + 2, uv))
        parent, _, _ = match_faces(sub, sub[:, :, :2], doubled, split_uv)
        self.assertEqual(parent.tolist(), [1])

    def test_off_surface_is_refused(self):
        tri, uv, sub = self.data()
        sub[:, :, 2] = .01
        with self.assertRaisesRegex(ValueError, 'Unmatched'):
            match_faces(sub, sub[:, :, :2], tri, uv)

    def test_triangle_crossing_parent_boundary_is_refused(self):
        tri, uv, sub = self.data()
        sub[0, 0] = [-.1, .1, 0]
        with self.assertRaisesRegex(ValueError, 'Unmatched'):
            match_faces(sub, sub[:, :, :2], tri, uv)

    def test_distant_centroid_search_expands_without_relaxing_threshold(self):
        tri, uv, sub = self.data()
        distractors = np.tile(tri, (8, 1, 1)) * .05 + np.array([.1, .1, .01])
        native = np.concatenate((distractors, tri))
        native_uv = np.concatenate((distractors[:, :, :2], uv))
        parent, _, error = match_faces(sub, sub[:, :, :2], native, native_uv, k=1)
        self.assertEqual(parent.tolist(), [8])
        self.assertLess(error, 1e-14)


if __name__ == '__main__':
    unittest.main()
