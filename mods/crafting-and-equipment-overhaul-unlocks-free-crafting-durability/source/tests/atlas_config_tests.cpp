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
 check(shipped.atlasEnabled&&shipped.atlasGoldSkin&&shipped.count==135,"Nexus enables ATLAS and gold skin; original 132 rules retained");
 const std::string legacy="[CraftingUnlocks]\nEnabled=1\nDefaultUnlock=1\n";
 auto old=parseOk(legacy);check(!old.atlasEnabled&&!old.atlasGoldSkin,"Legacy config defaults ATLAS and gold skin off");
 using equipment::BootAppearance;
 check(old.bootAppearance==BootAppearance::Default && shipped.bootAppearance==BootAppearance::Default,"Legacy and fresh installs use the original boot look");
 auto absolute=parseOk(legacy+"[AtlasEquipment]\nGoldSkeletonSkin=1\nBootAppearance = aBsOlUtE ; worn and clip\n");
 check(absolute.bootAppearance==BootAppearance::Absolute && absolute.atlasGoldSkin,"Independent, case-insensitive boot setting");
 check(parseOk(legacy+"[AtlasEquipment]\nBootAppearance=Default\n").bootAppearance==BootAppearance::Default,"Switch back to Default");
 const std::string bootSetting=legacy+"[AtlasEquipment]\nBootAppearance=";
 for(const auto& model:equipment::BootModels){
  for(const char* name:{model.key,model.english,model.german,model.nativeName}){
   auto configured=parseOk(bootSetting+name+"\n");
   check(!configured.unknownBootAppearance&&configured.bootAppearance==model.appearance,"Each boot name resolves to its model");
  }
  check(text.find(model.key)!=std::string::npos&&text.find(model.english)!=std::string::npos&&text.find(model.german)!=std::string::npos,"Model is documented in the shipped INI");
 }
 auto typo=parseOk(bootSetting+"not a boot\n");
 check(typo.enabled&&typo.unknownBootAppearance&&typo.bootAppearance==BootAppearance::Default,"A typo keeps crafting active and selects Default");
 u8 meshRows[4][0x28]={{11},{20},{18},{103}};
 check(equipment::bootMeshRow(meshRows,3,static_cast<u8>(absolute.bootAppearance))==2,"Absolute selects its native mesh row");
 check(equipment::bootMeshRow(meshRows,3,static_cast<u8>(old.bootAppearance))==1,"Default selects the Pizza Chef row");
 check(equipment::bootMeshRow(meshRows,2,18)==1,"Missing Absolute mesh uses Default");
 check(equipment::bootMeshRow(meshRows,1,18)==-1,"Missing both donors leaves table untouched");
 check(equipment::bootMeshRow(meshRows,4,18)==-1,"A pre-existing ATLAS row is never duplicated");
 for(bool flag:{false,true})for(bool unlock:{false,true}){
  auto s=parseOk(std::string("[CraftingUnlocks]\nEnabled=1\nDefaultUnlock=")+(unlock?"1":"0")+"\n[AtlasEquipment]\nEnabled="+(flag?"1":"0")+"\nGoldSkeletonSkin=1\n");
  check(s.atlasEnabled==flag&&s.atlasGoldSkin&&s.all==unlock,"ATLAS and skin toggles independent of normal unlock settings");
  for(auto key:{equipment::BootRecipe,equipment::SkeletonRecipe,equipment::CombatBokkaRecipe}){
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
 check(shipped.combatBokkaEnabled && !old.combatBokkaEnabled,"New and legacy config defaults");
 check(parseOk(legacy+"[AtlasEquipment]\nEnabled=1\n").combatBokkaEnabled,"Legacy ATLAS opt-in includes the new recipe");
 check(!parseOk(legacy+"[AtlasEquipment]\nCombatBokkaSkeleton=0\nEnabled=1\n").combatBokkaEnabled,"Explicit hybrid opt-out survives section order");
 check(parseOk(legacy+"[AtlasEquipment]\nEnabled=0\nCombatBokkaSkeleton=1\n").combatBokkaEnabled,"Hybrid recipe can be enabled alone");
 check(contains(DurabilityCraftedBaggage,equipment::CombatBokkaBag),"Hybrid is covered by durability");
 for(const auto* extra:{"CombatBokkaSkeleton=1\nCombatBokkaSkeleton=0","CombatBokkaSkeleton=2","BootAppearance=Absolute\nBootAppearance=Default","Enabled=2","Enabled=inherit","GoldSkeletonSkin=2","GoldSkeletonSkin=inherit","Unknown=1","Enabled=1\nEnabled=0","GoldSkeletonSkin=1\nGoldSkeletonSkin=0"}){
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
 check(FreeRecipeKeyCount==143,"Combined free-crafting recipe count");
 for(u32 i=1;i<FreeRecipeKeyCount;++i)check(FreeRecipeKeys[i-1]<FreeRecipeKeys[i],"Free recipe keys unique and sorted");
 std::cout<<"PASS "<<checks<<" ATLAS integration/config/identity assertions.\n";return 0;
 }catch(const std::exception&e){std::cerr<<"FAIL: "<<e.what()<<"\n";return 1;}}
