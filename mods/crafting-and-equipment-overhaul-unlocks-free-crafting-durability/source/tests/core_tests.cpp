#include "../src/core.hpp"
#include "../src/sha256.hpp"
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <stdexcept>
using namespace craft;
static unsigned assertions=0;
void check(bool ok,const char*why){++assertions;if(!ok)throw std::runtime_error(why);}
std::string hex(const u8*x,u32 n){std::string s;for(u32 i=0;i<n;++i){s+="0123456789abcdef"[x[i]>>4];s+="0123456789abcdef"[x[i]&15];}return s;}
Settings cfg(const std::string&s){Settings c={};Error e;check(parse(s.data(),s.size(),c,e),"valid INI");return c;}
Recipe item(u32 idx,u32 key,u8 usage=1){Recipe r={};r.instance={0x141234000,idx,0x12345678};r.resource=0x150000000+idx*0xB0;r.baggage=0x160000000+idx*0x88;r.key=key;r.usage=usage;r.valid=1;r.caseType=1;return r;}
int main(){try{
 const std::string head="[CraftingUnlocks]\nEnabled=1\nDefaultUnlock=1\n";Settings c=cfg(head);
 check(selected(c,0xAA),"default all");check(!c.atlasGoldSkin,"legacy INI defaults to normal ATLAS skin");Settings s=cfg(head+"[Items]\n0x000000AA=0\n0x000000BB=1\n0x000000CC=inherit\n");check(!selected(s,0xAA)&&selected(s,0xBB)&&selected(s,0xCC),"overrides");
 s=cfg(head+"[AtlasEquipment]\nEnabled=1\nGoldSkeletonSkin=1\n");check(s.atlasEnabled&&s.atlasGoldSkin,"ATLAS gold skin toggle parsed independently");
 s=cfg("\xEF\xBB\xBF [CRAFTINGUNLOCKS]\r\n Enabled = 1 ; x\r\nDefaultUnlock=0\r\nExportCatalogue=0\r\n[Items]\r\n0XFFFFFFFF=1\n");check(selected(s,0xFFFFFFFF)&&!selected(s,3)&&!s.dump,"BOM, spacing, case, selection");
 std::vector<std::string> bad={"", "[Items]\n0x12345678=1", "Enabled=1",head+"Enabled=1",head+"[Bad]\nKey=1",head+"Unknown=1",head+"ExportCatalogue=inherit",head+"[Items]\nFloaterLv1=1",head+"[Items]\n0xA=1",head+"[Items]\n0x12345678=2",head+"[Items]\n0x12345678=1\n0x12345678=inherit",head+"[Items",head+"[Items]\n0x12345678",head+"[Items]\n0xZZZZZZZZ=1",head+std::string("a\0b",3)};
 for(const auto&v:bad){Settings x={};Error e;check(!parse(v.data(),v.size(),x,e)&&e.message,"reject malformed settings");}
 for(u32 t:{0x100,0x118}){check(facility_mask(t)==2,"facility2");}
 check(facility_mask(0x101)==1,"facility1");for(u32 t:{0x102,0x202,0x402})check(facility_mask(t)==4,"facility4");for(u32 t:{0x103,0x108,0x203,0x403})check(facility_mask(t)==8,"facility8");check(facility_mask(0,true)==15&&facility_mask(0)==0,"null vs unknown");
 const u8 expected[10]={0,3,6,11,7,0,0,0,0,15};for(u32 u=0;u<10;++u)for(u32 m=0;m<16;++m)check(facility_allows(u,m)==bool(expected[u]&m),"native facility truth table");check(!supported_usage(2)&&!supported_usage(5)&&!supported_usage(7)&&!supported_usage(8),"separate groups excluded");
 std::vector<Recipe> r={item(0,0xAA),item(1,0xBB),item(2,0xCC)};Instance out[MaxMenu]={};Instance native[2]={r[0].instance,r[1].instance};native[0].flags|=0x8000;native[1].flags|=0x28000;
 auto before=r;auto b=build(c,r.data(),r.size(),native,2,3,out);check(b.ok&&b.added==1&&b.count==3,"append one without duplicates");check(std::memcmp(out,native,sizeof(native))==0,"vanilla byte exact");check((out[2].flags&0x8000)&&out[2].padding==r[2].instance.padding,"private copy bit");check(std::memcmp(before.data(),r.data(),r.size()*sizeof(Recipe))==0,"masters unchanged");
 s=cfg(head+"[Items]\n0x000000AA=0\n0x000000CC=0\n");b=build(s,r.data(),3,native,2,3,out);check(b.ok&&b.count==2&&!b.added&&out[0].flags==native[0].flags,"0 never relocks native");
 s=cfg("[CraftingUnlocks]\nEnabled=1\nDefaultUnlock=0\n[Items]\n0x000000CC=1\n");b=build(s,r.data(),3,nullptr,0,3,out);check(b.ok&&b.added==1&&(out[0].flags&0xFFF)==2,"whitelist exact item");
 Recipe special=item(7,OmnireflectorBootsKey,0); b=build(c,&special,1,nullptr,0,2,out); check(b.ok&&b.added==1,"Omnireflector Boots explicit special route"); special=item(8,ChiralBootsKey,0); b=build(c,&special,1,nullptr,0,2,out); check(b.ok&&b.added==1,"Chiral Boots explicit special route"); special=item(9,0x6C3A478Bu,0); b=build(c,&special,1,nullptr,0,2,out); check(b.ok&&b.added==1,"enemy-drop weapon explicit special route"); special=item(10,0x0CE5E07Au,0); b=build(c,&special,1,nullptr,0,2,out); check(b.ok&&b.added==1,"Ghost Blade explicit special route"); const u32 warriorKeys[]={0x22B02657u,0x22424DD4u,0x5029CED7u};
 const u32 warriorBags[]={0x0542C204u,0x05B0A987u,0x77DB2A84u};
 Settings wOn=cfg("[CraftingUnlocks]\nEnabled=1\nDefaultUnlock=0\n[Items]\n0x22B02657=1\n0x22424DD4=1\n0x5029CED7=1\n");
 for(u32 i=0;i<3;++i){Recipe w=item(880+i,warriorKeys[i],0);w.baggageKey=warriorBags[i];
   check(special_normal_recipe(w.key),"Warrior native special route");
   check(selected(wOn,w.key),"Warrior explicit INI selection");
   auto result=build(wOn,&w,1,nullptr,0,2,out);
   check(result.ok&&result.added==1&&result.count==1&&out[0].flags==((880+i)|0x8000u),"Warrior native recipe appears once");
   w.mission=true;check(eligibility(wOn,w,2)==Skip::Quest,"Warrior mission resource blocked");
   w.mission=false;w.valid=0;check(eligibility(wOn,w,2)==Skip::MissingResource,"Warrior invalid resource blocked");
 }
 Settings warriorThree=cfg("[CraftingUnlocks]\nEnabled=1\nDefaultUnlock=0\n[WarriorWeapons]\nAmmoMultiplier=3\n");
 check(warriorThree.silenced.warriorAmmoMultiplier==3,"Warrior ammo default 3x parse");
 Settings warriorNative=cfg("[CraftingUnlocks]\nEnabled=1\nDefaultUnlock=0\n");
 check(warriorNative.silenced.warriorAmmoMultiplier==1,"Older INIs remain native ammo");
 Settings guitarOff=cfg("[CraftingUnlocks]\nEnabled=1\nDefaultUnlock=0\n[Guitars]\nFreeChiralShots=0\n");
 Settings guitarOn=cfg("[CraftingUnlocks]\nEnabled=1\nDefaultUnlock=0\n[Guitars]\nFreeChiralShots=1\n");
 check(!guitarOff.silenced.freeChiralGuitarShots&&guitarOn.silenced.freeChiralGuitarShots,"Both guitar crystal cost ON/OFF setting");
 check(!warriorNative.silenced.freeChiralGuitarShots,"Older INIs never change guitar crystal usage");
 check(warriorNative.silenced.warriorReserveMode==0,"Older INIs retain original Warrior reserves");
 for(u32 reserveMode=0;reserveMode<=2;++reserveMode){
  auto x=cfg(std::string("[CraftingUnlocks]\nEnabled=1\nDefaultUnlock=0\n[WarriorWeapons]\nReserveMode=")+std::to_string(reserveMode)+"\nAmmoMultiplier=3\n");
  check(x.silenced.warriorReserveMode==reserveMode&&x.silenced.warriorAmmoMultiplier==3,"Warrior reserve modes 0..2 and magazine multiplier independent");
 }
 for(const auto&entry:{std::string("ReserveMode=3"),std::string("ReserveMode=inherit"),std::string("AmmoMultiplier=0"),std::string("AmmoMultiplier=11"),std::string("ReserveMode=1\nReserveMode=2")}){
  auto bad=std::string("[CraftingUnlocks]\nEnabled=1\nDefaultUnlock=0\n[WarriorWeapons]\n")+entry+"\n";
  Settings chk{};Error er{};check(!parse(bad.data(),(u32)bad.size(),chk,er),"Invalid Warrior reserve or magazine config rejected");
 }
 for(const auto&entry:{std::string("FreeChiralShots=2"),std::string("FreeChiralShots=inherit"),std::string("FreeChiralShots=1\nFreeChiralShots=0")}){
  auto bad=std::string("[CraftingUnlocks]\nEnabled=1\nDefaultUnlock=0\n[Guitars]\n")+entry+"\n";
  Settings chk{};Error er{};check(!parse(bad.data(),(u32)bad.size(),chk,er),"Invalid or repeated guitar crystal toggle rejected");
 }
 Settings exactWarrior=cfg("[CraftingUnlocks]\nEnabled=1\nDefaultUnlock=0\n[WarriorWeapons]\nAmmoMultiplier=1\nRifleMagazine=30\nShotgunMagazine=12\nGrenadeMagazine=6\nReserveMode=1\nReserveMagazines=6\n");
 check(exactWarrior.silenced.warriorMagazineRounds[0]==30&&exactWarrior.silenced.warriorMagazineRounds[1]==12&&exactWarrior.silenced.warriorMagazineRounds[2]==6,"Exactly requested Warrior magazine rounds");
 check(exactWarrior.silenced.warriorReserveMode==1&&exactWarrior.silenced.warriorReserveMagazineLimit==6,"Six magazines of finite native reserve");
 check(warriorNative.silenced.warriorReserveMagazineLimit==0&&warriorNative.silenced.warriorMagazineRounds[0]==0,"Old INI preserves ammo behavior");
 for(const auto &entry:{std::string("RifleMagazine=0"),std::string("ShotgunMagazine=301"),std::string("GrenadeMagazine=abc"),std::string("ReserveMagazines=31"),std::string("ReserveMagazines=-1"),std::string("RifleMagazine=30\nRifleMagazine=31")}){
  auto bad=std::string("[CraftingUnlocks]\nEnabled=1\nDefaultUnlock=0\n[WarriorWeapons]\n")+entry+"\n";
  Settings current{};Error e{};check(!parse(bad.data(),(u32)bad.size(),current,e),"Reject unsafe Warrior finite-stock parameters");
 }
 Settings warriorFive=cfg("[CraftingUnlocks]\nEnabled=1\nDefaultUnlock=0\n[WarriorWeapons]\nRifleMagazine=30\nShotgunMagazine=12\nGrenadeMagazine=6\nReserveMode=1\nReserveMagazines=5\n");
 check(warriorFive.silenced.warriorReserveMagazineLimit==5,"Five finite base reserve magazines selected");
 Settings redOn=cfg("[CraftingUnlocks]\nEnabled=1\nDefaultUnlock=0\n[RedGuitar]\nEnabled=1\n");
 check(redOn.silenced.redGuitarEnabled,"Red guitar optional enable");
 check(!warriorNative.silenced.redGuitarEnabled,"Existing INI does not activate red model");
 Recipe alt=item(11,0x66E31F44u,0); b=build(c,&alt,1,nullptr,0,2,out); check(b.ok&&!b.added,"Heavy MG remains blocked"); alt=item(12,0x64CE664Eu,0); b=build(c,&alt,1,nullptr,0,2,out); check(b.ok&&!b.added,"alternate Ghost Blade remains blocked"); alt=item(13,0x17B359C8u,0); b=build(c,&alt,1,nullptr,0,2,out); check(b.ok&&!b.added,"alternate Heavy MG remains blocked"); Recipe none=item(14,0x1234ABCD,0); b=build(c,&none,1,nullptr,0,2,out); check(b.ok&&!b.added,"other Usage=None remains blocked");
 Recipe q=item(3,44);for(int g=0;g<6;++g){q=item(3,44);switch(g){case 0:q.valid=0;break;case 1:q.usage=2;break;case 2:q.dlc=1;break;case 3:q.mission=true;break;case 4:q.duplicate=true;break;case 5:q.caseType=11;break;}b=build(c,&q,1,nullptr,0,15,out);check(b.ok&&!b.added,"safety filter");}
 r={item(0,3),item(1,3),item(2,4)};mark_duplicates(r.data(),r.size());check(r[0].duplicate&&r[1].duplicate&&!r[2].duplicate,"duplicate keys detected");
 r.clear();for(u32 i=0;i<513;++i)r.push_back(item(i,i));b=build(c,r.data(),r.size(),nullptr,0,3,out);check(!b.ok&&b.overLimit==1&&b.count==0&&!b.added,"overflow fails to vanilla, not truncation");b=build(c,r.data(),512,nullptr,0,3,out);check(b.ok&&b.count==512,"exact menu capacity");check(!build(c,nullptr,1,nullptr,0,3,out).ok&&!build(c,r.data(),1,nullptr,1,3,out).ok,"invalid input fails");
 std::mt19937 rng(1477);for(u32 round=0;round<2000;++round){u32 count=1+rng()%100,nativeN=rng()%(count+1);r.clear();std::vector<Instance>v;for(u32 i=0;i<count;++i){auto z=item(i,0x8000+i);z.usage=(u8)(rng()%10);z.dlc=(rng()%7==0);z.valid=(rng()%9!=0);r.push_back(z);if(i<nativeN)v.push_back(z.instance);}auto originalR=r;auto originalV=v;u8 mask=1u<<(rng()%4);b=build(c,r.data(),r.size(),v.data(),v.size(),mask,out);check(b.ok,"random bounded build");check(b.count>=nativeN&&b.count<=MaxMenu,"random count");check(!nativeN||std::memcmp(out,v.data(),v.size()*sizeof(Instance))==0,"random vanilla prefix");check(std::memcmp(originalR.data(),r.data(),r.size()*sizeof(Recipe))==0,"random immutable source");for(u32 i=nativeN;i<b.count;++i){u32 idx=out[i].flags&0xFFF;check(idx<count&&(out[i].flags&0x8000)&&eligibility(c,r[idx],mask)==Skip::None,"random addition permitted");for(u32 j=0;j<i;++j)check((out[j].flags&0xFFF)!=idx,"random no duplicate");}}
 const char*shaCases[]={"","abc","abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"};const char*shaExpected[]={"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855","ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad","248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1"};for(int i=0;i<3;++i){Sha256 sh;sh.update(shaCases[i],std::strlen(shaCases[i]));u8 d[32];sh.finish(d);check(hex(d,32)==shaExpected[i],"SHA-256 vector");}Sha256 sh;for(u32 i=0;i<1000000;++i)sh.update("a",1);u8 d[32];sh.finish(d);check(hex(d,32)=="cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0","SHA-256 million a streaming");
 std::cout<<"PASS "<<assertions<<" host assertions (policy/layout/selection/SHA); Windows/game hook NOT executed\n";return 0;
 }catch(const std::exception&e){std::cerr<<"FAIL after "<<assertions<<": "<<e.what()<<"\n";return 1;}}
