#include "../src/coffin_preview_policy.hpp"
#include <unordered_map>
#include <cstring>
#include <cstdio>
using namespace coffin_preview;
int main(){unsigned n=0,errors=0;auto ck=[&](bool b,const char*s){++n;if(!b){++errors;std::printf("FAIL %s\n",s);}};
 constexpr u64 image=0x140000000ull,vehicle=0x200000,owner=0x300000,skin=0x400000,row=0x500000,art=0x600000,sr=0x700000,ar=0x800000,model=0x900000,list=0xa00000,mgr=0xb00000,world=0xc00000,views=0xd00000,view=0xe00000,bag=0xf00000,cargo=0x475c0000004b25ull;
 u64 old[2]={0x1100000,0x1200000},parts[2]={0x1300000,0x1400000};std::unordered_map<u64,u64> mem;
 auto rd=[&](u64 p,auto&v){auto x=mem.find(p);if(x==mem.end())return false;std::memcpy(&v,&x->second,sizeof(v));return true;};
 auto id=[&](u64 p,u64 lo,u64 hi){mem[p+16]=lo;mem[p+24]=hi;};
 auto setup=[&](){mem.clear();mem[image+0x623ea48]=mgr;mem[mgr+0x24488]=1;mem[mgr+0x24490]=world;mem[world+8]=vehicle;mem[world+16]=cargo;
  mem[mgr+0x24240]=1;mem[mgr+0x24248]=views;mem[views]=view;mem[view+0xf8]=owner+0x20;mem[view+0x18]=cargo;mem[view+0x38]=bag;mem[bag+0x44]=0x2a889790;
  mem[skin+0x48]=owner;mem[owner]=image+0x3119bc8;mem[owner+0xc8]=skin;mem[skin+0x30]=sr;id(sr,SkinLo,SkinHi);mem[owner+0xd8]=art;mem[art+0x48]=owner;mem[art+0x30]=ar;id(ar,ArtLo,ArtHi);mem[ar+0x20]=model;id(model,ModelLo,ModelHi);mem[model+0x38]=6;mem[model+0x40]=list;
  for(u32 i=0;i<6;++i)mem[list+8ull*i]=0x1600000+i*0x100;
  for(u32 i=0;i<2;++i){mem[list+16ull*i]=old[i];id(old[i],PartIds[i][0],PartIds[i][1]);mem[old[i]]=mem[parts[i]]=image+0x3128830;mem[parts[i]+0x40]=mem[old[i]+0x40]=i?0:0x1700000;mem[old[i]+0xd0]=coffin_direct_assets::Parts[i].name;}
  mem[row+0xc8]=old[0];
 };
 setup();ck(cargoIdentity(rd,image,vehicle)==cargo,"live red vehicle maps to complete original cargo UID");
 auto baseline=mem;ck(viewOwns(rd,image,owner,cargo),"native view backlink and UID match");
 auto c=inspect(rd,image,skin,row,parts,cargo);ck(c.owner==owner&&c.replacement==parts[0]&&c.slot==0,"preview body accepted");
 ck(mem==baseline,"preview selection performs no native-data writes");
 mem[row+0xc8]=old[1];ck(inspect(rd,image,skin,row,parts,cargo).replacement==parts[1],"preview auxiliary accepted");
 ck(!inspect(rd,image,skin,row,parts,cargo+1).replacement,"ordinary coffin cargo excluded");
 setup();mem[view+0x18]=cargo^(1ull<<40);ck(!inspect(rd,image,skin,row,parts,cargo).replacement,"full UID compared, no low-index collision");
 setup();mem[view+0xf8]=owner+0x28;ck(!inspect(rd,image,skin,row,parts,cargo).replacement,"exact entity backlink required");
 setup();mem[mgr+0x24240]=0;mem[mgr+0x24258]=1;mem[mgr+0x24260]=views;ck(inspect(rd,image,skin,row,parts,cargo).replacement==parts[0],"native pending view allowed");
 setup();mem[mgr+0x24240]=4097;ck(!inspect(rd,image,skin,row,parts,cargo).replacement,"unbounded view count rejected");
 setup();mem[parts[0]+0x40]=0;ck(!inspect(rd,image,skin,row,parts,cargo).replacement,"lost collision rejected");
 setup();mem[sr+16]^=1;ck(!inspect(rd,image,skin,row,parts,cargo).replacement,"foreign skeleton resource rejected");
 setup();mem[model+0x38]=10;ck(!inspect(rd,image,skin,row,parts,cargo).replacement,"world model excluded from preview branch");
 setup();mem[world+8]=vehicle+0x100;ck(!cargoIdentity(rd,image,vehicle),"missing red vehicle disables preview");
 setup();mem[mgr+0x24488]=2;mem[world+0x28+8]=vehicle;mem[world+0x28+16]=cargo;ck(!cargoIdentity(rd,image,vehicle),"ambiguous registry rejected");
 setup();ck(!inspect(rd,image,skin,row,parts,0).replacement,"no identity means native fallback");
 PartCopy native{},story{},output{};at<u64>(native.bytes,0)=at<u64>(story.bytes,0)=image+0x3128830;
 at<u64>(native.bytes,16)=PartIds[0][0];at<u64>(native.bytes,24)=PartIds[0][1];at<u32>(native.bytes,0xd0)=coffin_direct_assets::Parts[0].name;
 at<u64>(native.bytes,0x40)=0x1700000;at<u64>(native.bytes,0x58)=0x9876;
 at<u64>(story.bytes,16)=coffin_direct_assets::Parts[0].newLo;at<u64>(story.bytes,24)=coffin_direct_assets::Parts[0].newHi;
 at<u64>(story.bytes,0x20)=0x1800000;at<u64>(story.bytes,0x38)=0x1900000;
 auto unchanged=native;ck(copyGraphics(output,native.bytes,story.bytes,0),"exact preview graph copy accepted");
 ck(at<u64>(output.bytes,0x40)==0x1700000,"original collision retained");
 ck(at<u64>(output.bytes,0x20)==0x1800000&&at<u64>(output.bytes,0x38)==0x1900000,"Story mesh and matching bounds selected");
 ck(at<u64>(output.bytes,0x58)==0x9876,"native transform retained");
 ck(!std::memcmp(native.bytes,unchanged.bytes,0xe0),"donor not edited");
 bool identical=true;for(u32 i=0x40;i<0xe0;++i)identical&=native.bytes[i]==output.bytes[i];ck(identical,"all remaining gameplay part fields byte-identical");
 at<u64>(native.bytes,16)^=1;ck(!copyGraphics(output,native.bytes,story.bytes,0),"unrelated preview donor rejected");
 std::printf("COFFIN_PREVIEW_ASSERTIONS=%u FAILURES=%u\n",n,errors);return errors?1:0;}
