"""Reproducible structural checks, not a Windows loader/game test."""
from pathlib import Path
import hashlib
import json
import struct
import re

root = Path(__file__).resolve().parents[1]
asi = root / 'release' / 'ds2_crafting_unlocks.asi'
b = asi.read_bytes()
assert b[:2] == b'MZ'
pe = struct.unpack_from('<I', b, 0x3c)[0]
assert b[pe:pe+4] == b'PE\0\0'
machine, section_count = struct.unpack_from('<HH', b, pe + 4)
assert machine == 0x8664
characteristics = struct.unpack_from('<H', b, pe + 22)[0]
assert characteristics & 0x2000
optional = pe + 24
assert struct.unpack_from('<H', b, optional)[0] == 0x20b
entry = struct.unpack_from('<I', b, optional + 16)[0]
dll_flags = struct.unpack_from('<H', b, optional + 70)[0]
assert entry and dll_flags & 0x40 and dll_flags & 0x100
unwind = struct.unpack_from('<II', b, optional + 112 + 3*8)
relocs = struct.unpack_from('<II', b, optional + 112 + 5*8)
assert all(unwind) and all(relocs)
report = (root/'docs'/'PE_HEADER_IMPORTS_UNWIND.txt').read_text()
imports = re.findall(r'DLL Name: (\S+)', report)
assert imports == ['KERNEL32.dll']
# Compare the compiled binding's source constants with actual captured instruction bytes.
rows = (root/'evidence'/'targeted'/'functions'/'0171D880_asm.tsv').read_text().splitlines()
anchor = next(line for line in rows if line.startswith('0x171DB1D\t'))
assert anchor.split('\t')[1] == 'E89E1E45FF'
assert '0x140b6f9c0' in anchor.lower()
assert 0x171DB1D+5+struct.unpack('<i',bytes.fromhex('9E1E45FF'))[0] == 0xB6F9C0
meta = (root/'evidence'/'PROGRAM_METADATA.txt').read_text()
exe_hash = re.search(r'imported_executable_sha256=(\w+)', meta).group(1)
source = (root/'src'/'mod.cpp').read_text()
const = re.search(r'ExpectedHash\[32\]=\{([^}]+)\}', source).group(1)
assert bytes(int(x.strip(),16) for x in const.split(',')).hex() == exe_hash
# New backpack binding: the master contains a real direct call and register setup.
asm = (root/'evidence/backpack/01529860_asm.txt').read_text()
assert '0x1529896\tCALL 0x140b6fc20' in asm
assert '0x1529883\tLEA R8,[RCX + 0x228]' in asm
assert '0x1529894\tMOV DL,0x5' in asm
assert 0x1529896+5+struct.unpack('<i',bytes.fromhex('856364FF'))[0] == 0xB6FC20
assert 'BackpackCallBytes[5]={0xE8,0x85,0x63,0x64,0xFF}' in source
assert 'u8 id=0;u32 key=0;' in source # Native table type is one byte, not padding.
copyasm = (root/'evidence/backpack/00B7E950_asm.txt').read_text()
assert 'MOV R13,RDX' in copyasm and 'MOVSXD RDI,R8D' in copyasm
assert b'DS2 Crafting Unlocks 1.0.0' in b
assert b'BACKPACK_MENU ' in b
config=(root/'release/ds2_crafting_unlocks.ini').read_text(encoding='ascii')
keys=re.findall(r'(?m)^(0x[0-9A-F]{8})=1\s*;',config)
assert len(keys)==119 and len(set(keys))==119
result = {
    'status':'PASS_STRUCTURAL_ONLY',
    'file':asi.name,
    'bytes':len(b),
    'sha256':hashlib.sha256(b).hexdigest(),
    'machine':'AMD64',
    'pe_format':'PE32+',
    'is_dll':True,
    'imports':imports,
    'relocations_present':True,
    'unwind_directory_present':True,
    'analysed_exe_sha256':exe_hash,
    'callsite_rva':'0x0171DB1D',
    'version':'1.0.0',
    'backpack_callsite_rva':'0x01529896',
    'backpack_callsite_expected_bytes':'E8 85 63 64 FF',
    'backpack_original_target_rva':'0x00B6FC20',
    'native_vector_copy_rva':'0x00B7E950',
    'new_call_bytes_origin':'Computed from master CALL rel32 and verified at runtime, not a raw capture',
    'config_keys':119,
    'new_backpack_keys':29,
    'callsite_expected_bytes':'E8 9E 1E 45 FF',
    'original_target_rva':'0x00B6F9C0',
    'windows_loader_test':False,
    'windows_patch_test':False,
    'game_test':False,
}
(root/'docs'/'BUILD_VALIDATION.json').write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps(result,indent=2))
