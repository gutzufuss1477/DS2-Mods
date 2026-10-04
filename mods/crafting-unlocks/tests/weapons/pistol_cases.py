"""Private graph, native descriptor lookup and native slot allocator regressions."""
import ctypes as C
import json
import struct
import unittest
import uuid
from capstone import Cs, CS_ARCH_X86, CS_MODE_64
from capstone.x86_const import X86_OP_MEM, X86_REG_RIP

def cases(api):
 ROOT,DLL,Image,Code,pack,value,ptr,pattern=api
 DLL.PistolSize.restype=C.c_uint
 DLL.PistolPart.argtypes=[C.c_void_p,C.c_uint];DLL.PistolPart.restype=C.c_void_p
 DLL.BuildPistol.argtypes=[C.c_void_p]*5+[C.c_uint64]*2;DLL.BuildPistol.restype=C.c_bool
 class PistolTests(unittest.TestCase):
  @classmethod
  def setUpClass(cls):cls.im=Image()
  def fixture(self):
   g=C.create_string_buffer(DLL.PistolSize());s,e,c,h=[C.create_string_buffer(n) for n in [0x70,72,0x70,0x188]]
   pack(s,0,'Q',0x111);pack(s,8,'Q',0x12300000002);pack(s,32,'I',3);pack(s,0x28,'Q',ptr(e));pack(s,0x68,'Q',0x445500)
   C.memmove(ptr(s)+16,uuid.UUID('67854c25-047e-3b4d-9055-a927e91564b2').bytes_le,16)
   C.memmove(ptr(c)+16,uuid.UUID('c58fa041-7cea-243a-8af9-56298569783b').bytes_le,16)
   pack(c,0,'Q',0x222);pack(c,8,'Q',0x99800000002);pack(c,0x20,'Q',ptr(h));pack(c,0x30,'Q',0x9988776655443322)
   pack(e,8,'Q',0xAA000);pack(e,24,'I',807473409);pack(e,32,'Q',ptr(c));pack(e,56,'Q',0xBB000)
   pack(h,0x29,'B',0x42);pack(h,0x2C,'I',0x22000400);pack(h,0x30,'I',0xA00000A9)
   pack(h,0x38,'Q',0xBAD100);pack(h,0x88,'Q',0xBAD200);pack(h,0x90,'Q',0xBAD300)
   for i in [1,2]:C.c_uint64.from_address(DLL.PistolPart(g,i)).value=0x999+i
   return g,s,e,c,h
  def test_private_pistol_preserves_normal_roughness_and_originals(self):
   g,s,e,c,h=self.fixture();before=[bytes(x) for x in [s,e,c,h]]
   self.assertTrue(DLL.BuildPistol(g,s,e,c,h,0x123456,0xABCDEF))
   self.assertEqual([bytes(x) for x in [s,e,c,h]],before)
   a=[DLL.PistolPart(g,i) for i in range(4)];ss,cc,hh,ee=[C.string_at(p,n) for p,n in zip(a,[112,112,392,72])]
   self.assertEqual(value(ss,0x28),a[3]);self.assertEqual(value(ee,32),a[1]);self.assertEqual(value(cc,32),a[2])
   self.assertEqual(ee[:24],before[1][:24]);self.assertEqual(ee[48:],before[1][48:])
   for x in [ss,cc]:self.assertEqual(value(x,8,'I'),1);self.assertEqual(value(x,12,'I'),0)
   self.assertEqual(cc[0x30:0x58],bytes(40));self.assertEqual(ss[0x30:0x48],bytes(24))
   self.assertEqual(value(hh,0x88),0,'Never share the streamed placed allocation')
   self.assertEqual(value(hh,0x38),0);self.assertEqual(value(hh,0x40),0)
   self.assertEqual(value(hh,0x80),0x123456);self.assertEqual(value(hh,0x90),0xABCDEF)
   self.assertEqual(value(hh,0x30,'I'),0xA0000001);self.assertEqual(value(hh,0x6C,'I'),0)
  def test_wrong_texture_or_missing_upload_is_refused(self):
   for idx,off,fmt,v in [(1,16,'Q',0),(1,32,'I',2),(2,24,'I',0),(2,32,'Q',0),(3,16,'Q',0),(4,0x29,'B',0)]:
    values=list(self.fixture());pack(values[idx],off,fmt,v);before=bytes(values[0])
    self.assertFalse(DLL.BuildPistol(*values,0x123456,0xABCDEF));self.assertEqual(bytes(values[0]),before)
   for resource,descriptor in [(0,1),(1,0)]:self.assertFalse(DLL.BuildPistol(*self.fixture(),resource,descriptor))
  def test_native_cpu_descriptor_lookup_uses_our_resident_view(self):
   g,s,e,c,h=self.fixture();self.assertTrue(DLL.BuildPistol(g,s,e,c,h,0x123456,0xABCDEF))
   manager=C.create_string_buffer(0x84A870);pack(manager,0x84A868,'I',127)
   start,end=0x21136D0,0x2113822;b=bytearray(4096);b[:end-start]=self.im.read(start,end-start)
   struct.pack_into('<Q',b,0x800,ptr(manager));md=Cs(CS_ARCH_X86,CS_MODE_64);md.detail=True
   for ins in md.disasm(bytes(b[:end-start]),self.im.base+start):
    for op in ins.operands:
     if op.type==X86_OP_MEM and op.mem.base==X86_REG_RIP:
      target=ins.address+ins.size+op.mem.disp-self.im.base
      # All other branches are deliberately unreachable for our resident view.
      if target==0x623FBC8:struct.pack_into('<i',b,ins.address-self.im.base-start+ins.disp_offset,0x800-(ins.address-self.im.base-start)-ins.size)
   mips=(C.c_uint*2)(0,255)
   with Code(bytes(b)) as code:
    fn=C.CFUNCTYPE(C.c_uint64,C.c_void_p,C.c_ubyte,C.c_void_p,C.c_uint64)(code)
    for buffer in [0,1]:self.assertEqual(fn(DLL.PistolPart(g,2),buffer,mips,0),0xABCDEF)
   self.assertEqual(C.c_uint.from_address(DLL.PistolPart(g,2)+0x34).value,127)
   with Code(self.im.read(0x21124D0,8)) as code:
    ready=C.CFUNCTYPE(C.c_bool,C.c_void_p)(code)
    self.assertTrue(ready(DLL.PistolPart(g,2)))
  def test_native_bindless_allocator_unique_and_exhaustion(self):
   pool=C.create_string_buffer(24);free=(C.c_uint*4)(1,2,3,4)
   pack(pool,0,'Q',1);pack(pool,8,'Q',ptr(free));pack(pool,0x14,'I',4)
   with Code(self.im.read(0x2071DF0,0x7D)) as code:
    fn=C.CFUNCTYPE(C.c_int,C.c_void_p)(code)
    self.assertEqual([fn(pool) for _ in range(4)],[1,2,3,-1]);self.assertEqual(value(pool,0x10,'I'),3)
  def test_lod_element_stride_matches_supported_executable(self):
   self.assertEqual(struct.unpack('<I',self.im.read(0x45078D0+16,4))[0],32)
 return PistolTests
