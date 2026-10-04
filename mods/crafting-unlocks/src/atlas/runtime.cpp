#include "win_api.hpp"
#include "../atlas_integration.hpp"
#include "branch.hpp"
#include "atlas.hpp"
using namespace equipment;
extern "C" int _fltused=0;
extern "C" void* memcpy(void*,const void*,SIZE_T);
extern "C" void* memset(void*,int,SIZE_T);
extern "C" int memcmp(const void*,const void*,SIZE_T);
extern "C" long _InterlockedCompareExchange(long volatile*,long,long);
extern "C" long _InterlockedExchange(long volatile*,long);
extern "C" long _InterlockedIncrement(long volatile*);
extern "C" long _InterlockedDecrement(long volatile*);
extern "C" long long _InterlockedCompareExchange64(long long volatile*,long long,long long);
#pragma intrinsic(_InterlockedCompareExchange,_InterlockedExchange,_InterlockedIncrement,_InterlockedDecrement,_InterlockedCompareExchange64)
extern "C" void LoadingThunk();

namespace {
#include "sites.generated.hpp"
constexpr u32 BranchCount=sizeof(Branches)/sizeof(Branches[0]),AtlasPatchStart=BranchCount+3+JumpGateCount,PatchCount=AtlasPatchStart+4;
u64 image;
bool recipesEnabled=false;
u8 skeletonVisualId=SkeletonNormalVisualId;
void (*logCallback)(const char*)=nullptr;
volatile long definitionBusy=0;
u64 nextDefinitions=0;
u32 announced=0;
bool readmem(u64 address,void* out,SIZE_T n){SIZE_T got=0;return address>=0x10000 && ReadProcessMemory(GetCurrentProcess(),(const void*)address,out,n,&got)&&got==n;}
template<class T> T read(u64 address){T value={};readmem(address,&value,sizeof(value));return value;}
bool writable(void*p,SIZE_T n){MemoryInfo info={};return p && VirtualQuery(p,&info,sizeof(info))==sizeof(info) && info.state==0x1000 && (info.protect==4||info.protect==0x40||info.protect==8||info.protect==0x80) && u64(p)>=u64(info.base) && u64(p)+n>=u64(p) && u64(p)+n<=u64(info.base)+info.size;}
void log(const char* message){if(logCallback)logCallback(message);}
void note(u32 bit,const char*message){if(!(announced&bit)){announced|=bit;log(message);}}

#include "atlas_runtime.inl"

using FootwearUpdate=void(*)(void*,float,u8);
void footwear(void* condition,float dt,u8 flag){
 definitions();
 ((FootwearUpdate)(image+0xE6E9D0))(condition,dt,flag);
 // Native equip caches the maximum separately. Update only the selected worn boot;
 // do not heal current durability or modify the user's other shoes.
 auto* parameter=at<void*>(condition,0x3D0);
 if(parameter && at<u8>(parameter,0x20)==BootId){
  auto* bag=at<void*>(condition,0x418);
  auto* resource=bag?at<void*>(bag,0x38):nullptr;
  if(resource && at<u32>(resource,0x44)==BootBag && at<u32>(resource,0x64)==3400)
   at<float>(condition,0x3E0)=3400.0f;
 }
}

struct Patch {u32 rva;u8 length,expected[16],replacement[16];DWORD protection;bool changed;};
Patch patches[PatchCount];void* cave=nullptr;
bool prepare(){
 for(u32 i=0;i<BranchCount;++i){const auto&s=Branches[i];auto&p=patches[i];p.rva=s.rva;p.length=s.length;memcpy(p.expected,s.expected,s.length);}
 auto&f=patches[BranchCount];f.rva=0xE6E4AE;f.length=5;
 const u8 fb[]={0xE8,0x1D,0x05,0,0};memcpy(f.expected,fb,5);
 auto&l=patches[BranchCount+1];l.rva=0x10B8928;l.length=7;
 const u8 lb[]={0x49,0x8D,0x80,0x9A,0x1A,0,0};memcpy(l.expected,lb,7);
 auto&b=patches[BranchCount+2];b.rva=0xEA0681;b.length=5;
 const u8 bb[]={0x0F,0x94,0x44,0x24,0x60};memcpy(b.expected,bb,5);
 for(u32 i=0;i<JumpGateCount;++i){auto&j=patches[BranchCount+3+i];const auto&s=JumpGates[i];j.rva=s.rva;j.length=s.length;memcpy(j.expected,s.expected,s.length);}
 auto& ctor=patches[AtlasPatchStart];ctor.rva=0x1EC3DB7;ctor.length=5;
 const u8 cb[]={0xE8,0x64,0x8D,0xCA,0xFE};memcpy(ctor.expected,cb,5);
 auto& worn=patches[AtlasPatchStart+1];worn.rva=0xF430E0;worn.length=5;
 const u8 wb[]={0x48,0x89,0x5C,0x24,0x20};memcpy(worn.expected,wb,5);
 auto& hung=patches[AtlasPatchStart+2];hung.rva=0xF43E30;hung.length=11;
 const u8 hb[]={0x40,0x53,0x55,0x57,0x48,0x81,0xEC,0xD0,0,0,0};memcpy(hung.expected,hb,11);
 auto& visual=patches[AtlasPatchStart+3];visual.rva=SkeletonVisualRva;visual.length=sizeof(SkeletonVisualBytes);
 memcpy(visual.expected,SkeletonVisualBytes,sizeof(SkeletonVisualBytes));
 for(auto&p:patches){u8 bytes[16];if(!readmem(image+p.rva,bytes,p.length)||memcmp(bytes,p.expected,p.length))return false;}
 const u64 center=(image+0x1000000)&~u64(0xFFFF);
 for(u64 distance=0x10000;distance<0x60000000 && !cave;distance+=0x10000){
  cave=VirtualAlloc((void*)(center+distance),0x10000,0x3000,4);
  if(!cave && center>distance+0x10000)cave=VirtualAlloc((void*)(center-distance),0x10000,0x3000,4);
 }
 if(!cave)return false;
 for(u32 i=0;i<PatchCount;++i){
  u8* slot=(u8*)cave+i*256;auto&p=patches[i];
  if(i<BranchCount)branchCode(slot,Branches[i],image);
  else if(i==BranchCount+2)boostFactCode(slot,image+p.rva+p.length);
  else if(i>=BranchCount+3 && i<AtlasPatchStart){if(doubleJumpCode(slot,image+p.rva+p.length,JumpGates[i-BranchCount-3])>256)return false;}
  else if(i==AtlasPatchStart+3){if(skeletonVisualCode(slot,image+p.rva+p.length,skeletonVisualId)>256)return false;}
  else if(i>=AtlasPatchStart){
   Code c{slot};c.absolute(i==AtlasPatchStart?u64(&catalogueConstructor):i==AtlasPatchStart+1?u64(&wornMesh):u64(&hangingMesh));
   if(i>AtlasPatchStart){Code original{slot+128};original.bytes(p.expected,p.length);original.absolute(image+p.rva+p.length);
    if(i==AtlasPatchStart+1)wornMeshOriginal=slot+128;else hangingMeshOriginal=slot+128;}
  }
  else {Code c{slot};c.absolute(i==BranchCount?u64(&footwear):u64(&LoadingThunk));}
  const long long delta=(long long)u64(slot)-(long long)(image+p.rva+5);
  if(delta<(-2147483647LL-1)||delta>2147483647LL)return false;
  memset(p.replacement,0x90,p.length);p.replacement[0]=(i==BranchCount||i==BranchCount+1||i==AtlasPatchStart)?0xE8:0xE9;
  const i32 relative=i32(delta);memcpy(p.replacement+1,&relative,4);
 }
 DWORD old;return VirtualProtect(cave,0x10000,0x20,&old)&&FlushInstructionCache(GetCurrentProcess(),cave,0x10000);
}
struct Thread {HANDLE h;bool suspended;};Thread threads[512];u32 threadCount;
bool closeThreads(){bool ok=true;for(u32 i=0;i<threadCount;++i){auto&t=threads[i];if(t.suspended){bool resumed=false;for(u32 j=0;j<3&&!resumed;++j){if(ResumeThread(t.h)!=0xFFFFFFFF)resumed=true;else {DWORD code=259;if(GetExitCodeThread(t.h,&code)&&code!=259)resumed=true;}}if(!resumed)ok=false;}CloseHandle(t.h);}threadCount=0;return ok;}
bool collectThreads(){
 threadCount=0;HANDLE snapshot=CreateToolhelp32Snapshot(4,0);if(snapshot==InvalidHandle)return false;
 ThreadEntry entry={};entry.size=sizeof(entry);BOOL more=Thread32First(snapshot,&entry);bool ok=more!=0;
 const DWORD self=GetCurrentThreadId(),pid=GetCurrentProcessId();
 while(more){if(entry.pid==pid && entry.tid!=self){
  if(threadCount==512){ok=false;break;}HANDLE h=OpenThread(2|8|0x40,0,entry.tid);
  if(!h||GetProcessIdOfThread(h)!=pid||GetThreadId(h)!=entry.tid){if(h)CloseHandle(h);ok=false;break;}
  threads[threadCount++]={h,false};
 }entry.size=sizeof(entry);more=Thread32Next(snapshot,&entry);}
 if(ok && GetLastError()!=18)ok=false;CloseHandle(snapshot);if(!ok)closeThreads();return ok;
}
// All sites form one transaction. No logger, allocator or game code while threads are paused.
int install(){
 if(!collectThreads())return 0;bool ok=true;Context ctx={};ctx.flags=0x00100001;
 for(u32 i=0;i<threadCount&&ok;++i){auto&t=threads[i];if(SuspendThread(t.h)==0xFFFFFFFF){ok=false;break;}t.suspended=true;
  if(!GetThreadContext(t.h,&ctx)){ok=false;break;}
  for(const auto&p:patches)if(ctx.rip>image+p.rva && ctx.rip<image+p.rva+p.length){ok=false;break;}
 }
 if(ok)for(const auto&p:patches)if(memcmp((void*)(image+p.rva),p.expected,p.length)){ok=false;break;}
 if(ok)for(auto&p:patches){void*site=(void*)(image+p.rva);DWORD ignored;
  if(!VirtualProtect(site,p.length,0x40,&p.protection)){ok=false;break;}
  memcpy(site,p.replacement,p.length);p.changed=true;
  bool flushed=FlushInstructionCache(GetCurrentProcess(),site,p.length)!=0;
  bool protectedAgain=VirtualProtect(site,p.length,p.protection,&ignored)!=0;
  if(!flushed||!protectedAgain){ok=false;break;}
 }
 bool rollback=true;
 if(!ok)for(u32 i=PatchCount;i;--i){auto&p=patches[i-1];if(!p.changed)continue;DWORD ignored;
  void*site=(void*)(image+p.rva);
  if(!VirtualProtect(site,p.length,0x40,&ignored)){rollback=false;continue;}
  memcpy(site,p.expected,p.length);
  bool flushed=FlushInstructionCache(GetCurrentProcess(),site,p.length)!=0;
  bool protectedAgain=VirtualProtect(site,p.length,p.protection,&ignored)!=0;
  if(!flushed||!protectedAgain)rollback=false;else p.changed=false;
 }
 bool resumed=closeThreads();if(!rollback||!resumed)return 2;return ok?1:0;
}

}

