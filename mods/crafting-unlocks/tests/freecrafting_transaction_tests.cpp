// Exercise the actual production transaction with simulated memory/API failures.
// No Windows protection change, thread suspension or live process patch is performed.
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <vector>
#include "../src/core.hpp"
#include "../src/freecrafting_sites.hpp"
#include "cost_stub_definitions.hpp"
using namespace craft;
using DWORD=u32;using HANDLE=u64;
struct Context{u32 flags;u64 rip;};
struct Thread{HANDLE h;bool suspended;};
static Thread threads[2];static u32 threadCount;static u64 image;
static Settings settings{};static std::vector<u8> arena(0x1900000);
static u32 checks=0,protectCalls,flushCalls,suspendCalls,contextCalls,resumeCalls,writeFreeCalls;
static u32 failProtect,failFlush,failSuspend,failContext;
static bool failResume,failCollect;static u64 stoppedRip;
static std::vector<std::pair<u64,DWORD>> protections;
struct Text{void add(const char*){}void hex(u64){}void dec(u64){}};
void log(const Text&){}void say(const char*){}void Sleep(u32){}
bool readmem(u64 p,void*out,u32 n){if(p<image||p+n>image+arena.size())return false;std::memcpy(out,(void*)p,n);return true;}
template<class T>bool rd(u64 p,T&v){return readmem(p,&v,sizeof v);}
void*relayNear(u64 rva,u64){return(void*)(image+rva+0x100000);}
bool VirtualFree(void*,u64,u32){++writeFreeCalls;return true;}
HANDLE GetCurrentProcess(){return 1;}
bool VirtualProtect(void*p,u32,DWORD desired,DWORD*old){
 ++protectCalls;if(protectCalls==failProtect)return false;u64 page=(u64)p&~4095ull;
 for(auto&v:protections)if(v.first==page){*old=v.second;v.second=desired;return true;}
 protections.push_back({page,desired});*old=0x20;return true;
}
bool FlushInstructionCache(HANDLE,void*,u32){return ++flushCalls!=failFlush;}
bool collectThreads(){if(failCollect)return false;threadCount=2;threads[0]={1,false};threads[1]={2,false};return true;}
DWORD SuspendThread(HANDLE){return ++suspendCalls==failSuspend?0xFFFFFFFF:0;}
bool GetThreadContext(HANDLE,Context*c){c->rip=stoppedRip;return ++contextCalls!=failContext;}
bool resumeThreads(){++resumeCalls;for(auto&t:threads)t.suspended=false;return !failResume;}
void closeThreads(){threadCount=0;}
#include "../src/freecrafting_runtime.inl"
void check(bool ok,const char*message){++checks;if(!ok)throw std::runtime_error(message);}
void reset(){
 image=(u64)arena.data();protectCalls=flushCalls=suspendCalls=contextCalls=resumeCalls=writeFreeCalls=0;
 failProtect=failFlush=failSuspend=failContext=0;failResume=failCollect=false;stoppedRip=0;protections.clear();
 for(auto&p:freePatches)p={};
 for(const auto&s:CostSites)std::memcpy(arena.data()+s.rva,s.expected,s.length);
 for(const auto&s:MaterialLimitSites)std::memcpy(arena.data()+s.rva,s.expected,s.length);
 settings.freeCrafting=true;check(freeSignaturesMatch(),"initial signatures");check(prepareFreeRelays(),"prepare relays");
}
void unchanged(){for(const auto&p:freePatches){check(!p.changed,"patch still marked changed");
 check(!std::memcmp((void*)(image+p.spec->rva),p.spec->expected,p.spec->length),"original instruction not restored");}
 for(const auto&p:protections)check(p.second==0x20,"page protection not restored");
 check(!threads[0].suspended&&!threads[1].suspended,"captured thread left suspended");
}
int main(){try{
 reset();check(installFreeGroupOnce()==1,"group success");
 for(const auto&p:freePatches)check(!std::memcmp((void*)(image+p.spec->rva),p.replacement,p.spec->length),"installed instruction mismatch");
 check(restoreFreePatches(),"restore success");unchanged();
 for(u32 n=1;n<=2*FreePatchCount;++n){reset();failProtect=n;check(installFreeGroupOnce()==0,"protection failure not rolled back");unchanged();}
 for(u32 n=1;n<=FreePatchCount;++n){reset();failFlush=n;check(installFreeGroupOnce()==0,"cache failure not rolled back");unchanged();}
 for(u32 n=1;n<=2;++n){reset();failSuspend=n;check(installFreeGroupOnce()==0,"suspend failure");unchanged();
  reset();failContext=n;check(installFreeGroupOnce()==0,"context failure");unchanged();}
 reset();failCollect=true;check(installFreeGroupOnce()==0,"collect failure");unchanged();
 for(u32 i=0;i<FreePatchCount;++i){reset();arena[freePatches[i].spec->rva]^=1;
  check(!freeSignaturesMatch(),"mismatch accepted by preflight");
  check(installFreeGroupOnce()==0&&protectCalls==0,"write before all signatures checked");
  arena[freePatches[i].spec->rva]^=1;unchanged();
  reset();stoppedRip=image+freePatches[i].spec->rva+1;
  check(installFreeGroupOnce()==0&&protectCalls==0,"instruction pointer inside patch accepted");unchanged();
 }
 reset();failResume=true;check(installFreeGroupOnce()==2,"resume failure not marked critical");
 reset();failFlush=1;failProtect=3;check(installFreeGroupOnce()==2,"rollback failure not marked critical");
 reset();settings.freeCrafting=false;installFreeCrafting();check(!protectCalls&&!suspendCalls,"off setting modified code or threads");unchanged();
 reset();settings.freeCrafting=true;failProtect=1;installFreeCrafting();
 check(suspendCalls==4,"clean rollback was not retried once");
 for(const auto&p:freePatches)check(p.changed,"retry did not install complete group");
 check(restoreFreePatches(),"final restore");unchanged();
 std::printf("PASS %u transactional patch assertions; actual production transaction with simulated failure injection.\n",checks);
 std::puts("49 sites, protection/cache failures at every write position, mismatch/RIP checks, resume/rollback failures and FreeCrafting=0.");
 std::puts("No real Windows thread or page permissions were changed by this test.");return 0;
 }catch(const std::exception&e){std::fprintf(stderr,"FAIL %u: %s\n",checks,e.what());return 1;}}
