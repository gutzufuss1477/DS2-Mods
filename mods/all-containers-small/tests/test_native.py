"""Actual mod transformation + original game machine code in this test process.
Never launches, attaches to, or modifies a DS2 game process.
"""
import ctypes as C
from collections import Counter
import hashlib
import json
from pathlib import Path
import struct
import sys
import subprocess
import tempfile
import shutil
import os
import unittest

ROOT=Path(__file__).resolve().parents[1]
TYPES=Path(os.environ.get('DS2_TYPES',ROOT.parent/'analysis/odradek/odradek-game-ds2/src/main/resources/types.json'))
RESOURCES=Path(os.environ.get('DS2_RESOURCE_DIR',ROOT.parent/'more-silenced-guns/evidence/resources'))
(ROOT/'evidence').mkdir(exist_ok=True)
sys.path.insert(0,str(ROOT/'tools'))
from native_scan import Image

class Array(C.Structure):
    _fields_=[('count',C.c_int),('capacity',C.c_int),('data',C.POINTER(C.c_void_p))]
class Counts(C.Structure):
    _fields_=[(x,C.c_uint) for x in ('total','changed','small','special')]
DLL=C.CDLL(str(ROOT/'build/native_tests.dll'))
DLL.Transform.argtypes=[C.c_void_p];DLL.Transform.restype=C.c_bool
DLL.Patch.argtypes=[C.POINTER(Array),C.c_uint64,C.POINTER(Counts),C.c_void_p];DLL.Patch.restype=C.c_bool
DLL.Hash.argtypes=[C.c_void_p,C.c_uint,C.c_void_p]
DLL.Model.argtypes=[C.c_void_p,C.c_void_p,C.c_void_p];DLL.Model.restype=C.c_void_p
DLL.Appearance.argtypes=[C.c_void_p,C.c_void_p,C.c_void_p];DLL.Appearance.restype=C.c_void_p
DLL.IsGamePath.argtypes=[C.c_wchar_p,C.c_uint];DLL.IsGamePath.restype=C.c_bool
K=C.WinDLL('kernel32',use_last_error=True)
K.VirtualAlloc.argtypes=[C.c_void_p,C.c_size_t,C.c_uint,C.c_uint];K.VirtualAlloc.restype=C.c_void_p
K.VirtualFree.argtypes=[C.c_void_p,C.c_size_t,C.c_uint];K.VirtualFree.restype=C.c_int
K.VirtualProtect.argtypes=[C.c_void_p,C.c_size_t,C.c_uint,C.POINTER(C.c_uint)];K.VirtualProtect.restype=C.c_int
K.GetCurrentProcess.restype=C.c_void_p
K.FlushInstructionCache.argtypes=[C.c_void_p,C.c_void_p,C.c_size_t]
VT=0x1432084F8
def pack(b,off,fmt,value):struct.pack_into('<'+fmt,b,off,value)
def bag(volume,kind=0,attribute=0):
    b=C.create_string_buffer(bytes((i*37+19)%256 for i in range(0x88)),0x88)
    pack(b,0,'Q',VT);pack(b,0x48,'B',attribute);pack(b,0x49,'B',kind)
    pack(b,0x4a,'B',0);pack(b,0x4d,'B',volume)
    pack(b,0x44,'I',0);pack(b,0x7c,'B',0);pack(b,0x50,'Q',0)
    return b
def array_for(bags):
    refs=(C.c_void_p*len(bags))(*(C.addressof(b) for b in bags))
    return Array(len(bags),len(bags),refs),refs

