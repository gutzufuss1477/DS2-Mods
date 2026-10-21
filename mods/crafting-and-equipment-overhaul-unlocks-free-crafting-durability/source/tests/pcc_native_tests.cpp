// Executes the exact game's eligibility machine code in an isolated test process.
// The fact store is a stub; the caller, flags, selection loop, production callback
// and production assembly thunk execute natively. DS2 is never loaded or launched.
#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <array>
#include "../src/pcc_structures.hpp"
#include "../src/pcc_sites.generated.hpp"
using namespace craft;
static Settings settings{};
static u64 image;
static unsigned patches=0,logs=0,checks=0,originalCalls=0;
static bool originalTrue=false,wrongArguments=false;
static u64 expectedManager=0x12345678;
static void require(bool ok,const char*message){++checks;if(!ok){std::printf("FAIL: %s\n",message);std::exit(1);}}
static bool readmem(u64 address,void*out,u32 n){SIZE_T got=0;return address>=0x10000&&ReadProcessMemory(GetCurrentProcess(),(void*)address,out,n,&got)&&got==n;}
template<class T>static bool rd(u64 p,T&v){return readmem(p,&v,sizeof(v));}
struct Text {void add(const char*){} void dec(u64){}};
static void log(const Text&){++logs;}
static void say(const char*){++logs;}
static void*relayNear(u64,u64 target){auto*p=(u8*)(image+0x2010000);p[0]=0xff;p[1]=0x25;std::memset(p+2,0,4);std::memcpy(p+6,&target,8);return p;}
static bool patchOnce(u64 rva,const u8*bytes,void*relay){auto*p=(u8*)(image+rva);if(std::memcmp(p,bytes,5))return false;i32 delta=(i32)((u64)relay-image-rva-5);p[0]=0xe8;std::memcpy(p+1,&delta,4);++patches;return true;}
#include "../src/pcc_runtime.inl"
static u8 nativeFact(u64 manager,u64 context,u64 fact){
 ++originalCalls;if(manager!=expectedManager||context!=image+pcc::ContextRva||fact<0x10000)wrongArguments=true;
 return originalTrue?1:0;
}
template<class T>static void put(u8*p,u32 offset,T value){std::memcpy(p+offset,&value,sizeof(value));}
template<class T>static T at(const u8*p,u32 offset){T value{};std::memcpy(&value,p+offset,sizeof(value));return value;}
struct Fixture {
 std::array<u8,0x800> behavior{};std::array<u8,0x2200> entity{};
 std::array<u8,0x400> owner{};std::array<u8,0x100> weapon{};std::array<u8,0x80> trait{};
 std::array<std::array<u8,0x400>,12> ammo{};std::array<std::array<u8,0x40>,12> facts{};
 const pcc::TierIdentity&t;
 Fixture(unsigned tier):t(pcc::Tiers[tier]){
  put(behavior.data(),0,image+pcc::BehaviorVtableRva);put(behavior.data(),0x50,(u64)entity.data());put(behavior.data(),0x60,(u64)trait.data());
  put(behavior.data(),0x2e8,(i32)t.count);put(behavior.data(),0x2f0,0);
  put(entity.data(),0x2180,(u64)weapon.data());put(entity.data(),0x338,(u64)owner.data()+0x20);
  put(weapon.data(),0x20,t.id);weapon[0x3a]=30;weapon[0x72]=t.level;
  std::memcpy(weapon.data()+0x10,t.uuid,16);std::memcpy(trait.data()+0x10,t.trait,16);
  for(unsigned i=0;i<t.count;++i){auto*p=ammo[i].data();auto*f=facts[i].data();
   put(behavior.data(),0x68+i*0x28,t.ammo[i]);put(behavior.data(),0x70+i*0x28,(u64)p);
   put(p,0x20,t.ammo[i]);p[0x24]=17;p[0x24b]=1;put(p,0x378,(u64)f);
   for(const auto&a:pcc::Ammo)if(a.id==t.ammo[i]){p[0x248]=a.category;std::memcpy(p+0x10,a.uuid,16);std::memcpy(f+0x10,a.fact,16);}
  }
 }
 u8 available(unsigned i)const{return behavior[0x88+i*0x28];}
 void run(bool hook){
  auto*p=(u8*)(image+pcc::SiteRva);
  if(hook){i32 d=(i32)(0x2010000-pcc::SiteRva-5);p[0]=0xe8;std::memcpy(p+1,&d,4);}
  else std::memcpy(p,pcc::CallBytes,5);
  FlushInstructionCache(GetCurrentProcess(),(void*)image,0x6300000);
  ((void(*)(void*))(image+pcc::AvailabilityRva))(behavior.data());
 }
};
static void parserTests(){
 const char*base="[CraftingUnlocks]\nEnabled=1\nDefaultUnlock=0\n";
 Settings s{};Error e{};require(parse(base,(u32)std::strlen(base),s,e)&&!s.pccUnlockAll,"old INI defaults PCC off");
 for(const char*value:{"0","1","2","-1","inherit","true","01",""}){
  char text[256];std::snprintf(text,sizeof(text),"%s[PCCStructures]\nUnlockAll=%s\n",base,value);
  bool ok=parse(text,(u32)std::strlen(text),s,e);require(ok==(std::strcmp(value,"0")==0||std::strcmp(value,"1")==0),"strict PCC bool");
  if(ok)require(s.pccUnlockAll==(value[0]=='1'),"parsed PCC value");
 }
 for(const char*tail:{"[PCCStructures]\nEnabled=1\n","[PCCStructures]\nUnlockAll=1\nUnlockAll=0\n","[PCCStructures]\nUnlockAll=1\n[PCCStructures]\nUnlockAll=1\n"}){
  char text[300];std::snprintf(text,sizeof(text),"%s%s",base,tail);require(!parse(text,(u32)std::strlen(text),s,e),"reject unknown/duplicate PCC setting");
 }
}
static void unwindTests(){
 DWORD64 base=0;auto*entry=RtlLookupFunctionEntry((DWORD64)&PccCatalogueThunk,&base,nullptr);
 require(entry!=nullptr,"production thunk has Windows unwind entry");
 alignas(16) DWORD64 stack[32]{};
 const DWORD64 originalRsp=(DWORD64)&stack[16],returnAddress=0x1234567812345678;
 stack[16]=returnAddress;
 for(unsigned offset:{0u,4u,7u,12u,17u,21u}){
  CONTEXT context{};context.Rip=(DWORD64)&PccCatalogueThunk+offset;
  context.Rsp=originalRsp-((offset>0&&offset<21)?40:0);context.R15=0xabcdef;
  void*handler=nullptr;DWORD64 frame=0;
  RtlVirtualUnwind(UNW_FLAG_NHANDLER,base,context.Rip,entry,&context,&handler,&frame,nullptr);
  require(context.Rsp==originalRsp+8&&context.Rip==returnAddress&&context.R15==0xabcdef,"Windows unwinds thunk prologue/body/epilogue correctly");
 }
}
int main(){
 parserTests();
 unwindTests();
 image=(u64)VirtualAlloc(nullptr,0x6300000,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);require(image!=0,"allocate isolated native fixture");
 std::memcpy((void*)(image+pcc::AvailabilityRva),pcc::AvailabilityBytes,sizeof(pcc::AvailabilityBytes));
 std::memcpy((void*)(image+pcc::FactReaderRva),pcc::FactReaderBytes,sizeof(pcc::FactReaderBytes));
 put((u8*)image,(u32)(pcc::BehaviorVtableRva+0x208),image+pcc::AvailabilityRva);
 put((u8*)image,(u32)pcc::ManagerRva,expectedManager);
 require(pccSignaturesMatch(),"exact loaded eligibility/fact/vtable guards");
 installPccStructures();require(patches==0,"off installs no hook");settings.enabled=true;settings.pccUnlockAll=true;
 ((u8*)image)[pcc::AvailabilityRva+0x30]^=1;require(!pccSignaturesMatch(),"changed eligibility body rejected");installPccStructures();require(patches==0,"conflict blocks installation");((u8*)image)[pcc::AvailabilityRva+0x30]^=1;
 ((u8*)image)[pcc::FactReaderRva]^=1;require(!pccSignaturesMatch(),"fact reader hook conflict rejected");((u8*)image)[pcc::FactReaderRva]^=1;
 put((u8*)image,(u32)(pcc::BehaviorVtableRva+0x208),image+pcc::AvailabilityRva+1);require(!pccSignaturesMatch(),"vtable conflict rejected");put((u8*)image,(u32)(pcc::BehaviorVtableRva+0x208),image+pcc::AvailabilityRva);
 installPccStructures();require(patches==1,"one callsite installed");
 // Replace only the fact store in this synthetic image; keep the real caller.
 auto*stub=(u8*)(image+pcc::OriginalRva);stub[0]=0x48;stub[1]=0xb8;u64 fn=(u64)&nativeFact;std::memcpy(stub+2,&fn,8);stub[10]=0xff;stub[11]=0xe0;pccOriginal=&nativeFact;
 for(unsigned tier=0;tier<3;++tier){
  Fixture f(tier);auto weaponBefore=f.weapon;auto factsBefore=f.facts;auto ammoBefore=f.ammo;
  originalCalls=0;f.run(false);require(originalCalls==f.t.count,"native getter called once per fact");
  for(unsigned i=0;i<f.t.count;++i)require(!f.available(i),"native locked choices hidden");
  originalCalls=0;f.run(true);require(originalCalls==f.t.count&&!wrongArguments,"production thunk preserves native getter ABI");
  for(unsigned i=0;i<f.t.count;++i)require(f.available(i)==(f.t.ammo[i]!=353),"only standard native tier choices unlocked");
  require(at<i32>(f.behavior.data(),0x2f0)==0,"native initial selection chooses newly eligible entry");
  put(f.behavior.data(),0x2f0,99);f.run(true);require(at<i32>(f.behavior.data(),0x2f0)==0,"native repairs out-of-range selected index");
  if(tier){put(f.behavior.data(),0x2f0,10);f.run(true);require(at<i32>(f.behavior.data(),0x2f0)==0,"native skips locked excluded spring choice");}
  require(f.weapon==weaponBefore&&f.facts==factsBefore&&f.ammo==ammoBefore,"resources and facts unchanged");
  settings.pccUnlockAll=false;f.run(true);for(unsigned i=0;i<f.t.count;++i)require(!f.available(i),"option off preserves native");settings.pccUnlockAll=true;
  settings.enabled=false;f.run(true);for(unsigned i=0;i<f.t.count;++i)require(!f.available(i),"master off preserves native");settings.enabled=true;
  originalTrue=true;f.run(true);for(unsigned i=0;i<f.t.count;++i)require(f.available(i)==1,"native true results including excluded entry preserved");originalTrue=false;
  f.owner[0x327]=42;f.run(true);for(unsigned i=0;i<f.t.count;++i)require(f.available(i)==1,"nonplayer native path unaffected");f.owner[0x327]=0;
 }
 // Reject individual identity/layout drift without weakening the native result.
 for(unsigned mutation=0;mutation<15;++mutation){Fixture f(1);
  switch(mutation){
   case 0:f.weapon[0x10]^=1;break;case 1:f.trait[0x10]^=1;break;case 2:f.weapon[0x72]=1;break;
   case 3:put(f.behavior.data(),0,image+pcc::BehaviorVtableRva+8);break;
   case 4:f.ammo[0][0x10]^=1;break;case 5:f.facts[0][0x10]^=1;break;case 6:f.ammo[0][0x248]=0;break;
   case 7:f.ammo[0][0x24b]=0;break;case 8:put(f.ammo[0].data(),0x380,1);break;
   case 9:put(f.behavior.data(),0x68,(u16)339);break;case 10:f.ammo[0][0x24]=1;break;
   case 11:f.weapon[0x3a]=33;break;case 12:put(f.entity.data(),0x2180,(u64)0x1234);break;
   case 13:put(f.behavior.data(),0x60,(u64)0x1234);break;case 14:f.ammo[0][0x20]^=1;break;
  }
  f.run(true);require(f.available(0)==0,"context/identity drift fails closed");
 }
 Fixture content(0);put(content.ammo[0].data(),0x378,(u64)0);put(content.ammo[0].data(),0x380,1);u8 contentType=42;put(content.ammo[0].data(),0x388,(u64)&contentType);
 content.run(true);require(!content.available(0),"native content allowlist remains enforced");
 contentType=0;content.run(true);require(content.available(0)==1,"native content allowlist positive case");
 Fixture f(0);u64 ammo=(u64)f.ammo[0].data(),fact=(u64)f.facts[0].data(),behavior=(u64)f.behavior.data();
 auto reader=[](u64 p,void*out,u32 n){return readmem(p,out,n);};
 require(pcc::eligible(reader,image,expectedManager,image+pcc::ContextRva+1,fact,ammo,behavior)<0,"wrong context rejected");
 require(pcc::eligible(reader,image,expectedManager+1,image+pcc::ContextRva,fact,ammo,behavior)<0,"wrong manager rejected");
 require(pcc::eligible(reader,image,expectedManager,image+pcc::ContextRva,fact+1,ammo,behavior)<0,"wrong fact pointer rejected");
 put(f.behavior.data(),0x2e8,17);require(pcc::eligible(reader,image,expectedManager,image+pcc::ContextRva,fact,ammo,behavior)<0,"oversized list rejected before traversal");
 require(!wrongArguments,"all original calls retain RCX RDX R8");
 VirtualFree((void*)image,0,MEM_RELEASE);
 std::printf("PASS %u assertions: exact native eligibility code + production thunk/callback, 3 tiers, opt-in, identity guards, native exclusions, no resource/fact writes.\n",checks);
}
