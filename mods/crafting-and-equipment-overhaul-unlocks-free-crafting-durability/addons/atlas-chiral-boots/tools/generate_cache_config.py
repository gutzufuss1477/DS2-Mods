"""Pin the private test's already-built cache; no game writes."""
import json
import argparse
from native_bundle import ROOT
from serialize_native_geometry import GAME
from fix_array_reservations import validate_cache_reservations
from validate_controller_offsets import validate_controller_gate
p=argparse.ArgumentParser()
p.add_argument('--stage', required=True, help='Existing stage directory inside this addon')
a=p.parse_args()
stage=(ROOT/a.stage).resolve()
assert stage.is_relative_to(ROOT/'staging') and stage.is_dir()
validate_cache_reservations(stage)
validate_controller_gate(stage)
r=json.loads((stage/'report.json').read_text())
m=json.loads((stage/'bundle/overlay.json').read_text())
if 'clipOwnershipFixes' in r:
    from validate_stream_ownership import validate_stream_gate
    validate_stream_gate(stage)
if 'hangingMaterial' in r:
    from validate_hanging_shader import validate_shader_gate
    validate_shader_gate(stage)
def wide(s): return 'L'+json.dumps(str(s), ensure_ascii=True)
lines=['#pragma once','struct CacheFile {const WCHAR* original; const WCHAR* cache; const char* sourceHash; const char* cacheHash;};',
       'constexpr CacheFile CacheFiles[]={']
for f in m['files']:
    lines.append('{'+','.join([wide(GAME/f['gamePath']),wide(__import__('pathlib').Path(r['cache'])/__import__('pathlib').Path(f['gamePath']).name),
                              json.dumps(f['originalSha256']),json.dumps(f['resultSha256'])])+'},')
proof=json.loads((ROOT/'evidence/test03-diagnosis-test04-host-compatibility.json').read_text())
assert all(x['identical'] for x in proof['checks'])
lines+=['};', 'constexpr char CacheBaseGraphSHA[]='+json.dumps(proof['hostGraphSHA'])+';']
(ROOT/'src/cache_config.hpp').write_text('\n'.join(lines)+'\n')
