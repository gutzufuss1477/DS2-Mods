// Execute the production wrappers with bounded fake memory/native imports.
// This verifies ownership, locking and failure handling; it is not an engine test.
#include "../src/cosmetics.hpp"
#include <atomic>
#include <intrin.h>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>
using namespace craft;
unsigned checks=0;
void check(bool b,const char*m){++checks;if(!b)throw std::runtime_error(m);}
u64 image=0x140000000,manager=0x200000000;
constexpr u64 ManagerRva=0x623e540,VectorCopyRva=0xb7e950;
Settings settings{};
Recipe recipes[MaxRecipes];u32 recipeCount=0;Instance nativeCopy[MaxMenu];volatile long busy=0;
using VectorCopyFn=void(*)(Vector*,const Instance*,i32);
struct Text {void add(const char*){} void dec(u64){}};
void say(const char*){}void log(const Text&){}
u32 threadId=42;
u32 GetCurrentThreadId(){return threadId;}
bool held=false,lockFails=false,scanFails=false,badCategory=false,badVtable=false,badSignature=false;
u32 nativeCalls=0,copyCalls=0,predicateCalls=0,paletteCalls=0,patchCalls=0,allocs=0,frees=0;
int failPatch=-1,failAlloc=-1;
struct Region {u64 address;const void*data;u32 size;};std::vector<Region> memory;
void region(u64 a,const void*p,u32 n){memory.push_back({a,p,n});}
bool readmem(u64 a,void*out,u32 n){
 if(badSignature&&a==image+0x14914ce)return false;
 for(auto r:memory)if(a>=r.address&&a-r.address<=r.size&&n<=r.size-(a-r.address)){
  memcpy(out,(const u8*)r.data+(a-r.address),n);return true;
 }
 return false;
}
template<class T>bool rd(u64 a,T&v){return readmem(a,&v,sizeof v);}
bool TryAcquireSRWLockShared(void*){check(!held,"no recursive catalogue lock");if(lockFails)return false;held=true;return true;}
void ReleaseSRWLockShared(void*){check(held,"release held lock");held=false;}
bool scan(u64 m){check(held&&m==manager,"scan under correct shared lock");return !scanFails;}
void*relayNear(u64,u64){u32 i=allocs++;return i==failAlloc?nullptr:(void*)(0x100000ull+i*0x1000);}
bool VirtualFree(void*,u32,u32){++frees;return true;}
bool patchOnce(u64,const u8*,void*){u32 i=patchCalls++;return failPatch<0||i<u32(failPatch);}
void Sleep(u32){}
#include "../src/cosmetics_runtime.inl"
Instance nativeEntry={0x1431ff530,0x18101,0x12345678},engineResult[MaxMenu];
void nativeList(u64,u8,Vector*out){
 check(!held,"native producer outside catalogue lock");++nativeCalls;
 *out={1,1,&nativeEntry};
 region(u64(out),out,sizeof *out);region(u64(&nativeEntry),&nativeEntry,sizeof nativeEntry);
}
void copyList(Vector*out,const Instance*in,i32 n){
 check(!held,"native allocation/copy occurs after releasing catalogue lock");++copyCalls;
 memcpy(engineResult,in,n*16);*out={n,n,engineResult};
}
u8 predicateNative(u64 context,u64 key,u64 predicate){
 check(context==11&&key==22&&predicate==33,"all three predicate ABI arguments preserved");++predicateCalls;return 0;
}
bool probeOtherThread=true,expectOverride=true;
void paletteNative(u64){
 ++paletteCalls;auto expected=expectOverride?1:0;
 check(cosmetics_runtime::colorPredicate(11,22,33)==expected,"palette predicate scoped result");
 if(probeOtherThread){
  threadId=43;check(!cosmetics_runtime::colorPredicate(11,22,33),"other thread retains native gate");threadId=42;
 }
}
u8 resourceBytes[CosmeticItemCount][0xb0];Instance master[CosmeticItemCount];u32 masterCount=CosmeticItemCount;u64 masterPtr=u64(master);
void fixture(){
 settings={};settings.enabled=settings.all=settings.cosmeticsEnabled=settings.cosmeticsColors=true;
 recipeCount=CosmeticItemCount;memory.clear();region(image+ManagerRva,&manager,8);
 region(manager+0x38,&masterCount,4);region(manager+0x40,&masterPtr,8);
 for(u32 i=0;i<recipeCount;++i){
  const auto&k=CosmeticItems[i];auto&r=recipes[i];r={};r.key=k.key;r.baggageKey=k.baggageKey;r.valid=1;
  r.resource=0x300000000+i*0x100;r.baggage=0x400000000+i*0x100;r.usage=8;
  r.instance={image+0x31ff530,257+i,0};master[i]=r.instance;
  memset(resourceBytes[i],0,0xb0);u64 vt=image+0x3208868;memcpy(resourceBytes[i],&vt,8);
  memcpy(resourceBytes[i]+0x70,&k.category,2);region(r.resource,resourceBytes[i],0xb0);
 }
 for(const auto&h:cosmetics_runtime::Hooks)region(image+h.rva,h.bytes,5);
 cosmetics_runtime::listOriginal=&nativeList;cosmetics_runtime::copyOriginal=&copyList;
 cosmetics_runtime::paletteOriginal=&paletteNative;cosmetics_runtime::predicateOriginal=&predicateNative;
 cosmetics_runtime::enabled=1;cosmetics_runtime::paletteThread=0;
 held=lockFails=scanFails=badSignature=false;busy=0;threadId=42;
 nativeCalls=copyCalls=predicateCalls=paletteCalls=patchCalls=allocs=frees=0;failPatch=failAlloc=-1;
 probeOtherThread=expectOverride=true;
}
int main(){try{
 fixture();auto before=std::vector<Recipe>(recipes,recipes+recipeCount);auto masterBefore=std::vector<Instance>(master,master+masterCount);
 Vector out{};cosmetics_runtime::costumeList(manager,8,&out);
 check(nativeCalls==1&&copyCalls==1&&out.count==52,"native once, engine-owned full list");
 check(!memcmp(out.data,&nativeEntry,16),"existing native row unchanged");
 check(!memcmp(before.data(),recipes,sizeof(Recipe)*recipeCount)&&!memcmp(masterBefore.data(),master,sizeof master),"catalogue/flags untouched");
 check(!held&&!busy,"locks released");
 for(unsigned fault=0;fault<6;++fault){
  fixture();u64 m=manager;u8 use=8;
  switch(fault){case 0:cosmetics_runtime::enabled=0;break;case 1:settings.cosmeticsEnabled=false;break;
   case 2:use=5;break;case 3:busy=1;break;case 4:lockFails=true;break;case 5:m+=0x1000;break;}
  cosmetics_runtime::costumeList(m,use,&out);check(nativeCalls==1&&!copyCalls&&out.count==1,"disabled/wrong-route/busy native fallback");
 }
 fixture();scanFails=true;cosmetics_runtime::costumeList(manager,8,&out);
 check(!copyCalls&&!held&&!busy,"scan failure releases locks");
 fixture();resourceBytes[1][0x70]^=1;cosmetics_runtime::costumeList(manager,8,&out);
 check(out.count==51,"category mismatch excluded");
 fixture();resourceBytes[1][0]^=1;cosmetics_runtime::costumeList(manager,8,&out);
 check(out.count==51,"wrong resource type excluded");
 fixture();cosmetics_runtime::paletteInit(123);
 check(paletteCalls==1&&predicateCalls==2&&!cosmetics_runtime::paletteThread,"palette native once, scoped token released");
 check(!cosmetics_runtime::colorPredicate(11,22,33),"outside palette init no override");
 fixture();settings.cosmeticsColors=false;expectOverride=false;cosmetics_runtime::paletteInit(123);
 check(!cosmetics_runtime::paletteThread&&paletteCalls==1,"colors disabled retains native calls");
 fixture();cosmetics_runtime::paletteThread=43;expectOverride=false;probeOtherThread=false;cosmetics_runtime::paletteInit(123);
 check(cosmetics_runtime::paletteThread==43,"occupied scope not stolen/cleared");
 fixture();badSignature=true;cosmetics_runtime::enabled=0;
 check(!cosmetics_runtime::setup()&&!patchCalls&&!allocs&&!cosmetics_runtime::enabled,"signature mismatch installs nothing");
 fixture();failAlloc=1;cosmetics_runtime::enabled=0;
 check(!cosmetics_runtime::setup()&&!patchCalls&&frees==1&&!cosmetics_runtime::enabled,"allocation failure frees uninstalled relays");
 fixture();failPatch=1;cosmetics_runtime::enabled=0;
 check(!cosmetics_runtime::setup()&&!cosmetics_runtime::enabled&&frees==2,"partial installation remains inactive");
 fixture();cosmetics_runtime::enabled=0;
 check(cosmetics_runtime::setup()&&cosmetics_runtime::enabled&&patchCalls==3,"all three validated hooks activate together");
 std::printf("COSMETICS_RUNTIME_ASSERTIONS=%u FAILURES=0\n",checks);return 0;
}catch(const std::exception&e){std::fprintf(stderr,"FAIL: %s\n",e.what());return 1;}}
