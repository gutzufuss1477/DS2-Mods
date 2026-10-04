"""Run the real clone builder and selected original DS2 instructions in this
test process. Does not start, attach to, or write to the game.
Requires local resource survey and the SHA256-checked analysis/DS2.exe.
"""
import ctypes as C
import base64
import json
from pathlib import Path
import re
import struct
import subprocess
import sys
import tempfile
import unittest

ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'tests/atlas'))
from native_scan import Image
import zipfile
with zipfile.ZipFile(ROOT/'tests/fixtures/weapons-native.zip') as fixtures:
    fixtures.extractall(ROOT/'build/weapons-fixtures')
TYPES=json.loads((ROOT/'build/weapons-fixtures/types.json').read_text(encoding='utf-8'))
WEAPONS=json.loads((ROOT/'build/weapons-fixtures/weapon-comparison.json').read_text())
RESOURCES=ROOT/'build/weapons-fixtures/resources'
INDEX={p.stem.split('_',1)[1]:p for p in RESOURCES.glob('*.json')}
ENUMS={k:{v['name']:v['value'] for v in x.get('values',[])} for k,x in TYPES.items() if x['kind']=='enum'}
NAMES=['AssaultRifleG1Lv2_Multi','LightMachineGunG2Lv2_Multi','ShotgunG1Lv2_Multi','LargeCalibreHandgunG2Lv1_Multi','HandgunG2Lv1_Multi']
AMMO=[[16,17],[122,123,321],[69,70],[113,114],[101,102,99,100]]
SLOTS=[0,1,3,4,2]
IDS=[300,301,303,304,302]
DONORS=[[39,40],[39,40,40],[39,40],[107,108],[107,108,107,108]]
IDENTITIES=[(0x7FACC8DC,0x585E2C8F,5,0x3237D11E),(0x1C6095EF,0x49F9F2BF,353,0x23900F2E),(0x22843177,0x0576D524,55,0x6F1F28B5),(0x7623068F,0x51D1E2DC,1002,0x3BB81F4D),(0x17187678,0x30EA922B,1000,0x5A836FBA)]

def ref(s):
    g,i=re.fullmatch(r'<ref to (\d+):(\d+)>',s).groups()
    return json.loads(INDEX[g+'_'+i].read_text())

def pack(b,off,fmt,v):struct.pack_into('<'+fmt,b,off,v)
def value(b,off,fmt='Q'):return struct.unpack_from('<'+fmt,b,off)[0]
def ptr(b):return C.addressof(b)
def pattern(size,seed=19):return C.create_string_buffer(bytes((i*37+seed)%256 for i in range(size)),size)

class Sources(C.Structure):
    _fields_=[(n,C.c_void_p) for n in ['weapon','list','bag','recipe','name','description']]+[
        (n,C.c_void_p*4) for n in ['traits','ammo','soundDonors']]+[('traitsCount',C.c_uint),('primary',C.c_uint)]

DLL=C.CDLL(str(ROOT/'build/native_tests.dll'))
DLL.BundleSize.restype=C.c_uint
DLL.Prepare.argtypes=[C.c_void_p,C.c_uint,C.POINTER(Sources),C.c_bool,C.c_bool];DLL.Prepare.restype=C.c_bool
DLL.Part.argtypes=[C.c_void_p,C.c_uint,C.c_uint];DLL.Part.restype=C.c_void_p
DLL.Language.argtypes=[C.c_void_p,C.c_uint,C.c_bool]
DLL.Fabrication.argtypes=[C.c_void_p,C.c_bool]
DLL.MaterialVariant.argtypes=[C.POINTER(C.c_void_p),C.c_uint64];DLL.MaterialVariant.restype=C.c_int
DLL.MaterialSource.argtypes=[C.c_uint];DLL.MaterialSource.restype=C.c_void_p
DLL.MaterialMarkerHash.restype=C.c_uint
DLL.MaterialMarked.argtypes=[C.c_void_p,C.c_uint];DLL.MaterialMarked.restype=C.c_bool

DLL.MenuIcon.argtypes=[C.c_void_p,C.c_uint,C.c_void_p];DLL.MenuIcon.restype=C.c_bool

