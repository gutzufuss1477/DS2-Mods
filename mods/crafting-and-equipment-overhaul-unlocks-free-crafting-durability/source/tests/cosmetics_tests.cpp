#include "../src/cosmetics.hpp"
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>
using namespace craft;
unsigned checks=0;
void check(bool b,const char*m){++checks;if(!b)throw std::runtime_error(m);}
Settings config(const std::string&t){Settings s{};Error e{};check(parse(t.data(),u32(t.size()),s,e),"valid configuration");return s;}
CosmeticCandidate candidate(u32 i){
 const auto&k=CosmeticItems[i];Recipe r{};
 r.instance={0x1431ff530,100+i,0xdeadbeef};r.resource=0x200000+i*0x100;r.baggage=0x300000+i*0x100;
 r.key=k.key;r.baggageKey=k.baggageKey;r.usage=8;r.caseType=0;r.valid=1;
 return {r,k.category};
}
int main(int argc,char**argv){try{
 check(argc==2,"real installed INI argument");
 std::ifstream f(argv[1],std::ios::binary);check(bool(f),"INI readable");
 std::string text((std::istreambuf_iterator<char>(f)),{});auto s=config(text);
 check(s.enabled&&s.all&&s.cosmeticsEnabled&&s.cosmeticsColors,"existing INI activates native cosmetics without rewriting it");
 check(CosmeticItemCount==52,"full verified catalogue");
 std::vector<CosmeticCandidate> all;
 for(u32 i=0;i<52;++i)all.push_back(candidate(i));auto before=all;
 Instance out[MaxMenu];auto b=build_cosmetics(s,all.data(),52,nullptr,0,out);
 check(b.ok&&b.count==52&&b.added==52,"empty native menu receives 52 entries");
 for(u32 i=0;i<52;++i){check(out[i].flags==(all[i].recipe.instance.flags|0x8000),"native recipe ID retained");
  check(out[i].vtable==all[i].recipe.instance.vtable&&out[i].padding==0,"native working-copy ABI");
  check(eligibility(s,all[i].recipe,15)==Skip::UnsupportedPath,"cosmetics never enter fabrication");}
 check(!memcmp(before.data(),all.data(),all.size()*sizeof(all[0])),"master data immutable");
 for(u32 i=0;i<52;++i){
  auto q=s;q.rules[q.count++]={all[i].recipe.key,0};
  b=build_cosmetics(q,all.data(),52,nullptr,0,out);check(b.ok&&b.added==51,"per-item native progression");
  Instance native=all[i].recipe.instance;native.flags|=0xF8000;auto old=native;
  b=build_cosmetics(q,all.data(),52,&native,1,out);
  check(b.ok&&b.count==52&&b.added==51&&!memcmp(out,&native,16),"zero rule never relocks native item");
  check(!memcmp(&old,&native,16),"native input unchanged");
 }
 auto q=s;q.cosmeticsEnabled=false;
 b=build_cosmetics(q,all.data(),52,nullptr,0,out);check(b.ok&&!b.count,"cosmetics off");
 q=s;q.enabled=false;b=build_cosmetics(q,all.data(),52,nullptr,0,out);check(b.ok&&!b.count,"master off");
 q=s;q.all=false;q.count=0;b=build_cosmetics(q,all.data(),52,nullptr,0,out);check(b.ok&&!b.count,"default off");
 q.rules[q.count++]={all[7].recipe.key,1};
 b=build_cosmetics(q,all.data(),52,nullptr,0,out);check(b.ok&&b.count==1,"individual unlock with default off");
 for(unsigned fault=0;fault<12;++fault){
  auto c=all[0];auto&r=c.recipe;
  switch(fault){case 0:r.usage=9;break;case 1:r.key=0xabcd;break;case 2:r.baggageKey^=1;break;
  case 3:++c.category;break;case 4:r.valid=0;break;case 5:r.resource=0;break;case 6:r.baggage=0;break;
  case 7:r.caseType=11;break;case 8:r.instance.vtable=0;break;case 9:r.dlc=1;break;case 10:r.mission=1;break;case 11:r.duplicate=1;break;}
  b=build_cosmetics(s,&c,1,nullptr,0,out);check(b.ok&&!b.count,"unverified recipe rejected");
 }
 std::vector<Instance> full(MaxMenu);for(u32 i=0;i<MaxMenu;++i)full[i]={0x12345,2000+i,13};
 b=build_cosmetics(s,all.data(),52,full.data(),MaxMenu,out);
 check(!b.ok&&b.overLimit==52&&b.count==MaxMenu&&!b.added&&!memcmp(out,full.data(),MaxMenu*16),"overflow preserves native list");
 check(!build_cosmetics(s,all.data(),53,nullptr,0,out).ok,"candidate count bounded");
 check(!build_cosmetics(s,nullptr,1,nullptr,0,out).ok,"candidate pointer checked");
 check(!build_cosmetics(s,all.data(),52,nullptr,1,out).ok,"native pointer checked");
 check(!build_cosmetics(s,all.data(),52,nullptr,0,nullptr).ok,"output checked");
 check(cosmetic_colors_enabled(s),"existing global default enables palette");
 q=s;q.cosmeticsColors=false;check(!cosmetic_colors_enabled(q),"independent colors toggle");
 q=s;q.all=false;check(!cosmetic_colors_enabled(q),"default off respects native palettes");
 check(cosmetic_color_override(true,42,42),"scoped thread colors enabled");
 check(!cosmetic_color_override(true,42,43)&&!cosmetic_color_override(true,0,0)&&!cosmetic_color_override(false,42,42),"no override on another thread or outside scope");
 const std::string base="[CraftingUnlocks]\nEnabled=1\nDefaultUnlock=1\n";
 q=config(base+"[Cosmetics]\nEnabled=0\nUnlockColors=0\n");
 check(!q.cosmeticsEnabled&&!q.cosmeticsColors,"explicit cosmetics controls");
 for(const auto&bad:{"Enabled=2","Enabled=inherit","Enabled=1\nEnabled=0","Unknown=1","UnlockColors=-1"}){
  auto t=base+"[Cosmetics]\n"+bad+"\n";Error e{};check(!parse(t.data(),u32(t.size()),q,e),"malformed cosmetics config rejected");
 }
 check(!memcmp(before.data(),all.data(),all.size()*sizeof(all[0])),"all policy scenarios keep master immutable");
 std::printf("COSMETICS_POLICY_ASSERTIONS=%u FAILURES=0\n",checks);return 0;
}catch(const std::exception&e){std::fprintf(stderr,"FAIL: %s\n",e.what());return 1;}}
