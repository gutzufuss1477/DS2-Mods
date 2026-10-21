// Private v1.9.8 proof: redirect only DS2's verified CreateFileW/A import when
// it opens streaming_graph.core. The game's original file is NEVER written.
// A hash-verified, temporary cache is generated from the original at first use.
#include "virtual_graph_core.hpp"
namespace vgraph_runtime {
using namespace vgraph;
using craft::u64;
constexpr u32 CreateFileW_IAT=0x2C16F40u, CreateFileA_IAT=0x2C16F00u;
constexpr u32 ExecutableTimestamp=0x6A3DAE46u, ExecutableImageSize=0xB292000u;
constexpr u32 OriginalFileSize=OriginalSize, PatchedFileSize=PatchedSize;
static const u8 VanillaSHA[32]={
 0xac,0x0c,0x82,0x4e,0xf7,0x3d,0x15,0x51,0xd7,0x98,0x35,0x7d,0x12,0xc1,0x50,0xa9,
 0x63,0xeb,0x26,0x91,0x02,0x92,0xce,0x4a,0x4f,0x9a,0x6d,0x8c,0x92,0x3c,0xa3,0x92
};
static const u8 WorkingSHA[32]={
 0xd5,0x20,0x14,0xd7,0x35,0xbe,0x8b,0x3d,0x64,0x1a,0x0d,0xfc,0x6b,0x3d,0xab,0x07,
 0xa5,0x30,0xe8,0xcc,0x94,0x08,0x1d,0x28,0x45,0x6d,0x14,0x08,0xa0,0x17,0x45,0x50
};
using OpenW=HANDLE (WINAPI*)(const WCHAR*,DWORD,DWORD,void*,DWORD,DWORD,HANDLE);
using OpenA=HANDLE (WINAPI*)(const char*,DWORD,DWORD,void*,DWORD,DWORD,HANDLE);
OpenW originalW=nullptr;
OpenA originalA=nullptr;
volatile long installed=0,attempts=0,redirected=0,failed=0,cacheState=0;
WCHAR virtualPath[2048]={};
WCHAR nativeGraphPath[2048]={};
void (*trace)(const char*)=nullptr;
bool sameASCII(WCHAR a,char b){
 if(a>=L'A'&&a<=L'Z')a=WCHAR(a+32);
 if(b>='A'&&b<='Z')b=char(b+32);
 return a==WCHAR((unsigned char)b);
}
bool sameASCII(char a,char b){
 if(a>='A'&&a<='Z')a=char(a+32);
 if(b>='A'&&b<='Z')b=char(b+32);
 return a==b;
}
template<class C>bool matches(const C*name) {
 if(!name)return false;
 u32 length=0;while(name[length]&&length<2000)++length;
 const char suffix[]="streaming_graph.core";
 const u32 n=sizeof(suffix)-1;
 if(length<n || length>=2000)return false;
 if(length>n&&name[length-n-1]!='\\'&&name[length-n-1]!='/')return false;
 for(u32 k=0;k<n;++k)if(!sameASCII(name[length-n+k],suffix[k]))return false;
 return true;
}
bool matchesGraphFile(const WCHAR*path){return matches(path);}
bool matchesGraphFile(const char*path){return matches(path);}
bool digestEqual(const u8*data,u32 n,const u8*expected){
 craft::Sha256 s;
 for(u32 at=0;at<n;){
  const u32 block=(n-at)>65536u?65536u:(n-at);
  s.update(data+at,block);
  at+=block;
 }
 u8 result[32];s.finish(result);
 return memcmp(result,expected,32)==0;
}
void*alloc(u32 n){return VirtualAlloc(nullptr,n,0x3000,4);}
bool originalGraphPath(WCHAR*result) {
 u32 n=GetModuleFileNameW(nullptr,result,1800);
 if(!n||n>=1800)return false;
 while(n&&result[n-1]!=L'\\'&&result[n-1]!=L'/')--n;
 result[n]=0;
 const WCHAR end[]=L"LocalCacheWinGame\\package\\streaming_graph.core";
 for(u32 i=0;end[i];++i){if(n>=2047)return false;result[n++]=end[i];}
 result[n]=0;
 return true;
}
bool temporaryPath(WCHAR*result) {
 u32 n=GetTempPathW(1800,result);
 if(!n||n>=1800)return false;
 const WCHAR prefix[]=L"DS2_Crafting_VGraph_";
 for(u32 i=0;prefix[i];++i){if(n>=2047)return false;result[n++]=prefix[i];}
 DWORD pid=GetCurrentProcessId();WCHAR digits[16];u32 count=0;
 do{digits[count++]=WCHAR(L'0'+(pid%10));pid/=10;}while(pid);
 while(count){if(n>=2047)return false;result[n++]=digits[--count];}
 const WCHAR end[]=L"_v198.cache";
 for(u32 i=0;end[i];++i){if(n>=2047)return false;result[n++]=end[i];}
 result[n]=0;
 return true;
}
bool loadExact(const WCHAR*path,u8**result,u32*length) {
 *result=nullptr;*length=0;
 HANDLE h=CreateFileW(path,0x80000000u,3,nullptr,3,0x80,nullptr);
 if(h==InvalidHandle)return false;
 long long size=0;
 bool ok=GetFileSizeEx(h,&size)&&
         (size==OriginalFileSize||size==PatchedFileSize);
 u8*buffer=nullptr;
 if(ok){buffer=(u8*)alloc((u32)size);ok=buffer!=nullptr;}
 if(ok){
  u32 at=0;
  while(at<(u32)size){
   u32 chunk=((u32)size-at)>1048576u?1048576u:((u32)size-at),read=0;
   if(!ReadFile(h,buffer+at,chunk,&read,nullptr)||read!=chunk){ok=false;break;}
   at+=read;
  }
 }
 CloseHandle(h);
 if(!ok){if(buffer)VirtualFree(buffer,0,0x8000);return false;}
 *result=buffer;*length=(u32)size;return true;
}
bool writeExact(const WCHAR*path,const u8*buffer,u32 length){
 HANDLE h=CreateFileW(path,0x40000000u,0,nullptr,2,0x80,nullptr);
 if(h==InvalidHandle)return false;
 u32 at=0;bool ok=true;
 while(at<length){
  u32 chunk=(length-at)>1048576u?1048576u:(length-at),done=0;
  if(!WriteFile(h,buffer+at,chunk,&done,nullptr)||done!=chunk){ok=false;break;}
  at+=done;
 }
 if(ok)ok=FlushFileBuffers(h)!=0;
 if(!CloseHandle(h))ok=false;
 if(!ok)DeleteFileW(path);
 return ok;
}
long prepareCache(){
 if(!originalGraphPath(nativeGraphPath)||!temporaryPath(virtualPath))return 4;
 u8*input=nullptr;u32 size=0;
 if(!loadExact(nativeGraphPath,&input,&size))return 4;
 if(size==PatchedFileSize && digestEqual(input,size,WorkingSHA)){
  VirtualFree(input,0,0x8000);
  return 3; // already modified by external installation: transparently use it
 }
 if(size!=OriginalFileSize||!digestEqual(input,size,VanillaSHA)){
  VirtualFree(input,0,0x8000);
  return 4; // unknown modifications: never overwrite or silently merge
 }
 u8*out=(u8*)alloc(PatchedFileSize);
 if(!out){VirtualFree(input,0,0x8000);return 4;}
 bool ok=vgraph::build(input,size,out,PatchedFileSize)&&digestEqual(out,PatchedFileSize,WorkingSHA);
 VirtualFree(input,0,0x8000);
 u8*addonGraph=nullptr;u32 addonSize=0;
 if(ok)addon_host::merge(out,PatchedFileSize,&addonGraph,&addonSize);
 if(ok)ok=writeExact(virtualPath,addonGraph?addonGraph:out,addonGraph?addonSize:PatchedFileSize);
 addon_host::finish(ok,addonGraph);
 // A failed optional cache must not disable the main mod's own resource graph.
 if(!ok&&addonGraph)ok=writeExact(virtualPath,out,PatchedFileSize);
 VirtualFree(out,0,0x8000);
 return ok?2:4;
}
long getState(){
 long state=_InterlockedCompareExchange(&cacheState,1,0);
 if(!state){
  const long result=prepareCache();
  _InterlockedExchange(&cacheState,result);
  if(trace)trace(result==2?"VGRAPH_CACHE_READY: verified virtual graph, vanilla untouched.":result==3?
     "VGRAPH_PASSTHROUGH: existing graph already contains all validated resources.":
     "VGRAPH_UNAVAILABLE: source checksum, cache path or write failed. Original graph untouched.");
  return result;
 }
 if(state==1){
  // Addon cache generation can exceed 30 seconds on slower disks. Never open
  // a different graph in a second thread while the first is still preparing it.
  while(state==1){
   Sleep(10);
   state=_InterlockedCompareExchange(&cacheState,0,0);
  }
 }
 return state==1?4:state;
}
HANDLE WINAPI openW(const WCHAR*name,DWORD access,DWORD share,void*sec,DWORD disposition,DWORD flags,HANDLE templateFile){
 if(matchesGraphFile(name)&&disposition==3&&!(access&0x40000000u)){
  _InterlockedIncrement(&attempts);
  if(getState()==2){
   HANDLE h=originalW(virtualPath,access,share,sec,disposition,flags,templateFile);
   if(h!=InvalidHandle){_InterlockedIncrement(&redirected);return h;}
   _InterlockedIncrement(&failed);
   if(trace)trace("VGRAPH_REDIRECT_OPEN_FAILED: using original path.");
  }
 }
 return originalW(name,access,share,sec,disposition,flags,templateFile);
}
HANDLE WINAPI openA(const char*name,DWORD access,DWORD share,void*sec,DWORD disposition,DWORD flags,HANDLE templateFile){
 if(matchesGraphFile(name)&&disposition==3&&!(access&0x40000000u)){
  _InterlockedIncrement(&attempts);
  if(getState()==2){
   HANDLE h=originalW(virtualPath,access,share,sec,disposition,flags,templateFile);
   if(h!=InvalidHandle){_InterlockedIncrement(&redirected);return h;}
   _InterlockedIncrement(&failed);
   if(trace)trace("VGRAPH_REDIRECT_OPEN_FAILED: using original path.");
  }
 }
 return originalA(name,access,share,sec,disposition,flags,templateFile);
}
bool earlyInstall(u64 base){
 if(!base)return false;
 const u8*exe=(const u8*)base;
 const u32 nt=get32(exe+0x3C);
 if(nt<0x40||nt>0x800||get32(exe)!=0x00905A4Du||
    get32(exe+nt)!=0x00004550u||
    get32(exe+nt+8)!=ExecutableTimestamp||
    get32(exe+nt+0x50)!=ExecutableImageSize)return false;
 const u8 expected[5]={0xE8,0x9E,0x1E,0x45,0xFF};
 if(memcmp(exe+0x171DB1D,expected,5))return false;
 auto**slotW=(void**)(base+CreateFileW_IAT);
 auto**slotA=(void**)(base+CreateFileA_IAT);
 if(!*slotW||!*slotA)return false;
 originalW=(OpenW)*slotW;
 originalA=(OpenA)*slotA;
 // Both IAT slots lie on the same native 4 KiB page. Publish/restore
 // transactionally and never leave a half-installed hook after failure.
 DWORD previous=0,unused=0;
 if(!VirtualProtect(slotA,0x48,4,&previous))return false;
 *slotW=(void*)&openW;
 *slotA=(void*)&openA;
 if(!VirtualProtect(slotA,0x48,previous,&unused)){
  *slotW=(void*)originalW;
  *slotA=(void*)originalA;
  VirtualProtect(slotA,0x48,previous,&unused);
  return false;
 }
 _InterlockedExchange(&installed,1);
 return true;
}
void diagnostics(void(*fn)(const char*)){
 trace=fn;
 if(!installed){if(trace)trace("VGRAPH_IAT_NOT_INSTALLED: no virtual file redirect available.");return;}
 if(trace)trace("VGRAPH_IAT_INSTALLED: CreateFileW/A imports redirected for streaming_graph.core only.");
 long state=_InterlockedCompareExchange(&cacheState,0,0);
 if(trace&&state==2)trace("VGRAPH_CACHE_READY: virtual graph built before mod initialization.");
 else if(trace&&state==3)trace("VGRAPH_PASSTHROUGH: original already contains Warrior + Higgs.");
 else if(trace&&state==4)trace("VGRAPH_UNAVAILABLE: original graph unchanged, resource group injection failed.");
}
u32 openAttempts(){return u32(_InterlockedCompareExchange(&attempts,0,0));}
u32 successfulRedirects(){return u32(_InterlockedCompareExchange(&redirected,0,0));}
u32 errors(){return u32(_InterlockedCompareExchange(&failed,0,0));}
long status(){return _InterlockedCompareExchange(&cacheState,0,0);}
}
