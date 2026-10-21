"""Native menu lookup ABI, isolated UI graph, and exact custom-icon routing."""
import ctypes as C
import json
import struct
import unittest
import uuid
from capstone import Cs,CS_ARCH_X86,CS_MODE_64
from capstone.x86_const import X86_OP_MEM,X86_REG_RIP

def cases(api):
 ROOT,DLL,Image,Code,pack,value,ptr,pattern,Fixture=api
 DLL.CustomMenuIcon.argtypes=[C.c_char_p,C.c_uint,C.c_uint];DLL.CustomMenuIcon.restype=C.c_int
 DLL.SetPrivateIcon.argtypes=[C.c_void_p,C.c_uint,C.c_void_p];DLL.SetPrivateIcon.restype=C.c_bool
 DLL.PrivateIconSize.restype=C.c_uint
 DLL.PrivateIconPart.argtypes=[C.c_void_p,C.c_uint];DLL.PrivateIconPart.restype=C.c_void_p
 DLL.BuildPrivateIcon.argtypes=[C.c_void_p,C.c_uint]+[C.c_void_p]*2+[C.c_uint64]*2;DLL.BuildPrivateIcon.restype=C.c_bool
 def code(s):
  h=0
  for b in s:
   h^=b
   for _ in range(8):h=(h>>1)^(0x82f63b78 if h&1 else 0)
  return h&0x7fffffff
 names=[b'ut_ui_icon_wep_shg0_v02_msg_red',b'ut_ui_icon_wep_hag6_msg_red']
 uuids=['4c1ed9f8-ad98-cd48-8777-5e0b143f233e','b11f5fa7-8dd1-144f-a384-72946f63f159']
 class MenuTests(unittest.TestCase):
  @classmethod
  def setUpClass(cls):cls.im=Image()
  def fixture(self,slot=0):
   g=C.create_string_buffer(DLL.PrivateIconSize());ui=C.create_string_buffer(0x38);hw=C.create_string_buffer(0x188)
   C.memmove(ptr(ui)+16,uuid.UUID(uuids[slot]).bytes_le,16)
   pack(ui,0x24,'I',256);pack(ui,0x28,'I',160);pack(ui,0x30,'Q',0xBAAD)
   pack(hw,0x29,'B',75);pack(hw,0x88,'Q',0xD00D);pack(hw,0x30,'I',0xA9)
   for i in range(3):C.c_uint64.from_address(DLL.PrivateIconPart(g,i)).value=0x200+i
   return g,ui,hw
  def test_exact_name_only_and_uncomputed_native_hash(self):
   for slot,name in enumerate(names):
    self.assertEqual(DLL.CustomMenuIcon(name,len(name),code(name)),slot)
    self.assertEqual(DLL.CustomMenuIcon(name,len(name),0xffffffff),slot)
    for text in [b'ut_ui_icon_wep_hag5',b'ut_ui_icon_wep_hag5_msg_red',name+b'_s',name[:-1]+b'x']:
     self.assertEqual(DLL.CustomMenuIcon(text,len(text),code(name)),-1)
    self.assertEqual(DLL.CustomMenuIcon(name,len(name),0),-1)
  def test_private_names_change_only_custom_new_weapon_lists(self):
   for k in range(4):
    text=C.create_string_buffer(names[max(0,k-2)])
    f=Fixture(k);self.assertTrue(f.prepare());b=f.out(1)
    self.assertEqual(DLL.SetPrivateIcon(f.output,k,text),k>=2)
    expected=bytearray(b)
    if k>=2:pack(expected,0x50,'Q',ptr(text))
    self.assertEqual(f.out(1),bytes(expected));self.assertEqual(bytes(f.parts[1,0]),f.original[1,0])
  def test_private_ui_resources_preserve_extent_and_keep_distinct_identities(self):
   identities=set()
   for slot in range(2):
    g,ui,hw=self.fixture(slot);before=bytes(ui),bytes(hw)
    self.assertTrue(DLL.BuildPrivateIcon(g,slot,ui,hw,0x111100,0x222200))
    a=[DLL.PrivateIconPart(g,i) for i in range(3)];u,t,h=[C.string_at(p,n) for p,n in zip(a,[0x38,0x70,0x188])]
    self.assertEqual((bytes(ui),bytes(hw)),before)
    self.assertEqual(value(u,0x30),a[1]);self.assertEqual(value(t,0x20),a[2]);self.assertEqual(value(t,0x28),0)
    self.assertEqual(u[0x24:0x2C],struct.pack('<II',256,160))
    self.assertEqual(value(h,0x2C,'I'),0x20000000|512|(320<<15));self.assertEqual(value(h,0x30,'I'),1)
    self.assertEqual(value(h,0x88),0);self.assertEqual(value(h,0x80),0x111100);self.assertEqual(value(h,0x90),0x222200)
    for x in [u,t]:
     self.assertEqual(value(x,8,'I'),1);self.assertEqual(value(x,12,'I'),0)
     self.assertNotIn(x[16:32],identities);identities.add(x[16:32])
    self.assertEqual(h[0x58],2);self.assertNotEqual(u[16:32],before[0][16:32])
  def test_unexpected_ui_identity_size_or_format_refused(self):
   for slot in range(2):
    for index,offset,fmt,v in [(1,16,'Q',0),(1,0x20,'B',1),(1,0x24,'I',512),(2,0x29,'B',66)]:
     vals=list(self.fixture(slot));pack(vals[index],offset,fmt,v);before=bytes(vals[0])
     self.assertFalse(DLL.BuildPrivateIcon(vals[0],slot,*vals[1:],0x111100,0x222200));self.assertEqual(bytes(vals[0]),before)
    vals=self.fixture(slot)
    self.assertFalse(DLL.BuildPrivateIcon(vals[0],1-slot,*vals[1:],0x111100,0x222200),'Cannot serve the other weapon icon')
  def test_actual_catalogue_lookup_and_relocated_entry_preserve_original_icons(self):
   start,end=0x17BE390,0x17BE4F1;b=bytearray(4096);b[:end-start]=self.im.read(start,end-start)
   def native_string(text):
    x=C.create_string_buffer(16+len(text)+1);pack(x,0,'I',1);pack(x,4,'I',code(text));pack(x,8,'I',len(text));C.memmove(ptr(x)+16,text,len(text));return x
   empty=native_string(b'');struct.pack_into('<Q',b,0x700,ptr(empty)+16)
   b[0x600:0x674]=self.im.read(0xA4600,0x74)
   md=Cs(CS_ARCH_X86,CS_MODE_64);md.detail=True
   self.assertEqual(self.im.read(start,5),b'\x48\x89\x5c\x24\x08')
   for ins in md.disasm(bytes(b[:end-start]),self.im.base+start):
    off=ins.address-self.im.base-start
    for op in ins.operands:
     if op.type==X86_OP_MEM and op.mem.base==X86_REG_RIP:struct.pack_into('<i',b,off+ins.disp_offset,0x700-off-ins.size)
    if ins.mnemonic=='call' and ins.op_str==hex(self.im.base+0xA4600):struct.pack_into('<i',b,off+ins.imm_offset,0x600-off-ins.size)
   # Execute the displaced prologue in its own trampoline, exactly as installed.
   b[0x800:0x805]=b[:5];b[0x805]=0xE9;struct.pack_into('<i',b,0x806,5-0x80A)
   export=json.loads((ROOT/'build/weapons-fixtures/pistol-icon/DSUICatalogueImageResource_56_64601.json').read_text())
   keys=(C.c_uint*len(export['ImageNameHash']))(*export['ImageNameHash']);images=(C.c_uint64*len(keys))(*[0x100000+i*0x100 for i in range(len(keys))])
   resource=C.create_string_buffer(0x188);pack(resource,0x20,'I',len(keys));pack(resource,0x28,'Q',ptr(images));pack(resource,0xD8,'I',len(keys));pack(resource,0xE0,'Q',ptr(keys));pack(resource,0xA0,'Q',0xABCDEF)
   with Code(bytes(b)) as location:
    fn=C.CFUNCTYPE(C.c_uint64,C.c_void_p,C.c_void_p)(location+0x800)
    for text in [b'ut_ui_icon_wep_hag5',b'ut_ui_icon_wep_asr2_enem_gm2',b'ut_ui_icon_wep_shg0_v02',b'ut_ui_icon_wep_hag6']+names:
     s=native_string(text);sp=C.c_void_p(ptr(s)+16);result=fn(resource,C.byref(sp))
     expected=images[list(keys).index(code(text))] if code(text) in keys else 0xABCDEF
     self.assertEqual(result,expected);self.assertEqual(value(s,0,'I'),1,'Lookup returns a borrowed pointer')
 return MenuTests
