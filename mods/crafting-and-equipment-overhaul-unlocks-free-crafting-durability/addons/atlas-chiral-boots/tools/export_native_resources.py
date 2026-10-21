"""Export DS2 metadata with optional isolated byte/link/locator audit, read-only."""
from pathlib import Path
import argparse
import os
import subprocess
from prepare_resource_audit import ROOT, JAVA, MODULES

GAME = Path(r'C:\Program Files (x86)\Steam\steamapps\common\DEATH STRANDING 2 - ON THE BEACH')


def export(refs, destination, audit_groups=(), overrides=None, format='json'):
    destination = Path(destination).resolve()
    if not destination.is_relative_to(ROOT):
        raise ValueError('Exports must remain in addon workspace')
    destination.mkdir(parents=True, exist_ok=True)
    env = {k: v for k, v in os.environ.items() if not k.startswith('DS2_')}
    if overrides:
        allowed = {'DS2_KESTREL_GRAPH_OVERRIDE', 'DS2_ATLAS_LINKTABLE_OVERRIDE',
                   'DS2_APEX_CORE_OVERRIDE', 'DS2_KESTREL_EXT_STREAM_OVERRIDE'}
        if set(overrides) - allowed:
            raise ValueError('Unrecognized input override')
        env.update({k: str(v) for k, v in overrides.items()})
    if audit_groups:
        env['DS2_LAB_AUDIT_DIR'] = str(destination / 'raw')
        env['DS2_LAB_AUDIT_GROUPS'] = ','.join(map(str, audit_groups))
    command = [str(JAVA / 'java.exe'), '--enable-preview', '--enable-native-access=odradek.core',
               '--add-opens=java.base/sun.nio.ch=odradek.core',
               '--patch-module', 'odradek.game.ds2=' + str(ROOT / 'build/resource-audit/classes'),
               '--module-path', str(MODULES), '-m', 'odradek.app/sh.adelessfox.odradek.app.Main',
               'export', '-s', str(GAME), '-o', str(destination), '-f', format, *refs]
    log = destination / ('export-' + '-'.join(r.replace(':', '_') for r in refs[:3]) + '.log')
    with log.open('xb') as output:
        result = subprocess.run(command, env=env, stdout=output, stderr=subprocess.STDOUT)
    if result.returncode:
        raise RuntimeError(log.read_text(errors='replace')[-4000:])
    for ref in refs:
        found = list(destination.glob('*_' + ref.replace(':', '_') + '.' + format.rsplit('.', 1)[-1]))
        if len(found) != 1:
            raise ValueError('Missing/ambiguous export: ' + ref)
    print('Exported', len(refs), 'resources to', destination)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--audit-groups', nargs='*', type=int, default=[])
    parser.add_argument('refs', nargs='+')
    args = parser.parse_args()
    export(args.refs, args.output, args.audit_groups)