class Fixture:
    def __init__(self,k):
        self.k=k;self.s=Sources();self.parts={};self.original={}
        self.export=next(w for w in WEAPONS if w['weapon']['Id']==NAMES[k])
        recipe,bag,lid,lcode=IDENTITIES[k]
        for i,(name,size) in enumerate(zip(['weapon','list','bag','recipe','name','description'],[0x320,0xc0,0x88,0xb0,0x38,0x38])):
            b=pattern(size,19+i);self.parts[i,0]=b;setattr(self.s,name,ptr(b))
        w,l,b,r=[self.parts[i,0] for i in range(4)]
        pack(w,0x20,'H',ENUMS['EDSWeaponId'][NAMES[k]]);pack(w,0x28,'Q',ptr(l))
        pack(l,0x40,'I',lid);pack(l,0x44,'I',lcode);pack(b,0x40,'I',0);pack(b,0x44,'I',bag);pack(b,0x50,'Q',ptr(l));pack(r,0x20,'I',recipe);pack(r,0x28,'Q',ptr(b))
        self.s.traitsCount=len(self.export['traits']);self.s.primary=0
        self.ids=(C.c_ushort*len(AMMO[k]))(*AMMO[k])
        for i,t in enumerate(self.export['traits']):
            z=pattern(0x120,30+i);self.parts[6,i]=z;self.s.traits[i]=ptr(z)
            pack(z,0x28,'I',t['BindBehaviorIndex']);pack(z,0x30,'I',ENUMS['EDSWeaponBehaviorType'][t['WeaponBehaviorType']]);pack(z,0x34,'B',t['IsAttachment']);pack(z,0x37,'B',t['HasSuppressor'])
            pack(z,0x58,'I',len(t['SelectableAmmoIds']));pack(z,0x5c,'I',len(t['SelectableAmmoIds']));pack(z,0x60,'Q',ptr(self.ids))
        self.traitrefs=(C.c_void_p*self.s.traitsCount)(*self.s.traits[:self.s.traitsCount])
        pack(w,0x40,'I',self.s.traitsCount);pack(w,0x44,'I',self.s.traitsCount);pack(w,0x48,'Q',ptr(self.traitrefs))
        self.donors=[]
        for i,(aid,did) in enumerate(zip(AMMO[k],DONORS[k])):
            z=pattern(0x390,40+i);self.parts[7,i]=z;self.s.ammo[i]=ptr(z);pack(z,0x20,'H',aid)
            donor=pattern(0x390,50+i);self.donors.append(donor);self.s.soundDonors[i]=ptr(donor);pack(donor,0x20,'H',did);pack(donor,0x300,'Q',0x12340000+did)
        self.original={key:bytes(b) for key,b in self.parts.items()}
        self.output=pattern(DLL.BundleSize(),73)
    def prepare(self,enabled=True,german=False):return DLL.Prepare(self.output,self.k,C.byref(self.s),enabled,german)
    def address(self,part,i=0):return DLL.Part(self.output,part,i)
    def out(self,part,i=0):return C.string_at(self.address(part,i),len(self.parts[part,i]))

# A small W^X code container for original executable instruction fragments.
K=C.WinDLL('kernel32',use_last_error=True)
K.VirtualAlloc.argtypes=[C.c_void_p,C.c_size_t,C.c_uint,C.c_uint];K.VirtualAlloc.restype=C.c_void_p
K.VirtualProtect.argtypes=[C.c_void_p,C.c_size_t,C.c_uint,C.POINTER(C.c_uint)];K.VirtualProtect.restype=C.c_int
K.VirtualFree.argtypes=[C.c_void_p,C.c_size_t,C.c_uint];K.VirtualFree.restype=C.c_int
K.GetCurrentProcess.restype=C.c_void_p
K.FlushInstructionCache.argtypes=[C.c_void_p,C.c_void_p,C.c_size_t];K.FlushInstructionCache.restype=C.c_int
class Code:
    def __init__(self,data):
        self.p=K.VirtualAlloc(None,4096,0x3000,4)
        if not self.p:raise C.WinError(C.get_last_error())
        C.memmove(self.p,data,len(data));old=C.c_uint()
        if not K.VirtualProtect(self.p,4096,0x20,C.byref(old)) or not K.FlushInstructionCache(K.GetCurrentProcess(),self.p,4096):raise C.WinError(C.get_last_error())
    def __enter__(self):return self.p
    def __exit__(self,*_):K.VirtualFree(self.p,0,0x8000)

