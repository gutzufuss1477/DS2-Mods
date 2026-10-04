#include "../src/core.hpp"
#include "../src/freecrafting_sites.hpp"
#include "../src/durability_scope.hpp"
#include "../src/atlas/atlas.hpp"
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
using namespace craft;
static unsigned checks=0;
void check(bool v,const char* why){++checks;if(!v)throw std::runtime_error(why);}
Settings parseOk(const std::string& text){Settings s{};Error e{};check(parse(text.data(),text.size(),s,e),"INI parses");return s;}
template<unsigned N> bool contains(const u32(&values)[N],u32 key){for(auto v:values)if(v==key)return true;return false;}
int main(int argc,char**argv){try{
 check(argc==2,"Supply release INI");
 std::ifstream f(argv[1],std::ios::binary);check(bool(f),"INI readable");
 std::string text((std::istreambuf_iterator<char>(f)),{});auto shipped=parseOk(text);
 check(!shipped.atlasEnabled&&!shipped.atlasGoldSkin&&shipped.count==132,"ATLAS optional; gold skin opt-in; original 132 rules retained");
 const std::string legacy="[CraftingUnlocks]\nEnabled=1\nDefaultUnlock=1\n";
 auto old=parseOk(legacy);check(!old.atlasEnabled&&!old.atlasGoldSkin,"Legacy config defaults ATLAS and gold skin off");
 for(bool flag:{false,true})for(bool unlock:{false,true}){
  auto s=parseOk(std::string("[CraftingUnlocks]\nEnabled=1\nDefaultUnlock=")+(unlock?"1":"0")+"\n[AtlasEquipment]\nEnabled="+(flag?"1":"0")+"\nGoldSkeletonSkin=1\n");
  check(s.atlasEnabled==flag&&s.atlasGoldSkin&&s.all==unlock,"ATLAS and skin toggles independent of normal unlock settings");
  for(auto key:{equipment::BootRecipe,equipment::SkeletonRecipe}){
   Recipe r{};r.key=key;r.usage=flag?1:0;r.valid=1;r.resource=1;r.baggage=2;r.instance={3,7,0};
   Instance out[MaxMenu]{};
   auto b=build(s,&r,1,nullptr,0,2,out);
   check(b.ok&&b.count==unsigned(flag&&unlock),"Hidden recipes cannot be resurrected by DefaultUnlock");
   check(!special_normal_recipe(key),"ATLAS is never a Usage=None exception");
   if(flag){Instance native{3,7|0x8000,0};b=build(s,&r,1,&native,1,2,out);
    check(b.ok&&b.count==1&&!std::memcmp(out,&native,sizeof native),"Native unlocked ATLAS survives DefaultUnlock off without duplicates");}
   check(free_recipe(key)&&effective_cost_count(true,key,3)==0,"ATLAS free crafting covered");
   check(effective_cost_count(false,key,3)==3,"ATLAS costs preserved when free crafting off");
  }
 }
 for(const auto* extra:{"Enabled=2","Enabled=inherit","GoldSkeletonSkin=2","GoldSkeletonSkin=inherit","Unknown=1","Enabled=1\nEnabled=0","GoldSkeletonSkin=1\nGoldSkeletonSkin=0"}){
  auto bad=legacy+"[AtlasEquipment]\n"+extra+"\n";Settings s{};Error e{};
  check(!parse(bad.data(),bad.size(),s,e),"Invalid/duplicate ATLAS setting rejected");
 }
 auto disabled=parseOk("[CraftingUnlocks]\nEnabled=0\nDefaultUnlock=1\n[AtlasEquipment]\nEnabled=1\n");
 check(!disabled.enabled&&!selected(disabled,equipment::BootRecipe),"Master switch still wins");
 check(equipment::BootId==103&&equipment::SkeletonId==104,"Stable saved item IDs");
 check(equipment::BootRecipe==0x616526A9&&equipment::SkeletonRecipe==0x02161D6C,"Stable recipe identities");
 check(equipment::BootBag==0x3B0ECB3E&&equipment::SkeletonBag==0x4D918ADD,"Stable saved baggage keys");
 check(contains(DurabilityCraftedBaggage,equipment::BootBag)&&contains(DurabilityCraftedBaggage,equipment::SkeletonBag),"Both ATLAS bags covered by durability");
 check(contains(DurabilityBootBaggage,equipment::BootBag)&&!contains(DurabilityBootBaggage,equipment::SkeletonBag),"Boot wear scope exact");
 check(FreeRecipeKeyCount==134,"132 original plus two ATLAS recipes");
 for(u32 i=1;i<FreeRecipeKeyCount;++i)check(FreeRecipeKeys[i-1]<FreeRecipeKeys[i],"Free recipe keys unique and sorted");
 std::cout<<"PASS "<<checks<<" ATLAS integration/config/identity assertions.\n";return 0;
 }catch(const std::exception&e){std::cerr<<"FAIL: "<<e.what()<<"\n";return 1;}}
