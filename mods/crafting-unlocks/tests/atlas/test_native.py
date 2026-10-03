"""Native CPU tests of generated branch caves and capacity/state combination.

Loads a separate test-only DLL. Does not load the mod, open or write the game.
"""
import ctypes as C
import base64
import hashlib
import json
from pathlib import Path
import struct
import unittest
from itertools import product
import sys

from native_scan import Image, ROOT as WORKSPACE

ROOT=Path(__file__).resolve().parents[2]
DLL=C.CDLL(str(ROOT/'build/atlas_tests.dll'))
DLL.AtlasFabrication.argtypes=[C.c_void_p,C.c_bool]
DLL.TestLoading.argtypes=[C.c_void_p,C.c_void_p,C.c_float,C.c_float,C.c_float]
DLL.TestLoading.restype=C.c_bool
DLL.TestBootValues.argtypes=[C.POINTER(C.c_float)];DLL.TestBootValues.restype=C.c_bool
DLL.TestHash.argtypes=[C.c_void_p,C.c_uint,C.c_void_p]
DLL.ExpectedHash.argtypes=[C.c_void_p]
DLL.PrepareBranch.argtypes=[C.c_uint];DLL.PrepareBranch.restype=C.c_void_p
DLL.FreeBranch.argtypes=[C.c_void_p]
DLL.PrepareBoostFact.restype=C.c_void_p
DLL.PrepareDoubleJump.argtypes=[C.c_uint];DLL.PrepareDoubleJump.restype=C.c_void_p
DLL.PrepareNativeJumpInput.argtypes=[C.c_void_p,C.c_uint,C.c_bool];DLL.PrepareNativeJumpInput.restype=C.c_void_p
DLL.TestThunk.argtypes=[C.c_void_p]*4;DLL.TestThunk.restype=C.c_void_p
DLL.CapturedArgument.argtypes=[C.c_uint];DLL.CapturedArgument.restype=C.c_void_p
DLL.TestPreview.argtypes=[C.c_ubyte]+[C.c_void_p]*4;DLL.TestPreview.restype=C.c_bool
DLL.PreviewDescription.argtypes=[C.c_void_p];DLL.PreviewDescription.restype=C.c_void_p
DLL.PreviewChart.argtypes=[C.c_void_p];DLL.PreviewChart.restype=C.c_void_p
DLL.TestAtlas.argtypes=[C.c_void_p,C.c_bool]+[C.c_void_p]*8+[C.c_bool];DLL.TestAtlas.restype=C.c_bool
DLL.AtlasPart.argtypes=[C.c_void_p,C.c_uint];DLL.AtlasPart.restype=C.c_void_p
DLL.AtlasLanguage.argtypes=[C.c_void_p,C.c_bool]
DLL.TestAtlasVisuals.argtypes=[C.c_void_p]*3;DLL.TestAtlasVisuals.restype=C.c_bool
DLL.PrepareSkeletonVisual.restype=C.c_void_p
DLL.PrepareNativeUnlock.argtypes=[C.c_void_p,C.c_uint];DLL.PrepareNativeUnlock.restype=C.c_void_p
DLL.PrepareHipValidation.argtypes=[C.c_void_p,C.c_uint,C.c_bool];DLL.PrepareHipValidation.restype=C.c_void_p
DLL.TestPrepareDepot.argtypes=[C.c_void_p,C.c_void_p,C.c_uint,C.c_bool,C.c_bool];DLL.TestPrepareDepot.restype=C.c_bool
DLL.TestDepotAllocationBytes.restype=C.c_uint64
DLL.TestPublishDepot.argtypes=[C.c_void_p];DLL.TestPublishDepot.restype=C.c_bool
SITES=json.loads((ROOT/'tests/atlas/fixtures/patch-sites.json').read_text())

def item(id=104,level=2,cat=6,sub=1):
 p=C.create_string_buffer(0xa0);struct.pack_into('4B',p,0x20,id,level,cat,sub);return p

def loading(powered=True):
 p=C.create_string_buffer(bytes([0xA5])*0x100,0x100)
 for off,value in [(0,265.),(4,305.),(0x4c,100.),(0x50,100.)]:struct.pack_into('<f',p,off,value)
 p[0x6c]=1;p[0x6d]=int(powered);struct.pack_into('<I',p,0x80,2)
 return p

