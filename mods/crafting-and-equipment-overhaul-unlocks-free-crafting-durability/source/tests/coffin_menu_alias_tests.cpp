#include "../src/coffin_menu_alias.hpp"
#include "../src/freecrafting_sites.hpp"
#include <cstdio>
#include <cstring>
using namespace craft;
int main(){
 unsigned checks=0,errors=0;
 auto test=[&](bool ok,const char*name){++checks;if(!ok){++errors;std::printf("FAIL %s\n",name);}};
 Recipe r[4]={};
 Instance originals[5]={},out[512]={};
 r[0].key=0x0184CF05;r[0].baggageKey=0x2A889790;r[0].caseType=18;
 r[0].usage=9;r[0].valid=1;r[0].resource=0x100000ull;
 r[0].baggage=0x200000ull;r[0].instance={0x140ffff0ull,0x8000097u,0};
 originals[0]={0x140fff00ull,0x6000064u,0};
 originals[1]=r[0].instance;
 originals[2]={0x140fff88ull,0x6000183u,0};
 std::memcpy(out,originals,3*sizeof(Instance));
 auto a=addNativeCoffinMenuAlias(r,1,out,3,out,MaxMenu,2);
 test(a.added,"known original coffin menu identity");
 test(a.originalIndex==1&&a.catalogueIndex==0,"select exact native instance");
 test(std::memcmp(out+3,originals+1,sizeof(Instance))==0,"native item bitwise identical");
 test((out[3].flags&0xFFF)==(originals[1].flags&0xFFF),"same resource table index");
 test(free_recipe(0x0184CF05u),"original coffin already covered by FreeCrafting");
 test(effective_cost_count(true,0x0184CF05u,3)==0,"free crafting zero cost");
 test(effective_cost_count(false,0x0184CF05u,3)==3,"free crafting off uses native costs");
 auto x=addNativeCoffinMenuAlias(r,1,out,3,out,MaxMenu,1);
 test(!x.added,"no clone in forbidden facility");
 r[0].baggageKey^=1;
 test(!addNativeCoffinMenuAlias(r,1,out,3,out,MaxMenu,2).added,"wrong baggage key excluded");
 r[0].baggageKey=0x2A889790;r[0].mission=true;
 test(!addNativeCoffinMenuAlias(r,1,out,3,out,MaxMenu,2).added,"quest coffin excluded");
 r[0].mission=false;r[0].caseType=2;
 test(!addNativeCoffinMenuAlias(r,1,out,3,out,MaxMenu,2).added,"noncoffin excluded");
 r[0].caseType=18;r[0].valid=0;
 test(!addNativeCoffinMenuAlias(r,1,out,3,out,MaxMenu,2).added,"invalid resource excluded");
 r[0].valid=1;r[0].instance.flags^=0x1000;
 test(!addNativeCoffinMenuAlias(r,1,out,3,out,MaxMenu,2).added,"exact flags required");
 r[0].instance.flags^=0x1000;
 out[2]=out[1];
 test(!addNativeCoffinMenuAlias(r,1,out,3,out,MaxMenu,2).added,"ambiguous duplicate native entries refused");
 out[2]=originals[2];
 test(!addNativeCoffinMenuAlias(r,1,out,3,out,3,2).added,"no capacity avoids overwrite");
 r[1]=r[0];
 test(!addNativeCoffinMenuAlias(r,2,out,3,out,MaxMenu,2).added,"duplicate catalogue key refused");
 r[1]={};
 out[1]=originals[1];
 out[1].flags|=0x8000u;
 auto early=addNativeCoffinMenuAlias(r,1,out,3,out,MaxMenu,2);
 test(early.added,"early-unlocked original Coffinboard is accepted");
 test(out[3].flags==out[1].flags && out[3].vtable==out[1].vtable,
      "early-unlocked duplicate inherits actual displayed native instance");
 out[1]=originals[1];

 std::printf("COFFIN_MENU_ALIAS_ASSERTIONS %u ERRORS %u\n",checks,errors);
 return errors?1:0;
}