class CloneTests(unittest.TestCase):
    def test_shared_language_keeps_big_bore_german_and_honors_explicit_override(self):
        DLL.GermanLanguage.argtypes=[C.c_uint,C.c_char_p];DLL.GermanLanguage.restype=C.c_bool
        for setting,anchor,expected in [(0,b'Sturmgewehr [MZ] St.2',True),
            (0,b'Assault Rifle [MP] Lv.2',False),(1,b'Assault Rifle',True),
            (2,b'Sturmgewehr',False),(0,None,False),(0,b'',False),(0,b'Stu',False)]:
            self.assertEqual(DLL.GermanLanguage(setting,anchor),expected)
        german=DLL.GermanLanguage(0,b'Sturmgewehr [MZ] St.2')
        for k in range(5):
            f=Fixture(k);self.assertTrue(f.prepare(german=german))
            name=f.out(4);text=C.string_at(value(name,0x20),value(name,0x28,'I')).decode('utf-8')
            self.assertTrue(text.startswith('Schallgedämpft'))
            self.assertIn('[MZ]',text)
            if k==3:self.assertEqual(text,'Schallgedämpfte Großkaliber-Handfeuerwaffe [MZ]')
    def test_legacy_ids_remain_readable_without_a_fabrication_recipe(self):
        DLL.OwnWeapon.argtypes=[C.c_ushort];DLL.OwnWeapon.restype=C.c_bool
        DLL.OwnAmmo.argtypes=[C.c_ushort];DLL.OwnAmmo.restype=C.c_bool
        self.assertEqual([i for i in range(290,320) if DLL.OwnWeapon(i)],sorted(IDS))
        self.assertEqual([i for i in range(590,630) if DLL.OwnAmmo(i)],
                         [600,601,604,605,606,608,609,610,611,612,613,616,617])
        for k in range(4):
            f=Fixture(k);self.assertTrue(f.prepare())
            self.assertEqual(f.out(0)[16],SLOTS[k])
        DLL.Craftable.argtypes=[C.c_uint];DLL.Craftable.restype=C.c_bool
        self.assertEqual([k for k in range(5) if DLL.Craftable(k)],[0,1,2,3])
        legacy=Fixture(4);self.assertTrue(legacy.prepare(enabled=True))
        self.assertEqual(value(legacy.out(0),0x20,'H'),302)
        self.assertEqual(legacy.out(3)[0x52],0,'Legacy recipe must stay disabled even with Enabled=True')
        for (part,index) in legacy.parts:
            kind=part+1 if part<6 else (10+index if part==6 else 20+index)
            self.assertEqual(legacy.out(part,index)[16:32],struct.pack('<QQ',
                0x4B58A127E5310000|(kind<<8)|2,0x93CC28BE714D062F))
        self.assertEqual([value(legacy.out(7,i),0x20,'H') for i in range(4)],[608,609,610,611])
        self.assertEqual(legacy.out(0)[0x264:0x274],legacy.original[0,0][0x264:0x274])

    def test_red_menu_icons_copy_only_native_texture_string(self):
        files=['DSGameWeaponListItem_54209_0.json','DSGameWeaponListItem_48200_0.json']
        for k,name in enumerate(files):
            j=json.loads((RESOURCES/name).read_text(encoding='utf-8'))
            donor=pattern(0xc0);pack(donor,0x40,'I',j['ID']);pack(donor,0x44,'I',j['NameCode'])
            texture=C.create_string_buffer(j['UiTextureBaseName'].encode())
            pack(donor,0x50,'Q',ptr(texture));before=bytes(donor)
            f=Fixture(k);self.assertTrue(f.prepare());old=f.out(1)
            self.assertTrue(DLL.MenuIcon(f.output,k,donor));new=f.out(1)
            self.assertEqual(value(new,0x50),ptr(texture))
            self.assertEqual(new[:0x50]+new[0x58:],old[:0x50]+old[0x58:])
            self.assertEqual(bytes(donor),before)
            for variant in [1-k,2,99]:
                self.assertFalse(DLL.MenuIcon(f.output,variant,donor))
                self.assertEqual(f.out(1),new)
            self.assertFalse(DLL.MenuIcon(f.output,k,None))
            pack(donor,0x50,'Q',0);self.assertFalse(DLL.MenuIcon(f.output,k,donor))

    def test_material_targets_owned_resources_only(self):
        fixtures=[Fixture(k) for k in range(4)]
        for f in fixtures:self.assertTrue(f.prepare())
        bundles=(C.c_void_p*4)(*[ptr(f.output) for f in fixtures])
        for k,f in enumerate(fixtures):
            self.assertEqual(DLL.MaterialVariant(bundles,f.address(0)),k)
            self.assertEqual(DLL.MaterialVariant(bundles,ptr(f.parts[0,0])),-1)
        self.assertEqual(DLL.MaterialVariant(bundles,0),-1)
        self.assertEqual(DLL.MaterialVariant((C.c_void_p*4)(),fixtures[0].address(0)),-1)

    def test_material_donors_match_exported_mech_variations(self):
        import uuid
        files=['mech-materials/ArtPartsVariationResource_499_100140.json','mech-materials/ArtPartsVariationResource_20719_9.json','arsenal-0.5.0/ArtPartsVariationResource_922_0.json']
        for k,name in enumerate(files):
            j=json.loads((ROOT/'build/weapons-fixtures'/name).read_text())
            p=DLL.MaterialSource(k)
            self.assertEqual(C.string_at(p,16),uuid.UUID(j['ObjectUUID']).bytes_le)
            self.assertEqual(C.c_uint.from_address(p+16).value,len(j['ReplaceTextureSetResources']))
            self.assertFalse(j['ReplaceShaderVariableResources'])
        self.assertFalse(DLL.MaterialSource(3),'No unsupported rifle texture on the pistol')
        self.assertFalse(DLL.MaterialSource(99))

    def test_marker_is_bound_to_one_model_and_exact_native_entry(self):
        entries=C.create_string_buffer(120);other=C.create_string_buffer(120);h=DLL.MaterialMarkerHash()
        self.assertFalse(DLL.MaterialMarked(entries,3))
        pack(entries,40,'Q',h<<32);pack(entries,48,'I',h);pack(entries,52,'f',1);pack(entries,68,'B',1)
        self.assertTrue(DLL.MaterialMarked(entries,3));self.assertFalse(DLL.MaterialMarked(other,3))
        self.assertFalse(DLL.MaterialMarked(entries,257));self.assertFalse(DLL.MaterialMarked(None,3))
        pack(entries,40,'Q',(h<<32)|123)
        self.assertFalse(DLL.MaterialMarked(entries,3),'Another mesh is not the model marker')

    def test_trait_alignment_and_native_baggage_id(self):
        for k in range(4):
            f=Fixture(k);self.assertTrue(f.prepare())
            self.assertEqual(value(f.out(2),0x40,'I'),0)
            for i in range(f.s.traitsCount):self.assertEqual(f.address(6,i)%16,0)

    def test_original_weapon_performance_and_assets_are_preserved(self):
        identity=[(8,12),(0x10,0x20)]
        allowed={0:identity+[(0x20,0x22),(0x28,0x38),(0x40,0x50),(0x2d0,0x2e0),(0x2f0,0x2f8)],1:identity+[(0x20,0x48),(0x68,0x78)],2:identity+[(0x20,0x48),(0x50,0x58)],3:identity+[(0x20,0x24),(0x28,0x31),(0x38,0x50),(0x52,0x53),(0x72,0x73),(0x74,0x75),(0xa8,0xaa)],4:identity+[(0x20,0x2c)],5:identity+[(0x20,0x2c)],6:identity+[(0x37,0x38),(0x5c,0x68)],7:identity+[(0x20,0x22),(0x300,0x308)]}
        for k in range(4):
            f=Fixture(k);self.assertTrue(f.prepare())
            for (part,i),source in f.original.items():
                out=f.out(part,i);ranges=identity if part==6 and i else allowed[part]
                mask={n for a,b in ranges for n in range(a,b)}
                if k==2 and part==7:mask.update(range(0x2f8,0x300))
                self.assertFalse([n for n,(a,b) in enumerate(zip(source,out)) if a!=b and n not in mask],(k,part,i))
                self.assertEqual(bytes(f.parts[part,i]),source,'source mutated')
            # Entire source model UUID, costs, fire modes, RPM/recoil and projectile refs remain copied.
            self.assertEqual(f.out(0)[0x264:0x274],f.original[0,0][0x264:0x274])
            self.assertEqual(f.out(3)[0x58:0x68],f.original[3,0][0x58:0x68])
            self.assertEqual(value(f.out(6),0x30,'I'),2 if k==2 else 1)
            self.assertEqual(f.out(6)[0x37],1)

    def test_cloned_resource_graph_resolves_without_source_aliasing(self):
        identities=set()
        for k in range(4):
            f=Fixture(k);self.assertTrue(f.prepare())
            w,l,b,r=[f.out(i) for i in range(4)]
            self.assertEqual(value(w,0x20,'H'),IDS[k]);self.assertEqual(value(w,0x28),f.address(1))
            self.assertEqual(value(b,0x50),f.address(1));self.assertEqual(value(r,0x28),f.address(2))
            self.assertEqual(value(w,0x30),0);self.assertEqual(value(w,0x2d0),f.address(4));self.assertEqual(value(l,0x28),f.address(5))
            for i in range(f.s.traitsCount):self.assertEqual(C.c_uint64.from_address(value(w,0x48)+8*i).value,f.address(6,i))
            ids=value(f.out(6),0x60)
            for i in range(len(AMMO[k])):
                self.assertEqual(C.c_ushort.from_address(ids+2*i).value,600+4*SLOTS[k]+i)
                self.assertEqual(value(f.out(7,i),0x20,'H'),600+4*SLOTS[k]+i)
                self.assertEqual(value(f.out(7,i),0x300),0x12340000+DONORS[k][i])
            for part,i in f.parts:
                uid=f.out(part,i)[0x10:0x20];self.assertNotIn(uid,identities);identities.add(uid)
                self.assertEqual(value(f.out(part,i),8,'I'),1)
        self.assertEqual(len(identities),39)

    def test_grenade_launcher_retains_behavior_ammo_and_unsuppressed_flag(self):
        for k in [0,2]:
            f=Fixture(k);self.assertTrue(f.prepare());out=f.out(6,1)
            self.assertEqual(out[0x20:],f.original[6,1][0x20:]);self.assertEqual(value(out,0x30,'I'),3)
            self.assertEqual(out[0x37],0)

    def test_bad_sources_fail_before_any_output_writes(self):
        cases=[(0,0x20,'H',999),(1,0x40,'I',999),(1,0x44,'I',999),(2,0x44,'I',999),(2,0x50,'Q',0),(3,0x28,'Q',0),(3,0x20,'I',0),(6,0x30,'I',0),(6,0x30,'I',3),(6,0x34,'B',1),(6,0x28,'I',1),(6,0x58,'I',99),(7,0x20,'H',0)]
        for k in range(4):
            for part,off,fmt,v in cases:
                f=Fixture(k);pack(f.parts[part,0],off,fmt,v);before=bytes(f.output)
                self.assertFalse(f.prepare(),(k,part,off));self.assertEqual(bytes(f.output),before)
            for field in ['weapon','list','bag','recipe','name','description']:
                f=Fixture(k);setattr(f.s,field,None);self.assertFalse(f.prepare())
            f=Fixture(k);pack(f.donors[0],0x300,'Q',0);self.assertFalse(f.prepare())
            f=Fixture(k);f.s.primary=4;self.assertFalse(f.prepare())
            f=Fixture(k);f.s.traitsCount=5;self.assertFalse(f.prepare())
        f=Fixture(0);f.k=5;self.assertFalse(f.prepare())

    def test_recipe_toggle_retains_saved_identities_and_language_is_stable(self):
        for k in range(4):
            f=Fixture(k);self.assertTrue(f.prepare(enabled=False));self.assertEqual(f.out(3)[0x52],0)
            before={p:f.out(*p) for p in f.parts};DLL.Fabrication(f.output,True)
            self.assertEqual(f.out(3)[0x52],1)
            for german in [True,False,True]:
                DLL.Language(f.output,k,german)
                name=f.out(4);s=C.string_at(value(name,0x20),value(name,0x28,'I')).decode('utf-8')
                self.assertTrue(s.startswith('Schall' if german else 'Suppressed'))
                for p in f.parts:self.assertEqual(f.out(*p)[0x10:0x20],before[p][0x10:0x20])

class EvidenceTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):cls.im=Image()

    def test_schema_and_actual_resource_identities(self):
        fields={a['name']:a['offset'] for a in TYPES['DSWeaponTrait']['attrs'] if 'name' in a}
        self.assertEqual(fields['HasSuppressor'],0x37);self.assertEqual(fields['SelectableAmmoIds'],0x58)
        self.assertEqual(ENUMS['EDSWeaponBehaviorType']['Gun'],1)
        for k,name in enumerate(NAMES):
            x=next(w for w in WEAPONS if w['weapon']['Id']==name)
            self.assertEqual([ENUMS['EDSAmmoId'][a] for a in x['traits'][0]['SelectableAmmoIds']],AMMO[k])
            self.assertFalse(x['traits'][0]['HasSuppressor']);self.assertEqual(x['list']['ID'],IDENTITIES[k][2]);self.assertEqual(x['list']['NameCode'],IDENTITIES[k][3])
            matches=[]
            for p in RESOURCES.glob('DSGameCatalogueListItem_*.json'):
                recipe=json.loads(p.read_text())
                if recipe['NameCode']==IDENTITIES[k][0]:matches.append(recipe)
            self.assertEqual(len(matches),1);bag=ref(matches[0]['Baggage']);self.assertEqual(bag['NameCode'],IDENTITIES[k][1]);self.assertEqual(ref(bag['Contents'])['ObjectUUID'],x['list']['ObjectUUID'])

    def test_custom_ids_and_name_codes_do_not_collide_with_native_resources(self):
        natives={}
        for t,field in [('DSWeaponParameter','Id'),('DSAmmoParameter','Id'),('DSGameWeaponListItem','NameCode'),('DSGameBaggageListItem','NameCode'),('DSGameCatalogueListItem','NameCode')]:
            data=[json.loads(p.read_text()) for p in RESOURCES.glob(t+'_*.json')]
            enum={'DSWeaponParameter':'EDSWeaponId','DSAmmoParameter':'EDSAmmoId'}.get(t)
            natives[t]={ENUMS[enum][x[field]] if enum else x[field] for x in data}
        keys=set()
        for k in range(4):
            f=Fixture(k);self.assertTrue(f.prepare());self.assertNotIn(IDS[k],natives['DSWeaponParameter'])
            for i in range(len(AMMO[k])):self.assertNotIn(600+4*SLOTS[k]+i,natives['DSAmmoParameter'])
            for part,off,t in [(1,0x44,'DSGameWeaponListItem'),(2,0x44,'DSGameBaggageListItem'),(3,0x20,'DSGameCatalogueListItem')]:
                key=value(f.out(part),off,'I');self.assertNotIn(key,natives[t]);self.assertNotIn(key,keys);keys.add(key)

    def test_suppressed_sound_donors_are_real_native_sounds(self):
        ammo={ENUMS['EDSAmmoId'][x['Id']]:x for x in (json.loads(p.read_text()) for p in RESOURCES.glob('DSAmmoParameter_*.json'))}
        for donors in DONORS:
            for id in donors:
                sound=ammo[id]['FireSoundBySuppressor'];self.assertRegex(sound,r'^<ref to \d+:\d+>$')
                self.assertNotEqual(sound,'<ref to 1178:18>')

    def test_original_native_weapon_and_ammo_lookups_accept_all_custom_ids(self):
        for rva,off,ids in [(0x1fbea10,0x30,[1,18,35,42,47,300,301,302,303,304]),(0x1fbea60,0x40,[17,39,321,600,601,604,605,606,608,609,610,611,612,613,616,617])]:
            body=bytearray(self.im.read(rva,0x4c));self.assertEqual(body[:3],bytes.fromhex('4c8b1d'))
            body[:7]=bytes.fromhex('4989cb90909090') # substitute only the global manager load with R11=RCX
            manager=C.create_string_buffer(0x60);objects=[C.create_string_buffer(0x40) for _ in ids]
            for obj,id in zip(objects,ids):pack(obj,0x20,'H',id)
            refs=(C.c_void_p*len(ids))(*map(ptr,objects));pack(manager,off,'I',len(ids));pack(manager,off+8,'Q',ptr(refs))
            with Code(bytes(body)) as p:
                fn=C.CFUNCTYPE(C.c_void_p,C.c_void_p,C.c_ushort)(p)
                for obj,id in zip(objects,ids):self.assertEqual(fn(manager,id),ptr(obj))
                for missing in [0,299,603,614,65535]:self.assertIsNone(fn(manager,missing))
                pack(manager,off,'I',0);self.assertIsNone(fn(manager,ids[0]))

    def test_recorded_startup_lookup_302_resolves_before_native_dereference(self):
        # 0.5.0 dump: RDX=302, RAX=0, fault at DS2+EAA89B reading RAX+1DE.
        call=self.im.read(0xeaa896,5)
        self.assertEqual(call[0],0xe8)
        self.assertEqual(0xeaa89b+struct.unpack_from('<i',call,1)[0],0x1fbea10)
        compare=self.im.read(0xeaa89b,7)
        self.assertEqual(compare,bytes.fromhex('80b8de01000000'))
        code=bytearray(4096)
        wrapper=bytes.fromhex('4883ec28e8')+struct.pack('<i',0x100-9)+compare+bytes.fromhex('0f95c00fb6c04883c428c3')
        code[:len(wrapper)]=wrapper
        lookup=bytearray(self.im.read(0x1fbea10,0x4c))
        lookup[:7]=bytes.fromhex('4989cb90909090') # R11=manager argument; all lookup instructions remain native.
        code[0x100:0x100+len(lookup)]=lookup
        fixtures=[Fixture(k) for k in range(5)]
        for f in fixtures:self.assertTrue(f.prepare())
        manager=C.create_string_buffer(0x60);refs=(C.c_void_p*5)(*[f.address(0) for f in fixtures])
        pack(manager,0x30,'I',5);pack(manager,0x38,'Q',ptr(refs))
        with Code(bytes(code)) as p:
            original_lookup=C.CFUNCTYPE(C.c_void_p,C.c_void_p,C.c_ushort)(p+0x100)
            checked_read=C.CFUNCTYPE(C.c_uint,C.c_void_p,C.c_ushort)(p)
            self.assertEqual(original_lookup(manager,302),fixtures[4].address(0))
            self.assertEqual(checked_read(manager,302),int(fixtures[4].out(0)[0x1de]!=0))
            # This was the exact missing lookup in the withdrawn four-entry build.
            pack(manager,0x30,'I',4)
            self.assertIsNone(original_lookup(manager,302))

    def test_original_native_suppressor_branch_skips_firing_stimulus(self):
        raw=self.im.read(0x2029420,0x1a)
        self.assertEqual(raw[:7].hex(),'4438b350020000');self.assertEqual(raw[13:20].hex(),'4438b718030000')
        b=bytearray(b'\xcc'*4096);setup=bytes.fromhex('535741564889cb4889d74531f6');b[:len(setup)]=setup
        b[len(setup):len(setup)+5]=b'\xe9'+struct.pack('<i',0x100-len(setup)-5);b[0x100:0x11a]=raw
        struct.pack_into('<i',b,0x109,0x220-0x10d);struct.pack_into('<i',b,0x116,0x220-0x11a)
        b[0x11a:0x11f]=b'\xe9'+struct.pack('<i',0x200-0x11f)
        for off,v in [(0x200,1),(0x220,0)]:b[off:off+10]=bytes.fromhex('415e5f5bb8')+struct.pack('<I',v)+b'\xc3'
        ammo=C.create_string_buffer(0x390);component=C.create_string_buffer(0x320)
        with Code(bytes(b)) as p:
            fn=C.CFUNCTYPE(C.c_uint,C.c_void_p,C.c_void_p)(p)
            for firing,suppressed,expected in [(0,0,0),(0,1,0),(1,0,1),(1,1,0)]:
                pack(ammo,0x250,'B',firing);pack(component,0x318,'B',suppressed);self.assertEqual(fn(ammo,component),expected)

    def test_shotgun_uses_the_verified_native_suppressor_init_and_sound_stimulus_handler(self):
        # Real main vtables, identified by their native RTTI getters. The pellet
        # spawn function differs; suppressor init and the sound/AI handler do not.
        for table,getter in [(0x33ca7e0,0x1ff3910),(0x33caa90,0x1ff42d0)]:
            self.assertEqual(value(self.im.read(table,8),0)-self.im.base,getter)
            for slot,handler in [(13,0x2028040),(45,0x2029300)]:
                self.assertEqual(value(self.im.read(table+8*slot,8),0)-self.im.base,handler)
        self.assertEqual(self.im.read(0x2028087,4),bytes.fromhex('0fb65b37'))
        self.assertEqual(self.im.read(0x2028095,6),bytes.fromhex('889f18030000'))

    def test_original_native_sound_selection_uses_suppressed_reference(self):
        setup=bytes.fromhex('535741564889cb4889d74531f6')
        body=self.im.read(0x2029379,0x1a)
        with Code(setup+body+bytes.fromhex('4889d0415e5f5bc3')) as p:
            fn=C.CFUNCTYPE(C.c_uint64,C.c_void_p,C.c_void_p)(p)
            ammo=C.create_string_buffer(0x390);component=C.create_string_buffer(0x320)
            pack(ammo,0x2f8,'Q',0x11111111);pack(ammo,0x300,'Q',0x22222222)
            for suppressed,expected in [(0,0x11111111),(1,0x22222222)]:
                pack(component,0x318,'B',suppressed);self.assertEqual(fn(ammo,component),expected)

    def test_shotgun_both_native_sound_paths_select_proven_suppressed_audio(self):
        f=Fixture(2);self.assertTrue(f.prepare());component=C.create_string_buffer(0x320)
        setup=bytes.fromhex('535741564889cb4889d74531f6')
        body=self.im.read(0x2029379,0x1a)
        with Code(setup+body+bytes.fromhex('4889d0415e5f5bc3')) as p:
            fn=C.CFUNCTYPE(C.c_uint64,C.c_void_p,C.c_void_p)(p)
            for i,donor in enumerate([39,40]):
                for suppressed in [0,1]:
                    pack(component,0x318,'B',suppressed)
                    self.assertEqual(fn(f.address(7,i),component),0x12340000+donor)
                for off in [0x2f8,0x300]:self.assertEqual(value(f.out(7,i),off),0x12340000+donor)
                self.assertEqual(bytes(f.parts[7,i]),f.original[7,i],'Original shotgun ammo must remain untouched')
        # Launcher is a separate trait and still selects its native ammunition.
        self.assertEqual(f.out(6,1)[0x20:],f.original[6,1][0x20:])

    def test_hook_preserves_instruction_boundary_and_atlas_callsite(self):
        self.assertEqual(self.im.read(0xb6cb20,5),bytes.fromhex('48895c2408'))
        self.assertEqual(list(self.im.disasm(0xb6cb20,0xb6cb25))[0][1],5)
        native=self.im.read(0x1ec3db7,5);self.assertEqual(native[0],0xe8)
        self.assertEqual(0x1ec3dbc+struct.unpack_from('<i',native,1)[0],0xb6cb20)
        self.assertEqual(0x1ec3f28+struct.unpack_from('<i',self.im.read(0x1ec3f23,5),1)[0],0x1fbcb40)
        self.assertLess(0x1ec3db7,0x1ec3f23)
        self.assertEqual(self.im.read(0x1fad390,5),bytes.fromhex('e9bbc4ffff'))
        instruction=list(self.im.disasm(0x1fad390,0x1fad395))[0]
        self.assertEqual(instruction[1:3],(5,'jmp'))
        self.assertEqual(instruction[3],hex(self.im.base+0x1fa9850))

    def test_native_model_variable_wrapper_preserves_abi_and_instance_ownership(self):
        body=bytearray(b'\xcc'*4096);body[:0x86]=self.im.read(0x33bd40,0x86)
        controller=C.create_string_buffer(0x90);model=C.create_string_buffer(0xb0)
        pack(model,0x48,'Q',0x12340000)
        calls=[];allocations=[]
        @C.CFUNCTYPE(C.c_void_p,C.c_void_p)
        def allocate(owner):allocations.append(owner);return ptr(controller)
        @C.CFUNCTYPE(None,C.c_void_p,C.POINTER(C.c_uint),C.POINTER(C.c_float),C.c_uint,C.POINTER(C.c_uint))
        def set_variable(target,var,data,n,mesh):calls.append((target,var[0],tuple(data[:n]),mesh[0]))
        for offset,stub,fn in [(0x31,0x200,allocate),(0x6b,0x220,set_variable)]:
            self.assertEqual(body[offset],0xe8)
            struct.pack_into('<i',body,offset+1,stub-offset-5)
            body[stub:stub+14]=bytes.fromhex('ff2500000000')+struct.pack('<Q',C.cast(fn,C.c_void_p).value)
        with Code(bytes(body)) as p:
            native=C.CFUNCTYPE(None,C.c_void_p,C.POINTER(C.c_uint),C.POINTER(C.c_float),C.c_uint,C.POINTER(C.c_uint))(p)
            variable=C.c_uint(976688365);mesh=C.c_uint(0);data=(C.c_float*1)(3)
            native(model,C.byref(variable),data,1,C.byref(mesh))
            self.assertEqual(allocations,[0x12340000]);self.assertEqual(value(model,0xa0),ptr(controller));self.assertEqual(value(controller,0x78),ptr(model))
            mesh.value=1434199023;data[0]=15
            native(model,C.byref(variable),data,1,C.byref(mesh))
            self.assertEqual(len(allocations),1)
            self.assertEqual(calls,[(ptr(controller),976688365,(3.0,),0),(ptr(controller),976688365,(15.0,),1434199023)])

    def test_native_variable_controller_writes_recognizable_instance_marker(self):
        # Exercise the original controller insertion, verifying our marker
        # layout against actual engine-produced bytes, not a mirrored struct.
        start,end=0x347d30,0x348135;b=bytearray(4096);b[:end-start]=self.im.read(start,end-start)
        controller=C.create_string_buffer(0x80);entries=C.create_string_buffer(40)
        variable=C.c_uint(DLL.MaterialMarkerHash());mesh=C.c_uint(0);color=C.c_float(1)
        @C.CFUNCTYPE(None,C.c_void_p,C.c_int,C.c_void_p)
        def insert(owner,index,entry):
            C.memmove(entries,entry,40);C.c_uint.from_address(owner).value=1
            C.c_void_p.from_address(owner+8).value=ptr(entries)
        stubs={0x34d840:0x600,0x2aca3b4:0x620,0x34eb10:0x640,0x34dd40:0x660,0x2ac7ea0:0x680}
        b[0x600:0x606]=bytes.fromhex('b8ffffffffc3')
        memcpy=C.CDLL('msvcrt').memcpy
        for offset,fn in [(0x620,memcpy),(0x640,insert)]:
            b[offset:offset+12]=b'\x48\xb8'+struct.pack('<Q',C.cast(fn,C.c_void_p).value)+b'\xff\xe0'
        b[0x660]=0xc3;b[0x680]=0xc3
        for addr,size,op,args in self.im.disasm(start,end):
            off=addr-self.im.base-start
            if op=='call' and args.startswith('0x'):
                dest=int(args,16)-self.im.base
                if dest in stubs:struct.pack_into('<i',b,off+1,stubs[dest]-off-5)
            if addr-self.im.base==0x347d46:struct.pack_into('<i',b,off+3,0x700-off-7)
        with Code(bytes(b)) as code:
            fn=C.CFUNCTYPE(C.c_bool,C.c_void_p,C.c_void_p,C.c_void_p,C.c_uint,C.c_void_p)(code)
            self.assertTrue(fn(ptr(controller),C.addressof(variable),C.addressof(color),1,C.addressof(mesh)))
        self.assertEqual(C.c_uint.from_buffer(controller,0x50).value,1)
        self.assertTrue(DLL.MaterialMarked(entries,1))

    def test_native_texture_replacement_selects_matching_model_part(self):
        # Execute the actual native mesh-selection/argument code, stubbing only
        # GPU/streaming work. The missing part must never bind a texture.
        start,end=0x349ab0,0x349cd0
        b=bytearray(4096);b[:end-start]=self.im.read(start,end-start)
        model=C.create_string_buffer(0xa8);resource=C.create_string_buffer(0x98)
        parts=C.create_string_buffer(96);part_owner=C.create_string_buffer(0xd0)
        representation=C.create_string_buffer(0x48);stream_owner=C.create_string_buffer(0x30)
        render=C.create_string_buffer(0x98);mapping=C.create_string_buffer(16)
        names=(C.c_uint*2)(1872310964,123);wanted=(C.c_uint*2)(1872310964,98765)
        targets=C.create_string_buffer(16);pack(targets,0,'I',2);pack(targets,8,'Q',ptr(wanted))
        pack(resource,0x88,'I',2);pack(resource,0x90,'Q',ptr(names))
        pack(model,0x30,'Q',ptr(resource));pack(model,0x58,'Q',ptr(parts))
        pack(parts,8,'Q',ptr(part_owner));pack(parts,0x28,'i',7)
        pack(part_owner,0xc8,'Q',ptr(representation));pack(representation,0x40,'Q',ptr(stream_owner));pack(stream_owner,0x28,'Q',ptr(render))
        node=C.create_string_buffer(b'TextureSetBindings');node_ref=C.c_void_p(ptr(node));texture=C.create_string_buffer(0x70)
        calls=[];keys=[]
        @C.CFUNCTYPE(C.c_void_p,C.c_void_p,C.c_void_p)
        def find(owner,key):keys.append((owner,C.c_uint64.from_address(key).value));return ptr(mapping)
        @C.CFUNCTYPE(C.c_uint64,C.c_void_p,C.c_void_p,C.c_void_p,C.c_void_p,C.c_uint)
        def bind(owner,name,tex,table,index):calls.append((owner,name,tex,table,index));return 0
        # Native hash helper preserves R11 (the part hash); a Python callback
        # would not promise that internal register convention.
        addresses={0xa4600:0x400,0x341190:0x420,0x166e10:0x440,0x166d80:0x460,0x166470:0x480,0x230030:0x4a0}
        b[0x400:0x406]=b'\xb8'+struct.pack('<I',0x12345)+b'\xc3'
        for offset,fn in [(0x420,find),(0x440,bind)]:
            b[offset:offset+12]=b'\x48\xb8'+struct.pack('<Q',C.cast(fn,C.c_void_p).value)+b'\xff\xe0'
        for offset in [0x460,0x480,0x4a0]:b[offset]=0xc3
        for addr,size,op,args in self.im.disasm(start,end):
            off=addr-self.im.base-start
            if op in ('call','jmp') and args.startswith('0x'):
                dest=int(args,16)-self.im.base
                if dest in addresses:struct.pack_into('<i',b,off+1,addresses[dest]-off-5)
            if addr-self.im.base==0x349c25:struct.pack_into('<i',b,off+3,0x700-off-7)
        with Code(bytes(b)) as code:
            fn=C.CFUNCTYPE(C.c_uint64,C.c_void_p,C.c_void_p,C.c_void_p,C.c_void_p,C.c_void_p)(code)
            fn(ptr(render),ptr(model),ptr(targets),C.addressof(node_ref),ptr(texture))
        self.assertEqual(keys,[(ptr(model)+0x60,(1872310964<<32)|0x12345)])
        self.assertEqual(calls,[(ptr(render),C.addressof(node_ref),ptr(texture),ptr(mapping),7)])

