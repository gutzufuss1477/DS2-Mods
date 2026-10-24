"""Stable optional visuals contract. Copy this gate with future main-mod branches."""
from pathlib import Path
import hashlib,json
root=Path(__file__).resolve().parents[1]
contract=json.loads((root/'config/visual-router-v1.json').read_text())
for path,expected in contract['frozenSources'].items():
 assert hashlib.sha256((root/path).read_bytes().replace(b'\r\n',b'\n')).hexdigest()==expected, 'VISUAL_ROUTER_V1_CHANGED: '+path
runtime=(root/'src/virtual_graph_runtime.inl').read_text()
assert 'addon_host::merge(' in runtime and 'addon_host::finish(' in runtime
print('VISUAL_ROUTER_V1_PASS: frozen ABI, independent module discovery, live graph merge retained.')