class NativeCode:
    def __init__(self):
        self.image=Image()
        self.size=0x6240000
        self.base=K.VirtualAlloc(None,self.size,0x2000,4)
        if not self.base:raise OSError(C.get_last_error())
        self.pages=[(0x1000,0x1000),(0xbe8000,0x1000),(0x11dc000,0x2000),(0x1231000,0x2000),(0x623e000,0x1000)]
        for rva,size in self.pages:
            if not K.VirtualAlloc(self.base+rva,size,0x1000,4):raise OSError(C.get_last_error())
        for rva,size in [(0x1231f40,0x100),(0x11dcb00,0x2c0),(0x11dcdc0,0x558),(0xbe8d60,0xcb)]:
            C.memmove(self.base+rva,self.image.read(rva,size),size)
        # A tiny ABI entry then the EXACT native resource -> cached footprint path.
        # Only the relative CALL is relocated to its original target in our copy.
        source=self.image.read(0x11c8576,0x15)
        assert source[:8]==bytes.fromhex('49 8b 48 38 0f b6 49 4d')
        code=bytearray(bytes.fromhex('4c 8b c1')+source+b'\xc3')
        struct.pack_into('<i',code,12,0x1231f40-(0x1000+16))
        C.memmove(self.base+0x1000,bytes(code),len(code))
        # Exact crash-site address calculation, stopping before the unsafe load.
        # Preserve nonvolatile registers; RCX is the cart row from the dump.
        fault=self.image.read(0x11ce820,15)
        assert fault==bytes.fromhex('4c 8b 7b 48 4d 85 ff 49 8d 57 30 49 0f 44 d0')
        probe=bytes.fromhex('53 41 57 48 89 cb 41 b8 50 00 00 00')+fault+bytes.fromhex('48 89 d0 41 5f 5b c3')
        C.memmove(self.base+0x1100,probe,len(probe))
        old=C.c_uint()
        for rva,size in self.pages[:-1]:
            if not K.VirtualProtect(self.base+rva,size,0x20,C.byref(old)):raise OSError(C.get_last_error())
            K.FlushInstructionCache(K.GetCurrentProcess(),self.base+rva,size)
        self.footprint=C.CFUNCTYPE(C.c_uint,C.c_ubyte)(self.base+0x1231f40)
        self.initialize=C.CFUNCTYPE(None,C.c_void_p)(self.base+0x1000)
        self.model=C.CFUNCTYPE(C.c_void_p,C.c_void_p,C.c_void_p)(self.base+0x11dcb00)
        self.appearance=C.CFUNCTYPE(C.c_void_p,C.c_void_p,C.c_void_p)(self.base+0x11dcdc0)
        self.cart_read_address=C.CFUNCTYPE(C.c_void_p,C.c_void_p)(self.base+0x1100)
    def close(self):K.VirtualFree(self.base,0,0x8000)

