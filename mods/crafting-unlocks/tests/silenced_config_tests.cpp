#include "../src/core.hpp"
#include "../src/freecrafting_sites.hpp"
#include "../src/durability_scope.hpp"
#include "../src/weapons/core.hpp"
#include "../src/atlas/atlas.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
using namespace craft;
static unsigned checks=0;
void check(bool v,const char* why){++checks;if(!v)throw std::runtime_error(why);}
Settings parseOk(const std::string& text){Settings s{};Error e{};check(parse(text.data(),u32(text.size()),s,e),"INI parses");return s;}
template<unsigned N> bool contains(const u32(&values)[N],u32 key){for(auto v:values)if(v==key)return true;return false;}
int main(int argc,char**argv){try{
 check(argc==2,"Supply release INI");std::ifstream f(argv[1],std::ios::binary);check(bool(f),"INI readable");
 std::string text((std::istreambuf_iterator<char>(f)),{});auto shipped=parseOk(text);
 check(shipped.atlasEnabled&&shipped.atlasGoldSkin&&shipped.silenced.enabled,"Nexus enables ATLAS gold and suppressed weapons");
 check(shipped.count==132&&!shipped.freeCrafting&&!shipped.durabilityEnabled,"Existing item rules/cost/durability defaults preserved");
 const std::string old="[CraftingUnlocks]\nEnabled=1\nDefaultUnlock=1\n";
 auto legacy=parseOk(old);check(!legacy.silenced.enabled&&!legacy.atlasEnabled&&!legacy.atlasGoldSkin,"Older INIs preserve opt-in recipe/skin behavior");
 check(legacy.silenced.blackRed&&legacy.silenced.language==0,"Existing saved weapon appearance/language defaults retained");
 for(bool enabled:{false,true})for(bool all:{false,true})for(unsigned language:{0,1,2}){
  auto s=parseOk(std::string("[CraftingUnlocks]\nEnabled=1\nDefaultUnlock=")+(all?"1":"0")+"\n[SuppressedWeapons]\nEnabled="+(enabled?"1":"0")+"\nLanguage="+std::to_string(language)+"\n");
  check(s.silenced.enabled==enabled&&s.silenced.language==language,"Recipe/language settings parsed independently");
  for(u32 k=0;k<silenced::Count;++k){const auto&d=silenced::Definitions[k];
   bool visible=enabled&&s.silenced.recipes[k<4?k:0]&&silenced::craftable(k);
   Recipe r{};r.key=d.newRecipe;r.usage=visible?1:0;r.valid=1;r.resource=1;r.baggage=2;r.instance={3,7,0};Instance out[MaxMenu]{};
   auto result=build(s,&r,1,nullptr,0,2,out);
   check(result.ok&&result.count==unsigned(visible&&all),"Hidden recipes cannot be resurrected by early unlock");
   check(!special_normal_recipe(d.newRecipe),"Custom weapons never bypass Usage=None filtering");
   if(visible){Instance native{3,7|0x8000,0};result=build(s,&r,1,&native,1,2,out);
    check(result.ok&&result.count==1&&!std::memcmp(out,&native,sizeof native),"Native unlocked recipe retained once with DefaultUnlock off");}
   if(k<4){check(free_recipe(d.newRecipe)&&effective_cost_count(true,d.newRecipe,3)==0,"Shared FreeCrafting covers each new recipe");
    check(effective_cost_count(false,d.newRecipe,3)==3,"Normal costs preserved when disabled");}
   else check(!free_recipe(d.newRecipe),"Legacy recipe remains unpublished/unlisted");
   check(contains(DurabilityCraftedBaggage,d.newBag),"Shared durability covers current and saved legacy weapons");
   check(!contains(DurabilityBootBaggage,d.newBag),"Weapons never enter boot wear policy");
   check(d.newBag!=equipment::BootBag&&d.newBag!=equipment::SkeletonBag,"ATLAS and weapon saved identities are distinct");
  }
 }
 const char* keys[]={"Enabled","AssaultRifleL2","MachineGunL2","ShotgunL2","BigBoreHandgun","BlackRed","MGVisualSuppressor","ShotgunVisualSuppressor","BigBoreVisualSuppressor"};
 for(unsigned i=0;i<9;++i){
  for(int value:{0,1}){
   auto s=parseOk(old+"[SuppressedWeapons]\n"+keys[i]+"="+std::to_string(value)+"\n");
   bool actual=i==0?s.silenced.enabled:i<5?s.silenced.recipes[i-1]:i==5?s.silenced.blackRed:s.silenced.attachments[i-6];
   check(actual==bool(value),"Every feature switch honored");
  }
  for(const auto* value:{"2","-1","inherit","true"}){auto bad=old+"[SuppressedWeapons]\n"+keys[i]+"="+value+"\n";Settings s{};Error e{};check(!parse(bad.data(),u32(bad.size()),s,e),"Invalid boolean rejected");}
  auto duplicate=old+"[SuppressedWeapons]\n"+keys[i]+"=1\n"+keys[i]+"=0\n";Settings s{};Error e{};check(!parse(duplicate.data(),u32(duplicate.size()),s,e),"Duplicate switch rejected");
 }
 for(const auto* extra:{"Language=3","Language=inherit","Language=1\nLanguage=2","Unknown=1"}){auto bad=old+"[SuppressedWeapons]\n"+extra+"\n";Settings s{};Error e{};check(!parse(bad.data(),u32(bad.size()),s,e),"Invalid language/unknown setting rejected");}
 auto disabled=parseOk("[CraftingUnlocks]\nEnabled=0\nDefaultUnlock=1\n[SuppressedWeapons]\nEnabled=1\n");
 check(!disabled.enabled&&!selected(disabled,silenced::Definitions[0].newRecipe),"Global master still wins");
 check(FreeRecipeKeyCount==138&&DurabilityCraftedBaggageCount==139,"Exact combined cost and durability scope");
 for(u32 i=1;i<FreeRecipeKeyCount;++i)check(FreeRecipeKeys[i-1]<FreeRecipeKeys[i],"Cost identities sorted/unique");
 for(u32 i=1;i<DurabilityCraftedBaggageCount;++i)check(DurabilityCraftedBaggage[i-1]<DurabilityCraftedBaggage[i],"Durability identities sorted/unique");
 auto normal=parseOk(old+"[AtlasEquipment]\nEnabled=1\nGoldSkeletonSkin=0\n[SuppressedWeapons]\nEnabled=1\n");
 check(normal.atlasEnabled&&!normal.atlasGoldSkin&&normal.silenced.enabled,"Gold can be disabled independently without hiding equipment/weapons");
 std::cout<<"PASS "<<checks<<" suppressed weapons integration/config/policy assertions.\n";return 0;
 }catch(const std::exception&e){std::cerr<<"FAIL: "<<e.what()<<"\n";return 1;}}
