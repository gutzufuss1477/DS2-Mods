"""Regression tests for private model resources and real native attachment ABI.

Native fragments run in this test process with engine/GPU work stubbed. They
cannot establish in-game stability or visible placement.
"""
import ctypes as C
import json
import struct
import unittest
import uuid
from capstone import Cs, CS_ARCH_X86, CS_MODE_64
from capstone.x86_const import X86_OP_MEM, X86_REG_RIP


def cases(api):
    ROOT,DLL,Image,Code,pack,value,ptr,pattern=api
    DLL.AttachmentSize.restype=C.c_uint
    DLL.AttachmentPart.argtypes=[C.c_void_p,C.c_uint];DLL.AttachmentPart.restype=C.c_void_p
    DLL.BuildAttachment.argtypes=[C.c_void_p]*6;DLL.BuildAttachment.restype=C.c_bool
    DLL.BuildAttachmentForVariant.argtypes=[C.c_void_p,C.c_uint]+[C.c_void_p]*5
    DLL.BuildAttachmentForVariant.restype=C.c_bool
    DLL.AttachmentEligible.argtypes=[C.c_int,C.c_uint64,C.c_uint64,C.c_bool,C.c_bool]
    DLL.AttachmentEligible.restype=C.c_bool

    class AttachmentTests(unittest.TestCase):
        @classmethod
        def setUpClass(cls):
            cls.im=Image()
            cls.md=Cs(CS_ARCH_X86,CS_MODE_64);cls.md.detail=True

        def fragment(self,start,end,callbacks=None):
            b=bytearray(4096);b[:end-start]=self.im.read(start,end-start)
            location=0x500
            stubs={}
            for target,fn in (callbacks or {}).items():
                stubs[target]=location
                b[location:location+12]=b'\x48\xb8'+struct.pack('<Q',C.cast(fn,C.c_void_p).value)+b'\xff\xe0'
                location+=0x20
            for ins in self.md.disasm(bytes(b[:end-start]),self.im.base+start):
                off=ins.address-self.im.base-start
                for op in ins.operands:
                    if op.type==X86_OP_MEM and op.mem.base==X86_REG_RIP:
                        target=ins.address+ins.size+op.mem.disp-self.im.base
                        b[location:location+64]=self.im.read(target,64)
                        struct.pack_into('<i',b,off+ins.disp_offset,location-off-ins.size)
                        location+=0x60
                if ins.mnemonic in ('call','jmp','jne') and ins.op_str.startswith('0x'):
                    target=int(ins.op_str,16)-self.im.base
                    if target in stubs:
                        self.assertEqual(ins.imm_size,4)
                        struct.pack_into('<i',b,off+ins.imm_offset,stubs[target]-off-ins.size)
            self.assertLess(location,4096)
            return bytes(b)

        def fixture(self,variant=1):
            g=C.create_string_buffer(DLL.AttachmentSize())
            addresses=[DLL.AttachmentPart(g,i) for i in range(7)]
            for index,start,end,rtti in [(0,0x2febc0,0x2febf9,True),(1,0x181a00,0x181a93,False),(2,0x28b4b0,0x28b589,False)]:
                with Code(self.fragment(start,end)) as code:
                    if rtti:C.CFUNCTYPE(None,C.c_void_p,C.c_void_p)(code)(None,addresses[index])
                    else:C.CFUNCTYPE(C.c_void_p,C.c_void_p)(code)(addresses[index])
            s,d,m,p=[pattern(n) for n in [0xc8,0xb8,0xa0,0xe0]]
            for off in [0x28,0x38,0x50,0x70,0x98]:pack(s,off,'I',0)
            for off in [0xa8,0xb8]:pack(s,off,'Q',0)
            for off in [0x28,0x60,0x78,0xac]:pack(d,off,'I',0)
            pack(s,0x60,'Q',ptr(d));pack(d,0x20,'Q',ptr(m))
            pack(p,0x80,'I',0);pack(p,0xd0,'I',1434199023)
            helper_files={1:'suppressor-model/SkeletonHelpers_20719_287.json',
                          2:'arsenal-0.5.0/SkeletonHelpers_19337_516.json',
                          3:'arsenal-0.5.0/SkeletonHelpers_499_78806.json'}
            j=json.loads((ROOT/'build/weapons-fixtures'/helper_files[variant]).read_text())
            h=next(x for x in j['Helpers'] if x['Name']=='HLP_MuzzleFlash')['Link']
            link=C.create_string_buffer(80)
            C.memmove(ptr(link),uuid.UUID(h['Joint'][13:-1]).bytes_le,16)
            for col in range(4):
                for i,key in enumerate('XYZW'):pack(link,16+col*16+i*4,'f',h['Offset'][f'Col{col}'][key])
            return g,addresses,(s,d,m,p,link)

        def test_new_attachments_use_their_own_animated_muzzle_and_unique_resources(self):
            identities=set()
            for variant,resource_slot,scale in [(1,1,1.0),(2,3,1.0),(3,4,0.65)]:
                g,a,donors=self.fixture(variant);before=[bytes(x) for x in donors]
                self.assertTrue(DLL.BuildAttachmentForVariant(g,variant,*donors))
                self.assertEqual([bytes(x) for x in donors],before)
                for p in a:
                    identity=C.string_at(p+16,16)
                    self.assertNotIn(identity,identities);identities.add(identity)
                    self.assertEqual(identity[0],resource_slot)
                mover=C.string_at(a[2],0x130)
                self.assertEqual(mover[0x80:0xd0],before[4])
                for off in [0x30,0x44,0x58]:self.assertAlmostEqual(value(mover,off,'f'),scale)
                self.assertEqual(value(mover,0x6c,'f'),1.0)
                self.assertAlmostEqual(value(mover,0x64,'f'),-0.444*scale)
                self.assertAlmostEqual(value(mover,0x68,'f'),-0.065*scale)
                self.assertTrue(DLL.AttachmentEligible(variant,(1<<27)|(1<<28),0x434b600,True,False))
            for bad in [0,4,99]:
                g,_,donors=self.fixture();before=bytes(g)
                self.assertFalse(DLL.BuildAttachmentForVariant(g,bad,*donors));self.assertEqual(bytes(g),before)

        def test_private_graph_preserves_donors_and_native_skeletons(self):
            g,a,donors=self.fixture();before=[bytes(x) for x in donors]
            self.assertTrue(DLL.BuildAttachment(g,*donors))
            self.assertEqual([bytes(x) for x in donors],before)
            headers=[C.string_at(x,32) for x in a]
            self.assertEqual(len({h[16:32] for h in headers}),7)
            for h in headers:
                self.assertEqual(value(h,8,'I'),1)
                self.assertEqual(value(h,12,'I'),0,'Copied native streaming ownership must be cleared')
            for index in [2,6]:self.assertEqual(a[index]%16,0)
            s,d,m,p=[C.string_at(a[i],n) for i,n in zip([3,4,5,6],[0xc8,0xb8,0xa0,0xe0])]
            for off in [0x68,0x88,0x90]:self.assertEqual(value(s,off),value(before[0],off))
            for off in [0x48,0x50]:self.assertEqual(value(m,off),value(before[2],off))
            self.assertEqual(p[0x20:0xd4],before[3][0x20:0xd4])
            self.assertEqual(value(s,0x60),a[4]);self.assertEqual(s[0x80],1)
            self.assertEqual(value(d,0x20),a[5]);self.assertEqual(value(d,0xa8,'I'),1)
            self.assertEqual(C.string_at(value(m,0x28),8),struct.pack('<Q',a[6]))
            self.assertEqual(value(m,0x38,'I'),1);self.assertEqual(value(m,0x40),value(m,0x28))
            self.assertEqual(p[0xd4],0)
            child=C.string_at(a[0],0x50);entity=C.string_at(a[1],0xb8);mover=C.string_at(a[2],0x130)
            self.assertEqual(value(child,0x20),a[1]);self.assertEqual(value(child,0x28),a[2])
            self.assertEqual(child[0x30:0x4b],bytes(0x1b),'No facts/spawn scripts/nonexclusive dependency')
            self.assertEqual(entity[0x58:],bytes(0x60),'Dispatch caches are fresh, never cloned')
            self.assertEqual(value(entity,0x48,'I'),2)
            self.assertEqual(C.string_at(value(entity,0x50),16),struct.pack('<QQ',a[3],a[4]))
            self.assertEqual(entity[0x22],0);self.assertEqual(entity[0x3c],1)
            self.assertEqual(mover[0x80:0xd0],before[4])

        def test_unexpected_donor_graph_is_rejected_without_writes(self):
            for donor,offset,fmt,val in [(0,0x60,'Q',0),(0,0x70,'I',1),(0,0xa8,'Q',123),
                                         (1,0xac,'I',1),(2,0x48,'Q',0),(3,0x80,'I',1),(3,0xd0,'I',7)]:
                g,a,donors=self.fixture();pack(donors[donor],offset,fmt,val);before=bytes(g)
                self.assertFalse(DLL.BuildAttachment(g,*donors));self.assertEqual(bytes(g),before)

        def test_attachment_requires_owned_player_weapon_and_live_render_model(self):
            valid=[1,(1<<27)|(1<<28),0x434b600,True,False]
            self.assertTrue(DLL.AttachmentEligible(*valid))
            for index,bad_values in [(0,[-1,0,4]),(1,[0,1<<27,1<<28,valid[1]|(1<<45)]),
                                     (2,[0,0x44c02e0]),(3,[False]),(4,[True])]:
                for v in bad_values:
                    args=valid.copy();args[index]=v;self.assertFalse(DLL.AttachmentEligible(*args))

        def test_native_create_component_message_passes_owner_resource_and_uuid(self):
            calls=[];message=C.create_string_buffer(24);pack(message,16,'Q',0x223344)
            @C.CFUNCTYPE(C.c_void_p,C.c_void_p)
            def generate(p):C.memmove(p,b'0123456789ABCDEF',16);return p
            @C.CFUNCTYPE(C.c_void_p,C.c_void_p,C.c_void_p,C.c_void_p)
            def create(owner,resource,identifier):calls.append((owner,resource,C.string_at(identifier,16)));return 0x112233
            with Code(self.fragment(0x13e030,0x13e064,{0x2070190:generate,0x1484a0:create})) as code:
                C.CFUNCTYPE(C.c_void_p,C.c_void_p,C.c_void_p)(code)(0x123456,ptr(message))
            self.assertEqual(calls,[(0x123456,0x223344,b'0123456789ABCDEF')])

        def test_native_child_cleanup_removes_only_its_live_child(self):
            calls=[];lookups=[];skip=[False]
            component=C.create_string_buffer(0x70);child=C.create_string_buffer(0xb0)
            @C.CFUNCTYPE(C.c_void_p,C.c_void_p,C.c_void_p)
            def lookup(owner,t):lookups.append(owner);return 1 if skip[0] else 0
            @C.CFUNCTYPE(None,C.c_void_p)
            def remove(owner):calls.append(owner)
            with Code(self.fragment(0x308590,0x3085d6,{0x11ffa0:lookup,0x134d60:remove})) as code:
                fn=C.CFUNCTYPE(None,C.c_void_p)(code)
                fn(ptr(component));self.assertFalse(calls);self.assertFalse(lookups)
                pack(component,0x50,'Q',ptr(child)+0x20);fn(ptr(component))
                self.assertEqual(calls,[ptr(child)]);self.assertEqual(lookups,[ptr(child)+0xa0])
                skip[0]=True;fn(ptr(component));self.assertEqual(calls,[ptr(child)])

        def test_native_child_initialization_defers_spawn_until_world_membership(self):
            calls=[];component=C.create_string_buffer(0x70);owner=C.create_string_buffer(0xa0)
            pack(component,0x48,'Q',ptr(owner))
            @C.CFUNCTYPE(None,C.c_void_p)
            def spawn(p):calls.append(p)
            with Code(self.fragment(0x3085f0,0x30860d,{0x2fe2d0:spawn})) as code:
                fn=C.CFUNCTYPE(None,C.c_void_p)(code)
                fn(ptr(component));self.assertFalse(calls);self.assertEqual(component[0x38],b'\1')
                pack(owner,0x98,'Q',1<<28);fn(ptr(component));self.assertEqual(calls,[ptr(component)])

    return AttachmentTests
