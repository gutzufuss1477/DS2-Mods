#include "../src/core.hpp"
#include "../src/durability_scope.hpp"
#include <cmath>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
using namespace craft;

extern "C" u32 DurabilityMultiplierBits=0x40000000u;
extern "C" u32 DurabilityUnbreakableValue=0;
extern "C" u32 InvokeDurabilityContents(unsigned char*,u32,u32);
extern "C" unsigned long long InvokeDurabilityBoots(unsigned char*,unsigned char*,u32);
extern "C" unsigned long long CaptureBootCmpFlags(u32);

static unsigned long long checks=0;
static void check(bool x,const char*m){++checks;if(!x)throw std::runtime_error(m);}
static u32 bits(float v){u32 x;std::memcpy(&x,&v,4);return x;}
static float value(u32 x){float v;std::memcpy(&v,&x,4);return v;}
static Settings cfg(const std::string&t){Settings s{};Error e{};check(parse(t.data(),(u32)t.size(),s,e),"config parse");return s;}
static void near(float a,float b,float eps,const char*m){check(std::fabs(a-b)<=eps,m);}

static u32 invokeContents(u32 baggageKey,float oldv,float nativeNew){
 unsigned char obj[0x100]{}; unsigned char resource[0x88]{};
 std::memcpy(resource+0x44,&baggageKey,4);
 auto resourcePtr=(unsigned long long)resource;
 std::memcpy(obj+0x38,&resourcePtr,8);
 const u32 caseSentinel=0x42C60000u;
 std::memcpy(obj+0x84,&caseSentinel,4);
 u32 out=InvokeDurabilityContents(obj,bits(oldv),bits(nativeNew));
 u32 after=0;std::memcpy(&after,obj+0x84,4);
 check(after==caseSentinel,"cargo/case +0x84 must remain untouched");
 return out;
}

struct BootFixture{
 unsigned char player[0x500]{};
 unsigned char frame[0x240]{};
 unsigned char baggage[0x100]{};
 unsigned char resource[0x100]{};

 void setKey(u32 key){
  auto baggagePtr=(unsigned long long)baggage;
  auto resourcePtr=(unsigned long long)resource;
  std::memcpy(player+0x418,&baggagePtr,8);
  std::memcpy(baggage+0x38,&resourcePtr,8);
  std::memcpy(resource+0x44,&key,4);
 }
 void setWear(float wear){std::memcpy(frame+0x200,&wear,4);}
 void nullBaggage(){unsigned long long z=0;std::memcpy(player+0x418,&z,8);}
 void nullResource(){unsigned long long z=0;std::memcpy(baggage+0x38,&z,8);}
};

static float invokeBoot(BootFixture&f,u32 flagSeed,u32&flags){
 auto r=InvokeDurabilityBoots(f.player,f.frame,flagSeed);
 flags=(u32)(r>>32);
 return value((u32)r);
}