class Tests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):cls.native=NativeCode()
    @classmethod
    def tearDownClass(cls):cls.native.close()

    def test_01_native_identity_and_hook(self):
        im=self.native.image
        self.assertEqual(im.read(0xb6da86,5),bytes.fromhex('e8 05 1e 01 00'))
        self.assertEqual(im.read(0xb6da7b,11),bytes.fromhex('49 8b 8f 88 01 00 00 49 8d 57 18'))
        self.assertEqual(struct.unpack('<Q',im.read(0x32084f8,8))[0],im.base+0xbd32c0)
        self.assertEqual(im.read(0x11dcb00,6),bytes.fromhex('40 53 48 83 ec 20'))
        self.assertEqual(im.read(0x11dcdc0,5),bytes.fromhex('48 89 5c 24 08'))

    def test_02_all_normal_classes_only_change_volume(self):
        for kind in range(20):
            for volume in range(9):
                for attribute in range(6):
                    with self.subTest(kind=kind,volume=volume,attribute=attribute):
                        b=bag(volume,kind,attribute);before=bytes(b)
                        expected=1<=volume<=6 and kind not in (5,6,8,11,18) and attribute!=4
                        self.assertEqual(DLL.Transform(b),expected)
                        want=bytearray(before)
                        if expected:want[0x4d]=0
                        self.assertEqual(bytes(b),want)
                        self.assertFalse(DLL.Transform(b))

    def test_03_native_cached_footprint_after_transformation(self):
        expected=[1,2,4,6,8,12,16,24,28]
        for v,units in enumerate(expected):self.assertEqual(self.native.footprint(v),units)
        for v in range(9):
            b=bag(v);DLL.Transform(b)
            instance=C.create_string_buffer(0x160);pack(instance,0x38,'Q',C.addressof(b))
            self.native.initialize(instance)
            self.assertEqual(struct.unpack_from('<H',instance,0xd8)[0],1 if v<=6 else expected[v])

    def test_04_array_transaction_and_counts(self):
        bags=[bag(v) for v in range(9)];a,refs=array_for(bags);out=Counts()
        self.assertTrue(DLL.Patch(C.byref(a),0x140000000,C.byref(out),None))
        self.assertEqual((out.total,out.changed,out.small,out.special),(9,6,1,2))

    def test_05_invalid_late_entry_does_not_partially_modify(self):
        for mode in ('type','volume','case','attribute','readonly','null'):
            bags=[bag(3),bag(2)];a,refs=array_for(bags);deny=None
            if mode=='type':pack(bags[1],0,'Q',VT+8)
            if mode=='volume':pack(bags[1],0x4d,'B',255)
            if mode=='case':pack(bags[1],0x49,'B',255)
            if mode=='attribute':pack(bags[1],0x48,'B',255)
            if mode=='readonly':deny=C.addressof(bags[1])+0x4d
            if mode=='null':refs[1]=None
            before=[bytes(b) for b in bags];out=Counts()
            self.assertFalse(DLL.Patch(C.byref(a),0x140000000,C.byref(out),deny),mode)
            self.assertEqual([bytes(b) for b in bags],before,mode)

    def test_06_bad_array_headers(self):
        for a in [Array(-1,0,None),Array(65537,65537,None),Array(2,1,None),Array(1,1,None)]:
            out=Counts();self.assertFalse(DLL.Patch(C.byref(a),0x140000000,C.byref(out),None))
        out=Counts();self.assertFalse(DLL.Patch(None,0x140000000,C.byref(out),None))

    def test_07_exported_game_definitions(self):
        types=json.loads(TYPES.read_text())
        enums={k:{v['name']:v['value'] for v in d.get('values',[])} for k,d in types.items() if d['kind']=='enum'}
        files=list(RESOURCES.glob('DSGameBaggageListItem_*.json'))
        self.assertGreater(len(files),1000)
        stats=Counter();sizes=Counter()
        fields=[('AttributeOfBaggage',0x48,'BaggageAttribute'),('TypeOfCase',0x49,'BaggageCaseType'),('TypeOfVolume',0x4d,'Volume')]
        for path in files:
            data=json.loads(path.read_text());b=bag(0)
            pack(b,0x44,'I',data['NameCode'])
            pack(b,0x7c,'B',0 if data['CapturedAnimalType']=='None' else 1)
            for field,offset,suffix in fields:
                pack(b,offset,'B',enums['EDSGameBaggageListItem_'+suffix][data[field]])
            before=bytes(b);v=before[0x4d];sizes[data['TypeOfVolume']]+=1
            changed=DLL.Transform(b)
            stats['converted' if changed else 'already_S' if v==0 else 'preserved']+=1
            self.assertEqual(bytes(b)[:0x4d],before[:0x4d]);self.assertEqual(bytes(b)[0x4e:],before[0x4e:])
            if changed:self.assertEqual(self.native.footprint(b[0x4d][0]),1)
        (ROOT/'evidence/resource-test.json').write_text(json.dumps({'definitions':len(files),'result':stats,'source_sizes':sizes},indent=2)+'\n')

    def test_08_native_standard_model_selection(self):
        entries=[C.create_string_buffer(0x38) for _ in range(33)]
        refs=(C.c_void_p*33)(*(C.addressof(e) for e in entries))
        for i,e in enumerate(entries):pack(e,0x20,'B',i);pack(e,0x28,'Q',0xA000+i)
        manager=C.create_string_buffer(0x30);pack(manager,0x20,'i',33);pack(manager,0x28,'Q',C.addressof(refs))
        observed={}
        for kind in [0,1,2,3,4,7,8,9,10,12,13,14,15,16,17,19]:
            b=bag(3,kind);pack(b,0x44,'I',0);pack(b,0x7c,'B',0);DLL.Transform(b)
            selector=self.native.model(manager,b)-0xA000;observed[str(kind)]=selector
            if kind in (0,3,4,13,15,16):self.assertEqual(selector,0)
            if kind==2:self.assertEqual(selector,8)
            if kind==7:self.assertEqual(selector,14)
        (ROOT/'evidence/native-model-test.json').write_text(json.dumps({'model_selectors_after_size_S':observed,'note':'Some dedicated case types choose fixed models, independently of TypeOfVolume.'},indent=2)+'\n')

    def test_09_hash_verifier(self):
        for data in (b'',b'abc',bytes(range(256))*1000):
            buf=C.create_string_buffer(data);out=C.create_string_buffer(32);DLL.Hash(buf,len(data),out)
            self.assertEqual(bytes(out),hashlib.sha256(data).digest())

    def test_10_all_container_models_use_real_small_case(self):
        entries=[C.create_string_buffer(0x38) for _ in range(33)]
        refs=(C.c_void_p*33)(*(C.addressof(e) for e in entries))
        for i,e in enumerate(entries):pack(e,0x20,'B',i);pack(e,0x28,'Q',0xA000+i)
        manager=C.create_string_buffer(0x30);pack(manager,0x20,'i',33);pack(manager,0x28,'Q',C.addressof(refs))
        checked=0
        for country in range(3):
            for kind in [0,1,2,3,4,7,9,10,12,13,14,15,16,17,19]:
                for key in (0,0xdc9c87d):
                    b=bag(3,kind);pack(b,0x44,'I',key);pack(b,0x4a,'B',country)
                    DLL.Transform(b);before=bytes(b)
                    actual=DLL.Model(manager,b,self.native.model)
                    self.assertEqual(actual,0xA004 if country==2 else 0xA000,(country,kind,key))
                    self.assertEqual(bytes(b),before);checked+=1
        (ROOT/'evidence/forced-small-model-test.json').write_text(json.dumps({'cases':checked,'passed':True,'native_result':'Normal S model 0 / Australia S model 4 for eligible containers; functional entities excluded'},indent=2)+'\n')

    def test_11_native_small_appearance_matches_small_model(self):
        entries=[C.create_string_buffer(0x38) for _ in range(128)]
        refs=(C.c_void_p*128)(*(C.addressof(e) for e in entries))
        for i,e in enumerate(entries):pack(e,0x21,'B',i);pack(e,0x30,'Q',0xB000+i)
        manager=C.create_string_buffer(0x30);pack(manager,0x20,'i',128);pack(manager,0x28,'Q',C.addressof(refs))
        C.c_uint64.from_address(self.native.base+0x623ea48).value=C.addressof(manager)
        checked=0
        for country in range(3):
            for kind in [0,1,2,3,4,7,9,10,12,13,14,15,16,17,19]:
                for attribute in (0,1,2,3,5):
                    for found in (0,1):
                        b=bag(0,kind,attribute);pack(b,0x4a,'B',country)
                        instance=C.create_string_buffer(0x160);pack(instance,0x38,'Q',C.addressof(b));pack(instance,0xe8,'I',found<<18)
                        before=(bytes(b),bytes(instance))
                        actual=DLL.Appearance(None,instance,self.native.appearance)
                        # Original game selector applied to a genuine Normal/S fixture.
                        standard=bag(0,0,attribute);pack(standard,0x44,'I',0);pack(standard,0x4a,'B',country)
                        pack(standard,0x50,'Q',0);pack(standard,0x7c,'B',0)
                        normal=C.create_string_buffer(0x160);pack(normal,0x38,'Q',C.addressof(standard));pack(normal,0xe8,'I',found<<18)
                        expected=self.native.appearance(None,normal)
                        self.assertEqual(actual,expected,(country,kind,attribute,found))
                        self.assertEqual((bytes(b),bytes(instance)),before);checked+=1
        (ROOT/'evidence/forced-small-appearance-test.json').write_text(json.dumps({'cases':checked,'passed':True,'native_result':'Appearance agrees with the original Normal/S path; real cargo identity/state untouched'},indent=2)+'\n')

    def test_12_special_entities_are_passed_through(self):
        seen=[]
        @C.CFUNCTYPE(C.c_void_p,C.c_void_p,C.c_void_p)
        def original(a,b):seen.append(b);return 0x12345678
        for b in [bag(7),bag(8),bag(0,5),bag(0,6),bag(0,8),bag(0,11),bag(0,18),bag(0,0,4),bag(3)]:
            seen.clear();before=bytes(b)
            self.assertEqual(DLL.Model(None,b,original),0x12345678);self.assertEqual(seen,[C.addressof(b)])
            instance=C.create_string_buffer(0x160);pack(instance,0x38,'Q',C.addressof(b));seen.clear()
            self.assertEqual(DLL.Appearance(None,instance,original),0x12345678)
            self.assertEqual(seen,[C.addressof(instance)]);self.assertEqual(bytes(b),before)

    def test_13_startup_crash_cart_component_regression(self):
        # Original selectors + the original fault address calculation, with
        # synthetic entity/component storage. No game process or full entity simulation.
        component=C.create_string_buffer(0xc8)
        cart=C.create_string_buffer(0x100);small=C.create_string_buffer(0x100)
        entries=[C.create_string_buffer(0x38) for _ in range(3)]
        refs=(C.c_void_p*3)(*(C.addressof(e) for e in entries))
        for e,kind,entity in zip(entries,(0,18,25),(small,cart,cart)):
            pack(e,0x20,'B',kind);pack(e,0x28,'Q',C.addressof(entity))
        manager=C.create_string_buffer(0x30);pack(manager,0x20,'i',3);pack(manager,0x28,'Q',C.addressof(refs))
        row=C.create_string_buffer(0x68)
        # The old universal Normal/S replacement has no cart component.
        self.assertEqual(self.native.cart_read_address(row),0x50)
        paths=[]
        for path in RESOURCES.glob('DSGameBaggageListItem_*.json'):
            data=json.loads(path.read_text())
            if data['TypeOfCase']!='Cart':continue
            b=bag(2,8,1);pack(b,0x44,'I',data['NameCode']);before=bytes(b)
            self.assertFalse(DLL.Transform(b));self.assertEqual(bytes(b),before)
            selected=DLL.Model(manager,b,self.native.model)
            self.assertEqual(selected,C.addressof(cart))
            # Native registration stores returned cart component +20 at row+48.
            pack(row,0x48,'Q',C.addressof(component)+0x20)
            self.assertEqual(self.native.cart_read_address(row),C.addressof(component)+0x50)
            paths.append(path.name)
        self.assertEqual(len(paths),3)
        (ROOT/'evidence/cart-crash-regression.json').write_text(json.dumps({'real_cart_definitions':paths,'negative_control_fault_read':'0x50','fixed_selection':'original Cart/EnemyCart entity','limitation':'Synthetic component storage; no in-game test'},indent=2)+'\n')

    def test_14_functional_fixed_models_preserved(self):
        entries=[C.create_string_buffer(0x38) for _ in range(33)]
        refs=(C.c_void_p*33)(*(C.addressof(e) for e in entries))
        for i,e in enumerate(entries):pack(e,0x20,'B',i);pack(e,0x28,'Q',0xA000+i)
        manager=C.create_string_buffer(0x30);pack(manager,0x20,'i',33);pack(manager,0x28,'Q',C.addressof(refs))
        for key,expected in [(0xd3fccfd,25),(0x5d1d2feb,24),(0x4111fb17,32),(0x3041bd9b,32)]:
            b=bag(3);pack(b,0x44,'I',key);before=bytes(b)
            self.assertFalse(DLL.Transform(b));self.assertEqual(bytes(b),before)
            self.assertEqual(DLL.Model(manager,b,self.native.model),0xA000+expected)

    def test_15_captured_animals_preserve_cage_identity_and_volume(self):
        for volume in (0,2,6):
            b=bag(volume,12);pack(b,0x7c,'B',2);before=bytes(b)
            self.assertFalse(DLL.Transform(b));self.assertEqual(bytes(b),before)
            seen=[]
            @C.CFUNCTYPE(C.c_void_p,C.c_void_p,C.c_void_p)
            def original(a,p):seen.append(p);return 123
            self.assertEqual(DLL.Model(None,b,original),123);self.assertEqual(seen,[C.addressof(b)])
            instance=C.create_string_buffer(0x160);pack(instance,0x38,'Q',C.addressof(b));seen.clear()
            self.assertEqual(DLL.Appearance(None,instance,original),123);self.assertEqual(seen,[C.addressof(instance)])

    def test_16_executable_filter(self):
        for path in ['DS2.exe',r'C:\Game\DS2.EXE','C:/Game/ds2.exe']:
            self.assertTrue(DLL.IsGamePath(path,len(path)))
        for path in ['',r'C:\DS2.exe\CrashReport.exe','DS2.exe.bak','notDS2.exe','DS2.exe/']:
            self.assertFalse(DLL.IsGamePath(path,len(path)))

    def test_17_actual_asi_ignores_helper_process_log(self):
        with tempfile.TemporaryDirectory(prefix='small-cargo-helper-') as temp:
            temp=Path(temp);asi=temp/'ds2_all_containers_small.asi';log=temp/'ds2_all_containers_small.log'
            shutil.copyfile(ROOT/'release/ds2_all_containers_small.asi',asi)
            sentinel=b'Existing DS2 process log\r\n';log.write_bytes(sentinel)
            script='import ctypes,sys,time; ctypes.WinDLL(sys.argv[1]); time.sleep(0.5)'
            subprocess.run([sys.executable,'-c',script,str(asi)],check=True,timeout=10,capture_output=True)
            self.assertEqual(log.read_bytes(),sentinel)

if __name__=='__main__':unittest.main(verbosity=2)
