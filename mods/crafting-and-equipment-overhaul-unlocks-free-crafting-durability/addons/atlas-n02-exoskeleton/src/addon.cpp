// ATLAS N02 1.0.1: single-ASI addon; compatible boot companion ABI1.
// The host owns all item definitions, recipes, stats and save identities.
#include "vendor/win_api.hpp"
#include "vendor/sha256.hpp"
#include "visual_policy.hpp"
#include "crafting_addon_api.h"
#include "graph_merge.hpp"
#include "embedded_payload.hpp"
#include "visual_archive_api.h"
using namespace chiral;
bool sameDigest(const char*a,const char*b){if(!a||!b)return false;for(unsigned i=0;i<64;++i)if(!a[i]||a[i]!=b[i])return false;return a[64]==0&&b[64]==0;}
extern "C" int _fltused=0;
extern "C" void* memcpy(void*d,const void*s,SIZE_T n){for(SIZE_T i=0;i<n;++i)((u8*)d)[i]=((const u8*)s)[i];return d;}
extern "C" void* memset(void*d,int v,SIZE_T n){for(SIZE_T i=0;i<n;++i)((u8*)d)[i]=(u8)v;return d;}
extern "C" int memcmp(const void*a,const void*b,SIZE_T n){for(SIZE_T i=0;i<n;++i)if(((const u8*)a)[i]!=((const u8*)b)[i])return ((const u8*)a)[i]-((const u8*)b)[i];return 0;}
extern "C" long _InterlockedCompareExchange(long volatile*,long,long);
extern "C" long _InterlockedExchange(long volatile*,long);
extern "C" long _InterlockedIncrement(long volatile*);
extern "C" long _InterlockedDecrement(long volatile*);
extern "C" long long _InterlockedCompareExchange64(long long volatile*,long long,long long);
#pragma intrinsic(_InterlockedCompareExchange,_InterlockedExchange,_InterlockedIncrement,_InterlockedDecrement,_InterlockedCompareExchange64)