int main(){try{
 const std::string base="[CraftingUnlocks]\nEnabled=1\nDefaultUnlock=1\n";
 auto legacy=cfg(base);
 check(!legacy.durabilityEnabled&&!legacy.durabilityUnbreakable&&legacy.durabilityMilli==2000,"legacy defaults");
 auto s=cfg(base+"[Durability]\nEnabled=1\nMultiplier=2.5\nUnbreakable=0\n");
 check(s.durabilityEnabled&&!s.durabilityUnbreakable&&s.durabilityMilli==2500,"durability config");

 for(u32 m=1000;m<=1000000;++m){
  float f=(float)m/1000.0f;
  check(milliFloatBits(m)==bits(f),"exact milli float bits");
 }

 std::string bads[]={
  base+"[Durability]\nMultiplier=0.999\n",
  base+"[Durability]\nMultiplier=1000.001\n",
  base+"[Durability]\nMultiplier=2.0001\n",
  base+"[Durability]\nMultiplier=x\n",
  base+"[Durability]\nEnabled=2\n",
  base+"[Durability]\nUnbreakable=inherit\n",
  base+"[Durability]\nMultiplier=2\nMultiplier=3\n"};
 for(auto&t:bads){
  Settings q{};Error e{};
  check(!parse(t.data(),(u32)t.size(),q,e),"reject invalid durability setting");
 }

 check(DurabilityCraftedBaggageCount==120,"120 scoped crafted baggage IDs");
 check(DurabilityBootBaggageCount==7,"7 scoped boot baggage IDs");
 for(u32 i=1;i<DurabilityCraftedBaggageCount;++i)
  check(DurabilityCraftedBaggage[i-1]<DurabilityCraftedBaggage[i],"crafted allowlist sorted unique");
 for(u32 i=1;i<DurabilityBootBaggageCount;++i)
  check(DurabilityBootBaggage[i-1]<DurabilityBootBaggage[i],"boot allowlist sorted unique");
 for(u32 boot:DurabilityBootBaggage){
  bool found=false;
  for(u32 item:DurabilityCraftedBaggage)if(item==boot){found=true;break;}
  check(found,"boot ID must also be in crafted allowlist");
 }

 struct Case{u32 milli;bool unbreakable;float oldv,newv,expected;};
 Case cases[]={
  {1000,false,100,90,90},
  {2000,false,100,90,95},
  {5000,false,100,90,98},
  {10000,false,100,90,99},
  {2500,false,100,90,96},
  {2000,false,100,0,50},
  {3000,false,100,33,77.666664f},
  {2000,true,100,1,100},
  {1000000,false,100,0,99.9f}
 };
 for(const auto&c:cases){
  DurabilityMultiplierBits=milliFloatBits(c.milli);
  DurabilityUnbreakableValue=c.unbreakable?1:0;
  for(u32 key:DurabilityCraftedBaggage)
   near(value(invokeContents(key,c.oldv,c.newv)),c.expected,0.0002f,"crafted item durability scaling");
 }

 DurabilityMultiplierBits=milliFloatBits(2000);
 DurabilityUnbreakableValue=0;
 const u32 cargoKeys[]={0u,0xFFFFFFFFu,0x12345678u,0xDEADBEEFu};
 for(u32 key:cargoKeys)
  near(value(invokeContents(key,100,60)),60,0.0001f,"non-crafted cargo remains native");
 DurabilityUnbreakableValue=1;
 for(u32 key:cargoKeys)
  near(value(invokeContents(key,100,60)),60,0.0001f,"unbreakable must not affect non-crafted cargo");
 near(value(invokeContents(DurabilityCraftedBaggage[0],100,60)),100,0.0001f,"unbreakable crafted item");

 // Boots: the thunk replays the native load from [RBP+0x200], then scopes only seven boot baggage IDs.
 constexpr u32 FlagMask=0x8D5u; // CF, PF, AF, ZF, SF, OF
 const u32 flagSeeds[]={0u,0x13579BDFu,0xFFFFFFFFu};
 for(u32 seed:flagSeeds){
  const u32 expectedFlags=(u32)CaptureBootCmpFlags(seed)&FlagMask;
  for(u32 multiplier: {1000u,2000u,5000u,10000u}){
   DurabilityMultiplierBits=milliFloatBits(multiplier);
   DurabilityUnbreakableValue=0;
   for(u32 boot:DurabilityBootBaggage){
    BootFixture f{};f.setKey(boot);f.setWear(10.0f);
    u32 flags=0;float out=invokeBoot(f,seed,flags);
    near(out,10.0f/((float)multiplier/1000.0f),0.0002f,"boot wear multiplier");
    check((flags&FlagMask)==expectedFlags,"boot thunk must preserve live EFLAGS");
   }
  }
  DurabilityMultiplierBits=milliFloatBits(2000);
  DurabilityUnbreakableValue=1;
  for(u32 boot:DurabilityBootBaggage){
   BootFixture f{};f.setKey(boot);f.setWear(10.0f);
   u32 flags=0;float out=invokeBoot(f,seed,flags);
   near(out,0.0f,0.0f,"unbreakable boot wear must be zero");
   check((flags&FlagMask)==expectedFlags,"unbreakable boot thunk must preserve live EFLAGS");
  }

  // A crafted item that is not a boot must not be treated as footwear by this path.
  u32 nonBoot=0;
  for(u32 item:DurabilityCraftedBaggage){
   bool isBoot=false;for(u32 boot:DurabilityBootBaggage)if(item==boot){isBoot=true;break;}
   if(!isBoot){nonBoot=item;break;}
  }
  check(nonBoot!=0,"need non-boot crafted fixture");
  {
   BootFixture f{};f.setKey(nonBoot);f.setWear(10.0f);
   u32 flags=0;near(invokeBoot(f,seed,flags),10.0f,0.0f,"non-boot footwear path stays native");
   check((flags&FlagMask)==expectedFlags,"non-boot path flags");
  }
  {
   BootFixture f{};f.setKey(0xDEADBEEFu);f.setWear(10.0f);
   u32 flags=0;near(invokeBoot(f,seed,flags),10.0f,0.0f,"unknown footwear path stays native");
   check((flags&FlagMask)==expectedFlags,"unknown path flags");
  }
  {
   BootFixture f{};f.setKey(DurabilityBootBaggage[0]);f.setWear(10.0f);f.nullBaggage();
   u32 flags=0;near(invokeBoot(f,seed,flags),10.0f,0.0f,"null current boots stays native");
   check((flags&FlagMask)==expectedFlags,"null baggage flags");
  }
  {
   BootFixture f{};f.setKey(DurabilityBootBaggage[0]);f.setWear(10.0f);f.nullResource();
   u32 flags=0;near(invokeBoot(f,seed,flags),10.0f,0.0f,"null boots resource stays native");
   check((flags&FlagMask)==expectedFlags,"null resource flags");
  }
 }

 std::cout<<"PASS "<<checks<<" scoped durability assertions.\n";
 std::cout<<"120 crafted baggage IDs + 7 boot IDs tested; cargo/container +0x84 never patched.\n";
 std::cout<<"Boots multiplier/unbreakable and live EFLAGS preservation verified.\n";
 return 0;
 }catch(const std::exception&e){
  std::cerr<<"FAIL after "<<checks<<": "<<e.what()<<"\n";
  return 1;
 }}
