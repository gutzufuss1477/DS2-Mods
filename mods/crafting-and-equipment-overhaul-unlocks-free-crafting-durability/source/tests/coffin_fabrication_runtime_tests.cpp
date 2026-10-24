#include "../src/win_api.hpp"
#include "../src/coffin_menu_policy.hpp"
#include "../src/coffin_persistence_policy.hpp"
#include <intrin.h>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <atomic>
#include <vector>
#include <cstdio>
#include <cstring>
using namespace craft;
constexpr u64 image=0x140000000,alias=0x200000,normal=0x200100,recipe=0x210000,bag=0x220000;
constexpr u64 redRow=0x240000,normalRow=0x240100,manager=0x250000,worldRows=0x260000;
std::unordered_map<u64,u64> memory;std::mutex memoryLock;
template<class T>bool rd(u64 p,T&v){std::lock_guard<std::mutex>g(memoryLock);auto it=memory.find(p);if(it==memory.end())return false;std::memcpy(&v,&it->second,sizeof(v));return true;}
void field(u64 p,u64 v){std::lock_guard<std::mutex>g(memoryLock);memory[p]=v;}
bool ownAlias(u64 p){return p==alias;}
bool prepareDirect(){return true;}
volatile long enabled=1;
struct Text{void add(const char*){}void dec(u64){}void hex(u64,unsigned){}};
void say(const char*){}void log(const Text&){}
bool failTLS=false;
BOOL testTLS(DWORD slot,void*p){if(failTLS){failTLS=false;return 0;}return TlsSetValue(slot,p);}
#define TlsSetValue testTLS
#include "../src/coffin_fabrication_runtime.inl"
#undef TlsSetValue
std::atomic<unsigned>checks=0,failures=0,nativeRows=0,nativeCreates=0;
void ck(bool yes,const char*label){++checks;if(!yes){++failures;std::printf("FAIL %s\n",label);}}
struct Job {u64 entity,instance=alias;bool expected=true,resetInside=false,nested=false;u32 facility=7;};
thread_local Job*job=nullptr;
u64 nativeCreate(u64 resource,u64 parameters,u64 guid){
 ++nativeCreates;ck(resource==0x7777&&parameters==0x8888&&guid==0x9999,"native factory receives original arguments");return job->entity;
}
u64 nativeRow(u64 instance,u32 facility){
 ++nativeRows;ck(instance==job->instance&&facility==job->facility,"native special row receives original arguments");
 if(job->nested){Job nested{job->entity+0x1000,normal,false};Job*parent=job;job=&nested;
  auto r=fabrication_runtime::CoffinFabricateVehicle(normal,nested.facility,normalRow);
  ck(r==nested.entity,"nested native return retained");job=parent;}
 if(job->resetInside)fabrication_runtime::reset();
 auto result=fabrication_runtime::created(0x7777,0x8888,0x9999);
 ck(fabrication_runtime::isOwner(result)==job->expected,"variant established before model initialization and native row return");return result;
}
void entity(u64 p,u64 serial,bool coffin=true){field(p,image+(coffin?redcoffin::CoffinVehicleVtableRva:0x1234));field(p+0x10,serial);field(p+0x18,serial+1);}
void run(Job j,u64 row=redRow){job=&j;auto n=nativeRows.load(),c=nativeCreates.load();
 u64 result=fabrication_runtime::CoffinFabricateVehicle(j.instance,j.facility,row);
 ck(result==j.entity,"native return value preserved");ck(!TlsGetValue(fabrication_runtime::tls),"scope restored");
 ck(nativeRows==n+(j.nested?2:1)&&nativeCreates==c+(j.nested?2:1),"native functions called once per engine call");job=nullptr;}
