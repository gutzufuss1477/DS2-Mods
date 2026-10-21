"""Prepare the opt-in development INI without changing any existing setting."""
from pathlib import Path
import zipfile
ROOT=Path(__file__).resolve().parents[1]
destination=ROOT/'build/pcc-dev1/ds2_crafting_unlocks.ini'
destination.parent.mkdir(parents=True,exist_ok=True)
with zipfile.ZipFile(ROOT/'release/DS2_Crafting_Equipment_Overhaul_v1.7.0.zip') as z:
    base=z.read('ds2_crafting_unlocks.ini')
assert b'[PCCStructures]' not in base
destination.write_bytes(base+b'\r\n; PCC development extension 1.7.1-pcc-dev1. Restart after changes.\r\n'
    b'[PCCStructures]\r\nUnlockAll=0            ; 1 = early standard structure choices within the native PCC tier.\r\n'
    b'; Hot Spring Digger excluded. Network, bandwidth, terrain and costs stay native.\r\n')
print(destination)
