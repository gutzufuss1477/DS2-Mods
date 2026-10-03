#include "../../src/atlas/win_api.hpp"
#include "../../src/atlas/branch.hpp"
#include "../../src/atlas/sha256.hpp"
#include "../../src/atlas/atlas.hpp"
using namespace equipment;
#include "../../src/atlas/sites.generated.hpp"
extern "C" int _fltused=0;
extern "C" void* memcpy(void*d,const void*s,SIZE_T n){auto*x=(u8*)d;auto*y=(const u8*)s;while(n--)*x++=*y++;return d;}
extern "C" void* memset(void*d,int v,SIZE_T n){auto*x=(u8*)d;while(n--)*x++=u8(v);return d;}
extern "C" BOOL WINAPI DllMain(HMODULE,DWORD,void*){return 1;}
#define EXPORT extern "C" __declspec(dllexport)
void* captured[3];
extern "C" void CombineLoading(void* output,void* state,void* inventory){captured[0]=output;captured[1]=state;captured[2]=inventory;}
EXPORT void* CapturedArgument(u32 index){return index<3?captured[index]:nullptr;}
EXPORT bool TestPreview(u8 id,void*desc,void*chart,void*params,void*out){
 return preparePreview(*(PreviewBundle*)out,id,(const u8*)desc,(const u8*)chart,*(const u8(*)[6][0x30])params);
}
EXPORT u32 PreviewSize(){return sizeof(PreviewBundle);}
EXPORT u32 AtlasSize(){return sizeof(AtlasBundle);}
bool failDepotAllocate,failDepotCopy;u64 depotAllocationBytes;
u64 TestDepotAllocate(u64 n){depotAllocationBytes=n;return failDepotAllocate?0:u64(VirtualAlloc(nullptr,n,0x3000,4));}
bool TestDepotCopy(u64 from,void*to,u64 n){if(failDepotCopy)return false;memcpy(to,(void*)from,n);return true;}
EXPORT bool TestPrepareDepot(void*out,void*depot,u32 extra,bool failAllocate,bool failCopy){
 failDepotAllocate=failAllocate;failDepotCopy=failCopy;depotAllocationBytes=0;
 return prepareDepotCopy(*(DepotPlan*)out,u64(depot),extra,&TestDepotAllocate,&TestDepotCopy);
}
EXPORT u64 TestDepotAllocationBytes(){return depotAllocationBytes;}
EXPORT bool TestPublishDepot(void*out){return publishDepot(*(DepotPlan*)out);}
EXPORT bool TestAtlas(void*out,bool boot,void*item,void*list,void*bag,void*recipe,void*name,void*desc,void*chart,void*rows,bool german){
 return prepareAtlas(*(AtlasBundle*)out,boot,(u8*)item,(u8*)list,(u8*)bag,(u8*)recipe,(u8*)name,(u8*)desc,(u8*)chart,*(const u8(*)[6][0x30])rows,german);
}
EXPORT void AtlasLanguage(void*out,bool german){atlasLanguage(*(AtlasBundle*)out,german);}
EXPORT bool TestAtlasVisuals(void*out,const void*item,const void*list){return atlasVisuals(*(AtlasBundle*)out,(const u8*)item,(const u8*)list);}
EXPORT void* AtlasPart(void*out,u32 part){auto&b=*(AtlasBundle*)out;void*parts[]={b.item,b.list,b.bag,b.recipe,b.name,b.preview.description,b.preview.chart};return part<7?parts[part]:nullptr;}
EXPORT void* PreviewDescription(void*out){return ((PreviewBundle*)out)->description;}
EXPORT void* PreviewChart(void*out){return ((PreviewBundle*)out)->chart;}
EXPORT bool TestLoading(void*output,const void*item,float battle,float boost,float bokka){return combineLoading(output,item,battle,boost,bokka);}
EXPORT bool TestBootValues(const float* values){return bootParametersCompatible(values);}
EXPORT void TestHash(const void*data,u32 count,u8*out){craft::Sha256 hash;hash.update(data,count);hash.finish(out);}
EXPORT void ExpectedHash(u8*out){memcpy(out,ExecutableDigest,32);}
EXPORT u32 BranchCount(){return sizeof(Branches)/sizeof(Branches[0]);}
// Return a callable fixture containing the ACTUAL generated native machine code.
// This runs in the test process, never in the game process.
EXPORT void* PrepareBranch(u32 index){
 if(index>=BranchCount())return nullptr;
 auto*page=(u8*)VirtualAlloc(nullptr,4096,0x3000,4);if(!page)return nullptr;
 Code entry{page};entry.b(0x55); // preserve RBP (requested subtype is BPL in kind 2)
 const u8 setup[]={0x48,0x89,0xD5,0x48,0x89,0xC8}; // rbp=rdx; rax=rcx
 entry.bytes(setup,sizeof(setup));entry.jump(u64(page)+0x200);
 Code yes{page+0x100};yes.b(0x5D);yes.b(0xB8);yes.d(1);yes.b(0xC3);
 Code no{page+0x110};no.b(0x5D);no.b(0x31);no.b(0xC0);no.b(0xC3);
 auto spec=Branches[index];spec.rva=0x100-spec.length;spec.notEqual=0x110;
 branchCode(page+0x200,spec,u64(page));
 DWORD old;if(!VirtualProtect(page,4096,0x20,&old)||!FlushInstructionCache(GetCurrentProcess(),page,4096)){VirtualFree(page,0,0x8000);return nullptr;}
 return page;
}
EXPORT void FreeBranch(void* p){VirtualFree(p,0,0x8000);}
EXPORT void* PrepareSkeletonVisual(){
 auto*page=(u8*)VirtualAlloc(nullptr,4096,0x3000,4);if(!page)return nullptr;
 // Test ABI: item, result, original local subtype. Preserve host R14 and return
 // the selector, visibility group, parameter pointer and native output flags.
 Code e{page};const u8 setup[]={0x41,0x56,0x49,0x89,0xD1,0x48,0x89,0xCA,0x45,0x89,0xC6,0x31,0xC0,0x48,0x85,0xD2};
 e.bytes(setup,sizeof(setup));u32 absent=e.jcc(0x84);
 const u8 id[]={0x0F,0xB6,0x42,0x20};e.bytes(id,4);e.fix(absent,e.size);e.jump(u64(page)+0x200);
 Code end{page+0x100};const u8 result[]={0x41,0x89,0x01,0x45,0x89,0x71,4,0x49,0x89,0x51,8,0x9C,0x41,0x8F,0x41,0x10,0x41,0x5E,0xC3};end.bytes(result,sizeof(result));
 if(skeletonVisualCode(page+0x200,u64(page)+0x100)>256){VirtualFree(page,0,0x8000);return nullptr;}
 DWORD old;if(!VirtualProtect(page,4096,0x20,&old)||!FlushInstructionCache(GetCurrentProcess(),page,4096)){VirtualFree(page,0,0x8000);return nullptr;}
 return page;
}
// Execute the original unlock predicate. Its three external queries are
// relocated to locked/no-DLC stubs, and its globals to empty test state.
EXPORT void* PrepareNativeUnlock(const u8*original,u32 length){
 if(length!=0x67 || original[0x18]!=0xE8 || original[0x29]!=0xE9 || original[0x5B]!=0xE9)return nullptr;
 auto*page=(u8*)VirtualAlloc(nullptr,4096,0x3000,4);if(!page)return nullptr;
 memcpy(page,original,length);
 const u32 branches[]={0x18,0x29,0x5B},globals[]={0x42,0x49};
 for(u32 off:branches){Code c{page+off};c.b(off==0x18?0xE8:0xE9);c.d(u32((page+(off==0x18?0x200:0x210))-(page+off+5)));}
 for(u32 off:globals)at<i32>(page,off+3)=i32(0x300-(off+7));
 const u8 dlc[]={0xC7,0x01,0,0,0,0,0x31,0xC0,0xC3};memcpy(page+0x200,dlc,sizeof(dlc));
 const u8 locked[]={0x31,0xC0,0xC3};memcpy(page+0x210,locked,sizeof(locked));
 DWORD old;if(!VirtualProtect(page,4096,0x20,&old)||!FlushInstructionCache(GetCurrentProcess(),page,4096)){VirtualFree(page,0,0x8000);return nullptr;}
 return page;
}
// Original reciprocal hip/skeleton validation sections of 12372C0. The normal
// capacity/type checks precede these sections and are untouched in production.
// Only slot lookup and synchronization calls are redirected to fixture helpers.
EXPORT void* PrepareHipValidation(const u8*original,u32 length,bool patched){
 constexpr u32 begin=0x127C,end=0x1417,base=0x1237000;
 if(length!=end-begin)return nullptr;
 auto*page=(u8*)VirtualAlloc(nullptr,0x4000,0x3000,4);if(!page)return nullptr;
 memcpy(page+begin,original,length);
 // ABI: moving-bag array RCX, destination slot RDX, swap/removal array R8.
 Code e{page};const u8 setup[]={0x53,0x56,0x57,0x41,0x54,0x41,0x56,0x41,0x57,0x48,0x83,0xEC,0x28,
  0x48,0x89,0xCB,0x49,0x89,0xD7,0x4D,0x89,0xC4,0x0F,0xB6,0x3A,0x83,0xFF,0x11};
 e.bytes(setup,sizeof(setup));u32 skeleton=e.jcc(0x84);e.jump(u64(page)+0x1351);
 e.fix(skeleton,e.size);e.jump(u64(page)+begin);
 Code done{page+0x37C};done.b(0x31);done.b(0xC0);
 const u8 restore[]={0x48,0x83,0xC4,0x28,0x41,0x5F,0x41,0x5E,0x41,0x5C,0x5F,0x5E,0x5B,0xC3};done.bytes(restore,sizeof(restore));
 Code invalid{page+0x14E6};invalid.b(0xB0);invalid.b(2);invalid.jump(u64(page)+0x37E);
 Code next{page+end};next.jump(u64(page)+0x37C);
 const u32 lookup[]={0x12AC,0x12C1,0x1362},bag[]={0x12B4,0x12C9},lock[]={0x1374,0x1381,0x13AF,0x1411};
 for(u32 off:lookup){Code c{page+off};c.b(0xE8);c.d(u32(0x2000-off-5));}
 for(u32 off:bag){Code c{page+off};c.b(0xE8);c.d(u32(0x2010-off-5));}
 for(u32 off:lock){Code c{page+off};c.b(0xE8);c.d(u32(0x2020-off-5));c.b(0x90);}
 const u8 getslot[]={0x0F,0xB6,0xD2,0x48,0x8B,0x04,0xD1,0xC3};memcpy(page+0x2000,getslot,sizeof(getslot));
 const u8 getbag[]={0x48,0x8B,0x41,0x38,0xC3};memcpy(page+0x2010,getbag,sizeof(getbag));
 const u8 unlocked[]={0xB0,1,0xC3};memcpy(page+0x2020,unlocked,sizeof(unlocked));
 u32 count=0;
 for(u32 i=0;i<BranchCount();++i){auto s=Branches[i];if(s.rva!=0x1238299 && s.rva!=0x12383C6)continue;
  u32 off=s.rva-base;
  for(u32 j=0;j<s.length;++j)if(page[off+j]!=s.expected[j]){VirtualFree(page,0,0x8000);return nullptr;}
  if(patched){
   auto*slot=page+0x2400+count*256;Code site{page+off};site.jump(u64(slot));while(site.size<s.length)site.b(0x90);
   s.rva-=base;s.notEqual-=base;if(branchCode(slot,s,u64(page))>256){VirtualFree(page,0,0x8000);return nullptr;}
  }
  ++count;
 }
 if(count!=2){VirtualFree(page,0,0x8000);return nullptr;}
 DWORD old;if(!VirtualProtect(page,0x4000,0x20,&old)||!FlushInstructionCache(GetCurrentProcess(),page,0x4000)){VirtualFree(page,0,0x8000);return nullptr;}
 return page;
}
EXPORT void* PrepareBoostFact(){
 auto*page=(u8*)VirtualAlloc(nullptr,4096,0x3000,4);if(!page)return nullptr;
 // Save R12, allocate an original-frame stack slot, and pass item/query.
 Code e{page};const u8 setup[]={0x41,0x54,0x48,0x83,0xEC,0x70,0x49,0x89,0xCC,0x80,0xFA,2};
 e.bytes(setup,sizeof(setup));e.jump(u64(page)+0x200);
 // Return the stored boolean in bit 0 and the preserved input ZF in bit 8.
 Code end{page+0x100};const u8 result[]={0x0F,0x94,0xC2,0x0F,0xB6,0xC2,0xC1,0xE0,8,0x0F,0xB6,0x4C,0x24,0x60,0x09,0xC8,0x48,0x83,0xC4,0x70,0x41,0x5C,0xC3};
 end.bytes(result,sizeof(result));boostFactCode(page+0x200,u64(page)+0x100);
 DWORD old;if(!VirtualProtect(page,4096,0x20,&old)||!FlushInstructionCache(GetCurrentProcess(),page,4096)){VirtualFree(page,0,0x8000);return nullptr;}
 return page;
}
EXPORT void* PrepareDoubleJump(u32 index){
 if(index>=JumpGateCount)return nullptr;const auto&s=JumpGates[index];
 auto*page=(u8*)VirtualAlloc(nullptr,4096,0x3000,4);if(!page)return nullptr;
 Code e{page};const u8 setup[]={0x49,0x89,0xCA,0x49,0x89,0xCB,0x49,0x89,0xD0};
 e.bytes(setup,sizeof(setup));
 if(s.reg==0){const u8 ctx[]={0x48,0x89,0xC8};e.bytes(ctx,3);}else {e.b(0xB8);e.d(0x11111111);}
 e.b(0xB9);e.d(0x22222222);e.b(0xBA);e.d(0x33333333);e.jump(u64(page)+0x200);
 Code end{page+0x100};
 const u8 result[]={0x9C,0x41,0x8F,0x40,0x28,0x49,0x89,0x00,0x49,0x89,0x48,8,0x49,0x89,0x50,0x10,0x4D,0x89,0x50,0x18,0x4D,0x89,0x58,0x20,0x49,0x8B,0x40,0x28,0x48,0xC1,0xE8,6,0x83,0xE0,1};
 end.bytes(result,sizeof(result));
 if(!s.prompt){end.b(0x83);end.b(0xF0);end.b(1);}end.b(0xC3);
 if(doubleJumpCode(page+0x200,u64(page)+0x100,s)>256){VirtualFree(page,0,0x8000);return nullptr;}
 DWORD old;if(!VirtualProtect(page,4096,0x20,&old)||!FlushInstructionCache(GetCurrentProcess(),page,4096)){VirtualFree(page,0,0x8000);return nullptr;}
 return page;
}
// Execute the complete original FEF200 input predicate in the test process.
// Relocate its one external context query to a controlled boolean stub. All
// native input, suppression and consumed-latch instructions remain unchanged.
EXPORT void* PrepareNativeJumpInput(const u8*original,u32 length,bool patched){
 if(length!=0x52 || original[0x32]!=0xE8)return nullptr;
 for(u32 i=0;i<8;++i)if(original[0x3B+i]!=JumpGates[1].expected[i])return nullptr;
 auto*page=(u8*)VirtualAlloc(nullptr,4096,0x3000,4);if(!page)return nullptr;
 memcpy(page,original,length);
 Code call{page+0x32};call.b(0xE8);call.d(u32((page+0x300)-(page+0x37)));
 const u8 stub[]={0x8A,0x01,0xC3};memcpy(page+0x300,stub,sizeof(stub));
 if(patched){Code site{page+0x3B};site.jump(u64(page)+0x200);site.b(0x90);site.b(0x90);site.b(0x90);
  if(doubleJumpCode(page+0x200,u64(page)+0x43,JumpGates[1])>256){VirtualFree(page,0,0x8000);return nullptr;}}
 DWORD old;if(!VirtualProtect(page,4096,0x20,&old)||!FlushInstructionCache(GetCurrentProcess(),page,4096)){VirtualFree(page,0,0x8000);return nullptr;}
 return page;
}

EXPORT void AtlasFabrication(void* out,bool enabled){atlasFabrication(*(AtlasBundle*)out,enabled);}
