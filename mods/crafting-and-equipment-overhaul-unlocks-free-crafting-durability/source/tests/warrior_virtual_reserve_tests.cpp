#include "../src/weapons/warrior_policy.hpp"
#include <cstdio>
#include <initializer_list>
static unsigned tests=0,errors=0;
static void ck(bool ok,const char*name){
 ++tests;
 if(!ok){++errors;std::printf("FAIL %s\n",name);}
}
int main(){
 for(unsigned mag:{30u,12u,6u}){
  warrior::ReserveLedger one{};
  warrior::seed(one,mag,5);
  ck(one.seeded&&one.remaining==5*mag,"initial five magazine reserve");
  ck(warrior::afterNativeInit(one,0)==5*mag,"zero accessory base reserve");
  unsigned x=5*mag;
  for(unsigned i=0;i<5;++i){
   unsigned next=x-mag;
   warrior::afterReload(one,x,next);
   ck(one.remaining==next,"virtual reserve spent exactly once");
   unsigned gearOnly=warrior::beforeNativeSync(one,next);
   ck(gearOnly==0,"backpack inventory never gains virtual ammo");
   ck(warrior::afterNativeInit(one,0)==next,"weapon switch never duplicates reserve");
   x=next;
  }
  ck(one.remaining==0,"reserve depleted after five reloads");
  ck(warrior::afterNativeInit(one,0)==0,"zero stock remains at zero");
  for(unsigned nativeGear:{4u,mag,mag*3}){
   warrior::ReserveLedger s{};
   warrior::seed(s,mag,5);
   unsigned start=warrior::afterNativeInit(s,nativeGear);
   ck(start==5*mag+nativeGear,"backpack bonus adds on top of base reserve");
   unsigned first=start-mag;
   warrior::afterReload(s,start,first);
   ck(s.remaining==4*mag,"initial reload costs one virtual magazine");
   ck(warrior::beforeNativeSync(s,first)==nativeGear,"native backpack accessory capacity preserved after reload");
   ck(warrior::afterNativeInit(s,nativeGear)==first,"re-equip keeps consumed reserve");
   warrior::afterReload(s,first,nativeGear);
   ck(s.remaining==0,"all five virtual magazines can be consumed");
   ck(warrior::beforeNativeSync(s,nativeGear)==nativeGear,"accessory stock survives virtual reserve exhaustion");
   ck(warrior::afterNativeInit(s,nativeGear)==nativeGear,"remaining accessory stock still usable");
  }
 }
 ck(warrior::cap(36,1,30)==30,"rifle magazine size");
 ck(warrior::cap(8,1,12)==12,"shotgun magazine size");
 ck(warrior::cap(8,1,6)==6,"grenade magazine size");
 ck(warrior::cap(36,3,0)==108,"backward-compatible legacy ammo multiplier");
 warrior::ReserveLedger a{};
 warrior::seed(a,12,5);
 warrior::seed(a,12,5);
 ck(a.remaining==60,"seed called repeatedly does not refill");
 ck(warrior::afterNativeInit(a,4)==64,"shotgun screenshot 12/4 becomes 12/64 with a 4-round accessory");
 std::printf("TESTS %u FAILURES %u\n",tests,errors);
 return errors?1:0;
}