class NativeTests(unittest.TestCase):
 def test_original_hip_validation_allows_atlas_without_displacing_cargo(self):
  raw=Image().read(0x123827c,0x1417-0x127c);buffer=C.create_string_buffer(raw,len(raw));pages=[]
  moving=C.create_string_buffer(0x12008);removed=C.create_string_buffer(0x12008)
  owner=C.create_string_buffer(24*8);slots={i:C.create_string_buffer(0x200) for i in [6,7,17]}
  skeleton_bag=C.create_string_buffer(0x120);cargo=[C.create_string_buffer(0x120) for _ in range(3)]
  group=C.create_string_buffer(0x120);group_ptr=(C.c_uint64*1)(C.addressof(group));bag_ptr=(C.c_uint64*1)(C.addressof(skeleton_bag))
  struct.pack_into('<I',group,0x108,1);struct.pack_into('<Q',group,0x110,C.addressof(bag_ptr))
  for id,s in slots.items():
   s[0]=id;struct.pack_into('<Q',s,8,C.addressof(owner));struct.pack_into('<Q',owner,id*8,C.addressof(s))
  struct.pack_into('<Q',slots[17],0x18,C.addressof(group_ptr));struct.pack_into('<I',moving,0x12000,1)
  count=0
  try:
   for patched in [False,True]:
    page=DLL.PrepareHipValidation(buffer,len(raw),patched);self.assertTrue(page);pages.append(page)
    fn=C.CFUNCTYPE(C.c_ubyte,C.c_void_p,C.c_void_p,C.c_void_p)(page)
    # Equipping a skeleton with occupied hip slots, with/without explicit swaps.
    for id,sub in [(104,1),(21,1),(22,1),(23,1),(24,2),(26,2),(27,3),(29,3)]:
     p=item(id=id,sub=sub);struct.pack_into('<Q',skeleton_bag,0xc8,C.addressof(p))
     struct.pack_into('<Q',moving,0,C.addressof(skeleton_bag))
     for occupied,swapped in product(range(4),range(4)):
      rm=[]
      for i,slot in enumerate([6,7]):
       ptr=C.addressof(cargo[i]);struct.pack_into('<Q',slots[slot],0x38,ptr if occupied&(1<<i) else 0)
       if swapped&(1<<i):rm.append(ptr)
      struct.pack_into('<I',removed,0x12000,len(rm))
      for i,ptr in enumerate(rm):struct.pack_into('<Q',removed,i*8,ptr)
      before=[b.raw for b in [moving,removed,skeleton_bag,p,*slots.values(),*cargo,group,owner]]
      blocked=sub==1 and not (patched and id==104) and bool(occupied&~swapped)
      self.assertEqual(fn(moving,slots[17],removed),4 if blocked else 0,(patched,id,occupied,swapped))
      self.assertEqual([b.raw for b in [moving,removed,skeleton_bag,p,*slots.values(),*cargo,group,owner]],before)
      count+=1
    # Putting ordinary cargo on either hip while a skeleton remains equipped.
    for slot,(id,sub),equipped,swap in product([6,7],[(104,1),(21,1),(23,1),(26,2),(29,3)],[False,True],[False,True]):
     p=item(id=id,sub=sub);struct.pack_into('<Q',skeleton_bag,0xc8,C.addressof(p))
     struct.pack_into('<I',slots[17],0x10,int(equipped));struct.pack_into('<Q',moving,0,C.addressof(cargo[2]))
     struct.pack_into('<I',removed,0x12000,int(swap));struct.pack_into('<Q',removed,0,C.addressof(skeleton_bag))
     blocked=equipped and sub==1 and not swap and not (patched and id==104)
     self.assertEqual(fn(moving,slots[slot],removed),4 if blocked else 0,(patched,slot,id,equipped,swap))
     count+=1
  finally:
   for page in pages:DLL.FreeBranch(page)
  self.assertEqual(count,336)

 def test_skeleton_visual_selector_keeps_identity_and_effects(self):
  im=Image();self.assertEqual(im.read(0xec70f6,6),bytes.fromhex('0f b6 c0 83 c0 ea'))
  # Native case 26 selects the ordinary Boost Lv.3 APV at array index 7.
  self.assertEqual(struct.unpack('<I',im.read(0xec7520+(26-22)*4,4))[0],0xec719c)
  self.assertEqual(im.read(0xec71a7,4),bytes.fromhex('48 8b 71 38'))
  page=DLL.PrepareSkeletonVisual();self.assertTrue(page)
  fn=C.CFUNCTYPE(None,C.c_void_p,C.c_void_p,C.c_uint)(page);out=C.create_string_buffer(24)
  try:
   for id,cat,sub in product(range(256),[5,6,8],range(1,8)):
    p=item(id=id,cat=cat,sub=sub);before=p.raw;local_sub=sub if cat==6 else 255
    fn(p,out,local_sub);selector,group,ptr,flags=struct.unpack('<IIQQ',out.raw)
    target=id==104 and cat==6 and sub==1;visual=26 if target else id
    self.assertEqual(selector,(visual-22)&0xffffffff)
    self.assertEqual(group,2 if target else local_sub)
    self.assertEqual(ptr,C.addressof(p));self.assertEqual(p.raw,before)
    self.assertEqual(bool(flags&0x40),visual==22)
   fn(None,out,255);self.assertEqual(struct.unpack('<IIQ',out.raw[:16]),(0xffffffea,255,0))
  finally:DLL.FreeBranch(page)

 def test_native_unlock_predicate_accepts_atlas_with_locked_story(self):
  im=Image();raw=im.read(0xbe91a0,0x67);buffer=C.create_string_buffer(raw,len(raw))
  page=DLL.PrepareNativeUnlock(buffer,len(raw));self.assertTrue(page)
  fn=C.CFUNCTYPE(C.c_ubyte,C.c_void_p)(page)
  try:
   for fixture in json.loads((ROOT/'tests/atlas/fixtures/atlas-native-source-fixtures.json').read_text(encoding='utf8')):
    data=[C.create_string_buffer(base64.b64decode(x)) for x in fixture['data']]
    out=C.create_string_buffer(DLL.AtlasSize());boot=fixture['id']==11
    self.assertEqual(fn(data[3]),0) # Original recipe has a story fact, currently false.
    # Simulate every donor DLC combination: none can leak into ATLAS.
    for dlc in range(8):
     data[3][0x74]=dlc
     self.assertTrue(DLL.TestAtlas(out,boot,*data,True))
     recipe=DLL.AtlasPart(out,3);self.assertEqual(fn(recipe),1)
     self.assertEqual(C.c_ubyte.from_address(recipe+0x52).value,1)
     for off in [0x38,0x40,0x48]:self.assertEqual(C.c_uint64.from_address(recipe+off).value,0)
     self.assertEqual(C.c_ubyte.from_address(recipe+0x74).value,0)
     self.assertEqual(C.string_at(recipe+0x58,16),data[3].raw[0x58:0x68])
  finally:DLL.FreeBranch(page)

 def test_fabrication_toggle_preserves_saved_identities_and_gameplay(self):
  for fixture in json.loads((ROOT/'tests/atlas/fixtures/atlas-native-source-fixtures.json').read_text(encoding='utf8')):
   data=[C.create_string_buffer(base64.b64decode(x)) for x in fixture['data']]
   out=C.create_string_buffer(DLL.AtlasSize());boot=fixture['id']==11
   self.assertTrue(DLL.TestAtlas(out,boot,*data,True))
   original=out.raw;recipe=DLL.AtlasPart(out,3)
   offset=recipe-C.addressof(out)+0x52
   for enabled in [False,True,False,True]:
    DLL.AtlasFabrication(out,enabled)
    expected=bytearray(original);expected[offset]=int(enabled)
    self.assertEqual(out.raw,bytes(expected))

 def test_visual_donors_only_change_art_fields(self):
  for fixture in json.loads((ROOT/'tests/atlas/fixtures/atlas-native-source-fixtures.json').read_text(encoding='utf8')):
   data=[C.create_string_buffer(base64.b64decode(x)) for x in fixture['data']]
   boot=fixture['id']==11;out=C.create_string_buffer(DLL.AtlasSize())
   self.assertTrue(DLL.TestAtlas(out,boot,*data,True))
   dp=C.create_string_buffer(bytes([0xa5])*0xa0,0xa0);dl=C.create_string_buffer(bytes([0xb6])*0xc0,0xc0)
   struct.pack_into('4B',dp,0x20,20 if boot else 26,0 if boot else 2,5 if boot else 6,5 if boot else 2)
   parts=[DLL.AtlasPart(out,i) for i in range(7)];sizes=[0xa0,0xc0,0x88,0xb0,0x38,0x38,0x30]
   before=[C.string_at(p,n) for p,n in zip(parts,sizes)];donors=(dp.raw,dl.raw)
   self.assertTrue(DLL.TestAtlasVisuals(out,dp,dl))
   allowed=[set(range(0x38,0x40))|set(range(0x78,0x98)),set(range(0x50,0x5c))]+[set()]*5
   for i,(p,n) in enumerate(zip(parts,sizes)):
    after=C.string_at(p,n)
    for offset in range(n):
     self.assertEqual(after[offset],(dp.raw if i==0 else dl.raw)[offset] if offset in allowed[i] else before[i][offset],(i,offset))
   self.assertEqual((dp.raw,dl.raw),donors)
   saved=out.raw;dp[0x20]=11 if boot else 21
   self.assertFalse(DLL.TestAtlasVisuals(out,dp,dl));self.assertEqual(out.raw,saved)

 def test_depot_growth_stages_fresh_allocation_before_publishing(self):
  for fail_alloc,fail_copy in [(False,False),(True,False),(False,True)]:
   old=(C.c_uint64*2)(0x123456,0xabcdef);depot=C.create_string_buffer(0x48);plan=C.create_string_buffer(32)
   struct.pack_into('<iiQ',depot,0x38,2,2,C.addressof(old));before=depot.raw
   ok=DLL.TestPrepareDepot(plan,depot,2,fail_alloc,fail_copy)
   replacement=struct.unpack_from('<Q',plan,16)[0]
   try:
    self.assertEqual(ok,not fail_alloc and not fail_copy)
    self.assertEqual(depot.raw,before);self.assertEqual(list(old),[0x123456,0xabcdef])
    self.assertEqual(DLL.TestDepotAllocationBytes(),32)
    if ok:
     self.assertNotEqual(replacement,C.addressof(old));self.assertEqual(C.string_at(replacement,16),bytes(old))
     # Concurrent/source changes must prevent publication.
     struct.pack_into('<i',depot,0x38,1);changed=depot.raw
     self.assertFalse(DLL.TestPublishDepot(plan));self.assertEqual(depot.raw,changed)
     struct.pack_into('<i',depot,0x38,2);self.assertTrue(DLL.TestPublishDepot(plan))
     self.assertEqual(struct.unpack_from('<iiQ',depot,0x38),(2,4,replacement))
     self.assertEqual(list(old),[0x123456,0xabcdef])
   finally:
    if replacement:DLL.FreeBranch(replacement)

 def test_atlas_accepts_captured_game_resources_without_item_list_id_conflation(self):
  fixtures=json.loads((ROOT/'tests/atlas/fixtures/atlas-native-source-fixtures.json').read_text(encoding='utf-8'))
  for fixture in fixtures:
   raw=[base64.b64decode(x) for x in fixture['data']]
   data=[C.create_string_buffer(x,len(x)) for x in raw]
   boot=fixture['id']==11;out=C.create_string_buffer(DLL.AtlasSize())
   self.assertTrue(fixture['bag_links_to_list'])
   self.assertTrue(DLL.TestAtlas(out,boot,*data,True),fixture['name'])
   self.assertEqual([x.raw for x in data],raw)
   item=DLL.AtlasPart(out,0);li=DLL.AtlasPart(out,1)
   self.assertEqual(C.c_ubyte.from_address(item+0x20).value,103 if boot else 104)
   self.assertEqual(C.c_uint32.from_address(li+0x40).value,1000103 if boot else 1000104)
   # The incorrect v0.2.0 assumption must now be rejected, not accepted.
   struct.pack_into('<I',data[1],0x40,fixture['id'])
   self.assertFalse(DLL.TestAtlas(out,boot,*data,True))

 def test_double_jump_gate_only_waives_boost_rejection_for_worn_target(self):
  for gate in range(2):
   address=DLL.PrepareDoubleJump(gate);self.assertTrue(address)
   fn=C.CFUNCTYPE(C.c_uint,C.c_void_p,C.c_void_p)(address)
   context=C.create_string_buffer(0x200);entity=C.create_string_buffer(0x56d8)
   equip=C.create_string_buffer(0x1a20+128*0x60);regs=C.create_string_buffer(48)
   struct.pack_into('<Q',context,0x30,C.addressof(entity));struct.pack_into('<Q',entity,0x56d0,C.addressof(equip))
   native=lambda kind: kind==5 if gate==0 else kind!=4
   try:
    baseline={}
    for kind in [-1,0,1,2,3,4,5,6]:
     struct.pack_into('<i',context,0x1dc,kind)
     self.assertEqual(fn(context,regs),int(native(kind)));baseline[kind]=struct.unpack_from('<Q',regs,40)[0]
    for slot in [0,1,50,127]:
     entry=0x1a20+slot*0x60
     for id,cat_sub,worn,kind in product(range(128),[(6,1),(5,1),(6,2),(6,3),(8,1)],[0,1],[3,4,5]):
      cat,sub=cat_sub;p=item(id=id,cat=cat,sub=sub)
      struct.pack_into('<Q',equip,entry+0x20,C.addressof(p));equip[entry+0x10]=worn
      struct.pack_into('<i',context,0x1dc,kind)
      target=slot!=0 and worn and id==104 and cat==6 and sub==1
      before=(context.raw,entity.raw,equip.raw,p.raw)
      self.assertEqual(fn(context,regs),int(native(kind) or (kind==4 and target)),(gate,slot,id,cat,sub,worn,kind))
      values=struct.unpack('<6Q',regs)
      self.assertEqual(values[:5],(C.addressof(context) if gate==0 else 0x11111111,0x22222222,0x33333333,C.addressof(context),C.addressof(context)))
      flags=baseline[kind]
      if kind==4 and target:flags=flags|0x40 if gate==0 else flags&~0x40
      self.assertEqual(values[5],flags)
      self.assertEqual((context.raw,entity.raw,equip.raw,p.raw),before)
     equip[entry+0x10]=0
    struct.pack_into('<i',context,0x1dc,4)
    struct.pack_into('<Q',entity,0x56d0,0);self.assertEqual(fn(context,regs),0)
    struct.pack_into('<Q',context,0x30,0);self.assertEqual(fn(context,regs),0)
   finally:DLL.FreeBranch(address)

 def test_complete_native_jump_input_preserves_press_and_consumed_gates(self):
  original=Image().read(0xfef200,0x52)
  raw=C.create_string_buffer(original,len(original));pages=[]
  context=C.create_string_buffer(0x200);entity=C.create_string_buffer(0x56d8)
  equip=C.create_string_buffer(0x1a20+128*0x60);inputs=C.create_string_buffer(0x7320)
  special=C.create_string_buffer(1);p=item()
  for buf,off,ptr in [(context,0x30,entity),(context,0x28,inputs),(entity,0x56d0,equip),(equip,0x1a20+50*0x60+0x20,p)]:
   struct.pack_into('<Q',buf,off,C.addressof(ptr))
  entry=0x1a20+50*0x60
  try:
   for patched in [False,True]:
    page=DLL.PrepareNativeJumpInput(raw,len(original),patched);self.assertTrue(page);pages.append(page)
    fn=C.CFUNCTYPE(C.c_ubyte,C.c_void_p)(page)
    for id,kind,worn,consumed,press,suppressed,special_state in product([104,23,26,29,30],[3,4,5],[0,1],[0,1],[0,1],[0,1],[None,0,1]):
     p[0x20]=id;equip[entry+0x10]=worn
     struct.pack_into('<i',context,0x1dc,kind);context[0x1cc]=consumed
     inputs[0x2c0]=8 if press else 0;inputs[0x7314]=8 if suppressed else 0
     struct.pack_into('<Q',context,0xc0,0 if special_state is None else C.addressof(special));special[0]=special_state or 0
     expected=not consumed and press and not suppressed and special_state!=1 and (kind!=4 or (patched and worn and id==104))
     before=(context.raw,entity.raw,equip.raw,inputs.raw,p.raw)
     self.assertEqual(fn(context),int(expected),(patched,id,kind,worn,consumed,press,suppressed,special_state))
     self.assertEqual((context.raw,entity.raw,equip.raw,inputs.raw,p.raw),before)
  finally:
   for page in pages:DLL.FreeBranch(page)

 def test_boost_fact_preserves_native_flags_and_other_equipment(self):
  address=DLL.PrepareBoostFact();self.assertTrue(address)
  fn=C.CFUNCTYPE(C.c_uint,C.c_void_p,C.c_ubyte)(address)
  try:
   for id in range(128):
    for cat in [5,6,8]:
     for sub in range(1,8):
      for query in [1,2,3]:
       target=id==104 and cat==6 and sub==1
       original=query==2
       self.assertEqual(fn(item(id=id,cat=cat,sub=sub),query),int(original or target)|(int(original)<<8))
  finally:DLL.FreeBranch(address)

 def test_preview_clones_leave_shared_resources_untouched(self):
  resources=json.loads((ROOT/'tests/atlas/fixtures/native-preview-resources.json').read_text())
  for id,expected in [(103,[8,8,8,16,8,12]),(104,[10,16,16,4,16,12])]:
   original=next(x for x in resources if x['id']==(11 if id==103 else 21))
   desc=C.create_string_buffer(bytes([0x31])*0x38,0x38)
   chart=C.create_string_buffer(bytes([0x42])*0x30,0x30);struct.pack_into('<i',chart,0x20,6)
   params=C.create_string_buffer(6*0x30)
   for i,row in enumerate(original['charts']):
    struct.pack_into('<B',params,i*0x30+0x20,row['type']);struct.pack_into('<i',params,i*0x30+0x24,row['value'])
   before=[x.raw for x in [desc,chart,params]];out=C.create_string_buffer(DLL.PreviewSize())
   self.assertTrue(DLL.TestPreview(id,desc,chart,params,out))
   self.assertEqual([x.raw for x in [desc,chart,params]],before)
   dp=DLL.PreviewDescription(out);cp=DLL.PreviewChart(out)
   self.assertNotEqual(dp,C.addressof(desc));self.assertNotEqual(cp,C.addressof(chart))
   textptr=C.c_uint64.from_address(dp+0x20).value;length=C.c_uint32.from_address(dp+0x28).value
   text=C.string_at(textptr,length).decode('utf-8');self.assertIn('ATLAS-Ausrüstung',text)
   self.assertEqual(C.c_uint32.from_address(dp+8).value,1)
   self.assertEqual(C.c_uint32.from_address(cp+8).value,1)
   if id==104:self.assertIn('+180 kg',text);self.assertIn('Packgröße: L',text)
   array=C.c_uint64.from_address(cp+0x28).value
   for i,value in enumerate(expected):
    pp=C.c_uint64.from_address(array+8*i).value
    self.assertTrue(C.addressof(out)<=pp<C.addressof(out)+C.sizeof(out))
    self.assertEqual(C.c_int32.from_address(pp+0x24).value,value)
    self.assertEqual(C.c_ubyte.from_address(pp+0x20).value,original['charts'][i]['type'])
    self.assertEqual(C.c_uint32.from_address(pp+8).value,1)
   # Conflicting diagram values / a dynamic IndexFact / other items must fail.
   for other in [14,23,30,39]:self.assertFalse(DLL.TestPreview(other,desc,chart,params,out))
   struct.pack_into('<Q',params,0x28,1);self.assertFalse(DLL.TestPreview(id,desc,chart,params,out))
   struct.pack_into('<Q',params,0x28,0);struct.pack_into('<i',params,0x24,999)
   self.assertFalse(DLL.TestPreview(id,desc,chart,params,out))

 def test_atlas_independent_resource_graph_and_localization(self):
  resources=json.loads((ROOT/'tests/atlas/fixtures/native-preview-resources.json').read_text())
  all_codes=set();all_uuids=set()
  for boot in [True,False]:
   source=11 if boot else 21;new=103 if boot else 104
   source_bag=0x45711b80 if boot else 0x56d383f7
   source_list=0x4b48c47b if boot else 0x58ea5c0c
   source_recipe=0x6e7d4315 if boot else 0x7ddfdb62
   data=[C.create_string_buffer(n) for n in [0xa0,0xc0,0x88,0xb0,0x38,0x38,0x30,6*0x30]]
   p,l,b,r,n,d,c,rows=data
   struct.pack_into('4B',p,0x20,source,0,5 if boot else 6,5 if boot else 1)
   struct.pack_into('<II',l,0x40,1000001 if boot else 1000011,source_list)
   struct.pack_into('<I',b,0x44,source_bag);struct.pack_into('<I',r,0x20,source_recipe)
   for buf,off,ptr in [(p,0x40,l),(b,0x50,l),(r,0x28,b)]:struct.pack_into('<Q',buf,off,C.addressof(ptr))
   struct.pack_into('<I',c,0x20,6)
   for i,row in enumerate(next(x for x in resources if x['id']==source)['charts']):
    struct.pack_into('<B',rows,i*0x30+0x20,row['type']);struct.pack_into('<i',rows,i*0x30+0x24,row['value'])
   # Distinct packed/worn references and costs must survive the clone.
   p[0x78:0x98]=bytes(range(32));r[0x58:0x68]=bytes(range(16));b[0x4d]=4
   before=[x.raw for x in data];out=C.create_string_buffer(DLL.AtlasSize())
   self.assertTrue(DLL.TestAtlas(out,boot,*data,True));self.assertEqual(before,[x.raw for x in data])
   parts=[DLL.AtlasPart(out,i) for i in range(7)]
   read=lambda i,off,fmt:struct.unpack(fmt,C.string_at(parts[i]+off,struct.calcsize(fmt)))[0]
   text=lambda i:C.string_at(read(i,0x20,'<Q'),read(i,0x28,'<I')).decode('utf-8')
   self.assertEqual(read(0,0x20,'B'),new);self.assertEqual(read(1,0x40,'<I'),1000103 if boot else 1000104)
   for i,off,target in [(0,0x40,1),(0,0x28,4),(0,0x30,5),(1,0x20,4),(1,0x28,5),(1,0x98,6),(2,0x50,1),(3,0x28,2)]:
    self.assertEqual(read(i,off,'<Q'),parts[target])
   self.assertEqual(C.string_at(parts[0]+0x78,32),p.raw[0x78:0x98])
   self.assertEqual(C.string_at(parts[3]+0x58,16),r.raw[0x58:0x68])
   self.assertEqual(read(2,0x4d,'B'),4)
   self.assertAlmostEqual(read(2,0x60,'<f'),.2 if boot else 4)
   self.assertEqual(read(2,0x64,'<I'),3400 if boot else 20000)
   self.assertEqual(text(4),'ATLAS-Stiefel' if boot else 'ATLAS-Skelett')
   self.assertIn('ATLAS-Ausrüstung',text(5))
   for i,off in [(1,0x44),(2,0x44),(3,0x20)]:
    code=read(i,off,'<I');self.assertNotIn(code,all_codes);self.assertNotIn(code,[source_list,source_bag,source_recipe]);all_codes.add(code)
   for ptr in parts:
    uuid=C.string_at(ptr+0x10,16);self.assertNotIn(uuid,all_uuids);all_uuids.add(uuid)
   for german in [False,True,False]:
    DLL.AtlasLanguage(out,german)
    self.assertEqual(text(4),('ATLAS-Stiefel' if boot else 'ATLAS-Skelett') if german else ('ATLAS Boots' if boot else 'ATLAS Skeleton'))
   self.assertEqual(before,[x.raw for x in data])
   p[0x20]=new;self.assertFalse(DLL.TestAtlas(out,boot,*data,True))

 def test_atlas_hook_sites_and_relocated_prologues(self):
  im=Image()
  self.assertEqual(im.read(0x1ec3db7,5),bytes.fromhex('e8 64 8d ca fe'))
  for addr,raw in [(0xf430e0,'48 89 5c 24 20'),(0xf43e30,'40 53 55 57 48 81 ec d0 00 00 00')]:
   expected=bytes.fromhex(raw);self.assertEqual(im.read(addr,len(expected)),expected)
   instructions=list(im.disasm(addr,addr+len(expected)))
   self.assertEqual(sum(x[1] for x in instructions),len(expected))
   self.assertFalse(any('rip' in args or op.startswith('j') or op=='call' for _,_,op,args in instructions))

 def test_loading_thunk_passes_preview_inventory_and_replays_lea(self):
  for i in range(100):
   args=[0x100000+i*256+j*16 for j in range(4)]
   self.assertEqual(DLL.TestThunk(*args),args[3]+0x1a9a)
   self.assertEqual([DLL.CapturedArgument(j) for j in range(3)],args[:3])

 def test_branch_truth_tables_on_cpu(self):
  total=0
  self.assertEqual(DLL.BranchCount(),len(SITES))
  for i,spec in enumerate(SITES):
   address=DLL.PrepareBranch(i);self.assertTrue(address,spec['name'])
   fn=C.CFUNCTYPE(C.c_uint,C.c_void_p,C.c_ubyte)(address)
   try:
    for id in range(128):
     for cat in [5,6,8]:
      for sub in range(1,8):
       for query in ([0,1,2,3,4,5,6,7] if spec['kind'] in [1,2] else [0]):
        p=item(id=id,cat=cat,sub=sub)
        native=bytes.fromhex(spec['expected'])[3] if spec['kind'] in [0,3] else query
        target=(id==104 and cat==6 and sub==1)
        expected=(sub==native)
        if target:
         if spec['kind']==3:expected=False
         elif spec['kind']==0 or query in [2,3]:expected=True
        self.assertEqual(fn(p,query),int(expected),(spec['name'],id,cat,sub,query))
        total+=1
   finally:DLL.FreeBranch(address)
  print(f'Native branch cases executed: {total}')

 def test_capacity_max_keeps_other_bonuses(self):
  p=loading();before=p.raw
  self.assertTrue(DLL.TestLoading(p,item(),100,80,180))
  for off,value in [(0,345.),(4,385.),(0x4c,180.),(0x50,180.)]:self.assertEqual(struct.unpack_from('<f',p,off)[0],value)
  for off in [0x66,0x68,0x69,0x6a]:self.assertEqual(p[off],b'\1')
  self.assertEqual(struct.unpack_from('<II',p,0x78),(2,2))
  allowed=set()
  for off,n in [(0,8),(0x4c,8),(0x66,1),(0x68,3),(0x78,8)]:allowed.update(range(off,off+n))
  for off in range(0x100):
   if off not in allowed:self.assertEqual(p[off],before[off:off+1],hex(off))

 def test_empty_battery_keeps_native_passive_battle_capacity(self):
  p=loading(False);self.assertTrue(DLL.TestLoading(p,item(),100,80,180))
  self.assertEqual(struct.unpack_from('<f',p,0)[0],265)
  self.assertEqual(p[0x68],b'\0');self.assertEqual(p[0x6a],b'\0')

 def test_other_equipment_and_invalid_data_are_untouched(self):
  for other in [item(id=x) for x in range(128) if x!=104]+[item(cat=5),item(sub=2),item(level=0)]:
   p=loading();before=p.raw
   self.assertFalse(DLL.TestLoading(p,other,100,80,180));self.assertEqual(p.raw,before)
  for bad in [float('nan'),float('inf'),-1,1001]:
   p=loading();before=p.raw
   self.assertFalse(DLL.TestLoading(p,item(),bad,80,180));self.assertEqual(p.raw,before)
  p=loading();p[0x6c]=0;before=p.raw
  self.assertFalse(DLL.TestLoading(p,item(),100,80,180));self.assertEqual(p.raw,before)

 def test_boot_conflicts_are_rejected(self):
  values=(C.c_float*6)(0,1,1,1,1,0);self.assertTrue(DLL.TestBootValues(values))
  values=(C.c_float*6)(1,.5,1.2,.5,.5,0);self.assertTrue(DLL.TestBootValues(values))
  values[5]=1;self.assertFalse(DLL.TestBootValues(values))
  values[5]=0;values[2]=99;self.assertFalse(DLL.TestBootValues(values))

 def test_hash_guard_against_standard_library(self):
  for data in [b'',b'abc',bytes(range(256))*257]:
   out=C.create_string_buffer(32);DLL.TestHash(data,len(data),out)
   self.assertEqual(out.raw,hashlib.sha256(data).digest())
  expected=C.create_string_buffer(32);DLL.ExpectedHash(expected)
  with (WORKSPACE/'analysis/DS2.exe').open('rb') as f:
   self.assertEqual(expected.raw,hashlib.file_digest(f,'sha256').digest())

if __name__=='__main__':unittest.main(verbosity=2)
