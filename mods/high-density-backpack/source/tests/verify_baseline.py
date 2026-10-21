"""Verify every patch/call window against the exact on-disk game executable."""
import argparse, hashlib, re, struct
from pathlib import Path
parser=argparse.ArgumentParser();parser.add_argument('exe',type=Path);args=parser.parse_args()
data=args.exe.read_bytes()
expected='bf3d1c665545930bc850d8f5df486f7395885bb729d4fd408fdb03390de0765b'
assert hashlib.sha256(data).hexdigest()==expected,'Unsupported executable SHA-256'
pe=struct.unpack_from('<I',data,0x3c)[0]
assert struct.unpack_from('<I',data,pe+8)[0]==0x6A3DAE46
assert struct.unpack_from('<I',data,pe+0x50)[0]==0xB292000
section_count=struct.unpack_from('<H',data,pe+6)[0];optional=struct.unpack_from('<H',data,pe+20)[0]
sections=[]
for i in range(section_count):
    p=pe+24+optional+i*40
    size,rva,raw_size,raw=struct.unpack_from('<IIII',data,p+8);sections.append((rva,raw_size,raw))
def get(rva,size):
    for base,length,raw in sections:
        if base<=rva and rva+size<=base+length:return data[raw+rva-base:raw+rva-base+size]
    raise AssertionError('Unmapped RVA '+hex(rva))
def octets(s):return bytes(int(v,16) for v in re.findall(r'0x([0-9A-Fa-f]+)',s))
root=Path(__file__).resolve().parents[1];source=(root/'src/high_density_backpack_modules.cpp').read_text()
windows=[];checks=0
for table in ['PATCHES','INLINE_HOOKS']:
    body=source.split(' '+table+'[] = {',1)[1].split('\n};',1)[0]
    rows=re.findall(r'\{0x([0-9A-Fa-f]+)u?,(\d+),\{([^}]+)\}',body)
    assert rows,table
    for addr,length,raw in rows:
        rva=int(addr,16);length=int(length);expected_bytes=octets(raw)[:length]
        assert get(rva,length)==expected_bytes,(table,hex(rva))
        windows.append((rva,rva+length));checks+=1
body=source.split(' HOOKS[] = {',1)[1].split('\n};',1)[0]
for addr,target,fields in re.findall(r'\{0x([0-9A-Fa-f]+),0x([0-9A-Fa-f]+),(.*)',body):
    rva=int(addr,16);dest=int(target,16)
    opcode=b'\xe9' if fields.endswith('false,true},') else b'\xe8'
    assert get(rva,5)==opcode+struct.pack('<i',dest-rva-5),hex(rva)
    windows.append((rva,rva+5));checks+=1
for a,b in zip(sorted(windows),sorted(windows)[1:]):assert a[1]<=b[0],('Overlapping patches',a,b)
sig=(root/'src/baseline_signatures.h').read_text()
for addr,length,raw in re.findall(r'\{0x([0-9A-Fa-f]+)u?,(\d+),\{([^}]+)\}',sig):
    rva=int(addr,16);length=int(length)
    assert get(rva,length)==octets(raw)[:length],hex(rva);checks+=1
fixture=(root/'tests/native_charm_loop.h').read_text().split('[]={',1)[1].split('}',1)[0]
assert octets(fixture)==get(0x1D14C00,0x26),'Native crash-regression fixture differs from EXE'
# If Capstone is installed, also reject split inline instructions and verify
# each extended loop is exactly CMP register,2 -> CMP register,8.
try:
    import capstone
except ImportError:
    capstone=None
if capstone:
    md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_64)
    body=source.split(' INLINE_HOOKS[] = {',1)[1].split('\n};',1)[0]
    for addr,length in re.findall(r'\{0x([0-9A-Fa-f]+)u?,(\d+),',body):
        rva=int(addr,16);length=int(length)
        assert sum(i.size for i in md.disasm(get(rva,length),rva))==length
    for rva in [0xDACA8F,0xE5205A,0xE6C61A,0x1058FDE,0x19C374F,0x1A6FF1D,0x1D14C21,0x1E1B63B]:
        ins=list(md.disasm(get(rva,3),rva));assert len(ins)==1 and ins[0].mnemonic=='cmp' and ins[0].op_str.endswith(', 2')
    # Instruction boundaries alone are insufficient: another basic block may
    # jump into an overwritten instruction (the test1 Cryptobiosis crash).
    exception_rva,exception_size=struct.unpack_from('<II',data,pe+24+112+3*8)
    functions=struct.iter_unpack('<III',get(exception_rva,exception_size))
    branches=[];covered=set()
    for start,end,unwind in functions:
        owned=[(a,b) for a,b in windows if start<=a<end]
        if not owned:continue
        covered.update(owned)
        for addr,size,mnemonic,operands in md.disasm_lite(get(start,end-start),start):
            if (mnemonic.startswith('j') or mnemonic=='call') and operands.startswith('0x'):
                branches.append((addr,int(operands,16)))
    assert covered==set(windows),'Patch outside checked runtime function'
    def incoming_interior(patches):
        return [(src,dest,a,b) for src,dest in branches for a,b in patches if a<dest<b]
    assert (0x1D14C24,0x1D14C17,0x1D14C13,0x1D14C1A) in incoming_interior([(0x1D14C13,0x1D14C1A)]),'Verifier failed to detect test1 crash'
    assert not incoming_interior(windows),('Branch into overwritten instruction',incoming_interior(windows))
    print(f'PASS: {len(branches)} direct branches in affected functions; test1 back-edge defect detected, current windows safe')
print(f'PASS: exact EXE hash, {checks} byte/call guards, {len(windows)} non-overlapping patch windows'+(', instruction boundaries' if capstone else ''))
