#include "win_api.hpp"
#include "sha256.hpp"
#include "backpack.hpp"
extern "C" void* memcpy(void*d,const void*s,SIZE_T n){auto*a=(volatile u8*)d;auto*b=(const volatile u8*)s;while(n--)*a++=*b++;return d;}
extern "C" void* memset(void*d,int c,SIZE_T n){auto*a=(volatile u8*)d;while(n--)*a++=(u8)c;return d;}
extern "C" int memcmp(const void*a,const void*b,SIZE_T n){auto*x=(const u8*)a;auto*y=(const u8*)b;while(n--){if(*x!=*y)return *x<*y?-1:1;++x;++y;}return 0;}
extern "C" long _InterlockedCompareExchange(long volatile*,long,long);
extern "C" long _InterlockedExchange(long volatile*,long);
#pragma intrinsic(_InterlockedCompareExchange)
#pragma intrinsic(_InterlockedExchange)
using namespace craft;
namespace {
constexpr u64 ManagerRva=0x623E540, OriginalRva=0xB6F9C0, SiteRva=0x171DB1D;
constexpr u64 BackpackSiteRva=0x1529896, BackpackOriginalRva=0xB6FC20, VectorCopyRva=0xB7E950;
constexpr u8 BackpackCallBytes[5]={0xE8,0x85,0x63,0x64,0xFF};
constexpr u32 MaxSnapshots=1024, ConfigLimit=1024*1024;
constexpr u8 CallBytes[5]={0xE8,0x9E,0x1E,0x45,0xFF};
constexpr u8 ExpectedHash[32]={0xbf,0x3d,0x1c,0x66,0x55,0x45,0x93,0x0b,0xc8,0x50,0xd8,0xf5,0xdf,0x48,0x6f,0x73,0x95,0x88,0x5b,0xb7,0x29,0xd4,0xfd,0x40,0x8f,0xdb,0x03,0x39,0x0d,0xe0,0x76,0x5b};
HMODULE module;u64 image;WCHAR exePath[2048],folder[2048],iniPath[2048],logPath[2048],catPath[2048],templatePath[2048];
Settings settings;char configData[ConfigLimit+1];u8 hashChunk[65536];
Recipe recipes[MaxRecipes];u32 recipeCount;const char* scanIssue="not_started";i32 scanResourceCount=0,scanInstanceCount=0;u32 scanIndex=0;Instance nativeCopy[MaxMenu];
struct Snapshot {Vector header;Instance entries[MaxMenu];};
Snapshot* snapshots;u32 snapshotCount;volatile long busy=0;u32 menuCount=0;u64 lastCatalogueManager=0;u32 lastCatalogueCount=0;u64 warnedManager=0;u32 warnedCount=0;
using ListFn=Vector*(*)(u64,u64);ListFn original;
using BackpackListFn=void(*)(u64,u8,Vector*);
using VectorCopyFn=void(*)(Vector*,const Instance*,i32);
BackpackListFn originalBackpack;
Instance backpackOutput[MaxMenu];u32 backpackMenuCount=0;
struct Text {char b[1536];u32 n=0;void add(const char*s){while(*s&&n<sizeof(b)-1)b[n++]=*s++;b[n]=0;}void ch(char c){if(n<sizeof(b)-1)b[n++]=c;b[n]=0;}void dec(u64 x){char q[24];u32 c=0;do{q[c++]=char('0'+x%10);x/=10;}while(x);while(c)ch(q[--c]);}void hex(u64 x,u32 digits=8){static const char*h="0123456789ABCDEF";add("0x");while(digits)ch(h[(x>>(4*--digits))&15]);}void clean(const char*s){for(u32 i=0;s[i]&&i<384;++i){u8 c=(u8)s[i];ch(c<32||c==127?' ':s[i]);}}};
bool readmem(u64 p,void*out,u32 n){if(p<0x10000 || p+n<p)return false;SIZE_T done=0;return ReadProcessMemory(GetCurrentProcess(),(void*)p,out,n,&done)&&done==n;}
template<class T>bool rd(u64 p,T&v){return readmem(p,&v,sizeof(v));}
bool path(WCHAR*out,const WCHAR*name){u32 i=0;for(;folder[i];++i){if(i>=2000)return false;out[i]=folder[i];}for(u32 j=0;name[j];++j){if(i>=2047)return false;out[i++]=name[j];}out[i]=0;return true;}
bool writeall(HANDLE h,const void*p,u32 n){auto*b=(const u8*)p;while(n){DWORD w=0;if(!WriteFile(h,b,n,&w,nullptr)||!w)return false;b+=w;n-=w;}return true;}
void log(const Text&t){HANDLE h=CreateFileW(logPath,4,3,nullptr,4,0x80,nullptr);if(h==InvalidHandle)return;writeall(h,t.b,t.n);writeall(h,"\r\n",2);CloseHandle(h);}
void say(const char*s){Text t;t.add(s);log(t);}
bool filehash(const WCHAR*p,u8 out[32]){HANDLE h=CreateFileW(p,0x80000000,1,nullptr,3,0x80,nullptr);if(h==InvalidHandle)return false;Sha256 sha;DWORD n=0;bool ok=true;u64 bytes=0;for(;;){if(!ReadFile(h,hashChunk,sizeof(hashChunk),&n,nullptr)){ok=false;break;}if(!n)break;bytes+=n;sha.update(hashChunk,n);}CloseHandle(h);if(ok&&bytes){sha.finish(out);return true;}return false;}
bool config(){HANDLE h=CreateFileW(iniPath,0x80000000,1,nullptr,3,0x80,nullptr);if(h==InvalidHandle){say("CONFIG_ERROR: supplied INI missing; no hook installed.");return false;}long long n=0;bool ok=GetFileSizeEx(h,&n)&&n>0&&n<=ConfigLimit;DWORD got=0;if(ok)ok=ReadFile(h,configData,(DWORD)n,&got,nullptr)&&got==(DWORD)n;CloseHandle(h);if(!ok){say("CONFIG_ERROR: unreadable/oversized INI; no hook installed.");return false;}Error e;if(!parse(configData,(u32)n,settings,e)){settings.enabled=false;Text t;t.add("CONFIG_ERROR line=");t.dec(e.line);t.add(" ");t.add(e.message);log(t);return false;}return true;}
bool validCost(u64 res){i32 n=0;u64 p=0;if(!rd(res+0x58,n)||!rd(res+0x60,p)||n<0||n>6||(!p&&n))return false;u32 mask=0;for(i32 i=0;i<n;++i){struct Cost{u16 type,pad;u32 amount;};Cost c;if(!rd(p+8*i,c)||c.type>5||(mask&(1u<<c.type)))return false;mask|=1u<<c.type;}return true;}
bool scan(u64 manager){
 scanIssue="manager_header";scanResourceCount=0;scanInstanceCount=0;scanIndex=0;
 i32 nr=0,ni=0;u64 rp=0,ip=0,symbols=0,special=0;
 if(!rd(manager+8,nr)||!rd(manager+0x10,rp)||!rd(manager+0x38,ni)||!rd(manager+0x40,ip)||!rd(manager+0x190,symbols))return false;
 scanResourceCount=nr;scanInstanceCount=ni;scanIssue="counts_arrays_or_symbols";
 if(nr<1||nr>(i32)MaxRecipes||ni<1||ni>(i32)MaxRecipes||!rp||!ip||!symbols||!rd(symbols+0x28,special))return false;
 recipeCount=0;
 for(i32 i=0;i<ni;++i){scanIndex=(u32)i;scanIssue="master_instance_or_resource_pointer";Recipe r={};if(!rd(ip+16*i,r.instance))return false;u32 index=r.instance.flags&0xFFF;if(index>=(u32)nr||!rd(rp+8*index,r.resource)||!r.resource)return false;
  scanIssue="resource_bytes";u8 raw[0xB0];if(!readmem(r.resource,raw,sizeof(raw)))return false;
  memcpy(&r.key,raw+0x20,4);memcpy(&r.baggage,raw+0x28,8);r.usage=raw[0x52];r.dlc=raw[0x74];r.valid=0;
  u8 bag[0x88];if(r.baggage&&readmem(r.baggage,bag,sizeof(bag))){memcpy(&r.baggageKey,bag+0x44,4);r.caseType=bag[0x49];i32 mission=0;memcpy(&mission,bag+0x74,4);r.mission=mission>0;r.valid=(r.resource!=special && validCost(r.resource) && r.instance.vtable!=0);}
  // Coffinboard is a separate native creation branch; require its symbol resource.
  if(r.caseType==18){i32 vehicles=0;u64 vehicleArray=0,coffin=0;if(!rd(symbols+0x40,vehicles)||vehicles<3||!rd(symbols+0x48,vehicleArray)||!vehicleArray||!rd(vehicleArray+0x10,coffin)||!coffin)r.valid=0;}
  recipes[recipeCount++]=r;
 }
 mark_duplicates(recipes,recipeCount);scanIssue="ok";return true;
}
const char* usage(u8 u){switch(u){case 0:return "None";case 1:return "Normal";case 2:return "Vehicle";case 3:return "Post";case 4:return "SafetyHouse";case 5:return "BackPackCustomize";case 6:return "RawMaterial";case 7:return "VehicleCustomize";case 8:return "CostumeCustomize";case 9:return "All";default:return "Unknown";}}
const char* reason(Skip s){switch(s){case Skip::None:return "eligible";case Skip::NotSelected:return "vanilla_by_config";case Skip::UnsupportedPath:return "separate_native_path";case Skip::MissingResource:return "resource_not_validated";case Skip::DlcSpecial:return "dlc_native_only";case Skip::Quest:return "mission_specific_baggage";case Skip::Duplicate:return "ambiguous_namecode";case Skip::Facility:return "facility_not_supported";case Skip::AlreadyNative:return "native";}return "unknown";}
void itemName(const Recipe&r,char out[385]){
 out[0]=0;if(!r.baggage)return;
 // This native getter writes an engine String (one char*), not a borrowed const char*.
 using GetName=u64*(*)(u64,u64*);using Destroy=void(*)(u64*);u64 string=0;
 ((GetName)(image+0xBE75E0))(r.baggage,&string);
 if(string){for(u32 i=0;i<384;++i){char c=0;if(!rd(string+i,c))break;out[i]=c;out[i+1]=0;if(!c)break;}((Destroy)(image+0xA38A0))(&string);}
}
void exportCatalogue(u64 manager,u8 mask){
 if(!settings.dump||(lastCatalogueManager==manager&&lastCatalogueCount==recipeCount))return;
 HANDLE csv=CreateFileW(catPath,0x40000000,1,nullptr,2,0x80,nullptr);
 HANDLE tpl=CreateFileW(templatePath,0x40000000,1,nullptr,2,0x80,nullptr);
 if(csv==InvalidHandle&&tpl==InvalidHandle){say("CATALOGUE_WRITE_ERROR: cannot write catalogue/template next to ASI.");return;}
 bool csvOK=csv!=InvalidHandle,tplOK=tpl!=InvalidHandle;
 const char*head="recipe_key\tname\tusage\tbag_namecode\tmaster_flags\tstatus_at_this_facility\r\n";
 if(csv!=InvalidHandle){u32 n=0;while(head[n])++n;csvOK=writeall(csv,head,n);}
 const char*intro="; Generated from the loaded DS2 catalogue. THIS FILE IS NOT READ BY THE MOD.\r\n; Copy selected entries into [Items] in ds2_crafting_unlocks.ini.\r\n; inherit = DefaultUnlock; 0 = native progression; 1 = early unlock.\r\n; Hex keys are real catalogue NameCodes, not EDSItemId enum numbers.\r\n; Separate/native-only categories are comments, not supported unlock promises.\r\n\r\n[Items]\r\n";
 if(tpl!=InvalidHandle){u32 n=0;while(intro[n])++n;tplOK=writeall(tpl,intro,n);}
 for(u32 i=0;i<recipeCount;++i){const Recipe&r=recipes[i];char name[385]={};if(r.valid)itemName(r,name);
  Text t;t.hex(r.key);t.ch('\t');if(name[0])t.clean(name);else t.add("[name unavailable]");t.ch('\t');t.add(usage(r.usage));t.ch('\t');t.hex(r.baggageKey);t.ch('\t');t.hex(r.instance.flags);t.ch('\t');t.add(reason(r.usage==5?backpack_eligibility(settings,r):eligibility(settings,r,mask)));t.add("\r\n");if(csv!=InvalidHandle)csvOK=writeall(csv,t.b,t.n)&&csvOK;
  Text q;q.add("; ");if(name[0])q.clean(name);else q.add("[name unavailable]");q.add(" | ");q.add(usage(r.usage));q.add(" | ");q.add(reason(r.usage==5?backpack_eligibility(settings,r):eligibility(settings,r,mask)));q.add("\r\n");
  if((!supported_usage(r.usage)&&(r.usage!=5||backpack_kind(r.key)==BackpackKind::None))||!r.valid||(r.dlc&1)||r.mission||r.duplicate)q.add("; native-only: ");
  q.hex(r.key);q.add("=inherit\r\n\r\n");if(tpl!=InvalidHandle)tplOK=writeall(tpl,q.b,q.n)&&tplOK;
 }
 if(csv!=InvalidHandle)CloseHandle(csv);if(tpl!=InvalidHandle)CloseHandle(tpl);
 if(csvOK&&tplOK){lastCatalogueManager=manager;lastCatalogueCount=recipeCount;say("CATALOGUE_EXPORTED: TSV + generated item selection template.");}else say("CATALOGUE_WRITE_ERROR: export incomplete.");
}
Vector* makeMenu(Vector* native,u64 facility){
 u64 manager=0;Vector v={};u32 facilityType=0;
 if(!native||!rd((u64)native,v)||v.count<0||v.count>(i32)MaxMenu||v.capacity<v.count||(!v.data&&v.count)||!rd(image+ManagerRva,manager)||!manager){say("VANILLA: invalid native vector header or catalogue manager.");return native;}
 if(facility&&!rd(facility+0xD0,facilityType))return native;u8 mask=facility_mask(facilityType,facility==0);
 if(!mask){Text t;t.add("VANILLA: unknown facility type=");t.hex(facilityType);log(t);return native;}
 if(v.count&&!readmem((u64)v.data,nativeCopy,v.count*sizeof(Instance)))return native;
 if(!TryAcquireSRWLockShared((void*)(manager+0x138))){say("VANILLA: catalogue busy; no global data changed.");return native;}
 bool ok=scan(manager);ReleaseSRWLockShared((void*)(manager+0x138));
 if(!ok){Text t;t.add("VANILLA: catalogue validation failed step=");t.add(scanIssue);t.add(" resources=");t.dec((u32)scanResourceCount);t.add(" instances=");t.dec((u32)scanInstanceCount);t.add(" index=");t.dec(scanIndex);log(t);return native;}
 // Native string calls and all file I/O are OUTSIDE the catalogue lock.
 if(warnedManager!=manager||warnedCount!=recipeCount){
  for(u32 i=0;i<settings.count;++i){bool found=false;for(u32 j=0;j<recipeCount;++j)if(recipes[j].key==settings.rules[i].key){found=true;break;}
   if(!found){Text t;t.add("CONFIG_WARNING: recipe key not in currently loaded catalogue: ");t.hex(settings.rules[i].key);log(t);}
  }warnedManager=manager;warnedCount=recipeCount;
 }
 exportCatalogue(manager,mask);
 if(snapshotCount==MaxSnapshots){say("VANILLA: snapshot lifetime budget reached; restart game.");return native;}
 Snapshot&s=snapshots[snapshotCount];Build b=build(settings,recipes,recipeCount,nativeCopy,v.count,mask,s.entries);
 Text t;t.add("MENU ");t.dec(++menuCount);t.add(" facility=");t.hex(facilityType);t.add(" recipes=");t.dec(recipeCount);t.add(" native=");t.dec(v.count);t.add(" added=");t.dec(b.added);t.add(" total=");t.dec(b.count);t.add(" overflow=");t.dec(b.overLimit);log(t);
 if(!b.ok||!b.added)return native;
 s.header.count=(i32)b.count;s.header.capacity=MaxMenu;s.header.data=s.entries;++snapshotCount;
 // Retain every published snapshot until process exit: UI entries hold raw instance pointers.
 return &s.header;
}
extern "C" Vector* craftingList(u64 unused,u64 facility){
 // Exactly one original call, with its real register arguments retained.
 Vector*native=original(unused,facility);
 if(_InterlockedCompareExchange(&busy,1,0)!=0)return native;
 Vector*result=makeMenu(native,facility);_InterlockedExchange(&busy,0);return result;
}

// Validate the read-only native ID maps consumed by 01529860/01526F50.
// No recipe availability/state bits are touched by this check.
bool backpackMappingsMatch() {
 for(const auto& item:BackpackItems) {
  u64 table=0;u32 count=0;
  if(item.kind==BackpackKind::Module){table=image+0x2DD25B0;count=95;}
  else if(item.kind==BackpackKind::Charm){table=image+0x2DD37D0;count=9;}
  else if(item.kind==BackpackKind::Cover){table=image+0x2DD3780;count=2;}
  bool found=false;
  for(u32 i=0;i<count;++i){u8 id=0;u32 key=0;
   if(!rd(table+8*i,id)||!rd(table+8*i+4,key))return false;
   if(key!=item.key)continue;
   if(item.kind==BackpackKind::Module)found=id>0&&!(id>=20&&id<=94);
   else if(item.kind==BackpackKind::Charm)found=id>0&&id<9;
   else found=id<2;
   break;
  }
  if(!found){Text t;t.add("BACKPACK_MAPPING_BLOCKED key=");t.hex(item.key);log(t);return false;}
 }
 return true;
}
void augmentBackpack(u64 manager,Vector*out) {
 Vector v={};u64 currentManager=0;
 if(!out||!rd(image+ManagerRva,currentManager)||!manager||currentManager!=manager||
    !rd((u64)out,v)||v.count<0||v.count>(i32)MaxMenu||v.capacity<v.count||(!v.data&&v.count)) {
  say("BACKPACK_VANILLA: invalid menu vector/manager.");return;
 }
 // This verified call site supplies MenuInfo+0x228, never a catalogue-manager vector.
 const u64 address=(u64)out;
 if(address>=manager&&address-manager<0x200){say("BACKPACK_VANILLA: refusing manager-owned vector.");return;}
 if(v.count&&!readmem((u64)v.data,nativeCopy,(u32)v.count*sizeof(Instance)))return;
 if(!TryAcquireSRWLockShared((void*)(manager+0x138))){say("BACKPACK_VANILLA: catalogue busy.");return;}
 bool ok=scan(manager);ReleaseSRWLockShared((void*)(manager+0x138));
 if(!ok){Text t;t.add("BACKPACK_VANILLA: catalogue validation failed step=");t.add(scanIssue);log(t);return;}
 exportCatalogue(manager,15);
 const Build b=build_backpack(settings,recipes,recipeCount,nativeCopy,(u32)v.count,backpackOutput);
 if(b.ok&&b.added) {
  // Destination remains engine-owned. This is the exact copy routine already used
  // by the original producer. Native reserve/reallocation, constructors and later
  // menu destruction retain the game's allocator and ownership convention.
  // Run AFTER releasing the catalogue lock, on the original native menu thread.
  ((VectorCopyFn)(image+VectorCopyRva))(out,backpackOutput,(i32)b.count);
 }
 u32 moduleAdds=0,coverAdds=0,charmAdds=0;
 for(u32 j=(u32)v.count;j<b.count&&b.ok;++j){
  const u32 index=backpackOutput[j].flags&0xFFF;
  for(u32 i=0;i<recipeCount;++i)if((recipes[i].instance.flags&0xFFF)==index){
   switch(backpack_kind(recipes[i].key)){case BackpackKind::Module:++moduleAdds;break;case BackpackKind::Cover:++coverAdds;break;case BackpackKind::Charm:++charmAdds;break;default:break;}break;
  }
 }
 Text t;t.add("BACKPACK_MENU ");t.dec(++backpackMenuCount);t.add(" native=");t.dec((u32)v.count);
 t.add(" added=");t.dec(b.added);t.add(" total=");t.dec(b.count);t.add(" overflow=");t.dec(b.overLimit);
 t.add(" modules=");t.dec(moduleAdds);t.add(" covers=");t.dec(coverAdds);t.add(" charms=");t.dec(charmAdds);
 t.add(b.ok?" status=LIST_READY":" status=VANILLA");log(t);
}
extern "C" void backpackList(u64 manager,u8 requestedUsage,Vector*out) {
 // Exactly one original producer call; it preserves all native progression results.
 originalBackpack(manager,requestedUsage,out);
 if(requestedUsage!=5||_InterlockedCompareExchange(&busy,1,0)!=0)return;
 augmentBackpack(manager,out);
 _InterlockedExchange(&busy,0);
}

struct Thread {HANDLE h;DWORD id;bool suspended;};Thread threads[512];u32 threadCount;
void closeThreads(){for(u32 i=0;i<threadCount;++i)if(threads[i].h){CloseHandle(threads[i].h);threads[i].h=nullptr;}threadCount=0;}
bool resumeThreads(){bool all=true;for(u32 i=0;i<threadCount;++i){Thread&t=threads[i];if(!t.suspended)continue;bool ok=false;for(u32 retry=0;retry<3;++retry){if(ResumeThread(t.h)!=0xFFFFFFFF){ok=true;break;}DWORD exit=259;if(GetExitCodeThread(t.h,&exit)&&exit!=259){ok=true;break;}}t.suspended=false;if(!ok)all=false;}return all;}
bool collectThreads(){threadCount=0;HANDLE snap=CreateToolhelp32Snapshot(4,0);if(snap==InvalidHandle)return false;ThreadEntry e={};e.size=sizeof(e);BOOL more=Thread32First(snap,&e);bool ok=more!=0;DWORD pid=GetCurrentProcessId(),self=GetCurrentThreadId();
 while(more){if(e.pid==pid&&e.tid!=self){if(threadCount==512){ok=false;break;}HANDLE h=OpenThread(2|8|0x40,0,e.tid);if(!h||GetProcessIdOfThread(h)!=pid||GetThreadId(h)!=e.tid){if(h)CloseHandle(h);ok=false;break;}threads[threadCount++]={h,e.tid,false};}e.size=sizeof(e);more=Thread32Next(snap,&e);}
 if(ok && GetLastError()!=18)ok=false;
 CloseHandle(snap);if(!ok)closeThreads();return ok;
}
bool patchOnce(u64 siteRva,const u8* expected,void* relay){
 if(!collectThreads())return false;
 bool ok=true;Context c={};c.flags=0x00100001;
 for(u32 i=0;i<threadCount;++i){Thread&t=threads[i];if(SuspendThread(t.h)==0xFFFFFFFF){ok=false;break;}t.suspended=true;if(!GetThreadContext(t.h,&c)||(c.rip>image+siteRva&&c.rip<image+siteRva+5)){ok=false;break;}}
 u8*site=(u8*)(image+siteRva);bool written=false;DWORD old=0;
 if(ok&&memcmp(site,expected,5)!=0)ok=false;
 if(ok){i32 displacement=(i32)((u64)relay-(image+siteRva+5));u8 patch[5]={0xE8};memcpy(patch+1,&displacement,4);if(VirtualProtect(site,5,0x40,&old)){
   memcpy(site,patch,5);written=true;
   if(!FlushInstructionCache(GetCurrentProcess(),site,5)){memcpy(site,expected,5);FlushInstructionCache(GetCurrentProcess(),site,5);written=false;}
   DWORD ignored=0;if(!VirtualProtect(site,5,old,&ignored)){// Roll back while all captured threads remain suspended.
     memcpy(site,expected,5);FlushInstructionCache(GetCurrentProcess(),site,5);VirtualProtect(site,5,old,&ignored);written=false;
   }
  }}
 bool resumed=resumeThreads();closeThreads();if(!resumed)say("CRITICAL: a thread resume failed; close and restart the game.");return written;
}
void* relayNear(u64 siteRva,u64 destination){u64 base=(image+siteRva)&~u64(0xFFFF);for(u64 distance=0x10000;distance<0x70000000;distance+=0x10000){for(int side=0;side<2;++side){if(side&&base<=distance+0x10000)continue;u64 address=side?base-distance:base+distance;long long delta=(long long)address-(long long)(image+siteRva+5);if(delta<(-2147483647LL-1)||delta>2147483647LL)continue;void*p=VirtualAlloc((void*)address,0x1000,0x3000,4);if(p){u8 jump[14]={0xFF,0x25,0,0,0,0};u64 target=destination;memcpy(jump+6,&target,8);memcpy(p,jump,14);DWORD old;if(!VirtualProtect(p,0x1000,0x20,&old)||!FlushInstructionCache(GetCurrentProcess(),p,14)){VirtualFree(p,0,0x8000);return nullptr;}return p;}}}return nullptr;}
DWORD WINAPI init(void*){
 u32 n=GetModuleFileNameW(module,folder,2048);if(!n||n>=2048)return 0;while(n&&folder[n-1]!='\\'&&folder[n-1]!='/')--n;folder[n]=0;
 if(!path(iniPath,L"ds2_crafting_unlocks.ini")||!path(logPath,L"ds2_crafting_unlocks.log")||!path(catPath,L"ds2_crafting_catalogue.tsv")||!path(templatePath,L"ds2_crafting_items.generated.ini"))return 0;
 Text session;session.add("DS2 Crafting Unlocks 1.0.0 | fabrication + backpack | exact-build gate | pid=");session.dec(GetCurrentProcessId());session.add(" tick=");session.dec(GetTickCount64());log(session);
 if(!config())return 0;if(!settings.enabled){say("DISABLED: no patch installed.");return 0;}
 image=(u64)GetModuleHandleW(nullptr);n=GetModuleFileNameW(nullptr,exePath,2048);if(!n||n>=2048){say("VERSION_BLOCKED: executable path unavailable.");return 0;}
 u8 hash[32];if(!filehash(exePath,hash)||memcmp(hash,ExpectedHash,32)){say("VERSION_BLOCKED: installed DS2.exe SHA-256 differs from analysed binary. No patch.");return 0;}

 u8 sig[5];
 if(!readmem(image+SiteRva,sig,5)||memcmp(sig,CallBytes,5)){
  say("SIGNATURE_BLOCKED: normal fabrication callsite changed or owned by another mod. No patch.");return 0;
 }
 if(!readmem(image+BackpackSiteRva,sig,5)||memcmp(sig,BackpackCallBytes,5)||!backpackMappingsMatch()){
  say("SIGNATURE_BLOCKED: backpack callsite/maps differ from analysed binary. No patch.");return 0;
 }
 HMODULE pinned;if(!GetModuleHandleExW(1|4,(WCHAR*)&craftingList,&pinned)){say("INIT_FAILED: DLL pin failed.");return 0;}
 snapshots=(Snapshot*)VirtualAlloc(nullptr,sizeof(Snapshot)*MaxSnapshots,0x3000,4);
 if(!snapshots){say("INIT_FAILED: snapshot allocation failed.");return 0;}
 original=(ListFn)(image+OriginalRva);originalBackpack=(BackpackListFn)(image+BackpackOriginalRva);
 void*normalRelay=relayNear(SiteRva,(u64)&craftingList);
 void*backpackRelay=relayNear(BackpackSiteRva,(u64)&backpackList);
 if(!normalRelay||!backpackRelay){
  if(normalRelay)VirtualFree(normalRelay,0,0x8000);if(backpackRelay)VirtualFree(backpackRelay,0,0x8000);
  say("INIT_FAILED: near-relay allocation failed. No patch.");return 0;
 }
 bool normalInstalled=false;
 for(u32 attempt=0;attempt<12;++attempt){if(patchOnce(SiteRva,CallBytes,normalRelay)){normalInstalled=true;break;}Sleep(50);}
 if(!normalInstalled){VirtualFree(normalRelay,0,0x8000);VirtualFree(backpackRelay,0,0x8000);
  say("HOOK_NOT_INSTALLED: fabrication thread/signature/protection validation failed. Vanilla unchanged.");return 0;
 }
 say("HOOK_INSTALLED: fabrication RVA 0x0171DB1D -> original + menu-local recipe copies.");
 bool backpackInstalled=false;
 for(u32 attempt=0;attempt<12;++attempt){if(patchOnce(BackpackSiteRva,BackpackCallBytes,backpackRelay)){backpackInstalled=true;break;}Sleep(50);}
 if(!backpackInstalled){VirtualFree(backpackRelay,0,0x8000);
  say("PARTIAL: fabrication hook active, backpack hook NOT installed. Restart; send this log.");return 0;
 }
 say("HOOK_INSTALLED: backpack RVA 0x01529896 -> original + engine-owned menu-local copies. Open backpack customization.");
 return 0;
}
}
extern "C" BOOL WINAPI DllMain(HMODULE h,DWORD reason,void*){if(reason==1){module=h;DisableThreadLibraryCalls(h);HANDLE thread=CreateThread(nullptr,0,&init,nullptr,0,nullptr);if(thread)CloseHandle(thread);}return 1;}
