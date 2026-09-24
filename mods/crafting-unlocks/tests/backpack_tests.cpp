// Offline tests of the production policy; no game functions are simulated as proven.
#include "../src/backpack.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
using namespace craft;
static unsigned checks=0;
void check(bool ok,const char*message){++checks;if(!ok)throw std::runtime_error(message);}
Settings parseFile(const std::string&text){Settings s{};Error e{};check(parse(text.data(),text.size(),s,e),"shipped INI parses");return s;}
Recipe make(u32 i,u32 key,u8 usage=5){Recipe r{};r.instance={0x140123400ull,i,0xFACEB00Cu};r.resource=0x150000000ull+0xB0*i;r.baggage=0x160000000ull+0x100*i;r.key=key;r.usage=usage;r.valid=1;r.caseType=1;return r;}
bool includes(const Instance*out,u32 n,u32 idx){for(u32 i=0;i<n;++i)if((out[i].flags&0xFFF)==idx)return true;return false;}
int main(int argc,char**argv){try{
 check(argc==2,"supply the actual shipped INI");std::ifstream f(argv[1],std::ios::binary);check(bool(f),"INI readable");
 std::string text((std::istreambuf_iterator<char>(f)),{});auto s=parseFile(text);
 check(s.count==119&&s.enabled&&s.all&&!s.dump,"119 default-on rules; diagnostics off");
 std::vector<Recipe> all;unsigned normalN=0,moduleN=0,coverN=0,charmN=0;
 for(u32 i=0;i<s.count;++i){auto k=backpack_kind(s.rules[i].key);check(s.rules[i].value==1,"every shipped rule starts at 1");
  all.push_back(make(i,s.rules[i].key,k==BackpackKind::None?9:5));
  switch(k){case BackpackKind::None:++normalN;break;case BackpackKind::Module:++moduleN;break;case BackpackKind::Cover:++coverN;break;case BackpackKind::Charm:++charmN;break;}
 }
 check(normalN==90&&moduleN==19&&coverN==2&&charmN==8,"reviewed route/category counts");auto before=all;
 Instance out[MaxMenu]{};auto b=build_backpack(s,all.data(),all.size(),nullptr,0,out);
 check(b.ok&&b.count==29&&b.added==29,"all 29 backpack recipes selected");
 for(u32 i=0;i<b.count;++i){check(out[i].flags&0x8000,"added private copies set unlock bit");check(all[out[i].flags&0xFFF].usage==5,"only backpack route");}
 check(!std::memcmp(before.data(),all.data(),all.size()*sizeof(Recipe)),"no master changes");
 b=build(s,all.data(),all.size(),nullptr,0,2,out);check(b.ok&&b.count==90,"normal route remains separated");
 for(u32 i=0;i<s.count;++i){
  const bool bp=all[i].usage==5;const u32 n=bp?29:90;Settings excluded=s;excluded.rules[i].value=0;
  b=bp?build_backpack(excluded,all.data(),all.size(),nullptr,0,out):build(excluded,all.data(),all.size(),nullptr,0,2,out);
  check(b.ok&&b.added==n-1&&!includes(out,b.count,i),"zero excludes only early access on correct route");
  Instance native=all[i].instance;native.flags|=0xA8000;auto nativeBefore=native;
  b=bp?build_backpack(excluded,all.data(),all.size(),&native,1,out):build(excluded,all.data(),all.size(),&native,1,2,out);
  check(b.ok&&b.count==n&&b.added==n-1,"later native unlock is retained despite zero");
  check(!std::memcmp(out,&native,sizeof native),"native flags/vtable/padding byte-exact");
  check(!std::memcmp(&native,&nativeBefore,sizeof native),"native input unmodified");
  check(!std::memcmp(all.data(),before.data(),all.size()*sizeof(Recipe)),"original recipe resources/flags immutable");
  unsigned found=0;for(u32 j=0;j<b.count;++j)if((out[j].flags&0xFFF)==i)++found;
  check(found==1,"no duplicate when native unlock arrives");
 }
 // Existing patches/other native content are retained, but no new patches are unlocked.
 Recipe patch=make(900,0x1221BF91);Instance nativePatch=patch.instance;nativePatch.flags|=0x8000;
 b=build_backpack(s,&patch,1,nullptr,0,out);check(b.ok&&!b.added,"patch is not implicitly enabled");
 b=build_backpack(s,all.data(),all.size(),&nativePatch,1,out);check(b.ok&&b.count==30&&!std::memcmp(out,&nativePatch,sizeof nativePatch),"native patch preserved unchanged");
 Recipe item=make(1,BackpackItems[0].key);
 for(unsigned reason=0;reason<7;++reason){Recipe bad=item;switch(reason){case 0:bad.valid=0;break;case 1:bad.resource=0;break;case 2:bad.baggage=0;break;case 3:bad.dlc=1;break;case 4:bad.mission=true;break;case 5:bad.duplicate=true;break;case 6:bad.usage=9;break;}
  b=build_backpack(s,&bad,1,nullptr,0,out);check(b.ok&&!b.added,"unsafe or wrong-route candidate refused");}
 Settings disabled=s;disabled.enabled=false;b=build_backpack(disabled,all.data(),all.size(),&nativePatch,1,out);check(b.ok&&b.count==1&&!b.added,"disabled preserves native only");
 Settings none=s;none.count=0;none.all=false;b=build_backpack(none,all.data(),all.size(),nullptr,0,out);check(b.ok&&!b.added,"no implicit unlock with global zero");
 none.rules[none.count++]={BackpackItems[4].key,1};b=build_backpack(none,all.data(),all.size(),nullptr,0,out);check(b.ok&&b.added==1,"specific one overrides global zero");
 std::vector<Instance> full(MaxMenu);for(u32 i=0;i<MaxMenu;++i)full[i]={0x140001000ull,1000+i,0};
 b=build_backpack(s,all.data(),all.size(),full.data(),full.size(),out);check(!b.ok&&b.overLimit==29&&!b.added&&b.count==MaxMenu,"overflow leaves complete native list");
 check(!std::memcmp(out,full.data(),full.size()*sizeof(Instance)),"overflow native prefix preserved");
 check(!build_backpack(s,nullptr,1,nullptr,0,out).ok,"invalid recipe pointer rejected");
 check(!build_backpack(s,all.data(),all.size(),nullptr,1,out).ok,"invalid native pointer rejected");
 check(!build_backpack(s,all.data(),MaxRecipes+1,nullptr,0,out).ok,"invalid resource count rejected");
 std::mt19937 rng(4217);
 for(unsigned run=0;run<1000;++run){Settings q=s;std::vector<Instance>native;
  for(u32 i=0;i<q.count;++i){q.rules[i].value=(rng()%3)-1; if(all[i].usage==5&&rng()%2){auto n=all[i].instance;n.flags|=0x8000;native.push_back(n);}}
  const auto oldNative=native;b=build_backpack(q,all.data(),all.size(),native.data(),native.size(),out);
  check(b.ok&&b.count>=native.size()&&b.count<=29,"random bounded backpack list");
  check(native.empty()||!std::memcmp(out,native.data(),native.size()*sizeof(Instance)),"random native prefix unchanged");
  check(!std::memcmp(all.data(),before.data(),all.size()*sizeof(Recipe)),"random masters untouched");
  for(u32 j=native.size();j<b.count;++j){u32 i=out[j].flags&0xFFF;check(all[i].usage==5&&selected(q,all[i].key),"random new entry selected only");}
 }
 std::cout<<"PASS "<<checks<<" backpack/config assertions; 119 INI keys; 19 modules + 2 covers + 8 charms.\n";
 std::cout<<"Native progression was supplied as policy input. No Windows loader, engine-copy, or in-game test.\n";
 return 0;
 }catch(const std::exception&e){std::cerr<<"FAIL "<<checks<<": "<<e.what()<<'\n';return 1;}}
