// No DS2 process, no IAT modifications. Native Windows test of the ACTUAL
// Crafting DirectStorage wrapper's selection, forwarding and fail-safe paths.
// All handles are fake; every filesystem access is intentionally omitted.
#include "../src/win_api.hpp"
#include "../src/sha256.hpp"
#include <cstdio>
#include <cstring>
#include <cwchar>
extern "C" long _InterlockedCompareExchange(long volatile*,long,long);
extern "C" long _InterlockedExchange(long volatile*,long);
extern "C" long _InterlockedIncrement(long volatile*);
extern "C" long long _InterlockedCompareExchange64(long long volatile*,long long,long long);
#pragma intrinsic(_InterlockedCompareExchange)
#pragma intrinsic(_InterlockedExchange)
#pragma intrinsic(_InterlockedIncrement)
#pragma intrinsic(_InterlockedCompareExchange64)
#include "../src/coffin_story_archive_cache.inl"
#include "../src/coffin_story_archive_storage.inl"
static unsigned calls=0,privateCalls=0,privateLinks=0,otherCalls=0;
static bool refusePrivate=false;
static const WCHAR* seen=nullptr;
HANDLE WINAPI shoeOpenMock(const WCHAR*name,DWORD,DWORD,void*,DWORD,DWORD,HANDLE){
 ++calls;seen=name;
 const bool archive=std::wcscmp(name,coffin_archive_cache::privatePath)==0;
 const bool links=std::wcscmp(name,coffin_archive_cache::privateLinksPath)==0;
 if(archive||links){
  ++privateCalls;if(links)++privateLinks;
  if(refusePrivate)return InvalidHandle;
 }else ++otherCalls;
 return links ? reinterpret_cast<HANDLE>(0xBEEFu) :
        archive ? reinterpret_cast<HANDLE>(0xB00Du) :
                  reinterpret_cast<HANDLE>(0xCAFEu);
}
int main(){
 unsigned tests=0,failures=0;
 auto check=[&](bool ok,const char*name){++tests;if(!ok){++failures;std::printf("TEST_FAIL %s\n",name);}};
 auto&st=coffin_archive_storage::state;
 auto&cs=coffin_archive_cache::cacheState;
 coffin_archive_storage::originalOpenW=&shoeOpenMock;
 bool names=coffin_archive_cache::locations();
 check(names,"native path builder");
 check(coffin_archive_storage::matchesOriginal(
   L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\DEATH STRANDING 2 - ON THE BEACH\\LocalCacheWinGame\\package\\package.42.02.core.stream"),"exact 42.02 DSAR");
 check(coffin_archive_storage::matchesOriginal(
   L"c:/games/DS2/LOCALCACHEWINGAME/package/PACKAGE.42.02.CORE.STREAM"),"case insensitive");
 check(!coffin_archive_storage::matchesOriginal(L"xpackage.42.02.core.stream"),"partial leaf prefix refused");
 check(!coffin_archive_storage::matchesOriginal(L"C:\\ds2\\package.42.00.core.stream"),"never redirect ordinary game 42.00");
 check(!coffin_archive_storage::matchesOriginal(L"C:\\ds2\\streaming_graph.core"),"never redirect graph");
 check(!coffin_archive_storage::matchesOriginal(L"C:\\ds2\\user-save.dat"),"never redirect savegame");
 const WCHAR*native=L"C:\\DS2\\LocalCacheWinGame\\package\\package.42.02.core.stream";
 const WCHAR*save=L"C:\\DS2\\userdata\\save01.dat";
 constexpr DWORD READ=0x80000000u,WRITE=0x40000000u;
 auto invoke=[&](const WCHAR*path,DWORD acc,DWORD mode){
  return coffin_archive_storage::readOverlay(path,acc,3,nullptr,mode,0x80,nullptr);
 };
 _InterlockedExchange(&st,2);_InterlockedExchange(&cs,2);
 calls=privateCalls=otherCalls=0;
 check(invoke(native,READ,3)==reinterpret_cast<HANDLE>(0xB00Du),"read native file201 routed to private cache");
 check(privateCalls==1&&otherCalls==0,"no redundant filesystem open on happy path");
 const WCHAR* links=L"C:\\DS2\\LocalCacheWinGame\\package\\streaming_links.stream";
 check(invoke(links,READ,3)==reinterpret_cast<HANDLE>(0xBEEFu),
       "new private links supplied before extended native Story Graph");
 check(privateLinks==1&&privateCalls==2&&otherCalls==0,
       "native Graph uses one 2048-byte extended links resource");
 check(invoke(L"C:\\DS2\\LocalCacheWinGame\\package\\l200_aus\\streaming_links.stream",READ,3)==
       reinterpret_cast<HANDLE>(0xCAFEu),"regional links never intercepted");
 check(invoke(save,READ,3)==reinterpret_cast<HANDLE>(0xCAFEu),"savegame always forwarded");
 check(invoke(native,WRITE,3)==reinterpret_cast<HANDLE>(0xCAFEu),"writes always forwarded");
 check(invoke(native,READ,2)==reinterpret_cast<HANDLE>(0xCAFEu),"non OPEN_EXISTING forwarded");
 check(invoke(L"C:\\DS2\\package.42.00.core.stream",READ,3)==reinterpret_cast<HANDLE>(0xCAFEu),
       "native vanilla 42.00 always forwarded");
 check(invoke(L"C:\\DS2\\streaming_graph.core",READ,3)==reinterpret_cast<HANDLE>(0xCAFEu),
       "native graph always forwarded");
 check(privateCalls==2&&privateLinks==1&&otherCalls==6,
       "only exact DSAR and canonical links private; six unrelated forwarded");
 refusePrivate=true;
 check(invoke(native,READ,3)==InvalidHandle,
       "failed private archive read must not pass through while Graph enlarged");
 check(invoke(links,READ,3)==InvalidHandle,
       "failed private links read must not pass through 2048-byte-short Shoe cache");
 check(privateCalls==4&&privateLinks==2&&otherCalls==6&&calls==10,
       "fail-closed on both native resources without fallback to wrong buffers");
 refusePrivate=false;
 _InterlockedExchange(&st,-1);
 check(invoke(native,READ,3)==reinterpret_cast<HANDLE>(0xCAFEu),"disabled hook entirely passthrough");
 _InterlockedExchange(&st,2);_InterlockedExchange(&cs,-1);
 check(invoke(native,READ,3)==reinterpret_cast<HANDLE>(0xCAFEu),"failed cache builder entirely passthrough");
 coffin_archive_storage::originalOpenW=nullptr;
 check(invoke(native,READ,3)==InvalidHandle,"invalid shoe chain refuses");
 std::printf("COFFIN_NATIVE_DIRECTSTORAGE_CHAIN_TESTS %u ERRORS %u\n",tests,failures);
 return failures?1:0;
}
