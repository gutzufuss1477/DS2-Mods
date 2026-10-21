import sys
import copy
import unittest
from pathlib import Path
import numpy as np

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from boot_geometry import build_stream, read_stream, remap_skin, quantize_weights


class GeometryTests(unittest.TestCase):
    def data(self):
        return dict(positions=np.array([[0, 0, 0], [1, 0, 0], [0, 1, 0]], dtype='<f4'),
                    normals=np.tile([0., 0., 1.], (3, 1)),
                    tangents=np.array([[1., 0, 0, 1], [1., 0, 0, -1], [1., 0, 0, 1]]),
                    uv=np.arange(24, dtype=float).reshape(3, 4, 2) / 16,
                    triangles=np.array([[0, 1, 2]], dtype='<u2'),
                    bones=np.tile(np.arange(6), (3, 1)),
                    weights=np.tile([.6, .2, .1, .09, .009999, .000001], (3, 1)))

    def test_joint_order_is_mapped_by_name_not_index(self):
        b, w = remap_skin(np.array([[2, 0]]), np.array([[.75, .25]]),
                         ['toe', 'unused', 'foot'], ['foot', 'toe'])
        self.assertEqual(b[0, :2].tolist(), [0, 1])
        self.assertEqual(w[0, :2].tolist(), [.75, .25])

    def test_missing_weighted_bone_refused(self):
        with self.assertRaisesRegex(ValueError, 'missing'):
            remap_skin(np.array([[0]]), np.array([[1.]]), ['toe'], ['foot'])

    def test_vertex_indices_address_full_native_skeleton(self):
        native = ['root', 'hips', 'upper-leg', 'knee', 'foot', 'toe', 'cloth']
        b, w = remap_skin(np.array([[0, 1]]), np.array([[.75, .25]]),
                         ['foot', 'toe'], native)
        self.assertEqual(b[0, :2].tolist(), [4, 5])
        # A compact inverse-bind subset [4, 5] must not change these to [0, 1].
        self.assertEqual(w[0, :2].tolist(), [.75, .25])

    def test_eight_slot_pants_keep_all_seven_influences(self):
        b, w = remap_skin(np.array([range(7)]), np.ones((1, 7))/7,
                         list('abcdefg'), list('gfedcba'), slots=8)
        self.assertEqual(b.shape, (1, 8))
        self.assertEqual(b[0, :7].tolist(), [6, 5, 4, 3, 2, 1, 0])
        q = quantize_weights(w)
        self.assertEqual(int(q.sum()), 255)
        self.assertTrue(np.all(q[0, :7] > 0))
        self.assertEqual(q[0, 7], 0)

    def test_seventh_influence_never_truncated(self):
        with self.assertRaisesRegex(ValueError, 'truncation'):
            remap_skin(np.array([range(7)]), np.ones((1, 7))/7, list('abcdefg'), list('abcdefg'))

    def test_tiny_influences_survive_byte_quantization(self):
        w = np.array([[.999995, .000001, .000001, .000001, .000001, .000001]])
        q = quantize_weights(w)
        self.assertEqual(q.tolist(), [[250, 1, 1, 1, 1, 1]])
        self.assertEqual(int(q.sum()), 255)

    def test_invalid_weights_are_not_silently_normalized(self):
        for w in ([[.4, .4]], [[1.1, -.1]], [[float('nan'), 0]]):
            with self.assertRaises(ValueError):
                quantize_weights(w)

    def test_native_bytes_alignment_uv_and_handedness(self):
        data = self.data()
        blob, descriptor = build_stream(data, list('abcdef'), list('fedcba'))
        self.assertEqual(list(blob[12:18]), [5, 4, 3, 2, 1, 0])
        self.assertEqual(sum(blob[20:26]), 255)
        self.assertEqual(blob[18:20], b'\0\0')
        normal_offset = descriptor['sections']['normalTangent']['offset']
        self.assertEqual(int.from_bytes(blob[normal_offset:normal_offset+4], 'little'), (1 << 30) | (511 << 20))
        result = read_stream(blob, descriptor)
        for key in ('positions', 'triangles', 'uv'):
            np.testing.assert_array_equal(result[key], data[key])
        np.testing.assert_array_equal(result['bitangentSign'], [1, -1, 1])
        self.assertTrue(np.all(result['weights'] > 0))

    def test_bad_indices_uv_and_missing_uv_layer_refused(self):
        for key, value in [('triangles', np.array([[0, 1, 3]])),
                           ('uv', np.full((3, 4, 2), 1e10)),
                           ('uv', np.zeros((3, 3, 2)))]:
            d = self.data(); d[key] = value
            with self.assertRaises(ValueError):
                build_stream(d, list('abcdef'), list('abcdef'))

    def test_truncated_binary_refused(self):
        blob, desc = build_stream(self.data(), list('abcdef'), list('abcdef'))
        with self.assertRaises(ValueError):
            read_stream(blob[:-1], desc)

    def test_malformed_section_layout_refused(self):
        blob, desc = build_stream(self.data(), list('abcdef'), list('abcdef'))
        for field, value in [('length', 4), ('stride', 12), ('offset', 0)]:
            broken = copy.deepcopy(desc)
            broken['sections']['uv'][field] = value
            with self.assertRaises(ValueError):
                read_stream(blob, broken)


if __name__ == '__main__':
    unittest.main()
