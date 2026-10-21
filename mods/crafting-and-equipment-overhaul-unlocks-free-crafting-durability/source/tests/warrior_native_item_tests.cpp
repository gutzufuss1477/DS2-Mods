#include "../src/weapons/warrior_policy.hpp"
#include <cstdio>
#include <initializer_list>
int main(){
 unsigned tests=0,errors=0;
 auto check=[&](bool pass,const char*msg){++tests;if(!pass){++errors;std::printf("FAIL %s\n",msg);}};
 for(unsigned mag: {30u,12u,6u}){
  warrior::NativeGrantSet grants{};
  unsigned native=mag==30?70:mag==12?4:6;
  auto table=(unsigned long long)(0x1000000+mag);
  check(warrior::totalStock(native,mag,5)==native+mag*5,"base magazine reserve added once");
  check(grants.mark(table),"new per-weapon category table accepted");
  check(grants.contains(table),"same table persisted");
  check(!grants.mark(table),"duplicate initializations cannot re-credit same table");
  check(grants.mark(table+4096),"distinct weapon item may have its own reserve");
  check(grants.count==2,"exactly two actual tables registered");
  unsigned remaining=warrior::totalStock(native,mag,5);
  for(unsigned i=0;i<5;++i){
   check(remaining>=mag,"native reload has sufficient finite rounds");
   remaining-=mag;
   check(remaining==native+(4-i)*mag,"native reload decreases the true stock exactly");
  }
  check(remaining==native,"all five bonus magazines depleted, native accessory bonus remains");
 }
 warrior::NativeGrantSet hostile{};
 check(!hostile.mark(0x1234),"reject nonpointer table identity");
 check(warrior::totalStock(0,30,5)==150,"rifle HUD 30/150 without accessories");
 check(warrior::totalStock(0,12,5)==60,"shotgun HUD 12/60 without accessories");
 check(warrior::totalStock(0,6,5)==30,"grenade HUD 6/30 without accessories");
 check(warrior::totalStock(70,30,5)==220,"rifle ammo-loader accessories add proportionally");
 check(warrior::totalStock(100001,30,5)==0,"reject nonsensical native ammo");
 std::printf("v191 per-item native ammo: %u tests, %u failures\n",tests,errors);
 return errors?1:0;
}
