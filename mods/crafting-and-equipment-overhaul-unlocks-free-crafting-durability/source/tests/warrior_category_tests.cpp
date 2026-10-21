#include "../src/weapons/warrior_policy.hpp"
#include <cstdio>
int main(){
 unsigned pass=0;
 auto ck=[&](bool ok,const char*message){++pass;if(!ok){std::printf("FAIL: %s\n",message);return false;}return true;};
 // DS2's per-weapon inventory contains MULTIPLE ammo categories, not one.
 unsigned char cells[7*8]={};
 unsigned char cats[]={1,3,5,7,10,13,15};
 for(unsigned i=0;i<7;++i)cells[i*8]=cats[i];
 if(!ck(warrior::findNativeCategoryIndex(cells,7,1)==0,"Rifle category first slot"))return 1;
 if(!ck(warrior::findNativeCategoryIndex(cells,7,5)==2,"Shotgun category index 2"))return 1;
 if(!ck(warrior::findNativeCategoryIndex(cells,7,13)==5,"Grenade category index 5"))return 1;
 if(!ck(warrior::findNativeCategoryIndex(cells,7,6)==-1,"Missing category left alone"))return 1;
 if(!ck(warrior::findNativeCategoryIndex(cells,65,1)==-1,"Out-of-bounds table rejected"))return 1;
 if(!ck(warrior::findNativeCategoryIndex(nullptr,7,1)==-1,"Null table rejected"))return 1;
 if(!ck(warrior::reserveFloor(70,30,5)==150,"Rifle tooltip 30/150"))return 1;
 if(!ck(warrior::reserveFloor(4,12,5)==60,"Shotgun tooltip 12/60"))return 1;
 if(!ck(warrior::reserveFloor(6,6,5)==30,"Grenade tooltip 6/30"))return 1;
 if(!ck(warrior::reserveFloor(210,30,5)==210,"Above-floor backpack bonus preserved"))return 1;
 if(!ck(warrior::reserveFloor(0,30,5)==150,"Empty rifle starts with standard reserve"))return 1;
 if(!ck(warrior::reserveFloor(0,12,5)==60,"Empty shotgun starts with standard reserve"))return 1;
 if(!ck(warrior::reserveFloor(0,6,5)==30,"Empty grenade starts with standard reserve"))return 1;
 std::printf("PASS: %u Warrior native multi-category tests\n",pass);
}