from attachment_cases import cases
AttachmentTests=cases((ROOT,DLL,Image,Code,pack,value,ptr,pattern))
from pistol_cases import cases as pistol_cases
PistolTests=pistol_cases((ROOT,DLL,Image,Code,pack,value,ptr,pattern))
from menu_icon_cases import cases as menu_cases
MenuTests=menu_cases((ROOT,DLL,Image,Code,pack,value,ptr,pattern,Fixture))

class RealGpuTests(unittest.TestCase):
    def test_menu_icon_gpu_upload_preserves_complete_bc7_alpha_data(self):
        child="import ctypes as C,sys; d=C.CDLL(sys.argv[1]); r=[d.ValidatePrivateIconGpu(i) for i in range(2)]; print('D3D12 BC7 menu readbacks:',r); assert r==[0,0]"
        r=subprocess.run([sys.executable,'-c',child,str(ROOT/'build/pistol_gpu_test.dll')],capture_output=True,text=True,timeout=20)
        self.assertEqual(r.returncode,0,r.stdout+r.stderr)
    def test_independent_d3d12_upload_reads_back_all_eleven_mips(self):
        child="import ctypes as C,sys; d=C.CDLL(sys.argv[1]); r=d.ValidatePistolGpu(); print('D3D12 full BC1 readback:',r); assert r==0"
        r=subprocess.run([sys.executable,'-c',child,str(ROOT/'build/pistol_gpu_test.dll')],capture_output=True,text=True,timeout=20)
        self.assertEqual(r.returncode,0,r.stdout+r.stderr)

if __name__=='__main__':unittest.main(verbosity=2)
