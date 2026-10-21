#include "../src/weapons/warrior_policy.hpp"
#include <cstdio>
int main(){
 unsigned count=0,errors=0;
 auto ck=[&](bool good,const char*name){++count;if(!good){++errors;std::printf("FAIL %s\n",name);}};
 unsigned mags[3]={30,12,6},original[3]={70,4,6},reserves[3]={150,60,30};
 for(unsigned k=0;k<3;++k){
  unsigned full=warrior::fabricationTotal(mags[k],5);
  ck(full==mags[k]+reserves[k],"total weapon baggage amount");
  ck(full-mags[k]==reserves[k],"correct fabrication reserve readout");
  ck(warrior::reconciledNativeReserve(original[k],mags[k],5,original[k])==reserves[k],"old weapon receives missing reserve");
  ck(warrior::reconciledNativeReserve(reserves[k],mags[k],5,original[k])==reserves[k],"new crafted weapon never receives extra ammo");
  ck(warrior::reconciledNativeReserve(reserves[k]+20,mags[k],5,original[k])==reserves[k]+20,"new equipped bonus preserved");
  ck(warrior::reconciledNativeReserve(original[k]+10,mags[k],5,original[k])==reserves[k]+10 || original[k]+10>=reserves[k],"old item accessory bonus preserved");
 }
 ck(warrior::fabricationTotal(0,5)==0,"reject invalid mag");
 ck(warrior::fabricationTotal(12,31)==0,"reject invalid reserve count");
 ck(warrior::reconciledNativeReserve(100001,12,5,4)==0,"reject implausible stock");
 std::printf("WARRIOR_PREVIEW_ASSERTIONS %u ERRORS %u\n",count,errors);
 return errors?1:0;
}