extern "C" void CombineLoading(void* output,void* state,void* inventory){
 if(!output||!state||!inventory||!at<u8>(output,0x6C)||at<u32>(output,0x80)!=2)return;
 // Re-read the SAME inventory used by the native projection. The menu can use
 // a preview inventory, so consulting the live equipment manager here is wrong.
 using Slot=void*(*)(void*,u8);using Baggage=void*(*)(void*);
 void* slot=((Slot)(image+0x1198FD0))(inventory,0x11);
 void* bag=slot?((Baggage)(image+0x11943B0))(slot):nullptr;
 void* item=bag?at<void*>(bag,0xC8):nullptr;
 if(!targetSkeleton(item))return;
 void* settings=at<void*>(state,0x11A8);void* action=settings?at<void*>(settings,0x68):nullptr;
 if(!action)return;
 combineLoading(output,item,at<float>(action,0xA8),at<float>(action,0x9C),at<float>(state,0x10F4));
}
// The host owns version verification, module pinning, configuration and logging.
// Keep this on its single initialization worker, before native catalogue creation.
bool InstallAtlasEquipment(unsigned long long gameImage,bool enableRecipes,bool goldSkeletonSkin,void (*logger)(const char*)){
 image=gameImage;recipesEnabled=enableRecipes;skeletonVisualId=goldSkeletonSkin?SkeletonGoldVisualId:SkeletonNormalVisualId;logCallback=logger;
 if(GetModuleHandleW(L"ds2_overpowered_equipment.asi")){
  log("ATLAS_BLOCKED: standalone ATLAS ASI is also loaded. Remove it and restart; one provider is required.");return false;
 }
 if(!prepare()){if(cave){VirtualFree(cave,0,0x8000);cave=nullptr;}log("ATLAS_HOOKS_BLOCKED: native bytes differ or relay allocation failed; no ATLAS patches applied.");return false;}
 int result=0;for(u32 attempt=0;attempt<20&&!result;++attempt){result=install();if(!result)Sleep(50);}
 if(result==2){log("CRITICAL: ATLAS rollback/resume failed; restart game. Executable file was not modified.");return false;}
 if(!result){VirtualFree(cave,0,0x8000);cave=nullptr;log("ATLAS_HOOKS_BLOCKED: transaction could not complete; native code retained.");return false;}
 log(recipesEnabled?"ATLAS_ON: fabrication enabled; stable item IDs 103/104; 36 hooks installed.":"ATLAS_OFF: fabrication hidden; stable resources and 36 scoped hooks retained for existing saved items.");
 return true;
}
