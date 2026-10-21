#include "../src/weapons/warrior_policy.hpp"
#include <cstdio>
#include <initializer_list>
int main(){
 unsigned assertions=0;
 auto ck=[&](bool c){++assertions;if(!c){std::puts("FAIL");return false;}return true;};
 for(unsigned mag: {30u,12u,6u}){
  if(!ck(warrior::cap(36,1,mag)==mag))return 1;
  for(unsigned remaining=0;remaining<=mag*12;remaining++){
   unsigned actual=warrior::boundedStock(remaining,mag,6);
   if(!ck(actual==(remaining<mag*6?remaining:mag*6)))return 1;
  }
 }
 if(!ck(warrior::cap(36,3,0)==108))return 1;
 if(!ck(warrior::boundedStock(1000,30,0)==1000))return 1;
 if(!ck(warrior::boundedStock(200,12,6)==72))return 1;
 std::printf("PASS %u finite 6-magazine Warrior reserves policy assertions\n",assertions);
}
