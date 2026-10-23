#include "../src/coffin_persistence_policy.hpp"
#include <cstdio>
#include <cstring>
#include <unordered_map>
using namespace coffin_persistence;
static Entry database[Capacity];
int main(){unsigned n=0,failed=0;auto ck=[&](bool b,const char*s){++n;if(!b){++failed;std::printf("FAIL %s\n",s);}};
 u8 saved[RecordSize]={};u64 uid=0x475a0000004b26;u32 key=0x2a889790;
 std::memcpy(saved+0xd0,&uid,8);std::memcpy(saved+0x11c,&key,4);
 auto e=fingerprint(saved);u32 count=0;
 ck(e.cargo==uid,"full native cargo UID");ck(put(database,count,e,true)&&count==1,"record red save");
 auto h=seal(database,count);ck(valid(h,database,sizeof(h)+count*sizeof(Entry)),"restart file validates");
 auto load=fingerprint(saved);ck(find(database,count,load)==0&&database[0].red,"exact loaded bytes recover red");
 ck(!put(database,count,e,true)&&count==1,"repeated save deduplicated");
 saved[0x88]=1;ck(find(database,count,fingerprint(saved))<0,"same UID different save record does not inherit marker");
 auto other=fingerprint(saved);ck(put(database,count,other,true)&&count==2,"new save keeps older save association");
 saved[0x88]=0;ck(find(database,count,fingerprint(saved))==0,"older red save still recognized");
 auto native=fingerprint(saved);ck(put(database,count,native,false)&&!database[0].red,"native collision explicitly clears red");
 ck(database[1].red,"other red snapshot retained");
 auto unknown=e;unknown.digest[0]^=1;ck(!put(database,count,unknown,false)&&count==2,"normal saves do not fill metadata");
 h=seal(database,count);auto bad=h;bad.version++;ck(!valid(bad,database,sizeof(h)+count*sizeof(Entry)),"unknown metadata version refused");
 bad=h;bad.count=Capacity+1;ck(!valid(bad,database,sizeof(h)+count*sizeof(Entry)),"oversized file refused before hashing");
 ck(!valid(h,database,sizeof(h)+count*sizeof(Entry)-1),"truncated metadata refused");
 database[0].digest[0]^=1;ck(!valid(h,database,sizeof(h)+count*sizeof(Entry)),"corruption detected");database[0].digest[0]^=1;
 saved[0x11c]^=1;ck(!fingerprint(saved).cargo,"other native item excluded");saved[0x11c]^=1;
 u32 full=Capacity;ck(!put(database,full,unknown,true)&&full==Capacity,"full database does not evict older saves");
 // Paired live measurement: QPID and cargo persist, GUID/addresses change.
 constexpr u64 im=0x140000000,mgr=0x200000,rows=0x300000,rec=0x400000,bag=0x500000,vehicle=0x600000,pid=0x10000000000001f7;
 std::unordered_map<u64,u64> mem={{im+0x623ea48,mgr},{mgr+0x24488,1},{mgr+0x24490,rows},{rows,rec},{rows+8,0},{rows+0x10,uid},{rows+0x18,pid},
 {rec,im+0x327da68},{rec+0x18,uid},{rec+0x38,bag},{bag+0x44,key},{vehicle,im+redcoffin::CoffinVehicleVtableRva},{vehicle+0x320,pid}};
 auto read=[&](u64 a,auto&v){auto i=mem.find(a);if(i==mem.end())return false;std::memcpy(&v,&i->second,sizeof(v));return true;};
 const auto original=mem;ck(worldCargo(read,im,vehicle)==uid,"saved QPID selects before live registry pointer exists");
 ck(mem==original,"native registry and saved IDs unchanged");mem[rows+0x18]=pid+1;ck(!worldCargo(read,im,vehicle),"different saved object excluded");
 mem[rows+0x18]=pid;mem[rows+8]=vehicle+0x1000;ck(!worldCargo(read,im,vehicle),"already bound different owner excluded");
 mem[rows+8]=vehicle;ck(worldCargo(read,im,vehicle)==uid,"live registry also resolves");
 mem[rec+0x18]=uid+1;ck(!worldCargo(read,im,vehicle),"stale/reused native record rejected");
 mem[rec+0x18]=uid;mem[bag+0x44]=0;ck(!worldCargo(read,im,vehicle),"other native cargo rejected");
 std::printf("COFFIN_PERSISTENCE_ASSERTIONS=%u FAILURES=%u\n",n,failed);return failed?1:0;
}
