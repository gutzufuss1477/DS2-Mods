import copy
import hashlib
import json
from pathlib import Path
import sys
import tempfile
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from overlay_cache import build_cache, Refused, digest


class OverlayTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory(prefix='atlas-overlay-test-')
        self.root = Path(self.tmp.name)
        self.game = self.root/'game'
        self.bundle = self.root/'bundle'
        self.cache = self.root/'cache'
        self.path = 'LocalCacheWinGame/package/streaming_graph.core'
        self.src = self.game/self.path
        self.src.parent.mkdir(parents=True)
        self.bundle.mkdir()
        self.original = b'UNMODIFIED_ORIGINAL_DATA'
        self.src.write_bytes(self.original)
        self.payload = self.bundle/'extension.bin'
        self.payload.write_bytes(b'addon')
        self.result = self.original + b'addon'
        self.manifest = {'schemaVersion':1, 'files':[{
            'gamePath':self.path, 'originalSha256':digest(self.src),
            'resultSha256':hashlib.sha256(self.result).hexdigest(), 'resultSize':len(self.result),
            'segments':[{'original':0,'length':len(self.original)},
                        {'payload':'extension.bin','sha256':digest(self.payload),'offset':0,'length':5}]}]}

    def tearDown(self):
        self.assertEqual(self.src.read_bytes(), self.original)
        self.assertEqual([p.name for p in self.src.parent.iterdir()], ['streaming_graph.core'])
        self.tmp.cleanup()

    def build(self, **kw):
        return build_cache(self.game,self.bundle,kw.get('manifest',self.manifest),kw.get('cache',self.cache))

    def refused(self, manifest=None, cache=None):
        with self.assertRaises(Refused):
            self.build(manifest=manifest or self.manifest, cache=cache or self.cache)

    def test_read_only_original_and_reusable_generation(self):
        out=self.build()
        self.assertEqual((out/'streaming_graph.core').read_bytes(),self.result)
        self.assertEqual(self.build(),out)
        self.assertEqual(json.loads((out/'ready.json').read_text()),self.manifest)

    def test_original_game_tree_never_output(self):
        for dest in [self.game,self.game/'new-cache',self.src.parent,self.root]:
            with self.subTest(dest=dest):self.refused(cache=dest)

    def test_source_bundle_never_output(self):
        self.refused(cache=self.bundle/'cache')

    def test_unknown_original_refused_before_writes(self):
        self.manifest['files'][0]['originalSha256']='0'*64
        self.refused()
        self.assertFalse(self.cache.exists())

    def test_payload_corruption_refused_before_writes(self):
        self.payload.write_bytes(b'other')
        self.refused()
        self.assertFalse(self.cache.exists())

    def test_cache_corruption_is_not_overwritten(self):
        out=self.build();p=out/'streaming_graph.core';p.write_bytes(b'foreign')
        self.refused()
        self.assertEqual(p.read_bytes(),b'foreign')

    def test_failed_result_not_published(self):
        self.manifest['files'][0]['resultSha256']='0'*64
        self.refused()
        self.assertEqual(list(self.cache.iterdir()),[])

    def test_second_file_failure_publishes_nothing(self):
        # Both routes refer to valid fixture originals; second output is deliberately invalid.
        second=copy.deepcopy(self.manifest['files'][0])
        second['gamePath']='LocalCacheWinGame/package/streaming_links.stream'
        other=self.game/second['gamePath'];other.write_bytes(self.original)
        second['resultSha256']='0'*64
        self.manifest['files'].append(second)
        try:
            self.refused();self.assertEqual(list(self.cache.iterdir()),[])
            self.assertEqual(other.read_bytes(),self.original)
        finally:other.unlink()

    def test_saves_and_arbitrary_game_paths_refused(self):
        for path in ['DS2.exe','save.dat','../save.dat',self.path.upper()]:
            with self.subTest(path=path):
                m=copy.deepcopy(self.manifest);m['files'][0]['gamePath']=path;self.refused(m)

    def test_payload_path_escape_refused(self):
        for path in ['../game/'+self.path, 'C:/save.dat', '\\server\\file', '/absolute']:
            with self.subTest(path=path):
                m=copy.deepcopy(self.manifest);m['files'][0]['segments'][1]['payload']=path;self.refused(m)

    def test_invalid_ranges(self):
        for offset,length in [(-1,1),(999,1),(0,-1),(0,999),(True,1)]:
            with self.subTest(offset=offset,length=length):
                m=copy.deepcopy(self.manifest);m['files'][0]['segments'][0]={'original':offset,'length':length};self.refused(m)

    def test_duplicate_route_refused(self):
        self.manifest['files']*=2;self.refused()

    def test_unsupported_format(self):
        self.manifest['schemaVersion']=2;self.refused()

    def test_existing_unrelated_file_is_preserved(self):
        self.cache.mkdir();foreign=self.cache/'unrelated.txt';foreign.write_bytes(b'keep')
        self.build();self.assertEqual(foreign.read_bytes(),b'keep')


if __name__=='__main__':unittest.main(verbosity=2)
