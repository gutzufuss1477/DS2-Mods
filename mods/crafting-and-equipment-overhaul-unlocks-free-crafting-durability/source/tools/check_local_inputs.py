"""Report missing local game-derived inputs without downloading or fabricating them."""
from pathlib import Path
import hashlib,json,sys
R=Path(__file__).resolve().parents[1]
spec=json.loads((R/'config/local-inputs.json').read_text())
errors=[]
for entry in spec['inputs']:
    p=R/entry['path']
    if not p.is_file():errors.append('Missing: '+entry['path']);continue
    if p.stat().st_size!=entry['bytes'] or hashlib.sha256(p.read_bytes()).hexdigest()!=entry['sha256']:
        errors.append('Size/SHA-256 mismatch: '+entry['path'])
if errors:
    print('\n'.join(errors));print('See BUILD.md and config/local-inputs.json. Game assets are not stored in Git.');sys.exit(1)
print('LOCAL_INPUTS_VERIFIED',len(spec['inputs']))