namespace {
constexpr char GameSHA[]="bf3d1c665545930bc850d8f5df486f7395885bb729d4fd408fdb03390de0765b";
HMODULE selfModule=nullptr;u64 image=0;
HANDLE logFile=InvalidHandle;
volatile long logBusy=0,meshBusy=0;

bool announced[2]={},blocked[2]={};
bool readmem(u64 address,void*out,SIZE_T n){
 SIZE_T got=0;return address>=0x10000&&ReadProcessMemory(GetCurrentProcess(),(const void*)address,out,n,&got)&&got==n;
}
template<class T>T read(u64 address){T v={};readmem(address,&v,sizeof(v));return v;}
bool writable(void*p,SIZE_T n){
 MemoryInfo info={};
 return p&&VirtualQuery(p,&info,sizeof(info))==sizeof(info)&&info.state==0x1000&&
  (info.protect==4||info.protect==0x40||info.protect==8||info.protect==0x80)&&
  u64(p)>=u64(info.base)&&u64(p)+n>=u64(p)&&u64(p)+n<=u64(info.base)+info.size;
}
void log(const char* s){
 if(logFile==InvalidHandle||_InterlockedCompareExchange(&logBusy,1,0))return;
 u32 n=0;while(s[n]&&n<1000)++n;DWORD done=0;
 char prefix[24]="[pid=";u32 k=5;char digits[10];u32 count=0,pid=GetCurrentProcessId();
 do{digits[count++]=char('0'+pid%10);pid/=10;}while(pid);
 while(count)prefix[k++]=digits[--count];prefix[k++]=']';prefix[k++]=' ';
 WriteFile(logFile,prefix,k,&done,nullptr);
 WriteFile(logFile,s,n,&done,nullptr);WriteFile(logFile,"\r\n",2,&done,nullptr);
 FlushFileBuffers(logFile);_InterlockedExchange(&logBusy,0);
}
void logValue(const char* label,const char* value){
 char line[1000];u32 n=0;
 for(u32 i=0;label[i]&&n<999;++i)line[n++]=label[i];
 for(u32 i=0;value[i]&&n<999;++i)line[n++]=value[i];
 line[n]=0;log(line);
}
bool hashFile(const WCHAR*path,char (&digest)[65]){
 digest[0]=0;
 HANDLE f=CreateFileW(path,0x80000000u,3,nullptr,3,0x80,nullptr);if(f==InvalidHandle)return false;
 u8* block=(u8*)VirtualAlloc(nullptr,65536,0x3000,4);if(!block){CloseHandle(f);return false;}
 craft::Sha256 hash;DWORD n=0;bool ok=true;
 while(true){if(!ReadFile(f,block,65536,&n,nullptr)){ok=false;break;}if(!n)break;hash.update(block,n);}
 CloseHandle(f);VirtualFree(block,0,0x8000);
 if(!ok)return false;
 u8 result[32];hash.finish(result);constexpr char hex[]="0123456789abcdef";
 for(u32 i=0;i<32;++i){digest[2*i]=hex[result[i]>>4];digest[2*i+1]=hex[result[i]&15];}
 digest[64]=0;return true;
}
bool moduleDigest(HMODULE module,char (&digest)[65]){
 WCHAR path[1024];digest[0]=0;u32 n=GetModuleFileNameW(module,path,1024);
 return n&&n<1024&&hashFile(path,digest);
}
void openLog(){
 static WCHAR path[2048];u32 n=GetModuleFileNameW(selfModule,path,2048);if(!n||n>=2048)return;
 while(n&&path[n-1]!=L'\\'&&path[n-1]!=L'/')--n;
 const WCHAR name[]=L"ds2_atlas_exoskeleton_addon.log";
 for(u32 i=0;i<sizeof(name)/sizeof(WCHAR);++i){if(n>=2048)return;path[n++]=name[i];}
 logFile=CreateFileW(path,4,3,nullptr,4,0x80,nullptr); // FILE_APPEND_DATA / OPEN_ALWAYS; addon log only
}
volatile long storageState=0; // 2 only after the verified DirectStorage IAT is bound.
#include "file_overlay.inl"
#include "storage_overlay.inl"
#include "body_visibility.inl"
#include "exo_runtime.inl"
namespace boot_ui {
#include "boot_menu.inl"
}
namespace exo_ui {
#include "exo_menu.inl"
}
void* passCatalogue(void*p){return p;}
void* catalogueVisuals(void*p,crafting_addon::CatalogueNext next){
 if(bootsEnabled)boot_ui::catalogueWithChiralIcon(p,&passCatalogue);
 return exo_ui::catalogueWithChiralIcon(p,next);
}
u64 iconVisuals(u64 c,const char*const*n,crafting_addon::IconNext next){
 if(exo_ui::privateIconName(read<u64>(u64(n))))return exo_ui::chiralCatalogueIcon(c,n,next);
 if(bootsEnabled)return boot_ui::chiralCatalogueIcon(c,n,next);
 return next(c,n);
}
constexpr u32 ChiralWornNames[]={0x6c1a0001,0x6c1a0002,0x6c1a0001,0x6c1a0002,
                               0x6c1a0001,0x6c1a0002,0x6c1a0001,0x6c1a0002};
constexpr u32 ChiralHungNames[]={0x6c1a0007,0x6c1a0008};

struct VisualCopy {
 u64 source;u32 offset;
 alignas(8)u8 resource[0x50];
 alignas(8)u8 rows[MaxRows][RowSize];
};
VisualCopy* copies[64]={};u32 copyCount=0;
// Called AFTER host meshFor but BEFORE the original native renderer update.
// Already-owned components are left alone, including when ordinary boots equip.
void bindMarker(void* component,bool hanging){
 if(overlayState!=2||!component||_InterlockedCompareExchange(&meshBusy,1,0))return;
 const u32 slot=hanging?1:0,offset=hanging?0x20:0x28,size=hanging?0x50:0x48;
 const u64 source=read<u64>(u64(component)+0x30);
 VisualCopy* found=nullptr;
 for(u32 i=0;i<copyCount;++i){
  auto*c=copies[i];
  if(c->offset!=offset)continue;
  if(source==u64(c->resource)){_InterlockedExchange(&meshBusy,0);return;}
  if(source==c->source){found=c;break;}
 }
 if(!found&&copyCount<64){
  u8 header[0x50]={};u8 rows[MaxRows][RowSize]={};
  if(readmem(source,header,size)){
   i32 count=at<i32>(header,offset),capacity=at<i32>(header,offset+4);
   u64 rowPointer=at<u64>(header,offset+8);
   i32 atlas=-1,donor=-1;
   // Only bind the exact host-owned ATLAS table, not arbitrary component data.
   const u64 expectedId=0x4A634CBDCE790000ull|(u64(hanging?21:20)<<8)|AtlasId;
   bool valid=at<u64>(header,0x10)==expectedId&&at<u64>(header,0x18)==0xB7AE21D89446A581ull&&
    count>0&&count<=i32(MaxRows)&&capacity>=count&&capacity<=i32(MaxRows)&&
    readmem(rowPointer,rows,u64(count)*RowSize)&&rowIndices((u8*)rows,count,atlas,donor);
   u32 names[8]={};
   if(valid){
    i32 n=at<i32>(rows[donor],8);u64 ptr=at<u64>(rows[donor],16);
    valid=n==(hanging?2:8)&&readmem(ptr,names,n*4)&&markerNames(names,n,hanging);
   }
   if(valid&&writable((void*)(source+8),4)){
    auto*c=(VisualCopy*)VirtualAlloc(nullptr,sizeof(VisualCopy),0x3000,4);
    if(c){
     c->source=source;c->offset=offset;copy(c->resource,header,size);
     if(makeMarkerRows((u8*)c->rows,(const u8*)rows,count)){
      at<u64>(c->rows[atlas],16)=hanging?u64(ChiralHungNames):u64(ChiralWornNames);
      at<u32>(c->resource,8)=1; // Permanent pin; no live unloading.
      at<u64>(c->resource,0x10)=0x4C41485249434800ull|slot;
      at<u64>(c->resource,0x18)=0xA511C0FF20261020ull;
      at<u64>(c->resource,offset+8)=u64(c->rows);
      // Cloned rows share native references. Keep their source owner alive.
      _InterlockedIncrement((long*)(source+8));copies[copyCount++]=c;found=c;
     }else VirtualFree(c,0,0x8000);
    }
   }
  }
 }
 if(found&&writable((u8*)component+0x30,8)){
  _InterlockedIncrement((long*)(found->resource+8));
  const u64 prior=(u64)_InterlockedCompareExchange64((long long*)((u8*)component+0x30),
                                                    (long long)u64(found->resource),(long long)source);
  if(prior==source){
   // Retain displaced source reference as a bounded lifetime pin.
   if(!announced[slot]){announced[slot]=true;log(hanging?
    "CHIRAL_TEST11_HANGING_BOUND: ATLAS row 103 uses private Chiral pair and isolated clip buffers.":
    "CHIRAL_TEST11_WORN_BOUND: ATLAS row 103 uses accepted Chiral cuffs; host stats untouched.");}
  }else _InterlockedDecrement((long*)(found->resource+8));
 }else if(!blocked[slot]&&source){
  blocked[slot]=true;log(hanging?"CHIRAL_HANGING_WAIT: host table not ready or validation refused; original retained.":
                                          "CHIRAL_WORN_WAIT: host table not ready or validation refused; original retained.");
 }
 _InterlockedExchange(&meshBusy,0);
}
void worn(void*p,crafting_addon::MeshNext chainWorn){
 if(!bootsEnabled){chainWorn(p);return;}
 const u64 priorResource=read<u64>(u64(p)+0x30);
 bool ready=overlayState==2&&syncBody(p,true);
 if(ready)bindMarker(p,false);
 else{
  const u64 source=read<u64>(u64(p)+0x30);
  for(auto*c:copies)if(c&&source==u64(c->resource)&&writable((u8*)p+0x30,8)){
   _InterlockedIncrement((long*)(c->source+8));
   _InterlockedCompareExchange64((long long*)((u8*)p+0x30),(long long)c->source,(long long)source);
   break;
  }
  static bool cacheReported=false,bodyReported=false;
  if(overlayState!=2){if(!cacheReported){cacheReported=true;log("CHIRAL_VISUAL_WAIT: private file overlay inactive; host appearance retained.");}}
  else if(!bodyReported){bodyReported=true;log("CHIRAL_BODY_WAIT: expected Sam body/controllers unavailable; worn host appearance retained.");}
 }
 if(priorResource!=read<u64>(u64(p)+0x30)&&writable((u8*)p+0x4b4,1))
  *((u8*)p+0x4b4)=0; // Native renderer refreshes both sides after a table transition.
 chainWorn(p);
 if(ready){syncBody(p,true);static bool reported=false;if(!reported){reported=true;log("CHIRAL_BODY_BOUND: original body hierarchy retained; cuff geometry follows ATLAS equip state.");}}
}
void hung(void*p,crafting_addon::MeshNext chainHung){if(bootsEnabled)bindMarker(p,true);chainHung(p);}

struct Patch {u64 address;u8 before[5],after[5];DWORD protection;bool changed;};
Patch patches[2]={};u32 patchCount=1;void* relay=nullptr;
bool prepare(){
 const u8 prologue[]={0x48,0x83,0xec,0x38,0x48,0x8b,0x41,0x60};u8 prior[8];
 if(!readmem(image+VisibilityRva,prior,8)||!same(prior,prologue,8))return false;
 patches[0].address=image+VisibilityRva;copy(patches[0].before,prior,5);
 u64 center=(image+0x1000000)&~u64(0xffff);
 for(u64 distance=0x10000;distance<0x60000000&&!relay;distance+=0x10000){
  relay=VirtualAlloc((void*)(center+distance),0x10000,0x3000,4);
  if(!relay&&center>distance+0x10000)relay=VirtualAlloc((void*)(center-distance),0x10000,0x3000,4);
 }
 if(!relay)return false;
 u8*stub=(u8*)relay;const u8 abs[]={0xff,0x25,0,0,0,0};copy(stub,abs,6);at<u64>(stub,6)=u64(&visibility);
 u8*next=stub+32;copy(next,prior,8);copy(next+8,abs,6);at<u64>(next,14)=image+VisibilityRva+8;
 chainVisibility=(void(*)(u64,i32,u8))next;setControllerVisibility=(void(*)(u64,u8))(image+0x3478E0);
 if(!relativeJump(patches[0].after,patches[0].address,u64(stub)))return false;
 if(exoEnabled){
  const u8 expected[]={0x48,0x89,0x5c,0x24,0x18};u8 original[5];
  if(!readmem(image+ExoUpdateRva,original,5)||!same(original,expected,5))return false;
  patches[1].address=image+ExoUpdateRva;copy(patches[1].before,original,5);
  u8*entry=stub+128;copy(entry,abs,6);at<u64>(entry,6)=u64(&updateExo);
  u8*chain=stub+160;copy(chain,original,5);copy(chain+5,abs,6);at<u64>(chain,11)=image+ExoUpdateRva+5;
  nextExoUpdate=(void(*)(u64,u64,u8))chain;
  if(!relativeJump(patches[1].after,patches[1].address,u64(entry)))return false;patchCount=2;
 }
 DWORD old=0;return VirtualProtect(relay,0x10000,0x20,&old)&&FlushInstructionCache(GetCurrentProcess(),relay,0x10000);
}
struct Thread {HANDLE handle;bool suspended;};Thread threads[512]={};u32 threadCount=0;
bool releaseThreads(){
 bool ok=true;
 for(u32 i=0;i<threadCount;++i){
  auto&t=threads[i];
  if(t.suspended){
   bool resumed=false;
   for(u32 retry=0;retry<3&&!resumed;++retry){
    if(ResumeThread(t.handle)!=0xffffffffu)resumed=true;
    else{DWORD exit=259;if(GetExitCodeThread(t.handle,&exit)&&exit!=259)resumed=true;}
   }if(!resumed)ok=false;
  }CloseHandle(t.handle);
 }
 threadCount=0;return ok;
}
bool collectThreads(){
 threadCount=0;HANDLE snapshot=CreateToolhelp32Snapshot(4,0);if(snapshot==InvalidHandle)return false;
 ThreadEntry row={};row.size=sizeof(row);bool more=Thread32First(snapshot,&row)!=0,ok=more;
 const DWORD self=GetCurrentThreadId(),pid=GetCurrentProcessId();
 while(more){
  if(row.pid==pid&&row.tid!=self){
   if(threadCount==512){ok=false;break;}
   HANDLE h=OpenThread(2|8|0x40,0,row.tid);
   if(!h||GetProcessIdOfThread(h)!=pid||GetThreadId(h)!=row.tid){if(h)CloseHandle(h);ok=false;break;}
   threads[threadCount++]={h,false};
  }row.size=sizeof(row);more=Thread32Next(snapshot,&row)!=0;
 }
 if(ok&&GetLastError()!=18)ok=false;CloseHandle(snapshot);if(!ok)releaseThreads();return ok;
}
int install(){
 if(!collectThreads())return 0;
 bool ok=true;Context context={};context.flags=0x00100001;
 for(u32 i=0;i<threadCount&&ok;++i){
  auto&t=threads[i];if(SuspendThread(t.handle)==0xffffffffu){ok=false;break;}t.suspended=true;
  if(!GetThreadContext(t.handle,&context)){ok=false;break;}
  for(u32 pi=0;pi<patchCount;++pi)if(auto&p=patches[pi];true)if(context.rip>p.address&&context.rip<p.address+5)ok=false;
 }
 // No allocation, logging or game calls while other threads are suspended.
 if(ok)for(u32 pi=0;pi<patchCount;++pi)if(auto&p=patches[pi];true)if(!same((void*)p.address,p.before,5))ok=false;
 if(ok)for(u32 pi=0;pi<patchCount;++pi)if(auto&p=patches[pi];true){
  DWORD ignored=0;
  if(!VirtualProtect((void*)p.address,5,0x40,&p.protection)){ok=false;break;}
  copy((void*)p.address,p.after,5);p.changed=true;
  const bool flushed=FlushInstructionCache(GetCurrentProcess(),(void*)p.address,5)!=0;
  const bool protectedAgain=VirtualProtect((void*)p.address,5,p.protection,&ignored)!=0;
  if(!flushed||!protectedAgain){ok=false;break;}
 }
 bool rollback=true;
 if(!ok)for(u32 pi=0;pi<patchCount;++pi)if(auto&p=patches[pi];true)if(p.changed){
  DWORD ignored=0;
  if(!VirtualProtect((void*)p.address,5,0x40,&ignored)){rollback=false;continue;}
  copy((void*)p.address,p.before,5);
  bool flushed=FlushInstructionCache(GetCurrentProcess(),(void*)p.address,5)!=0;
  bool protectedAgain=VirtualProtect((void*)p.address,5,p.protection,&ignored)!=0;
  if(!flushed||!protectedAgain)rollback=false;else p.changed=false;
 }
 bool resumed=releaseThreads();return (!rollback||!resumed)?2:ok?1:0;
}

int mergeCurrentGraph(const u8*src,u32 size,u8**out,u32*outSize){
 *out=nullptr;*outSize=0;openLog();log("ATLAS N02 1.0.1 | boot companion ABI1 | Crafting ABI1 + Router1 unchanged");
 char digest[65];
 if(!overlayInstalled||!moduleDigest(nullptr,digest)||!sameDigest(digest,GameSHA)){
  log("CHIRAL_ABI_REFUSED: game identity or early file hooks unavailable.");return 0;}
 if(storageState!=2&&!installStorageCore()){log("CHIRAL_ABI_REFUSED: DirectStorage unavailable.");return 0;}
 if(!selectExo()||!prepareBootCompanion()||!preparePortable()){log("CHIRAL_ABI_REFUSED: peer materials or original/cache/payload validation failed.");return 0;}
 const auto*graph=validatedEmbedded(L"graph-addon.bin",activeGraphHash);
 if(!graph){log("CHIRAL_ABI_REFUSED: embedded graph payload validation failed.");return 0;}
 const u8*delta=graph->data;u32 deltaSize=graph->size;
 u32 capacity=size+deltaSize;u8*merged=(u8*)VirtualAlloc(nullptr,capacity,0x3000,4);u32 length=0;
 bool ok=merged&&chiral_graph::merge(src,size,delta,deltaSize,merged,capacity,length);
 if(!ok){if(merged)VirtualFree(merged,0,0x8000);log("CHIRAL_GRAPH_REFUSED: owned groups conflict; current host graph retained.");return 0;}
 if(!prepare()){VirtualFree(merged,0,0x8000);log("CHIRAL_VISIBILITY_REFUSED: native signature unavailable.");return 0;}
 int result=0;for(u32 i=0;i<20&&!result;++i){result=install();if(!result)Sleep(50);}
 if(result!=1){VirtualFree(merged,0,0x8000);log(result==2?"CHIRAL_CRITICAL: native visibility rollback/thread resume failure.":"CHIRAL_VISIBILITY_REFUSED: native hook not installed.");return 0;}
 *out=merged;*outSize=length;log("CHIRAL_GRAPH_MERGED: four owned groups appended to CURRENT host graph; host preload list preserved.");return 1;
}
void releaseGraph(u8*p){if(p)VirtualFree(p,0,0x8000);}
void commitGraph(int success){_InterlockedExchange(&overlayState,success?2:-1);
 log(success?"CHIRAL_API1_READY: graph committed; worn, hanging and menu callbacks active.":"CHIRAL_API1_INACTIVE: host graph not committed; native appearance retained.");}
}
extern "C" __declspec(dllexport) int DS2CraftingAddonQueryV1(const crafting_addon::HostV1*host,crafting_addon::AddonV1*out){
 using namespace crafting_addon;
 if(!validHost(host)||!out||out->size!=sizeof(AddonV1)||out->version!=Version)return 0;
 *out={sizeof(AddonV1),Version,&mergeCurrentGraph,&releaseGraph,&commitGraph,&worn,&hung,&catalogueVisuals,&iconVisuals};return 1;
}
extern "C" __declspec(dllexport) const DS2VisualArchiveV1* DS2CraftingVisualArchiveV1(){
 if(storageState!=2||!activePortableFiles||cachePins[0]==InvalidHandle||cachePins[1]==InvalidHandle||!cachePins[0]||!cachePins[1])return nullptr;
 static DS2VisualArchiveV1 view={sizeof(DS2VisualArchiveV1),1,(void*)&openStorageW,nullptr,nullptr};
 view.archiveSHA256=activePortableFiles[0].resultHash;view.linksSHA256=activePortableFiles[1].resultHash;return &view;
}
#ifndef CHIRAL_SELF_TEST
extern "C" BOOL WINAPI DllMain(HMODULE module,DWORD reason,void*){
 if(reason==1){selfModule=module;DisableThreadLibraryCalls(module);if(earlyOverlay()&&!earlyStorage())overlayInstalled=false;}
 return 1;
}
#endif
