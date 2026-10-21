import sys
from pathlib import Path
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from native_bundle import encode_link, decode_link, output_path


class NativeBundleTests(unittest.TestCase):
    def test_native_varint_boundaries_and_mixed_links(self):
        values = [0, 1, 63, 64, 127, 128, 8191, 8192, 16383, 16384, 48074, 2**32-1]
        links = [(None, v) for v in values] + [(g, v) for g in values for v in values]
        raw = b''.join(encode_link(v) for v in links)
        at, decoded = 0, []
        while at < len(raw):
            link, at = decode_link(raw, at)
            decoded.append(link)
        self.assertEqual(decoded, links)
        self.assertEqual(at, len(raw))
        self.assertEqual(encode_link((None, 64)), b'\x80\x40')
        self.assertEqual(encode_link((1, 64)), b'\x41\x80\x40')

    def test_invalid_output_root_or_existing_source_refused(self):
        for path in [Path(__file__), Path('C:/Windows/chiral-test.bin')]:
            with self.assertRaises(ValueError):
                output_path(path)

    def test_invalid_numbers_refused(self):
        for v in [-1, 2**32]:
            with self.assertRaises(ValueError):
                encode_link((None, v))


if __name__ == '__main__':
    unittest.main()
