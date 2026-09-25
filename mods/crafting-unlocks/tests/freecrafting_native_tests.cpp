// Standalone Windows test: production thunks vs original instruction snippets.
// No game is loaded, attached to, modified or started by this executable.
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>
#include "../src/backpack.hpp"
#include "../src/freecrafting_sites.hpp"
#include "cost_fixture_generated.hpp"
using namespace craft;
extern "C" {
 alignas(16) u64 FixtureInput[16]={},FixtureOutput[16]={};
 alignas(16) u8 FixtureXmmIn[96]={},FixtureXmmOut[96]={};
 u64 FixtureFlagsIn=0x202,FixtureFlagsOut=0,FixtureExpectedRsp=0;
 CostThunk FixtureTarget=nullptr;
 void RunCostFixture0(); void RunCostFixture1();
}
static bool fixtureFree=true,captureUnwind=false,unwindFound=false;
static u32 currentSite=0;static u64 checks=0,cases=0,unwindChecks=0;
void check(bool value,const char*why){++checks;if(!value)throw std::runtime_error(why);}
extern "C" i32 FixtureSelectCount(const u8* resource) {
 u32 key; i32 count;std::memcpy(&key,resource+0x20,4);std::memcpy(&count,resource+0x58,4);
 if(captureUnwind){void*frames[32]={};DWORD64 base=0;
  auto f=RtlLookupFunctionEntry((DWORD64)CostThunks[currentSite],&base,nullptr);
  const USHORT n=CaptureStackBackTrace(0,32,frames,nullptr);unwindFound=false;
  if(f)for(USHORT i=0;i<n;++i){u64 pc=(u64)frames[i];if(pc>=base+f->BeginAddress&&pc<base+f->EndAddress)unwindFound=true;}
 }
 return effective_cost_count(fixtureFree,key,count);
}
struct Result {u64 regs[16];u8 xmm[96];u64 flags;u8 scratch[512];};
alignas(16) static u8 resource[0xB0],scratch[512];
Result execute(CostThunk target,unsigned alignment){
 std::memset(scratch,0xA5,sizeof scratch);FixtureTarget=target;
 if(alignment)RunCostFixture1();else RunCostFixture0();
 check(FixtureOutput[7]==FixtureExpectedRsp,"stack pointer not restored");
 Result r{};std::memcpy(r.regs,FixtureOutput,sizeof r.regs);r.regs[7]=0;
 std::memcpy(r.xmm,FixtureXmmOut,sizeof r.xmm);r.flags=FixtureFlagsOut;
 std::memcpy(r.scratch,scratch,sizeof scratch);return r;
}
void configTests(const char*path){
 std::ifstream f(path,std::ios::binary);check(bool(f),"INI missing");
 std::string text((std::istreambuf_iterator<char>(f)),{});Settings s{};Error e{};
 check(parse(text.data(),text.size(),s,e),"new INI does not parse");
 check(s.count==120&&s.enabled&&s.all&&!s.freeCrafting,"default INI changed existing behaviour");
 std::string on=text;auto pos=on.find("FreeCrafting=0");check(pos!=std::string::npos,"FreeCrafting setting absent");on[pos+13]='1';
 Settings enabled{};check(parse(on.data(),on.size(),enabled,e)&&enabled.freeCrafting,"FreeCrafting=1 not parsed");
 for(u32 i=0;i<s.count;++i){check(free_recipe(s.rules[i].key),"supported recipe missing from free-cost table");
  check(selected(s,s.rules[i].key)==selected(enabled,s.rules[i].key),"cost flag changed unlock selection");
  enabled.rules[i].value=0;check(!selected(enabled,enabled.rules[i].key),"cost flag unlocked excluded recipe");enabled.rules[i].value=1;
 }
 std::string old=text;auto end=old.find('\n',pos);old.erase(pos,end-pos+1);
 Settings legacy{};check(parse(old.data(),old.size(),legacy,e)&&!legacy.freeCrafting,"legacy INI compatibility");
 std::string bad=on;bad[pos+13]='2';check(!parse(bad.data(),bad.size(),legacy,e),"invalid FreeCrafting value accepted");
 bad=on+"\n[CraftingUnlocks]\nFreeCrafting=0\n";check(!parse(bad.data(),bad.size(),legacy,e),"duplicate FreeCrafting accepted");
 const u32 exclusions[]={0x1E7ECEC5,0x26C1FFEB,0x1221BF91,0x7D5BE518,0,0xFFFFFFFF};
 for(u32 key:exclusions)check(!free_recipe(key),"raw material/vehicle/patch/DLC included accidentally");
}
int main(int argc,char**argv){try{
 check(argc==2,"supply the shipped INI");configTests(argv[1]);
 std::vector<u32> keys(std::begin(FreeRecipeKeys),std::end(FreeRecipeKeys));
 for(u32 k:{0x1E7ECEC5u,0x26C1FFEBu,0x1221BF91u,0x7D5BE518u,0u,0xFFFFFFFFu})keys.push_back(k);
 const i32 quantities[]={0,1,6,-1};
 for(currentSite=0;currentSite<44;++currentSite)for(unsigned alignment=0;alignment<2;++alignment)
 for(unsigned enabled=0;enabled<2;++enabled)for(u32 key:keys)for(i32 count:quantities){
  fixtureFree=enabled!=0;std::memset(resource,0x5A,sizeof resource);
  for(unsigned i=0;i<16;++i)FixtureInput[i]=0x100000000ull+(u64)i*0x12345678ull+key;
  FixtureInput[6]=(u64)(scratch+128);FixtureInput[CostSourceRegister[currentSite]]=(u64)resource;
  for(unsigned i=0;i<96;++i)FixtureXmmIn[i]=(u8)(i+key);
  FixtureFlagsIn=(key&1)?0x202:0x246;std::memcpy(resource+0x20,&key,4);
  i32 expectedCount=effective_cost_count(fixtureFree,key,count);std::memcpy(resource+0x58,&expectedCount,4);
  captureUnwind=false;Result expected=execute(CostReferences[currentSite],alignment);
  std::memcpy(resource+0x58,&count,4);u8 original[sizeof resource];std::memcpy(original,resource,sizeof resource);
  captureUnwind=enabled&&key==keys[0]&&count==6;Result actual=execute(CostThunks[currentSite],alignment);
  if(captureUnwind){check(unwindFound,"Windows unwind did not traverse production thunk");++unwindChecks;}
  if(std::memcmp(&expected,&actual,sizeof actual)){
   std::printf("DIFFERENCE site=%u align=%u enabled=%u key=%08X count=%d\n",currentSite,alignment,enabled,key,count);
   throw std::runtime_error("native register/flags/XMM/memory differential failed");
  }
  check(!std::memcmp(original,resource,sizeof resource),"recipe data was modified");++cases;
 }
 check(unwindChecks==88,"not every thunk/alignment received a Windows unwind test");
 std::printf("PASS %llu native instruction cases, %llu assertions, %llu Windows unwind checks.\n",cases,checks,unwindChecks);
 std::puts("44 production thunks; 120 supported + 6 excluded keys; both stack alignments; flag on/off; original data unchanged.");
 std::puts("No game execution. This validates machine-code/ABI and configuration, not DS2 gameplay.");return 0;
 }catch(const std::exception&e){std::fprintf(stderr,"FAIL after %llu cases: %s\n",cases,e.what());return 1;}}
