#include "../src/coffin_session_policy.hpp"
#include <unordered_map>
#include <cstring>
#include <cstdio>
using namespace coffin_session;
int main(){unsigned n=0,failed=0;auto ck=[&](bool b,const char*s){++n;if(!b){++failed;std::printf("FAIL %s\n",s);}};
 constexpr u64 image=0x140000000ull,mgr=0x200000,rows=0x300000,record=0x400000,bag=0x500000,vehicle=0x600000,
  views=0x700000,view=0x800000,preview=0x900000,cargo=0x475c0000004b25ull;
 std::unordered_map<u64,u64> mem;
 auto rd=[&](u64 p,auto&v){auto x=mem.find(p);if(x==mem.end())return false;std::memcpy(&v,&x->second,sizeof(v));return true;};
 auto setup=[&](){mem={{image+0x623ea48,mgr},{mgr+0x24488,1},{mgr+0x24490,rows},{rows,record},{rows+8,vehicle},{rows+16,cargo},
  {record,image+0x327da68},{record+0x18,cargo},{record+0x38,bag},{bag+0x44,0x2a889790},
  {vehicle,image+redcoffin::CoffinVehicleVtableRva},{vehicle+0x10,0x12345678},{vehicle+0x18,0xabcde},
  {mgr+0x24240,1},{mgr+0x24248,views},{views,view},{view+0xf8,preview+0x20},{view+0x18,cargo},{view+0x38,bag}};};
 setup();const auto baseline=mem;auto first=identity(rd,image,vehicle);
 ck(first.owner==vehicle,"native live vehicle identity");ck(recordCargo(rd,image,record)==cargo,"measured native cargo record type");
 ck(cargoOwner(rd,image,cargo)==vehicle,"full cargo UID resolves exact world vehicle");
 ck(previewCargo(rd,image,preview)==cargo,"pickup preview maps to exact cargo");
 ck(same(first,identity(rd,image,cargoOwner(rd,image,previewCargo(rd,image,preview)))),"preview and detail select same Story association");
 ck(mem==baseline,"native data is only read");
 ck(!cargoOwner(rd,image,cargo^(1ull<<40)),"same low UID bits do not identify another item");
 ck(!cargoOwner(rd,image,0)&&!cargoOwner(rd,image,~u64(0)),"invalid UID rejected");
 mem[vehicle+0x10]++;ck(!same(first,identity(rd,image,vehicle)),"reused address with different object identity rejected");
 setup();mem[rows+8]=vehicle+0x1000;mem[vehicle+0x1000]=image+redcoffin::CoffinVehicleVtableRva;
 mem[vehicle+0x1010]=first.lo;mem[vehicle+0x1018]=first.hi;
 ck(!same(first,identity(rd,image,cargoOwner(rd,image,cargo))),"reloaded object cannot inherit stale session address");
 setup();mem[record+0x18]++;ck(!cargoOwner(rd,image,cargo),"registry and baggage record must agree");
 setup();mem[bag+0x44]=0;ck(!cargoOwner(rd,image,cargo)&&!recordCargo(rd,image,record),"other baggage excluded");
 setup();mem[record]=image+0x327da69;ck(!recordCargo(rd,image,record),"wrong object type excluded");
 setup();mem[mgr+0x24488]=257;ck(!cargoOwner(rd,image,cargo),"bounded world registry");
 setup();mem[mgr+0x24488]=2;mem[rows+0x28+16]=cargo;ck(!cargoOwner(rd,image,cargo),"duplicate cargo association rejected");
 setup();mem.erase(record+0x38);ck(!cargoOwner(rd,image,cargo),"unavailable object falls back to native");
 setup();mem[view+0xf8]=preview+0x28;ck(!previewCargo(rd,image,preview),"wrong preview owner excluded");
 setup();mem[mgr+0x24240]=0;mem[mgr+0x24258]=1;mem[mgr+0x24260]=views;ck(previewCargo(rd,image,preview)==cargo,"pending preview supported");
 setup();mem[mgr+0x24240]=2;mem[views+8]=view+0x1000;mem[view+0x10f8]=preview+0x20;mem[view+0x1018]=cargo+1;mem[view+0x1038]=bag;
 ck(!previewCargo(rd,image,preview),"ambiguous preview cargo rejected");
 std::printf("COFFIN_SESSION_ASSERTIONS=%u FAILURES=%u\n",n,failed);return failed?1:0;
}
