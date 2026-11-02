"""Build the APAS 4.1 development candidate with standard MSVC protections."""
import argparse
import hashlib
import json
from pathlib import Path
from build_helpers import build, toolchain

ROOT = Path(__file__).resolve().parents[1]

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--game', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    assert hashlib.sha256(args.game.read_bytes()).hexdigest() == 'bf3d1c665545930bc850d8f5df486f7395885bb729d4fd408fdb03390de0765b', 'Unsupported game fixture'
    assert args.output.is_absolute(), 'Use a dedicated absolute build directory'
    spec = dict(id='apas', directory='', compiler='MSVC', version='4.1.0',
                description='DS2 APAS 4.1.0-preview.1 - development test',
                sources=['src/apas.cpp', 'src/constructor.s'], file='ds2_apas_memory_costs.asi',
                libraries=['kernel32.lib', 'bcrypt.lib'], cfgEntryPoints=['ApasNativeConstructor'])
    tc = toolchain()
    result = build(spec, args.output, tc)
    fixture = dict(spec, id='tests', sources=['tests/runtime.cpp', 'src/constructor.s'],
                   file='apas_rebuild_tests.dll')
    build(fixture, args.output, tc)
    (args.output/'build-result.json').write_text(json.dumps(result, indent=2)+'\n')

if __name__ == '__main__':
    main()
