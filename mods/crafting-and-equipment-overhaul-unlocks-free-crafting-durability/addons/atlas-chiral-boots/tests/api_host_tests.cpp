#include "../src/vendor/win_api.hpp"
extern "C" void* memcpy(void*d,const void*s,SIZE_T n){for(SIZE_T i=0;i<n;++i)((u8*)d)[i]=((const u8*)s)[i];return d;}
extern "C" long _InterlockedCompareExchange(long volatile*,long,long);
extern "C" long _InterlockedExchange(long volatile*,long);
#pragma intrinsic(_InterlockedCompareExchange,_InterlockedExchange)
#include "../development/crafting-v198-addon-api/src/addon_host.inl"
API HANDLE WINAPI GetStdHandle(DWORD);
namespace {
u32 passed=0,failed=0,nativeCalls=0,addonCalls=0,commits=0,releases=0;
void check(bool b){++passed;if(!b)++failed;}
void native(void*){++nativeCalls;}
void mesh(void*p,crafting_addon::MeshNext n){++addonCalls;n(p);}
void* catalogueNative(void*p){++nativeCalls;return p;}
void* fakeCatalogue(void*p,crafting_addon::CatalogueNext n){++addonCalls;return n(p);}
u64 iconNative(u64 c,const char*const*){++nativeCalls;return c;}
u64 fakeIcon(u64 c,const char*const*s,crafting_addon::IconNext n){++addonCalls;return n(c,s);}
void commit(int success){commits=success?1:2;}
void release(u8*){++releases;}
}
extern "C" void mainCRTStartup(){
 using namespace addon_host;
 check(DS2CraftingQueryAddonApi(1)==&descriptor&&validHost(&descriptor));
 check(!DS2CraftingQueryAddonApi(0)&&!DS2CraftingQueryAddonApi(2));
 callbacks={sizeof(callbacks),1,nullptr,release,commit,mesh,mesh,fakeCatalogue,fakeIcon};
 void*p=(void*)1234;const char*n="native icon";
 addon_host::worn(p,native);addon_host::hanging(p,native);
 check(addon_host::catalogue(p,catalogueNative)==p);
 check(addon_host::icon(99,&n,iconNative)==99&&nativeCalls==4&&addonCalls==0);
 u8 dummy=0;finish(true,&dummy);check(active==1&&commits==1&&releases==1);
 addon_host::worn(p,native);addon_host::hanging(p,native);
 check(addon_host::catalogue(p,catalogueNative)==p);
 check(addon_host::icon(88,&n,iconNative)==88&&nativeCalls==8&&addonCalls==4);
 finish(false,&dummy);check(active==0&&commits==2&&releases==2);
 addon_host::worn(p,native);check(nativeCalls==9&&addonCalls==4);
 finish(true,nullptr);check(active==0&&releases==2);
 char line[]="HOST_API_TESTS: 10 checks, failures=0\r\n";DWORD wrote=0;
 if(failed)line[34]='1';WriteFile(GetStdHandle((DWORD)-11),line,sizeof(line)-1,&wrote,nullptr);ExitProcess(failed?1:0);
}
