#include "win_api.hpp"
#include "sha256.hpp"
using namespace smallcargo;
extern "C" long _InterlockedExchange(volatile long*,long);
extern "C" long _InterlockedCompareExchange(volatile long*,long,long);
#pragma intrinsic(_InterlockedExchange,_InterlockedCompareExchange)
extern "C" void* memcpy(void*d,const void*s,SIZE_T n){auto*a=(volatile u8*)d;auto*b=(const volatile u8*)s;while(n--)*a++=*b++;return d;}
extern "C" void* memset(void*d,int v,SIZE_T n){auto*a=(volatile u8*)d;while(n--)*a++=u8(v);return d;}
extern "C" int memcmp(const void*a,const void*b,SIZE_T n){auto*x=(const u8*)a;auto*y=(const u8*)b;while(n--){if(*x!=*y)return *x-*y;++x;++y;}return 0;}
namespace {
HMODULE module;u64 image;HANDLE logfile=InvalidHandle;void* relay=nullptr;
volatile long catalogueReady=0;
SelectModel nativeModel;SelectAppearance nativeAppearance;
struct Patch {u32 rva;u8 length,expected[6],replacement[6];DWORD protection;bool changed;};
Patch patches[3];
bool readmem(u64 p,void*out,u32 n){SIZE_T got=0;return p>=0x10000&&p+n>=p&&ReadProcessMemory(GetCurrentProcess(),(void*)p,out,n,&got)&&got==n;}
bool writable(void*p,u32 n){MemoryInfo m={};return p&&VirtualQuery(p,&m,sizeof(m))==sizeof(m)&&m.state==0x1000&&
 (m.protect==4||m.protect==8||m.protect==0x40||m.protect==0x80)&&u64(p)>=u64(m.base)&&u64(p)+n>=u64(p)&&u64(p)+n<=u64(m.base)+m.size;}
void log(const char*s){if(logfile==InvalidHandle)return;DWORD n=0,w=0;while(s[n])++n;WriteFile(logfile,s,n,&w,nullptr);WriteFile(logfile,"\r\n",2,&w,nullptr);FlushFileBuffers(logfile);}
void number(char*&s,u32 v){char b[12];u32 n=0;do{b[n++]=char('0'+v%10);v/=10;}while(v);while(n)*s++=b[--n];}
void add(char*&s,const char*t){while(*t)*s++=*t++;}
void collected(void* depot,Array* output){
 ((void(*)(void*,Array*))(image+CollectorRva))(depot,output);
 Counts counts;
 if(!patchArray(output,image,readmem,writable,counts)){
  log("CATALOGUE_BLOCKED: unexpected resource/table; no size changed.");return;
 }
 _InterlockedExchange(&catalogueReady,1);
 char message[200],*p=message;
 add(p,"CATALOGUE_READY total=");number(p,counts.total);add(p," converted_to_S=");number(p,counts.changed);
 add(p," already_S=");number(p,counts.small);add(p," special_preserved=");number(p,counts.special);*p=0;log(message);
}
void* selectModel(void* manager,const u8* bag){
 return _InterlockedCompareExchange(&catalogueReady,0,0)?model(manager,bag,nativeModel):nativeModel(manager,bag);
}
void* selectAppearance(void* context,const u8* instance){
 return _InterlockedCompareExchange(&catalogueReady,0,0)?appearance(context,instance,nativeAppearance):nativeAppearance(context,instance);
}
bool supportedImage(){
 static WCHAR path[4096];DWORD n=GetModuleFileNameW(nullptr,path,4096);if(!n||n>=4096)return false;
 HANDLE f=CreateFileW(path,0x80000000,7,nullptr,3,0,nullptr);if(f==InvalidHandle)return false;
 long long size=0;bool ok=GetFileSizeEx(f,&size)&&size==117851944;
 craft::Sha256 hash;static u8 buffer[65536];u64 total=0;
 while(ok){if(!ReadFile(f,buffer,sizeof(buffer),&n,nullptr)){ok=false;break;}if(!n)break;hash.update(buffer,n);total+=n;}
 CloseHandle(f);u8 digest[32];hash.finish(digest);
 return ok&&total==117851944&&!memcmp(digest,ExeDigest,32);
}
bool prepare(){
 const u32 rvas[]={HookRva,ModelRva,AppearanceRva};
 const u8* expected[]={ExpectedCall,ExpectedModel,ExpectedAppearance};
 const u8 lengths[]={5,6,5};
 for(u32 i=0;i<3;++i){auto&p=patches[i];p.rva=rvas[i];p.length=lengths[i];memcpy(p.expected,expected[i],p.length);
  u8 bytes[6];if(!readmem(image+p.rva,bytes,p.length)||memcmp(bytes,p.expected,p.length))return false;
 }
 const u64 site=image+HookRva,center=site&~0xFFFFull;
 for(u64 delta=0x10000;delta<0x60000000&&!relay;delta+=0x10000){
  relay=VirtualAlloc((void*)(center+delta),4096,0x3000,4);
  if(!relay&&center>delta+0x10000)relay=VirtualAlloc((void*)(center-delta),4096,0x3000,4);
 }
 if(!relay)return false;
 const u64 targets[]={u64(&collected),u64(&selectModel),u64(&selectAppearance)};
 for(u32 i=0;i<3;++i){auto&p=patches[i];u8*slot=(u8*)relay+i*128;
  const long long offset=(long long)u64(slot)-(long long)(image+p.rva+5);
  if(offset<(-2147483647LL-1)||offset>2147483647LL)return false;
  u8 stub[14]={0xFF,0x25,0,0,0,0};memcpy(stub+6,&targets[i],8);memcpy(slot,stub,14);
  memset(p.replacement,0x90,p.length);p.replacement[0]=i==0?0xE8:0xE9;
  const i32 relative=i32(offset);memcpy(p.replacement+1,&relative,4);
  if(i){
   // Both verified prologues contain no RIP-relative instructions.
   memcpy(slot+32,p.expected,p.length);const u64 resume=image+p.rva+p.length;
   memcpy(stub+6,&resume,8);memcpy(slot+32+p.length,stub,14);
   if(i==1)nativeModel=(SelectModel)(slot+32);else nativeAppearance=(SelectAppearance)(slot+32);
  }
 }
 DWORD old;return VirtualProtect(relay,4096,0x20,&old)&&FlushInstructionCache(GetCurrentProcess(),relay,4096);
}
struct Thread {HANDLE h;bool suspended;};Thread threads[512];u32 threadCount=0;
bool closeThreads(){
 bool ok=true;
 for(u32 i=0;i<threadCount;++i){auto&t=threads[i];if(t.suspended){bool resumed=false;
  for(u32 j=0;j<3&&!resumed;++j){if(ResumeThread(t.h)!=0xFFFFFFFF)resumed=true;
   else{DWORD code=259;if(GetExitCodeThread(t.h,&code)&&code!=259)resumed=true;}}
  if(!resumed)ok=false;}CloseHandle(t.h);
 }threadCount=0;return ok;
}
bool collectThreads(){
 HANDLE snap=CreateToolhelp32Snapshot(4,0);if(snap==InvalidHandle)return false;
 ThreadEntry e={};e.size=sizeof(e);BOOL more=Thread32First(snap,&e);bool ok=more!=0;
 const DWORD self=GetCurrentThreadId(),pid=GetCurrentProcessId();
 while(more){if(e.pid==pid&&e.tid!=self){
  if(threadCount==512){ok=false;break;}HANDLE h=OpenThread(2|8|0x40,0,e.tid);
  if(!h||GetProcessIdOfThread(h)!=pid||GetThreadId(h)!=e.tid){if(h)CloseHandle(h);ok=false;break;}
  threads[threadCount++]={h,false};
 }e.size=sizeof(e);more=Thread32Next(snap,&e);}
 if(ok&&GetLastError()!=18)ok=false;CloseHandle(snap);if(!ok)closeThreads();return ok;
}
// 0=retry/no changes, 1=installed, 2=critical cleanup failure, 3=too late.
int install(){
 if(!collectThreads())return 0;
 bool ok=true,late=false,rollback=true;DWORD ignored=0;
 Context ctx={};ctx.flags=0x00100001;
 for(u32 i=0;i<threadCount&&ok;++i){auto&t=threads[i];
  if(SuspendThread(t.h)==0xFFFFFFFF){ok=false;break;}t.suspended=true;
  if(!GetThreadContext(t.h,&ctx)){ok=false;break;}
  for(const auto&p:patches)if(ctx.rip>image+p.rva&&ctx.rip<image+p.rva+p.length)ok=false;
 }
 // B6D3F0 publishes the manager BEFORE collection. Refuse late load, because
 // changing an already running inventory would leave cached footprints stale.
 if(ok){u64 manager=0;if(!readmem(image+ManagerRva,&manager,8))ok=false;else if(manager){late=true;ok=false;}}
 if(ok)for(const auto&p:patches)if(memcmp((void*)(image+p.rva),p.expected,p.length))ok=false;
 if(ok)for(auto&p:patches){void*site=(void*)(image+p.rva);
  if(!VirtualProtect(site,p.length,0x40,&p.protection)){ok=false;break;}
  memcpy(site,p.replacement,p.length);p.changed=true;
  const bool flushed=FlushInstructionCache(GetCurrentProcess(),site,p.length)!=0;
  const bool restored=VirtualProtect(site,p.length,p.protection,&ignored)!=0;
  if(!flushed||!restored){ok=false;break;}
 }
 if(!ok)for(u32 i=3;i;--i){auto&p=patches[i-1];if(!p.changed)continue;void*site=(void*)(image+p.rva);
  if(!VirtualProtect(site,p.length,0x40,&ignored)){rollback=false;continue;}
  memcpy(site,p.expected,p.length);const bool flushed=FlushInstructionCache(GetCurrentProcess(),site,p.length)!=0;
  const bool restored=VirtualProtect(site,p.length,p.protection,&ignored)!=0;
  if(!flushed||!restored)rollback=false;else p.changed=false;
 }
 const bool resumed=closeThreads();if(!rollback||!resumed)return 2;return ok?1:late?3:0;
}
DWORD WINAPI worker(void*){
 // ASI loaders can also load us in a crash-report helper. Do not open/truncate
 // the game's log, or create hooks, in any executable other than DS2.exe.
 static WCHAR processPath[4096];DWORD processLength=GetModuleFileNameW(nullptr,processPath,4096);
 if(!processLength||processLength>=4096||!isGamePath(processPath,processLength))return 0;
 static WCHAR path[4096];DWORD n=GetModuleFileNameW(module,path,4096);if(!n||n>4000)return 0;
 while(n&&path[n-1]!=L'\\'&&path[n-1]!=L'/')--n;
 const WCHAR name[]=L"ds2_all_containers_small.log";for(u32 i=0;i<sizeof(name)/sizeof(WCHAR);++i)path[n+i]=name[i];
 logfile=CreateFileW(path,0x40000000,7,nullptr,2,0,nullptr);
 log("All Containers Small 1.0.0; DS2 1.10.89.0.");image=u64(GetModuleHandleW(nullptr));
 if(!supportedImage()){log("VERSION_BLOCKED: unsupported executable SHA256/size. No changes.");return 0;}
 HMODULE pinned=nullptr;if(!GetModuleHandleExW(5,(const WCHAR*)&worker,&pinned)){log("HOOK_BLOCKED: cannot pin module.");return 0;}
 if(!prepare()){if(relay)VirtualFree(relay,0,0x8000);log("HOOK_BLOCKED: original bytes differ or relay allocation failed.");return 0;}
 int result=0;for(u32 attempt=0;attempt<20&&!result;++attempt){result=install();if(!result)Sleep(25);}
 if(result==2){log("CRITICAL: rollback/thread-resume failed. Restart game; EXE file untouched.");return 0;}
 if(result!=1){VirtualFree(relay,0,0x8000);log(result==3?"LATE_LOAD_BLOCKED: catalogue already initializing; restart with an early ASI loader.":"HOOK_BLOCKED: transaction could not complete; native call retained.");return 0;}
 log("HOOKS_READY: 3 hooks; size S + small cases; functional entities preserved; waiting for native catalogue.");return 0;
}
}
extern "C" BOOL WINAPI DllMain(HMODULE instance,DWORD reason,void*){
 if(reason==1){module=instance;DisableThreadLibraryCalls(instance);HANDLE h=CreateThread(nullptr,0,worker,nullptr,0,nullptr);if(h)CloseHandle(h);}return 1;
}
