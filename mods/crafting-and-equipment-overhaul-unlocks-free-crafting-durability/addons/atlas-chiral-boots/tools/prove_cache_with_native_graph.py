"""Offline proof on the original graph; reproduces the verified Crafting graph.

No custom shoe binding is claimed. Uses only read access to the real game.
"""
from pathlib import Path
import hashlib
import json
import struct
from overlay_cache import build_cache, digest

ROOT=Path(__file__).resolve().parents[1]
GAME=Path('C:/Program Files (x86)/Steam/steamapps/common/DEATH STRANDING 2 - ON THE BEACH')
OUT=ROOT/'evidence/native-graph-proof'
SOURCE='LocalCacheWinGame/package/streaming_graph.core'
VANILLA='ac0c824ef73d1551d798357d12c150a963eb26910292ce4a4f9a6d8c923ca392'
EXPECTED='d52014d735be8b3d641a0dfc6b3dab07a530e8cc94081d28456d1408a0174550'


def main():
    assert not OUT.exists(), 'Preserve previous proof'
    before=digest(GAME/SOURCE)
    assert before==VANILLA
    OUT.mkdir()
    bundle=OUT/'input';bundle.mkdir()
    payload=struct.pack('<6I',28353917-16,556210,2367,558577,9676,2797)
    (bundle/'values.bin').write_bytes(payload)
    h=hashlib.sha256(payload).hexdigest()
    def original(start,end):return {'original':start,'length':end-start}
    def extra(offset,length):return {'payload':'values.bin','sha256':h,'offset':offset,'length':length}
    group=18589873;subgroups=23634861;end=subgroups+2215380;child=subgroups+4+14248*4
    segments=[original(0,8),extra(0,4),original(12,group+16),extra(4,8),
              original(group+24,subgroups),extra(12,4),original(subgroups+4,end),
              original(child,child+2365*4),extra(16,4),original(child,child+2365*4),
              extra(16,8),original(end,28334985)]
    manifest={'schemaVersion':1,'files':[{'gamePath':SOURCE,'originalSha256':VANILLA,
              'resultSha256':EXPECTED,'resultSize':28353917,'segments':segments}]}
    (bundle/'overlay.json').write_text(json.dumps(manifest,indent=2),encoding='utf-8')
    target=build_cache(GAME,bundle,manifest,OUT/'cache')
    assert digest(GAME/SOURCE)==before
    assert digest(target/'streaming_graph.core')==EXPECTED
    assert build_cache(GAME,bundle,manifest,OUT/'cache')==target
    result={'originalPath':str(GAME/SOURCE),'originalBeforeSha256':before,
            'originalAfterSha256':digest(GAME/SOURCE),'cache':str(target),
            'cacheSha256':EXPECTED,'payloadBytes':len(payload),'segments':len(segments),
            'originalUnchanged':True,'verifiedCacheReused':True,'gameLaunched':False,
            'scope':'Offline file assembly only; reproduces upstream graph. No Chiral model/runtime integration tested.'}
    (OUT/'result.json').write_text(json.dumps(result,indent=2),encoding='utf-8')
    print(json.dumps(result))


if __name__=='__main__':main()
