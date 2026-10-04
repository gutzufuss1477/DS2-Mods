#pragma once
#include "core.hpp"
namespace equipment {
struct BranchSpec {u32 rva,notEqual;u8 length,cmpLength,reg,kind;u8 expected[16];};
// kind: 0=constant subtype; 1=DL subtype; 2=BPL subtype; 3=exclude target.
struct Code {
 u8* start;u32 size=0;
 void b(u8 x){start[size++]=x;}
 void d(u32 x){for(u32 i=0;i<4;++i)b(u8(x>>(i*8)));}
 void q(u64 x){for(u32 i=0;i<8;++i)b(u8(x>>(i*8)));}
 void bytes(const u8*p,u32 n){for(u32 i=0;i<n;++i)b(p[i]);}
 void jump(u64 target){b(0xE9);d(u32(target-(u64(start)+size+4)));}
 void absolute(u64 target){b(0xFF);b(0x25);d(0);q(target);}
 u32 jcc(u8 cc){b(0x0F);b(cc);u32 p=size;d(0);return p;}
 void fix(u32 p,u32 target){u32 v=target-(p+4);for(u32 i=0;i<4;++i)start[p+i]=u8(v>>(i*8));}
 void cmpMem(u8 reg,u8 offset,u8 value){b(0x80);b(reg?0x79:0x78);b(offset);b(value);}
};
inline u32 branchCode(u8* output, const BranchSpec& s, u64 image) {
 Code c{output};
 c.b(0x9C); // Preserve input flags until we decide which comparison to execute.
 c.cmpMem(s.reg,0x20,SkeletonId);u32 other=c.jcc(0x85);
 c.cmpMem(s.reg,0x22,6);u32 category=c.jcc(0x85);
 c.cmpMem(s.reg,0x23,1);u32 subtype=c.jcc(0x85);
 u32 notRequested=0,requested=0;
 if(s.kind==1 || s.kind==2) {
  if(s.kind==2)c.b(0x40);c.b(0x80);c.b(s.kind==1?0xFA:0xFD);c.b(2);
  requested=c.jcc(0x84);
  if(s.kind==2)c.b(0x40);c.b(0x80);c.b(s.kind==1?0xFA:0xFD);c.b(3);
  notRequested=c.jcc(0x85);c.fix(requested,c.size);
 }
 c.b(0x9D);
 // Set genuine comparison flags: zero on union, nonzero on excluded battle speed.
 c.cmpMem(s.reg,0x20,s.kind==3?0:SkeletonId);
 c.jump(image+(s.kind==3?s.notEqual:s.rva+s.length));
 c.fix(other,c.size);c.fix(category,c.size);c.fix(subtype,c.size);
 if(notRequested)c.fix(notRequested,c.size);
 c.b(0x9D);c.bytes(s.expected,s.cmpLength);
 c.b(0x0F);c.b(0x85);c.d(u32(image+s.notEqual-(u64(output)+c.size+4)));
 c.jump(image+s.rva+s.length);
 return c.size;
}
// EA0681 publishes BoostSkeletonIsActive only after native active/battery gates.
// R12 is the active skeleton parameter. Keep the original SETE and all registers
// and flags; additionally publish true for the selected combined skeleton.
inline u32 boostFactCode(u8* output,u64 resume) {
 Code c{output};
 const u8 original[]={0x0F,0x94,0x44,0x24,0x60};c.bytes(original,5);
 c.b(0x9C);
 const u8 cmpId[]={0x41,0x80,0x7C,0x24,0x20,SkeletonId};c.bytes(cmpId,6);u32 other=c.jcc(0x85);
 const u8 cmpCat[]={0x41,0x80,0x7C,0x24,0x22,6};c.bytes(cmpCat,6);u32 category=c.jcc(0x85);
 const u8 cmpSub[]={0x41,0x80,0x7C,0x24,0x23,1};c.bytes(cmpSub,6);u32 subtype=c.jcc(0x85);
 // PUSHFQ moved the original stack slot by eight bytes.
 const u8 set[]={0xC6,0x44,0x24,0x68,1};c.bytes(set,5);
 c.fix(other,c.size);c.fix(category,c.size);c.fix(subtype,c.size);
 c.b(0x9D);c.jump(resume);return c.size;
}
struct JumpGateSpec {u32 rva;u8 length,reg;bool prompt;u8 expected[8];};
constexpr u32 SkeletonVisualRva=0xEC70F6;
constexpr u8 SkeletonVisualBytes[]={0x0F,0xB6,0xC0,0x83,0xC0,0xEA};
// EC7010 caches the REAL equipment ID before this point. Only its local art
// selector (AL) and visibility subtype (R14D) are mapped to Boost Lv.3. The
// parameter resource and cached identity remain ATLAS, including forced refresh.
inline u32 skeletonVisualCode(u8* output,u64 resume,u8 skeletonVisualId){
 Code c{output};
 c.b(0x3C);c.b(SkeletonId);u32 other=c.jcc(0x85);
 const u8 nonNull[]={0x48,0x85,0xD2};c.bytes(nonNull,3);u32 absent=c.jcc(0x84);
 const u8 cat[]={0x80,0x7A,0x22,6};c.bytes(cat,4);u32 category=c.jcc(0x85);
 const u8 sub[]={0x80,0x7A,0x23,1};c.bytes(sub,4);u32 subtype=c.jcc(0x85);
 c.b(0xB0);c.b(skeletonVisualId); // local item selector AL
 const u8 boost[]={0x41,0xBE,2,0,0,0};c.bytes(boost,6); // local mesh group R14D
 c.fix(other,c.size);c.fix(absent,c.size);c.fix(category,c.size);c.fix(subtype,c.size);
 c.bytes(SkeletonVisualBytes,sizeof(SkeletonVisualBytes)); // also restores native arithmetic flags
 c.jump(resume);return c.size;
}
constexpr JumpGateSpec JumpGates[]={
 // Only extend JumpLoop's native UI/input window. Extending JumpStart showed
 // the prompt during the boost takeoff animation, before a second jump worked.
 // Keep 106ACAD native; JumpLoop retains its animation/flight eligibility gates.
 {0x1066BC5,7,0,true,{0x83,0xB8,0xDC,1,0,0,5}},
 // The shared input predicate separately rejects kind 4 after checking the
 // jump press, input suppression, special context and already-consumed latch.
 {0xFEF23B,8,10,false,{0x41,0x83,0xBA,0xDC,1,0,0,4}},
};
constexpr u32 JumpGateCount=sizeof(JumpGates)/sizeof(JumpGates[0]);
// Preserve the boost jump kind/physics. Change only the comparison's ZF for a
// worn target during kind 4; the native code still owns the input and jump.
inline u32 doubleJumpCode(u8* output,u64 resume,const JumpGateSpec& s) {
 Code c{output};
 c.bytes(s.expected,s.length);
 const u8 save[]={0x9C,0x50,0x51,0x52};c.bytes(save,4);
 if(s.reg>=8)c.b(0x41);c.b(0x83);c.b(0xB8+(s.reg&7));c.d(0x1DC);c.b(4);
 u32 nativePass=c.jcc(0x85);
 c.b(s.reg>=8?0x49:0x48);c.b(0x8B);c.b(0x40+(s.reg&7));c.b(0x30);
 const u8 nonNull[]={0x48,0x85,0xC0};c.bytes(nonNull,3);u32 noEntity=c.jcc(0x84);
 const u8 equip[]={0x48,0x8B,0x80,0xD0,0x56,0,0,0x48,0x85,0xC0};c.bytes(equip,10);u32 noEquip=c.jcc(0x84);
 const u8 first[]={0x48,0x8D,0x90,0x90,0x1A,0,0,0xB9,0x7F,0,0,0};c.bytes(first,12);
 u32 loop=c.size;
 const u8 worn[]={0x80,0x3A,0};c.bytes(worn,3);u32 notWorn=c.jcc(0x84);
 const u8 param[]={0x48,0x8B,0x42,0x10,0x48,0x85,0xC0};c.bytes(param,7);u32 noParam=c.jcc(0x84);
 c.cmpMem(0,0x20,SkeletonId);u32 other=c.jcc(0x85);
 c.cmpMem(0,0x22,6);u32 category=c.jcc(0x85);
 c.cmpMem(0,0x23,1);u32 subtype=c.jcc(0x85);
 // Change only ZF in the saved result of the original comparison.
 const u8 allow[]={0x48,0x83,u8(s.prompt?0x4C:0x64),0x24,0x18,u8(s.prompt?0x40:0xBF)};c.bytes(allow,6);
 c.b(0xE9);u32 success=c.size;c.d(0);
 c.fix(notWorn,c.size);c.fix(noParam,c.size);c.fix(other,c.size);c.fix(category,c.size);c.fix(subtype,c.size);
 const u8 next[]={0x48,0x83,0xC2,0x60,0xFF,0xC9};c.bytes(next,6);u32 again=c.jcc(0x85);c.fix(again,loop);
 c.fix(success,c.size);c.fix(noEntity,c.size);c.fix(noEquip,c.size);c.fix(nativePass,c.size);
 const u8 restore[]={0x5A,0x59,0x58,0x9D};c.bytes(restore,4);
 c.jump(resume);return c.size;
}
}
