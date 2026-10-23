#include "../src/coffin_direct_model_policy.hpp"
#include <unordered_map>
#include <cstring>
#include <cstdio>
using namespace coffin_direct;
int main(){
 unsigned tests=0,errors=0;
 auto ck=[&](bool ok,const char*why){++tests;if(!ok){++errors;std::printf("FAIL %s\n",why);}};
 constexpr u64 image=0x140000000ull,skin=0x200000,vehicle=0x300000,row=0x400000;
 constexpr u64 art=0x500000,skinRes=0x600000,artRes=0x700000,model=0x800000,list=0x900000;
 constexpr u64 group=0xa00000,table=0xb00000,mainOld=0xc00000,auxOld=0xd00000;
 constexpr u64 mainNew=0xe00000,auxNew=0xf00000;
 std::unordered_map<u64,u64> mem;
 auto rd=[&](u64 at,auto&v){auto x=mem.find(at);if(x==mem.end())return false;std::memcpy(&v,&x->second,sizeof(v));return true;};
 auto putId=[&](u64 at,u64 lo,u64 hi){mem[at+0x10]=lo;mem[at+0x18]=hi;};
 u64 privatePartsOut[2]={};
 auto setup=[&](){
  mem.clear();mem[skin+0x48]=vehicle;mem[vehicle]=image+CoffinVehicleVtableRva;mem[vehicle+0xC8]=skin;
  mem[skin+0x30]=skinRes;putId(skinRes,NativeCoffinSkinned.lo,NativeCoffinSkinned.hi);
  mem[vehicle+0xD8]=art;mem[art+0x48]=vehicle;mem[art+0x30]=artRes;
  putId(artRes,NativeCoffinArt.lo,NativeCoffinArt.hi);mem[artRes+0x20]=model;
  putId(model,NativeCoffinArtModel.lo,NativeCoffinArtModel.hi);mem[model+0x38]=10;mem[model+0x40]=list;
  for(unsigned i=0;i<10;++i)mem[list+8*i]=0x1100000+0x100*i;
  mem[list+8]=mainOld;mem[list+6*8]=auxOld;mem[row+0xC8]=mainOld;
  mem[group+0x40]=coffin_direct_assets::Count;mem[group+0x44]=coffin_direct_assets::Count;
  mem[group+0x64]=8542;mem[group+0x48]=table;
  for(unsigned i=0;i<2;++i){
   const auto&p=coffin_direct_assets::Parts[i];u64 old=i?auxOld:mainOld,fresh=i?auxNew:mainNew;
   putId(old,p.oldLo,p.oldHi);putId(fresh,p.newLo,p.newHi);
   mem[old]=mem[fresh]=image+0x312ef28;mem[old+0xD0]=mem[fresh+0xD0]=p.name;
   mem[old+0x40]=mem[fresh+0x40]=0;mem[fresh+0x20]=0x1200000+i*0x100;
   mem[fresh+0x38]=0x1300000+i*0x100;mem[fresh+0x0c]=8542;
   mem[table+8ull*p.index]=fresh;
  }
 };
 setup();ck(privateParts(rd,group,privatePartsOut),"exact graph-registered pair accepted");
 ck(privatePartsOut[0]==mainNew&&privatePartsOut[1]==auxNew,"both registered resource pointers");
 auto c=inspect(rd,image,skin,row,privatePartsOut);
 ck(c.owner==vehicle&&c.original==mainOld&&c.replacement==mainNew&&c.slot==0,"native body selected");
 mem[row+0xC8]=auxOld;c=inspect(rd,image,skin,row,privatePartsOut);
 ck(c.replacement==auxNew&&c.slot==1,"native auxiliary part selected");
 mem[row+0xC8]=0x1110000;ck(!inspect(rd,image,skin,row,privatePartsOut).replacement,"accessories untouched");
 setup();mem[group+0x40]--;ck(!privateParts(rd,group,privatePartsOut),"old sidecar group rejected");
 setup();mem[group+0x64]=53868;ck(!privateParts(rd,group,privatePartsOut),"wrong registered group rejected");
 setup();mem[auxNew+0x18]^=1;ck(!privateParts(rd,group,privatePartsOut),"one wrong resource rejects entire pair");
 setup();mem[auxNew+0x40]=0x1230000;ck(!privateParts(rd,group,privatePartsOut),"foreign physics rejected");
 setup();mem[mainNew+0x38]=0;ck(!privateParts(rd,group,privatePartsOut),"missing bone bounds rejected");
 setup();mem[vehicle]=image+0x3119bc8;ck(!inspect(rd,image,skin,row,privatePartsOut).replacement,"Story Entity never selected");
 setup();mem[vehicle+0xC8]=skin+8;ck(!inspect(rd,image,skin,row,privatePartsOut).replacement,"owner backlink required");
 setup();mem[artRes+0x10]^=1;ck(!inspect(rd,image,skin,row,privatePartsOut).replacement,"original ArtPartsData required");
 setup();mem[skinRes+0x10]^=1;ck(!inspect(rd,image,skin,row,privatePartsOut).replacement,"original SkinnedModel required");
 setup();mem[model+0x38]=1000000;ck(!inspect(rd,image,skin,row,privatePartsOut).replacement,"unbounded part list rejected");
 setup();mem[list+2*8]=mainOld;ck(!inspect(rd,image,skin,row,privatePartsOut).replacement,"ambiguous pair rejected");
 setup();mem[auxOld+0x10]^=1;ck(!inspect(rd,image,skin,row,privatePartsOut).replacement,"missing second source rejects first");
 setup();mem[auxNew]^=8;ck(!inspect(rd,image,skin,row,privatePartsOut).replacement,"new resource native type mismatch");
 setup();mem[auxOld+0x40]=0x1500000;ck(!inspect(rd,image,skin,row,privatePartsOut).replacement,"native physics part never replaced");
 setup();mem.erase(list+8*9);ck(!inspect(rd,image,skin,row,privatePartsOut).replacement,"truncated source array rejected");
 u64 previous[2]={vehicle,vehicle+0x100};
 ck(!selected(true,2000,1000,vehicle,0,previous,2),"existing board excluded");
 ck(selected(true,2000,1000,vehicle+0x200,0,previous,2),"one new board allowed");
 ck(!selected(false,2000,1000,vehicle+0x200,0,previous,2),"unarmed ordinary craft untouched");
 ck(!selected(true,2000,1000,vehicle+0x200,vehicle+0x300,previous,2),"different new owner excluded");
 ck(!selected(true,91001,1000,vehicle+0x200,0,previous,2),"late model creation excluded");
 ck(!selected(true,999,1000,vehicle+0x200,0,previous,2),"time reversal excluded");
 ck(!selected(true,2000,0,vehicle+0x200,0,previous,2),"no manufacture timestamp");
 ck(!selected(true,2000,1000,vehicle+0x200,0,previous,1000),"invalid registry count");
 unsigned binds=0,adds=0;u64 bound=0,passSkin=0,passRow=0;u8 passFlag=0;
 auto bind=[&](u64 r,u64 p){ck(r==row,"native bind receives stack row");++binds;bound=p;};
 auto add=[&](u64 s,u64 r,u8 flag)->u64{++adds;passSkin=s;passRow=r;passFlag=flag;return 0x1234567800000003ull;};
 setup();c=inspect(rd,image,skin,row,privatePartsOut);
 ck(dispatch(c,skin,row,1,bind,add)==0x1234567800000003ull,"complete native return retained");
 ck(binds==1&&adds==1&&bound==mainNew,"one native assignment then one native add");
 ck(passSkin==skin&&passRow==row&&passFlag==1,"all native arguments preserved");
 binds=adds=0;dispatch({},skin,row,0,bind,add);
 ck(!binds&&adds==1&&passFlag==0,"fallback calls only native add exactly once");
 std::printf("COFFIN_DIRECT_MODEL_ASSERTIONS=%u FAILURES=%u\n",tests,errors);
 return errors?1:0;
}
