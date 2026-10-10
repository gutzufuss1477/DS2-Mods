from pathlib import Path
import struct
import sys

sites = {
    0x01187D34: bytes.fromhex('BA 0B 00 00 00'),
    0x01187FB8: bytes.fromhex('49 83 FF 2C'),
    0x0118809B: bytes.fromhex('49 83 FE 2C'),
    0x011881D1: bytes.fromhex('48 83 FB 0B'),
    0x01F9369A: bytes.fromhex('0F B6 48 37 84 C9'),
    0x01F93792: bytes.fromhex('0F B6 70 37 44 0F B6 70 36')
}

def check_binary(path):
    d=Path(path).read_bytes()
    pe=struct.unpack_from('<I',d,0x3c)[0]
    optional=pe+24
    section_count=struct.unpack_from('<H',d,pe+6)[0]
    sec=[]
    for i in range(section_count):
        s=optional+struct.unpack_from('<H',d,pe+20)[0]+40*i
        sec.append((struct.unpack_from('<I',d,s+12)[0],
                    struct.unpack_from('<I',d,s+16)[0],
                    struct.unpack_from('<I',d,s+20)[0]))
    def file_offset(rva):
        for va,n,off in sec:
            if va<=rva<va+n: return off+rva-va
        raise ValueError(hex(rva))
    for rva,v in sites.items():
        actual=d[file_offset(rva):file_offset(rva)+len(v)]
        assert actual==v, (hex(rva),actual.hex(),v.hex())
    print('PASS: six original-byte instruction sites in DS2.exe')

def stage(used,usable):
    return min(10,(used*10+usable-1)//usable)

def check_math():
    for capacity in range(160,481,16):
        usable=capacity//16
        assert 10<=usable<=30
        prev=0
        for used in range(usable+1):
            v=stage(used,usable)
            assert 0<=v<=10 and v>=prev
            prev=v
        assert stage(usable,usable)==10
    assert stage(10,20)==5 and stage(20,20)==10
    assert stage(10,30)==4
    print('PASS: 21 capacity configurations, monotonic ten-segment indicator')
if __name__=='__main__':
    check_math()
    if len(sys.argv)>1: check_binary(sys.argv[1])