int main(){
 field(alias,image+0x31ff530);field(alias+8,0);field(normal,image+0x31ff530);field(normal+8,0);
 field(image+0x623e540,0x230000);field(0x230008,1);field(0x230010,0x231000);field(0x231000,recipe);
 field(recipe+0x20,coffin_menu::RecipeKey);field(recipe+0x52,9);field(recipe+0x28,bag);
 field(bag+0x44,coffin_menu::BaggageKey);field(bag+0x49,18);
 for(auto row:{redRow,normalRow}){field(row,row==redRow?alias:normal);field(row+8,1);field(row+12,coffin_menu::RecipeKey);field(row+0x14,3);}
 ck(fabrication_runtime::initialize(),"Windows TLS initialized");fabrication_runtime::nativeRow=&nativeRow;fabrication_runtime::nativeCreate=&nativeCreate;fabrication_runtime::ready=1;
 for(u64 i=0;i<600;++i)entity(0x1000000+i*0x1000,100+i*2);
 // Ordinary ATLAS items never use the special-vehicle callsite.
 run({0x1000000,normal,false},normalRow);run({0x1001000});run({0x1002000,normal,false},normalRow);run({0x1003000});
 ck(fabrication_runtime::isOwner(0x1001000)&&fabrication_runtime::isOwner(0x1003000)&&!fabrication_runtime::isOwner(0x1000000),"red and normal coexist in either order");
 std::thread delayed([]{ck(fabrication_runtime::isOwner(0x1001000)&&fabrication_runtime::isOwner(0x1003000),"delayed worker sees both variants without TLS or time window");});delayed.join();
 Job outside{0x1004000,normal,false};job=&outside;fabrication_runtime::created(0x7777,0x8888,0x9999);job=nullptr;
 ck(!fabrication_runtime::isOwner(outside.entity),"unrelated world creation stays native");
 run({0x1005000,alias,true,false,true});ck(!fabrication_runtime::isOwner(0x1006000),"nested normal row cannot inherit red scope");
 // Native registry/cargo may arrive after model initialization.
 field(image+0x623ea48,manager);field(manager+0x24488,2);field(manager+0x24490,worldRows);
 for(u32 i=0;i<2;++i){u64 p=worldRows+i*0x28,record=0x270000+i*0x200,uid=0x475c0000004b25+i;
  field(p,record);field(p+8,i?0x1000000:0x1001000);field(p+0x10,uid);field(record+0x18,uid);field(record+0x38,bag);}
 field(0x270018,42);ck(!fabrication_runtime::isRed(0x475c0000004b25),"mismatched uncaptured cargo rejected");field(0x270018,0x475c0000004b25);
 ck(fabrication_runtime::isRed(0x475c0000004b25)&&!fabrication_runtime::isRed(0x475c0000004b26),"cargo UID maps to the exact red owner");
 field(worldRows+8,0);ck(fabrication_runtime::isRed(0x475c0000004b25),"stored cargo retains variant after live vehicle unload");field(worldRows+8,0x1001000);
 entity(0x1001000,9000);ck(!fabrication_runtime::isOwner(0x1001000),"address reused with changed GUID rejected");
 run({0x1003000,normal,false},normalRow);ck(!fabrication_runtime::isOwner(0x1003000),"normal allocation clears old marker");
 field(redRow+12,0);run({0x1007000,alias,false});field(redRow+12,coffin_menu::RecipeKey);
 field(redRow+0x14,0);run({0x1008000,alias,false});field(redRow+0x14,3);
 run({0x1009000,normal,false});field(redRow+8,0);run({0x100a000,alias,false});field(redRow+8,1);
 entity(0x100b000,9010,false);run({0x100b000,alias,false});run({0,alias,false});
 run({0x100c000,alias,false,true});ck(!fabrication_runtime::isOwner(0x1005000),"load reset clears old objects and stale in-flight scope");
 enabled=0;run({0x100d000,alias,false});enabled=1;
 failTLS=true;run({0x100e000,alias,false});ck(!fabrication_runtime::ready,"TLS failure disables capture but retains native call");fabrication_runtime::ready=1;
 std::vector<std::thread>workers;
 for(u32 i=0;i<24;++i)workers.emplace_back([i]{Job j{0x1100000+u64(i)*0x1000,i%2?normal:alias,i%2==0};job=&j;
  fabrication_runtime::CoffinFabricateVehicle(j.instance,j.facility,i%2?normalRow:redRow);ck(!TlsGetValue(fabrication_runtime::tls),"worker TLS restored");job=nullptr;});
 for(auto&t:workers)t.join();for(u32 i=0;i<24;++i)ck(fabrication_runtime::isOwner(0x1100000+u64(i)*0x1000)==(i%2==0),"concurrent objects keep individual variants");
 auto before=memory;run({0x1120000});ck(memory==before,"callbacks leave native row, recipe, entity and registry bytes unchanged");
 fabrication_runtime::reset();ck(!fabrication_runtime::isOwner(0x1120000)&&!fabrication_runtime::isRed(0x475c0000004b25),"later save load clears fresh object and cargo associations");
 std::printf("PRODUCTION_FABRICATION_ASSERTIONS=%u FAILURES=%u\n",checks.load(),failures.load());return failures?1:0;
}
