#include "win_api.hpp"
#include "../silenced_integration.hpp"
#include "../addon_host.hpp"
#include "appearance.hpp"
#include "attachment.hpp"
#include "pistol.hpp"
#include "menu_icon.hpp"
#include "warrior_policy.hpp"
extern "C" int PistolGpuPoll(void*,unsigned long long*,unsigned long long*);
extern "C" unsigned long PistolGpuError();
extern "C" int PrivateIconGpuPoll(unsigned int,void*,unsigned long long*,unsigned long long*);
extern "C" unsigned long PrivateIconGpuError(unsigned int);
using namespace silenced;
extern "C" void* memcpy(void*,const void*,SIZE_T);
extern "C" void* memset(void*,int,SIZE_T);
extern "C" int memcmp(const void*,const void*,SIZE_T);
extern "C" long _InterlockedCompareExchange(long volatile*,long,long);
extern "C" long _InterlockedExchange(long volatile*,long);
extern "C" long _InterlockedIncrement(long volatile*);
#pragma intrinsic(_InterlockedCompareExchange,_InterlockedExchange,_InterlockedIncrement)
namespace {
constexpr u32 Site=0xB6CB20,WarriorReloadSite=0x202AB90,WarriorInitSite=0x202A7F0,WarriorSyncSite=0x202A990;
constexpr u8 WarriorReloadExpected[]={0x44,0x8B,0x81,0x28,0x06,0x00,0x00};
constexpr u8 WarriorInitExpected[]={0x48,0x89,0x5C,0x24,0x08,0x57};
constexpr u8 WarriorSyncExpected[]={0x40,0x53,0x8B,0x81,0x24,0x06,0x00,0x00};
constexpr u8 Expected[]={0x48,0x89,0x5C,0x24,0x08};
constexpr u32 PaintSite=0x1FAD390,PaintOriginal=0x1FA9850;
constexpr u8 PaintExpected[]={0xE9,0xBB,0xC4,0xFF,0xFF};
constexpr u32 WeaponType=0x44C6540,AmmoType=0x44C5D00,TraitType=0x44C0AE0,ListType=0x4333A00,RecipeType=0x43340C0,BagType=0x4334010,DepotType=0x605F3A0;
constexpr u32 WarriorBaggageKeys[3]={0x0542C204u,0x05B0A987u,0x77DB2A84u};
constexpr u32 WarriorBaggageOriginalAmounts[3]={100,16,12};
u64 image;void*cave=nullptr;void (*logCallback)(const char*)=nullptr;
using WarriorReloadFn=void(*)(u64);WarriorReloadFn originalWarriorReload=nullptr,originalWarriorInit=nullptr,originalWarriorSync=nullptr;
struct WarriorNativeStock {u64 entry=0;u32 rounds=0;u32 initialNative=0;};
struct WarriorStockCache {WarriorNativeStock items[384];u32 count=0;};
WarriorStockCache warriorStockCache[3];
volatile long warriorLocks[3]={},warriorGrantCount[3]={},warriorReloadCount[3]={},warriorRestoreCount[3]={},warriorInvalidCategoryLogged[3]={};
bool recipeEnabled[Count]={true,true,true,true,false,false};u32 languageSetting=0;
// V1 dispatch is always available, independent of loader order and weapon options.
constexpr bool addonIconRequested=true;
bool blackRed=true;volatile long paintLogged[Count]={},materialMissingLogged[Count]={};
bool visibleSuppressor[Count]={false,true,true,true,false,false};
u32 warriorAmmoMultiplier=1,warriorReserveMode=0,warriorMagazines[3]={},warriorReserveLimit=0;bool freeChiralGuitarShots=false;
constexpr u16 WarriorAmmoIds[]={316,317,318};
constexpr u8 WarriorAmmoLoaderCategory[]={1,3,10}; // native rifle, shotgun, grenade
constexpr u32 WarriorNativeMagazines[]={36,8,8};
struct AttachmentState {AttachmentGraph*graph=nullptr;volatile long busy=0,logged=0,waiting=0;u64 retry=0;bool failed=false;};
AttachmentState attachments[Count];
bool visualHook(){return blackRed||visibleSuppressor[1]||visibleSuppressor[2]||visibleSuppressor[3];}
struct MaterialCache {volatile long state=0;u64 resource=0;};MaterialCache materialCache[3];
Bundle* bundles[Count]={};volatile long registration=0;
using Constructor=void*(*)(void*);Constructor original=nullptr;
using WeaponPostUpdate=u64(*)(u64,u64);WeaponPostUpdate originalPaint=nullptr;
using IconLookup=u64(*)(u64,const char*const*);IconLookup originalIcon=nullptr;
NativeIconString customIconNames[]={CustomIconStrings[0],CustomIconStrings[1],CustomIconStrings[2]};
NativeIconString sourceIconNames[]={SourceIconStrings[0],SourceIconStrings[1],SourceIconStrings[2]};
bool readmem(u64 p,void*out,SIZE_T n){SIZE_T got=0;return p>=0x10000 && p+n>=p && ReadProcessMemory(GetCurrentProcess(),(void*)p,out,n,&got)&&got==n;}
template<class T>T read(u64 p){T v={};readmem(p,&v,sizeof(v));return v;}
bool writable(void*p,SIZE_T n){MemoryInfo m={};return p&&VirtualQuery(p,&m,sizeof(m))==sizeof(m)&&m.state==0x1000&&
 (m.protect==4||m.protect==8||m.protect==0x40||m.protect==0x80)&&u64(p)>=u64(m.base)&&u64(p)+n>=u64(p)&&u64(p)+n<=u64(m.base)+m.size;}
void log(const char*s){if(logCallback)logCallback(s);}
void logId(const char*s,u32 v){char b[200];u32 n=0;while(s[n]&&n<170){b[n]=s[n];++n;}char q[12];u32 c=0;do{q[c++]=char('0'+v%10);v/=10;}while(v);while(c)b[n++]=q[--c];b[n]=0;log(b);}
u64 typeOf(u64 p){u64 fn=read<u64>(read<u64>(p));if(fn<image||fn>=image+0x2B00000)return 0;return ((u64(*)(u64))fn)(p)-image;}
struct Survey {u64 weapons[Count]={},recipes[Count]={},ammo[Count][MaxAmmo]={},donors[Count][MaxAmmo]={},icons[2]={};u64 seen[8192]={};u64 warriorAmmo[3]={},warriorBaggage[3]={};u64 redModelWeapon=0;u32 visited=0;bool collision=false;};
bool scan(u64 depot,Survey&s,u32 depth){
 if(depth>32||s.visited==8192)return false;
 for(u32 i=0;i<s.visited;++i)if(s.seen[i]==depot)return true;
 s.seen[s.visited++]=depot;
 i32 n=read<i32>(depot+0x38);u64 table=read<u64>(depot+0x40);if(n<0||n>200000||(!table&&n))return false;
 for(i32 i=0;i<n;++i){
  u64 p=read<u64>(table+8*i);if(!p)continue;u64 t=typeOf(p);if(!t)return false;
  if(t==DepotType){if(!scan(p,s,depth+1))return false;continue;}
  u16 id=(t==WeaponType||t==AmmoType)?read<u16>(p+0x20):0;
  if((t==WeaponType&&ownWeapon(id))||(t==AmmoType&&ownAmmo(id)))s.collision=true;
  if(t==AmmoType)for(u32 z=0;z<3;++z)if(id==WarriorAmmoIds[z]){if(s.warriorAmmo[z]&&s.warriorAmmo[z]!=p)return false;s.warriorAmmo[z]=p;}
  if(t==BagType)for(u32 z=0;z<3;++z)if(read<u32>(p+0x44)==WarriorBaggageKeys[z]){
   if(s.warriorBaggage[z]&&s.warriorBaggage[z]!=p)return false;s.warriorBaggage[z]=p;
  }
  if(t==WeaponType&&id==186){if(s.redModelWeapon&&s.redModelWeapon!=p)return false;s.redModelWeapon=p;}
  for(u32 k=0;k<Count;++k){const auto&d=Definitions[k];
   u64*slot=nullptr;
   if(t==WeaponType&&id==d.source)slot=&s.weapons[k];
   if(t==ListType&&k<2&&read<u32>(p+0x40)==IconDonors[k].listId&&read<u32>(p+0x44)==IconDonors[k].nameCode)slot=&s.icons[k];
   if(t==RecipeType){u32 key=read<u32>(p+0x20);if(key==d.newRecipe)s.collision=true;if(key==d.recipe)slot=&s.recipes[k];}
   if((t==ListType||t==BagType)&&(read<u32>(p+0x44)==d.newList||read<u32>(p+0x44)==d.newBag||read<u32>(p+0x40)==d.newListId))s.collision=true;
   if(slot){if(*slot&&*slot!=p)return false;*slot=p;}
   if(t==AmmoType)for(u32 j=0;j<d.ammoCount;++j){
    if(id==d.ammo[j]){if(s.ammo[k][j]&&s.ammo[k][j]!=p)return false;s.ammo[k][j]=p;}
    if(id==d.soundDonors[j]){if(s.donors[k][j]&&s.donors[k][j]!=p)return false;s.donors[k][j]=p;}
   }
  }
 }
 return true;
}
bool readable(u64 p,u32 n){u8 b[0x400];return n<=sizeof(b)&&readmem(p,b,n);}
struct Pins {u64 p[96]={};u32 n=0;bool add(u64 v){if(!v)return false;for(u32 i=0;i<n;++i)if(p[i]==v)return true;if(n==96||!writable((void*)(v+8),4))return false;p[n++]=v;return true;}};
bool sources(const Survey&s,u32 k,Sources&o,Pins&pins){
 const auto&d=Definitions[k];u64 w=s.weapons[k],r=s.recipes[k];
 if(!readable(w,0x320)||!readable(r,0xB0)||!pins.add(w)||!pins.add(r))return false;
 u64 l=read<u64>(w+0x28),b=read<u64>(r+0x28);
 if(typeOf(l)!=ListType||typeOf(b)!=BagType||!readable(l,0xC0)||!readable(b,0x88)||!pins.add(l)||!pins.add(b))return false;
 u64 name=read<u64>(l+0x20),desc=read<u64>(l+0x28);
 if(!name)name=read<u64>(w+0x2D0);if(!desc)desc=read<u64>(w+0x2D8);
 if(!readable(name,0x38)||!readable(desc,0x38)||!pins.add(name)||!pins.add(desc))return false;
 o.weapon=(u8*)w;o.list=(u8*)l;o.bag=(u8*)b;o.recipe=(u8*)r;o.name=(u8*)name;o.description=(u8*)desc;
 i32 n=read<i32>(w+0x40);u64 array=read<u64>(w+0x48);if(n<=0||n>i32(MaxTraits))return false;
 o.traitsCount=u32(n);o.primary=MaxTraits;
 for(u32 i=0;i<u32(n);++i){u64 p=read<u64>(array+i*8);if(typeOf(p)!=TraitType||!readable(p,0x120)||!pins.add(p))return false;o.traits[i]=(u8*)p;
  if(read<u32>(p+0x30)==d.primaryBehavior && !read<u8>(p+0x34) && read<u32>(p+0x28)==0){if(o.primary!=MaxTraits)return false;o.primary=i;}
 }
 if(o.primary==MaxTraits)return false;
 u64 primary=u64(o.traits[o.primary]),ids=read<u64>(primary+0x60);if(read<u32>(primary+0x58)!=d.ammoCount)return false;
 for(u32 j=0;j<d.ammoCount;++j){
  if(read<u16>(ids+2*j)!=d.ammo[j]||!readable(s.ammo[k][j],0x390)||!readable(s.donors[k][j],0x390)||!pins.add(s.ammo[k][j])||!pins.add(s.donors[k][j]))return false;
  o.ammo[j]=(u8*)s.ammo[k][j];o.soundDonors[j]=(u8*)s.donors[k][j];
  u64 sound=read<u64>(s.donors[k][j]+0x300);if(!pins.add(sound))return false;
 }
 return true;
}
struct Plan {u64 depot=0,old=0,fresh=0; i32 count=0,capacity=0;};
bool plan(Plan&p,u64 depot,u32 extra){
 if(!writable((void*)(depot+0x38),16))return false;
 p.depot=depot;p.count=read<i32>(depot+0x38);p.old=read<u64>(depot+0x40);
 i32 oldcap=read<i32>(depot+0x3C);if(p.count<0||p.count>200000||oldcap<p.count||(!p.old&&p.count))return false;
 p.capacity=p.count+extra;p.fresh=((u64(*)(u64))(image+0xA19D0))(u64(p.capacity)*8);
 return p.fresh&&(!p.count||readmem(p.old,(void*)p.fresh,u64(p.count)*8));
}
bool unchanged(const Plan&p){return read<i32>(p.depot+0x38)==p.count&&read<u64>(p.depot+0x40)==p.old;}
void discard(Plan&p){if(p.fresh)((void(*)(u64))(image+0xA0D10))(p.fresh);p.fresh=0;}
void publish(Plan&p){at<u64>((void*)p.depot,0x40)=p.fresh;at<i32>((void*)p.depot,0x3C)=p.capacity;}
void append(u64 depot,void*p){_InterlockedIncrement((long*)((u8*)p+8));i32&n=at<i32>((void*)depot,0x38);at<u64>((void*)read<u64>(depot+0x40),8*n)=u64(p);++n;}
void configureWarriorAmmo(const Survey&s){
 bool overrides=false;for(u32 i=0;i<3;++i)overrides|=warriorMagazines[i]!=0;
 if(warriorAmmoMultiplier==1&&warriorReserveMode==0&&!overrides){log("WARRIOR_AMMO_NATIVE: capacity and reserve settings unchanged.");return;}
 u64 ptrs[3]={};
 for(u32 i=0;i<3;++i){
  u64 p=s.warriorAmmo[i];u16 id=0;u32 cap=0;
  if(!p||typeOf(p)!=AmmoType||!readable(p,0x160)||
     !readmem(p+0x20,&id,2)||id!=WarriorAmmoIds[i]||
     !readmem(p+0xD8,&cap,4)||cap!=WarriorNativeMagazines[i]||
     read<u8>(p+0x2D)!=0||read<u8>(p+0x2E)!=0||read<u8>(p+0x155)!=0||
     !writable((void*)(p+0x2D),2)||!writable((void*)(p+0xD8),4)||!writable((void*)(p+0x155),1)){
   logId("WARRIOR_AMMO_REFUSED: original ammo type/capacity/flags unexpected, index=",i);
   return;
  }
  ptrs[i]=p;
 }
 for(u32 i=0;i<3;++i){
  u64 p=ptrs[i];u32 cap=warrior::cap(WarriorNativeMagazines[i],warriorAmmoMultiplier,warriorMagazines[i]);
  at<u32>((void*)p,0xD8)=cap;
  if(warriorReserveMode>=1){
   at<u8>((void*)p,0x2D)=1; // allow native backpack spare-ammo containers
   at<u8>((void*)p,0x2E)=WarriorAmmoLoaderCategory[i];
  }
  if(warriorReserveMode==2)at<u8>((void*)p,0x155)=1; // native unlimited storage; magazine still finite
  logId("WARRIOR_MAGAZINE_ROUNDS count=",cap);
 }
 if(warriorReserveMode==1)log("WARRIOR_RESERVE_BACKPACK_ON: extra reloads sourced from equipped ammo containers.");
 else if(warriorReserveMode==2)log("WARRIOR_RESERVE_UNLIMITED_ON: magazine reloads have unlimited storage rounds.");
 else log("WARRIOR_RESERVE_NATIVE: only magazine capacity changed.");
}
void configureGuitarChiral(const Survey&s,const Bundle*newGuitar){
 if(!freeChiralGuitarShots){log("GUITARS_CHIRAL_NATIVE: both guitars retain native chiral crystal usage.");return;}
 constexpr u16 NativeGuitarAmmo=298;
 u64 pointers[2]={s.ammo[GuitarVariant][0],newGuitar?u64(newGuitar->ammo[0]):0};
 u16 expectedIds[2]={NativeGuitarAmmo,ammoId(GuitarVariant,0)};
 for(u32 i=0;i<2;++i){
  u64 p=pointers[i];u16 id=0;
  if(!p||typeOf(p)!=AmmoType||!readable(p,0x160)||!readmem(p+0x20,&id,2)||id!=expectedIds[i]||
     read<u32>(p+0x7C)!=2||read<float>(p+0x8C)!=150.f||read<float>(p+0x90)!=5.f||
     read<u8>(p+0x154)!=1||!writable((void*)(p+0x7C),4)||
     !writable((void*)(p+0x8C),8)){
   logId("GUITARS_FREE_CHIRAL_REFUSED: native chiral ammo signature mismatch for variant=",i);
   return;
  }
 }
 for(u32 i=0;i<2;++i){
  u8*p=(u8*)pointers[i];
  at<u32>(p,0x7C)=0;   // EDSAmmoRequiredMaterialType=None
  at<float>(p,0x8C)=0.f; // MaterialCostOfFullCharge
  at<float>(p,0x90)=0.f; // MaterialCostOfRequiredMinCharge
  logId("GUITARS_FREE_CHIRAL_ENABLED ammoId=",expectedIds[i]);
 }
}

// The fabrication menu derives displayed reserve from the static baggage Amount
// minus the current (patched) magazine size. These three baggage definitions are
// only changed at game startup after exact original-resource validation.
// Changing live player inventory/counters would instead interfere with reloads.
void configureWarriorFabricationPreview(const Survey&s){
 if(warriorReserveMode!=1||!warriorReserveLimit)return;
 u64 bag[3]={};
 u32 original[3]={},desired[3]={};
 for(u32 k=0;k<3;++k){
  u64 p=s.warriorBaggage[k];
  u32 magazine=warrior::cap(WarriorNativeMagazines[k],warriorAmmoMultiplier,warriorMagazines[k]);
  u64 content=read<u64>(p+0x50);
  u32 total=warrior::fabricationTotal(magazine,warriorReserveLimit);
  if(!p||typeOf(p)!=BagType||!readable(p,0x88)||!content||typeOf(content)!=ListType||
     read<u32>(p+0x44)!=WarriorBaggageKeys[k]||
     read<u32>(p+0x58)!=WarriorBaggageOriginalAmounts[k]||
     read<u32>(p+0x5C)!=0||!writable((void*)(p+0x58),4)||
     !total||total<=magazine||total>10000){
   logId("WARRIOR_PREVIEW_BLOCKED_RESOURCE index=",k);
   return; // fail closed: no partial changes
  }
  bag[k]=p;original[k]=read<u32>(p+0x58);desired[k]=total;
 }
 for(u32 k=0;k<3;++k){
  at<u32>((void*)bag[k],0x58)=desired[k];
  if(read<u32>(bag[k]+0x58)!=desired[k]){
   for(u32 j=0;j<3;++j)at<u32>((void*)bag[j],0x58)=original[j];
   log("WARRIOR_PREVIEW_ROLLED_BACK: write verification failed.");
   return;
  }
 }
 for(u32 k=0;k<3;++k){
  logId("WARRIOR_PREVIEW_TOTAL_AMMO index=",k);
  logId("WARRIOR_PREVIEW_TOTAL_AMMO value=",desired[k]);
  logId("WARRIOR_PREVIEW_RESERVE_AMMO value=",desired[k]-warrior::cap(WarriorNativeMagazines[k],warriorAmmoMultiplier,warriorMagazines[k]));
 }
}
void registerWeapons(){
 if(_InterlockedCompareExchange(&registration,1,0))return;
 if(read<u64>(image+0x623E540)||read<u64>(image+0x623FA50)){log("REGISTRATION_BLOCKED: native managers already exist.");_InterlockedExchange(&registration,3);return;}
 u64 game=read<u64>(image+0x623E310),items=read<u64>(game+0x280),catalogue=read<u64>(game+0x270);
 if(!game||!items||!catalogue||typeOf(items)!=DepotType||typeOf(catalogue)!=DepotType){log("REGISTRATION_BLOCKED: source depots missing.");_InterlockedExchange(&registration,3);return;}
 static Survey survey; // Large traversal storage stays off the initialization thread stack.
 if(!scan(items,survey,0)||!scan(catalogue,survey,0)||survey.collision){log("REGISTRATION_BLOCKED: ambiguous resource graph or occupied custom identity.");_InterlockedExchange(&registration,3);return;}
 Bundle*pending[Count]={};Pins pins;bool ok=true;u32 extra=0;bool german=languageSetting==1;
 for(u32 k=0;k<Count&&ok;++k){
  Sources s={};if(!sources(survey,k,s,pins)){logId("SOURCE_VALIDATION_FAILED variant=",k);ok=false;break;}
  if(k==0){char title[16]={};readmem(read<u64>(u64(s.name)+0x20),title,15);german=germanLanguage(languageSetting,title);}
  pending[k]=(Bundle*)VirtualAlloc(nullptr,sizeof(Bundle),0x3000,4);
  if(!pending[k]||!silenced::prepare(*pending[k],k,s,recipeEnabled[k],german)){logId("CLONE_VALIDATION_FAILED variant=",k);ok=false;break;}
  if(k==GuitarVariant){
   if(recipeEnabled[k]){at<u64>(pending[k]->list,0x50)=u64(customIconNames[2].data);log("RED_GUITAR_MENU_ICON_READY: independent red single-neck rail-guitar icon assigned.");}
   u64 red=survey.redModelWeapon;
   const u8 redUuid[16]={0x36,0x53,0xAE,0x94,0xC8,0x8F,0x3B,0x30,0xA7,0x63,0x55,0xD6,0x65,0x05,0xA8,0x0E};
   if(!red||typeOf(red)!=WeaponType||!readable(red,0x300)||memcmp((const void*)(red+0x264),redUuid,16)){
    log("RED_GUITAR_SOURCE_UNAVAILABLE: Higgs double-neck source not registered; no substitute model used.");
    ok=false;break;
   }
   silenced::copy(pending[k]->weapon+0x264,redUuid,16);
   log("RED_GUITAR_RESOURCE_PROTOTYPE: cloned Sam's native guitar game behavior, bound Higgs red 3D entity.");
  }
  if(blackRed&&k<2){
   u64 donor=survey.icons[k];char texture[64]={};
   bool valid=readable(donor,0xC0)&&readmem(read<u64>(donor+0x50),texture,sizeof(texture));
   u32 n=0;while(IconDonors[k].texture[n])++n;
   if(valid&&!memcmp(texture,IconDonors[k].texture,n+1)&&pins.add(donor)&&menuIcon(*pending[k],k,(void*)donor))
    logId("RED_MENU_ICON_READY weapon=",Definitions[k].id);
   else logId("RED_MENU_ICON_UNAVAILABLE: keeping original thumbnail; weapon=",Definitions[k].id);
  }
  if(blackRed&&k>=2&&k<ActiveCount&&assignPrivateIcon(*pending[k],k,customIconNames[k-2].data))logId("PRIVATE_MENU_NAME_READY weapon=",Definitions[k].id);
  extra+=1+Definitions[k].ammoCount;
 }
 Plan ip,cp;
 if(ok)ok=plan(ip,items,extra+(items==catalogue?((Count*2+ActiveCount+1)):0))&&(items==catalogue||plan(cp,catalogue,((Count*2+ActiveCount+1))));
 if(ok)ok=unchanged(ip)&&(items==catalogue||unchanged(cp));
 if(!ok){discard(ip);discard(cp);for(auto*p:pending)if(p)VirtualFree(p,0,0x8000);log("REGISTRATION_BLOCKED: no new definitions published.");_InterlockedExchange(&registration,3);return;}
 // All allocations and checks precede publishing either table. The native
 // startup thread owns both lists here; no worker writes loaded definitions.
 for(u32 i=0;i<pins.n;++i)_InterlockedIncrement((long*)(pins.p[i]+8));
 publish(ip);if(items!=catalogue)publish(cp);
 for(u32 k=0;k<Count;++k){auto*b=pending[k];bundles[k]=b;append(items,b->weapon);
  for(u32 j=0;j<Definitions[k].ammoCount;++j)append(items,b->ammo[j]);
  append(catalogue,b->list);append(catalogue,b->bag);if(craftable(k))append(catalogue,b->recipe);
  logId(craftable(k)?"WEAPON_REGISTERED id=":"LEGACY_WEAPON_REGISTERED id=",Definitions[k].id);
 }
 configureWarriorAmmo(survey);
 configureWarriorFabricationPreview(survey);
 configureGuitarChiral(survey,pending[GuitarVariant]);
 log("RESOURCES_READY: four craftable weapons plus hidden legacy weapon 302, thirteen ammunition definitions, four recipes; native primary-fire suppressor path. In-game validation pending.");
 _InterlockedExchange(&registration,2);
}
void*catalogueConstructor(void*p){registerWeapons();return addon_host::catalogue(p,original);}
u64 componentResource(u64 entity,u32 type){
 u32 n=read<u32>(entity+0x48);u64 a=read<u64>(entity+0x50),found=0;
 if(!a||n>64)return 0;
 for(u32 i=0;i<n;++i){u64 p=read<u64>(a+8*i);if(typeOf(p)==type){if(found)return 0;found=p;}}
 return found;
}
bool mainMeshReady(u64 entity){
 u64 model=read<u64>(entity+0xD8),resource=read<u64>(model+0x30);
 i32 count=read<i32>(resource+0x88);u64 names=read<u64>(resource+0x90),parts=read<u64>(model+0x58);
 if(!model||!resource||!names||!parts||count<1||count>128)return false;
 for(i32 i=0;i<count;++i)if(read<u32>(names+4*i)==code("MESH_Main")){
  u64 p=parts+48*i,owner=read<u64>(p+8),rep=read<u64>(owner+0xC8);
  return read<i32>(p+0x28)>=0&&read<u64>(read<u64>(rep+0x40)+0x28)!=0;
 }
 return false;
}
bool buildWeaponAttachment(u64 entity,u32 variant){
 auto&state=attachments[variant];
 // The original lookup is called only here, after a player-owned weapon exists in
 // the world. No replacement model hook or startup model-resource mutation.
 u64 rifle=((u64(*)(u64))(image+0x1FC8620))(u64(bundles[0]->weapon));
 if(typeOf(rifle)!=0x44C02E0)return false;
 u64 skinned=componentResource(rifle,0x41EFC50),data=componentResource(rifle,0x4205C00);
 u64 targetData=read<u64>(read<u64>(entity+0xD8)+0x30);
 u64 model=read<u64>(data+0x20),targetModel=read<u64>(targetData+0x20);
 if(typeOf(data)!=0x4205C00||typeOf(targetData)!=0x4205C00||typeOf(model)!=0x4206D70||
    typeOf(targetModel)!=0x4206D70||!readable(skinned,0xC8)||!readable(data,0xB8)||!readable(model,0xA0))return false;
 u32 n=read<u32>(model+0x20);u64 a=read<u64>(model+0x28),part=0;
 if(!a||!n||n>128)return false;
 for(u32 i=0;i<n;++i){u64 p=read<u64>(a+8*i);if(typeOf(p)==0x41E6CC0&&read<u32>(p+0xD0)==SuppressorHash){if(part)return false;part=p;}}
 if(!readable(part,0xE0))return false;
 u64 helpers=read<u64>(targetModel+0x30),helper=0;
 n=read<u32>(helpers+0x30);a=read<u64>(helpers+0x38);if(!a||!n||n>128)return false;
 for(u32 i=0;i<n;++i){char name[16]={};u64 p=a+112*i;
  if(readmem(read<u64>(p+96),name,16)&&!memcmp(name,"HLP_MuzzleFlash",16)){if(helper)return false;helper=p+16;}}
 if(!readable(helper,80))return false;
 Pins pins;const u64 donors[]={rifle,skinned,data,model,part,targetData,targetModel,helpers};
 for(u64 p:donors)if(!pins.add(p))return false;
 auto*g=(AttachmentGraph*)VirtualAlloc(nullptr,sizeof(AttachmentGraph),0x3000,4);if(!g)return false;
 // RTTI wrapper takes the destination in RDX. The other two calls are native
 // C++ constructors (RCX). Fresh EntityResource caches must start empty.
 ((void(*)(u64,void*))(image+0x2FEBC0))(image+0x4200CB0,g->child);
 ((void*(*)(void*))(image+0x181A00))(g->entity);
 ((void*(*)(void*))(image+0x28B4B0))(g->mover);
 if(!attachmentGraph(*g,(void*)skinned,(void*)data,(void*)model,(void*)part,(void*)helper,variant)){
  VirtualFree(g,0,0x8000);return false;
 }
 for(u32 i=0;i<pins.n;++i)_InterlockedIncrement((long*)(pins.p[i]+8));
 logId("ATTACHMENT_GRAPH: native constructors completed; weapon=",Definitions[variant].id);
 ((void(*)(void*,u64))(image+0x169930))(g->entity,0);
 state.graph=g;
 logId("ATTACHMENT_READY: isolated suppressor mesh at animated muzzle; visual fit requires confirmation; weapon=",Definitions[variant].id);
 return true;
}
void attachSuppressor(u64 entity,u32 variant){
 if(variant<1||variant>=ActiveCount||!visibleSuppressor[variant])return;
 auto&state=attachments[variant];
 u64 parent=read<u64>(entity+0x80),flags=read<u64>(entity+0x98);
 // Parent must be the player. Preview entities and pre-world instances cannot
 // create attachments, even if their equipment parameter is our weapon.
 if(!parent||!attachmentEligible(i32(variant),flags,typeOf(parent),mainMeshReady(entity),false))return;
 if(_InterlockedCompareExchange(&state.busy,1,0))return;
 using Lookup=u64(*)(u64,u64);
 u64 existing=((Lookup)(image+0x11FFA0))(entity+0xA0,image+0x4200EC0);
 if(!existing&&!state.failed){
  u64 now=GetTickCount64();
  if(!state.graph&&now>=state.retry){
   state.retry=now+1000;
   if(!buildWeaponAttachment(entity,variant)&&!_InterlockedCompareExchange(&state.waiting,1,0))
    logId("ATTACHMENT_WAITING: donor graph not ready; weapon=",Definitions[variant].id);
  }
  if(state.graph){
   alignas(16) u64 uuid[2]={};((void*(*)(void*))(image+0x2070190))(uuid);
   logId("ATTACHMENT_CREATE: requesting native child component; weapon=",Definitions[variant].id);
   u64 component=((u64(*)(u64,void*,const u64*))(image+0x1484A0))(entity,state.graph->child,uuid);
   if(component&&read<u64>(component+0x30)==u64(state.graph->child))
    logId("ATTACHMENT_COMPONENT: waiting for child entity; weapon=",Definitions[variant].id);
   else {state.failed=true;logId("ATTACHMENT_REFUSED: unexpected creation result; weapon=",Definitions[variant].id);}
  }
 }else if(state.graph&&read<u64>(existing+0x30)==u64(state.graph->child)){
  u64 weak=read<u64>(existing+0x50),child=weak>=0x10000?weak-0x20:0;
  if(child&&read<u64>(child+0xC8)&&!_InterlockedCompareExchange(&state.logged,1,0))
   logId("ATTACHMENT_CHILD: child and renderer exist; visual confirmation pending; weapon=",Definitions[variant].id);
 }
 _InterlockedExchange(&state.busy,0);
}
bool marked(u64 model){
 u64 controller=read<u64>(model+0xA0);if(!controller)return false;
 i32 count=read<i32>(controller+0x50);u64 entries=read<u64>(controller+0x58);
 if(count<1||count>256||!entries)return false;
 u8 data[40];
 for(i32 i=0;i<count;++i)if(readmem(entries+40*i,data,40)&&materialMarked(data,1))return true;
 return false;
}
u64 materialResource(u32 variant){
 const auto*d=materialDonor(variant);if(!d)return 0;
 auto&cache=materialCache[variant];long state=_InterlockedCompareExchange(&cache.state,1,0);
 if(state)return state==2?cache.resource:0;
 // Same locked UUID lookup used by DSWeaponParameter::GetEntityResource.
 using Lookup=u64(*)(u64,const u64*);
 u64 variation=((Lookup)(image+0x26DB800))(0,d->uuid);
 if(!variation||typeOf(variation)!=0x42067F0||read<u32>(variation+0x40)!=d->replacements||!writable((void*)(variation+8),4)){
  _InterlockedExchange(&cache.state,0);return 0;
 }
 // Keep the donor and its texture reference graph alive after scene changes.
 _InterlockedIncrement((long*)(variation+8));cache.resource=variation;_InterlockedExchange(&cache.state,2);
 return variation;
}
bool applyMechMaterials(u64 entity,u64 model,u32 variant){
 const auto*d=materialDonor(variant);u64 variation=materialResource(variant);if(!d||!variation)return false;
 u64 swaps=read<u64>(variation+0x48),renderer=read<u64>(entity+0xC8);
 u64 renderOwner=read<u64>(read<u64>(renderer+0x40)+0x28);
 u64 resource=read<u64>(model+0x30),names=read<u64>(resource+0x90),parts=read<u64>(model+0x58);
 i32 count=read<i32>(resource+0x88);if(!renderOwner||!swaps||!parts||!names||count<1||count>128)return false;
 // PostUpdate retries naturally until the main mesh has a render instance.
 bool ready=false;
 for(i32 i=0;i<count;++i)if(read<u32>(names+4*i)==code("MESH_Main")){
  u64 p=parts+48*i,owner=read<u64>(p+8),rep=read<u64>(owner+0xC8);
  ready=read<i32>(p+0x28)>=0&&read<u64>(read<u64>(rep+0x40)+0x28)!=0;
 }
 if(!ready)return false;
 u64 targets[7]={};
 for(u32 i=0;i<d->replacements;++i){
  u64 p=read<u64>(swaps+8*i),texture=read<u64>(p+0x28);u32 n=read<u32>(p+0x30);
  if(!p||typeOf(p)!=0x4206E20||!texture||typeOf(texture)!=0x5E1F410||
     !read<u64>(p+0x20)||n<1||n>16||!read<u64>(p+0x38))return false;
  targets[i]=p;
 }
 using Replace=u64(*)(u64,u64,u64,u64,u64);
 auto replace=(Replace)(image+0x349AB0);
 for(u32 i=0;i<d->replacements;++i){u64 p=targets[i];
  replace(renderOwner,model,p+0x30,p+0x20,read<u64>(p+0x28));
 }
 // An unused, instance-local shader variable records successful submission.
 u32 variable=MaterialMarker,mesh=0;float value=1.f;
 ((void(*)(u64,const u32*,const float*,u32,const u32*))(image+0x33BD40))(model,&variable,&value,1,&mesh);
 return marked(model);
}
PistolGraph*pistol=nullptr;volatile long pistolBusy=0;bool pistolFailed=false;u64 pistolRetry=0;
// Read only a bounded subset of the donor graph, checking every native type.
u64 findPistolSet(u64 p,u32 depth,u32&budget){
 if(!p||depth>12||!budget)return 0;--budget;u64 t=typeOf(p);
 if(t==0x5E1F410)return read<u64>(p+16)==PistolSetId[0]&&read<u64>(p+24)==PistolSetId[1]?p:0;
 u32 offset=0,stride=8,limit=128;bool direct=false;
 if(t==0x41E6CC0||t==0x5E1E750){offset=0x20;direct=true;}
 else if(t==0x4509C70){offset=0x68;stride=32;limit=16;}
 else if(t==0x4509BC0)offset=0x60;
 else if(t==0x6056430)offset=0xE8;
 else if(t==0x5F9D440){
  u32 sets=read<u32>(p+0x20);u64 table=read<u64>(p+0x28);if(sets>16)return 0;
  for(u32 s=0;s<sets;++s){u64 ts=table+40*s;u32 n=read<u32>(ts);u64 techs=read<u64>(ts+8);if(n>32)return 0;
   for(u32 i=0;i<n;++i){u64 tech=techs+160*i;u32 count=read<u32>(tech+16);u64 bindings=read<u64>(tech+24);if(count>128)return 0;
    for(u32 j=0;j<count;++j){u64 result=findPistolSet(read<u64>(bindings+40*j+16),depth+1,budget);if(result)return result;}
   }
  }return 0;
 }else return 0;
 if(direct)return findPistolSet(read<u64>(p+offset),depth+1,budget);
 u32 n=read<u32>(p+offset);u64 values=read<u64>(p+offset+8);if(n>limit)return 0;
 for(u32 i=0;i<n;++i){u64 result=findPistolSet(read<u64>(values+stride*i),depth+1,budget);if(result)return result;}
 return 0;
}
bool preparePistol(u64 model){
 if(pistol)return true;
 u64 art=read<u64>(read<u64>(model+0x30)+0x20);
 if(typeOf(art)!=0x4206D70)return false;
 u32 n=read<u32>(art+0x20);u64 parts=read<u64>(art+0x28);if(!n||n>128)return false;
 u64 donor=0;u32 targetCount=0,targets[16]={},budget=8192;
 for(u32 i=0;i<n;++i){u64 part=read<u64>(parts+8*i),found=findPistolSet(part,0,budget);
  if(found){if((donor&&donor!=found)||targetCount==16)return false;donor=found;targets[targetCount++]=read<u32>(part+0xD0);}
 }
 if(!donor||!targetCount)return false;
 u64 entries=read<u64>(donor+0x28),color=read<u64>(entries+32),gpu=read<u64>(color+0x20);
 if(!readable(donor,0x70)||!readable(entries,72)||typeOf(color)!=0x5E1F780||
    !readable(color,0x70)||!readable(gpu,0x188)||read<u64>(gpu)!=image+0x33E5D48)return false;
 Pins pins;if(!pins.add(donor))return false;
 u64 device=read<u64>(image+0x63E0E00),resource=0,descriptor=0;if(!device)return false;
 int status=PistolGpuPoll((void*)device,&resource,&descriptor);
 if(status<0){pistolFailed=true;logId("BIGBORE_GPU_REFUSED HRESULT=",PistolGpuError());return false;}if(!status)return false;
 auto*g=(PistolGraph*)VirtualAlloc(nullptr,sizeof(PistolGraph),0x3000,4);if(!g)return false;
 // VirtualAlloc does not run C++ field initializers.
 g->stringHash=code("TextureSetBindings");g->stringLength=18;g->stringRefs=1;copy(g->stringData,"TextureSetBindings",19);
 ((void(*)(u64,void*))(image+0x24E5B20))(0,g->color);
 ((void*(*)(void*))(image+0x2112EE0))(g->gpu);
 if(!pistolGraph(*g,(void*)donor,(void*)entries,(void*)color,(void*)gpu,resource,descriptor)){
  VirtualFree(g,0,0x8000);pistolFailed=true;log("BIGBORE_GRAPH_REFUSED: unexpected native texture layout.");return false;
 }
 g->targetCount=targetCount;g->targetCapacity=16;copy(g->targets,targets,4*targetCount);
 u64 manager=read<u64>(image+0x623FBC8);if(!manager){VirtualFree(g,0,0x8000);return false;}
 // Registration is published atomically under the same SRW lock as the native
 // descriptor copier. Refuse exhaustion before the native unchecked allocator.
 void*lock=(void*)(manager+0xBAA398);AcquireSRWLockExclusive(lock);
 u64 pool=manager+0xAAA378;u32 head=read<u32>(pool),capacity=read<u32>(pool+0x14);
 bool valid=head<capacity&&capacity<=262144;
 if(valid){i32 index=((i32(*)(u64))(image+0x2071DF0))(pool);valid=index>0&&u32(index)<capacity;
  if(valid){at<u32>(g->gpu,0x10)=(u32(index)<<2)|2;at<u64>((void*)manager,0x86A378+u32(index)*8)=u64(g->gpu);at<u8>((void*)manager,0xA6A378+u32(index))=0;}
 }
 ReleaseSRWLockExclusive(lock);
 if(!valid){pistolFailed=true;log("BIGBORE_REGISTRY_REFUSED: no valid native descriptor slot.");return false;}
 _InterlockedIncrement((long*)(donor+8));pistol=g;
 logId("BIGBORE_RED_READY: private resident BC1 texture, RBB/A view; targeted meshes=",targetCount);return true;
}
bool applyPistol(u64 entity,u64 model){
 if(pistolFailed||!mainMeshReady(entity)||_InterlockedCompareExchange(&pistolBusy,1,0))return false;
 bool ok=false;u64 now=GetTickCount64();
 if(pistol||now>=pistolRetry){pistolRetry=now+1000;
  if(preparePistol(model)){
   u64 owner=read<u64>(read<u64>(read<u64>(entity+0xC8)+0x40)+0x28);
   if(owner){((u64(*)(u64,u64,const void*,const void*,u64))(image+0x349AB0))(owner,model,&pistol->targetCount,&pistol->binding,u64(pistol->set));
    u32 variable=MaterialMarker,mesh=0;float value=1.f;
    ((void(*)(u64,const u32*,const float*,u32,const u32*))(image+0x33BD40))(model,&variable,&value,1,&mesh);ok=marked(model);
   }
  }
 }
 _InterlockedExchange(&pistolBusy,0);return ok;
}
struct MenuIconState {PrivateIconGraph*graph=nullptr;volatile long busy=0;bool failed=false;u64 retry=0;};
MenuIconState menuIcons[PrivateIconCount];
bool prepareMenuIcon(u64 source,u32 slot){
 auto&state=menuIcons[slot];
 if(state.graph)return true;
 u64 tex=read<u64>(source+0x30),gpu=read<u64>(tex+0x20);
 if(typeOf(source)!=0x6065810||typeOf(tex)!=0x5E1F780||!readable(source,0x38)||!readable(gpu,0x188)||
    read<u64>(gpu)!=image+0x33E5D48)return false;
 Pins pins;if(!pins.add(source))return false;
 u64 device=read<u64>(image+0x63E0E00),resource=0,descriptor=0;if(!device)return false;
 // The catalogue's first request occurs on its native UI thread, after device
 // initialization. Upload uses our own queue; the wait never depends on DS2's
 // UI/render thread and is bounded. No GPU work occurs in the startup hook.
 int status=PrivateIconGpuPoll(slot,(void*)device,&resource,&descriptor);
 for(u32 i=0;i<50&&status==0;++i){Sleep(1);status=PrivateIconGpuPoll(slot,(void*)device,&resource,&descriptor);}
 if(status<0){state.failed=true;logId("PRIVATE_MENU_GPU_REFUSED HRESULT=",PrivateIconGpuError(slot));return false;}
 if(!status)return false;
 auto*g=(PrivateIconGraph*)VirtualAlloc(nullptr,sizeof(PrivateIconGraph),0x3000,4);if(!g)return false;
 ((void(*)(u64,void*))(image+0x270D4D0))(0,g->ui);
 ((void(*)(u64,void*))(image+0x24E5B20))(0,g->texture);
 ((void*(*)(void*))(image+0x2112EE0))(g->gpu);
 if(!privateIconGraph(*g,slot,(void*)source,(void*)gpu,resource,descriptor)){
  VirtualFree(g,0,0x8000);state.failed=true;log("PRIVATE_MENU_GRAPH_REFUSED: unexpected icon format or identity.");return false;
 }
 u64 manager=read<u64>(image+0x623FBC8);if(!manager){VirtualFree(g,0,0x8000);return false;}
 void*lock=(void*)(manager+0xBAA398);AcquireSRWLockExclusive(lock);
 u64 pool=manager+0xAAA378;u32 head=read<u32>(pool),capacity=read<u32>(pool+0x14);
 bool valid=head<capacity&&capacity<=262144;
 if(valid){i32 index=((i32(*)(u64))(image+0x2071DF0))(pool);valid=index>0&&u32(index)<capacity;
  if(valid){at<u32>(g->gpu,0x10)=(u32(index)<<2)|2;at<u64>((void*)manager,0x86A378+u32(index)*8)=u64(g->gpu);at<u8>((void*)manager,0xA6A378+u32(index))=0;}
 }
 ReleaseSRWLockExclusive(lock);
 if(!valid){VirtualFree(g,0,0x8000);state.failed=true;log("PRIVATE_MENU_REGISTRY_REFUSED: no native descriptor slot.");return false;}
 _InterlockedIncrement((long*)(source+8));state.graph=g;
 logId("PRIVATE_MENU_RED_READY: 512x320 BC7 private texture uploaded; weapon=",slot<2?Definitions[slot+2].id:Definitions[GuitarVariant].id);return true;
}
u64 catalogueIcon(u64 catalogue,const char*const*name);
u64 catalogueIconDispatch(u64 catalogue,const char*const*name){return addon_host::icon(catalogue,name,&catalogueIcon);}
u64 catalogueIcon(u64 catalogue,const char*const*name){
 // Only our exact private names are redirected. All original icons use the
 // native lookup, including the retired yellow machine pistol.
 u64 string=read<u64>(u64(name));u32 length=read<u32>(string-8),hash=read<u32>(string-12);char data[64]={};
 if(!(blackRed||recipeEnabled[GuitarVariant])||length>=sizeof(data)||!readmem(string,data,length+1))return originalIcon(catalogue,name);
 i32 slot=customMenuIcon(data,length,hash);if(slot<0)return originalIcon(catalogue,name);
 auto&state=menuIcons[slot];const char*sourceName=sourceIconNames[slot].data;u64 fallback=originalIcon(catalogue,&sourceName);
 if(_InterlockedCompareExchange(&registration,0,0)!=2||typeOf(catalogue)!=0x4408340||!fallback||state.failed)return fallback;
 if(_InterlockedCompareExchange(&state.busy,1,0))return fallback;
 u64 now=GetTickCount64();
 if(!state.graph&&now>=state.retry){state.retry=now+1000;prepareMenuIcon(fallback,u32(slot));}
 u64 result=state.graph?u64(state.graph->ui):fallback;_InterlockedExchange(&state.busy,0);return result;
}
u64 weaponPostUpdate(u64 entity,u64 message){
 u64 result=originalPaint(entity,message);
 if(_InterlockedCompareExchange(&registration,0,0)!=2)return result;
 i32 k=materialVariant(bundles,read<u64>(entity+0x2180));if(k<0)return result;
 if(k>=1)attachSuppressor(entity,u32(k));
 if(!blackRed)return result;
 if(k==3){u64 model=read<u64>(entity+0xD8);
  if(model&&!marked(model)){
   if(applyPistol(entity,model)){
    if(!_InterlockedCompareExchange(&paintLogged[k],1,0))log("BIGBORE_RED_SUBMITTED: private pistol material bound; visual confirmation required.");
   }else if(!_InterlockedCompareExchange(&materialMissingLogged[k],1,0))log("BIGBORE_RED_WAITING: model, material or independent GPU upload not yet ready.");
  }
  return result;
 }
 if(!materialDonor(u32(k))){
  if(!_InterlockedCompareExchange(&materialMissingLogged[k],1,0))logId("MATERIAL_NOT_IMPLEMENTED: no matching native red texture donor; weapon=",Definitions[k].id);
  return result;
 }
 u64 model=read<u64>(entity+0xD8);
 if(!model||!readable(model,0xA8)||marked(model))return result;
 if(applyMechMaterials(entity,model,u32(k))){
  if(!_InterlockedCompareExchange(&paintLogged[k],1,0))logId("MECH_MATERIALS_SUBMITTED: native instance textures; visual confirmation required; weapon=",Definitions[k].id);
 }else if(!_InterlockedCompareExchange(&materialMissingLogged[k],1,0))logId("MATERIAL_WAITING: render model or native donor not ready; weapon=",Definitions[k].id);
 return result;
}
bool wantsWarriorStockCap(){return warriorReserveMode==1&&warriorReserveLimit>0;}
i32 warriorAmmoKind(u64 owner){
 if(owner<0x10000||!readable(owner+0x628,4))return -1;
 u64 slot=read<u64>(owner+0x5B0);
 u64 param=slot>=0x10000?read<u64>(slot+8):0;
 if(!param||!readable(param,0x160)||typeOf(param)!=AmmoType)return -1;
 u16 id=read<u16>(param+0x20);
 for(u32 k=0;k<3;++k)if(id==WarriorAmmoIds[k]&&
   read<u32>(param+0xD8)==warrior::cap(WarriorNativeMagazines[k],warriorAmmoMultiplier,warriorMagazines[k]))return i32(k);
 return -1;
}
// Native DS2 stores ammo-loader categories in a vector-like owner table.
 // Original 0x202A990 disassembly: category = owner[0x6A + 40*selected],
 // count=*(i32*)(table+0x200), cells at table+8*i, key=cell[0], rounds=cell[4].
 // Player tables contain MANY categories; count==1 matched NPC-only tables!
constexpr u8 WarriorNativeTableCategories[3]={1,5,13};
bool warriorNativeTable(u64 owner,u32 which,u64&entry,u32&nativeStock){
 entry=0;nativeStock=0;
 if(owner<0x10000||which>=3||!readable(owner+0x630,4))return false;
 i32 selected=read<i32>(owner+0x2F0);
 if(selected<0||selected>=8||!readable(owner+0x6A+40*u32(selected),1))return false;
 u8 requested=read<u8>(owner+0x6A+40*u32(selected));
 if(requested!=WarriorNativeTableCategories[which]){
  if(_InterlockedIncrement(&warriorInvalidCategoryLogged[which])<=2){
   logId("WARRIOR_NATIVE_CATEGORY_MISMATCH weapon=",which);
   logId("WARRIOR_NATIVE_CATEGORY_ACTUAL=",u32(requested));
  }
  return false;
 }
 u64 base=read<u64>(owner+0x300);
 if(base<0x10000||!readable(base+0x200,4))return false;
 i32 count=read<i32>(base+0x200);
 if(count<1||count>64||!readable(base,8*u32(count)))return false;
 i32 i=warrior::findNativeCategoryIndex((const unsigned char*)base,u32(count),requested);
 if(i<0)return false;
 u64 cell=base+8*u32(i);
 if(!writable((void*)(cell+4),4))return false;
 nativeStock=read<u32>(cell+4);
 if(nativeStock>100000u)return false;
 entry=cell;
 return true;
}
// The engine reinitializes ammo for player equipment after the first inventory
// sync. Granting only once to the table, without retaining its remaining rounds,
// lets a later native initialization silently restore the vanilla value.
static constexpr u32 VanillaWarriorReserve[3]={70,4,6};
WarriorNativeStock* warriorFindStock(u32 k,u64 entry){
 if(k>=3||entry<0x10000)return nullptr;
 auto&cache=warriorStockCache[k];
 for(u32 i=0;i<cache.count;++i)if(cache.items[i].entry==entry)return &cache.items[i];
 return nullptr;
}
WarriorNativeStock* warriorEnsureStock(u32 k,u64 entry,u32 native){
 if(k>=3)return nullptr;
 if(auto*prev=warriorFindStock(k,entry))return prev;
 auto&cache=warriorStockCache[k];
 if(cache.count>=384)return nullptr;
 u32 magazine=warrior::cap(WarriorNativeMagazines[k],warriorAmmoMultiplier,warriorMagazines[k]);
 if(magazine<1||magazine>300||native>100000u)return nullptr;
 // If a newly crafted Warrior gun already carries the full base reserve from
 // its baggage Amount, do not add it again. Older crafted guns still receive
 // the base minimum plus extra ammunition earned through backpack loaders.
 u32 desired=warrior::reconciledNativeReserve(native,magazine,warriorReserveLimit,VanillaWarriorReserve[k]);
 if(desired>100000u)return nullptr;
 auto&item=cache.items[cache.count++];
 item.entry=entry;item.rounds=desired;item.initialNative=native;
 if(_InterlockedIncrement(&warriorGrantCount[k])<=14){
  logId("WARRIOR_V195_GRANTED_KIND=",k);
  logId("WARRIOR_V195_NATIVE_BEFORE=",native);
  logId("WARRIOR_V195_STOCK_TARGET=",desired);
 }
 return &item;
}
void warriorRestoreCurrent(u64 owner,const char*source){
 if(!wantsWarriorStockCap())return;
 i32 kind=warriorAmmoKind(owner);
 if(kind<0||!writable((void*)(owner+0x628),4))return;
 u64 entry=0;u32 native=0;
 if(!warriorNativeTable(owner,u32(kind),entry,native))return;
 if(_InterlockedCompareExchange(&warriorLocks[kind],1,0)!=0)return;
 auto*ledger=warriorEnsureStock(u32(kind),entry,native);
 if(ledger&&writable((void*)(entry+4),4)){
  u32 actual=read<u32>(owner+0x628);
  if(actual!=ledger->rounds||native!=ledger->rounds){
   at<u32>((void*)owner,0x628)=ledger->rounds;
   at<u32>((void*)entry,4)=ledger->rounds;
   if(_InterlockedIncrement(&warriorRestoreCount[kind])<=18){
    logId("WARRIOR_V195_RESTORED_KIND=",u32(kind));
    log(source);
    logId("WARRIOR_V195_RESTORE_FROM=",actual);
    logId("WARRIOR_V195_RESTORE_TO=",ledger->rounds);
   }
  }
 }
 _InterlockedExchange(&warriorLocks[kind],0);
}
void warriorInitWithBaseReserve(u64 owner){
 if(!originalWarriorInit)return;
 originalWarriorInit(owner);
 warriorRestoreCurrent(owner,"WARRIOR_RESTORE_AFTER_INIT");
}
void warriorSyncWithoutVirtualReserve(u64 owner){
 if(!originalWarriorSync)return;
 originalWarriorSync(owner);
 warriorRestoreCurrent(owner,"WARRIOR_RESTORE_AFTER_SYNC");
}
void warriorReloadWithFiniteStock(u64 owner){
 if(!originalWarriorReload)return;
 if(!wantsWarriorStockCap()){originalWarriorReload(owner);return;}
 i32 kind=warriorAmmoKind(owner);
 if(kind<0||!writable((void*)(owner+0x628),4)){originalWarriorReload(owner);return;}
 u64 entry=0;u32 native=0;
 if(!warriorNativeTable(owner,u32(kind),entry,native)||!writable((void*)(entry+4),4)){
  originalWarriorReload(owner);return;
 }
 if(_InterlockedCompareExchange(&warriorLocks[kind],1,0)!=0){
  originalWarriorReload(owner);return;
 }
 WarriorNativeStock*ledger=warriorEnsureStock(u32(kind),entry,native);
 if(!ledger){_InterlockedExchange(&warriorLocks[kind],0);originalWarriorReload(owner);return;}
 u32 before=ledger->rounds;
 at<u32>((void*)owner,0x628)=before;
 at<u32>((void*)entry,4)=before;
 // Native game reload has finite magazine/stock semantics.
 originalWarriorReload(owner);
 u32 after=read<u32>(owner+0x628);
 if(after<=before){
  ledger->rounds=after;
  at<u32>((void*)entry,4)=after;
  if(_InterlockedIncrement(&warriorReloadCount[kind])<=20){
   logId("WARRIOR_V195_RELOAD_KIND=",u32(kind));
   logId("WARRIOR_V195_RELOAD_SPENT=",before-after);
   logId("WARRIOR_V195_RELOAD_RESERVE_LEFT=",after);
  }
 }else{
  // Native game must not silently mint ammo on a reload.
  at<u32>((void*)owner,0x628)=before;
  at<u32>((void*)entry,4)=before;
  if(_InterlockedIncrement(&warriorReloadCount[kind])<=8)logId("WARRIOR_V195_RELOAD_UPWARD_REJECTED=",u32(kind));
 }
 _InterlockedExchange(&warriorLocks[kind],0);
}
void absolute(u8*p,u64 target){const u8 bytes[]={0xFF,0x25,0,0,0,0};copy(p,bytes,6);at<u64>(p,6)=target;}
u8 replacement[5],paintReplacement[5],iconReplacement[5],warriorReloadReplacement[7],warriorInitReplacement[6],warriorSyncReplacement[8];
bool prepareHook(){
 u8 bytes[5];if(!readmem(image+Site,bytes,5)||memcmp(bytes,Expected,5))return false;
 if(visualHook()&&(!readmem(image+PaintSite,bytes,5)||memcmp(bytes,PaintExpected,5)))return false;
 if((blackRed||recipeEnabled[GuitarVariant]||addonIconRequested)&&(!readmem(image+IconSite,bytes,5)||memcmp(bytes,IconExpected,5)))return false;
 if(wantsWarriorStockCap()){u8 before[8];
 if(!readmem(image+WarriorReloadSite,before,7)||memcmp(before,WarriorReloadExpected,7))return false;
 if(!readmem(image+WarriorInitSite,before,6)||memcmp(before,WarriorInitExpected,6))return false;
 if(!readmem(image+WarriorSyncSite,before,8)||memcmp(before,WarriorSyncExpected,8))return false;
 }
 u64 center=(image+Site)&~u64(0xFFFF);
 for(u64 delta=0x10000;delta<0x60000000&&!cave;delta+=0x10000){
  cave=VirtualAlloc((void*)(center+delta),4096,0x3000,4);
  if(!cave&&center>delta+0x10000)cave=VirtualAlloc((void*)(center-delta),4096,0x3000,4);
 }
 if(!cave)return false;
 absolute((u8*)cave,u64(&catalogueConstructor));copy((u8*)cave+64,Expected,5);absolute((u8*)cave+69,image+Site+5);original=(Constructor)((u8*)cave+64);
 long long rel=(long long)u64(cave)-(long long)(image+Site+5);if(rel<(-2147483647LL-1)||rel>2147483647LL)return false;
 replacement[0]=0xE9;at<i32>(replacement,1)=i32(rel);DWORD old;
 if(visualHook()){
  // The original five bytes are a relative JMP; retain its native destination.
  absolute((u8*)cave+256,u64(&weaponPostUpdate));originalPaint=(WeaponPostUpdate)(image+PaintOriginal);
  rel=(long long)(u64(cave)+256)-(long long)(image+PaintSite+5);
  if(rel<(-2147483647LL-1)||rel>2147483647LL)return false;
  paintReplacement[0]=0xE9;at<i32>(paintReplacement,1)=i32(rel);
 }
 if(blackRed||recipeEnabled[GuitarVariant]||addonIconRequested){
  absolute((u8*)cave+512,u64(&catalogueIconDispatch));copy((u8*)cave+576,IconExpected,5);
  absolute((u8*)cave+581,image+IconSite+5);originalIcon=(IconLookup)((u8*)cave+576);
  rel=(long long)(u64(cave)+512)-(long long)(image+IconSite+5);
  if(rel<(-2147483647LL-1)||rel>2147483647LL)return false;
  iconReplacement[0]=0xE9;at<i32>(iconReplacement,1)=i32(rel);
 }
  if(wantsWarriorStockCap()){
   absolute((u8*)cave+640,u64(&warriorReloadWithFiniteStock));
   copy((u8*)cave+704,WarriorReloadExpected,7);
   absolute((u8*)cave+711,image+WarriorReloadSite+7);
   originalWarriorReload=(WarriorReloadFn)((u8*)cave+704);
   rel=(long long)(u64(cave)+640)-(long long)(image+WarriorReloadSite+5);
   if(rel<(-2147483647LL-1)||rel>2147483647LL)return false;
   warriorReloadReplacement[0]=0xE9;at<i32>(warriorReloadReplacement,1)=i32(rel);
   warriorReloadReplacement[5]=0x90;warriorReloadReplacement[6]=0x90;
   absolute((u8*)cave+800,u64(&warriorInitWithBaseReserve));
   copy((u8*)cave+864,WarriorInitExpected,6);
   absolute((u8*)cave+870,image+WarriorInitSite+6);
   originalWarriorInit=(WarriorReloadFn)((u8*)cave+864);
   rel=(long long)(u64(cave)+800)-(long long)(image+WarriorInitSite+5);
   if(rel<(-2147483647LL-1)||rel>2147483647LL)return false;
   warriorInitReplacement[0]=0xE9;at<i32>(warriorInitReplacement,1)=i32(rel);
   warriorInitReplacement[5]=0x90;
   absolute((u8*)cave+992,u64(&warriorSyncWithoutVirtualReserve));
   copy((u8*)cave+1056,WarriorSyncExpected,8);
   absolute((u8*)cave+1064,image+WarriorSyncSite+8);
   originalWarriorSync=(WarriorReloadFn)((u8*)cave+1056);
   rel=(long long)(u64(cave)+992)-(long long)(image+WarriorSyncSite+5);
   if(rel<(-2147483647LL-1)||rel>2147483647LL)return false;
   warriorSyncReplacement[0]=0xE9;at<i32>(warriorSyncReplacement,1)=i32(rel);
   warriorSyncReplacement[5]=warriorSyncReplacement[6]=warriorSyncReplacement[7]=0x90;

  }
  return VirtualProtect(cave,4096,0x20,&old)&&FlushInstructionCache(GetCurrentProcess(),cave,4096);
}
struct Thread {HANDLE h;bool suspended;};Thread threads[512];u32 threadCount=0;
bool closeThreads(){bool ok=true;for(u32 i=0;i<threadCount;++i){auto&t=threads[i];if(t.suspended){bool resumed=false;for(u32 j=0;j<3&&!resumed;++j){if(ResumeThread(t.h)!=0xFFFFFFFF)resumed=true;else{DWORD code=259;if(GetExitCodeThread(t.h,&code)&&code!=259)resumed=true;}}if(!resumed)ok=false;}CloseHandle(t.h);}threadCount=0;return ok;}
bool collectThreads(){
 HANDLE snap=CreateToolhelp32Snapshot(4,0);if(snap==InvalidHandle)return false;
 ThreadEntry e={};e.size=sizeof(e);BOOL more=Thread32First(snap,&e);bool ok=more!=0;const DWORD self=GetCurrentThreadId(),pid=GetCurrentProcessId();
 while(more){if(e.pid==pid&&e.tid!=self){if(threadCount==512){ok=false;break;}HANDLE h=OpenThread(2|8|0x40,0,e.tid);
  if(!h||GetProcessIdOfThread(h)!=pid||GetThreadId(h)!=e.tid){if(h)CloseHandle(h);ok=false;break;}threads[threadCount++]={h,false};}
  e.size=sizeof(e);more=Thread32Next(snap,&e);
 }
 if(ok&&GetLastError()!=18)ok=false;CloseHandle(snap);if(!ok)closeThreads();return ok;
}
int installHook(){
 if(!collectThreads())return 0;bool ok=true;Context c={};c.flags=0x00100001;
 for(u32 i=0;i<threadCount&&ok;++i){auto&t=threads[i];if(SuspendThread(t.h)==0xFFFFFFFF){ok=false;break;}t.suspended=true;
  if(!GetThreadContext(t.h,&c)|| (c.rip>image+Site&&c.rip<image+0xB6CD0A)||
     (visualHook()&&c.rip>=image+PaintSite&&c.rip<image+PaintSite+5)||
     ((blackRed||recipeEnabled[GuitarVariant]||addonIconRequested)&&c.rip>=image+IconSite&&c.rip<image+IconSite+5)||
      (wantsWarriorStockCap()&&c.rip>=image+WarriorReloadSite&&c.rip<image+WarriorReloadSite+7)||
      (wantsWarriorStockCap()&&c.rip>=image+WarriorInitSite&&c.rip<image+WarriorInitSite+6)||
      (wantsWarriorStockCap()&&c.rip>=image+WarriorSyncSite&&c.rip<image+WarriorSyncSite+8))ok=false;
 }
 // Refuse late attachment, including a catalogue/weapon manager created while
 // this worker was hashing the executable. Installation is startup-only.
 if(ok&&(read<u64>(image+0x623E540)||read<u64>(image+0x623FA50)||memcmp((void*)(image+Site),Expected,5)))ok=false;
 if(ok&&visualHook()&&memcmp((void*)(image+PaintSite),PaintExpected,5))ok=false;
 if(ok&&(blackRed||recipeEnabled[GuitarVariant]||addonIconRequested)&&memcmp((void*)(image+IconSite),IconExpected,5))ok=false;
  if(ok&&wantsWarriorStockCap()&&memcmp((void*)(image+WarriorReloadSite),WarriorReloadExpected,7))ok=false;
  if(ok&&wantsWarriorStockCap()&&memcmp((void*)(image+WarriorInitSite),WarriorInitExpected,6))ok=false;
  if(ok&&wantsWarriorStockCap()&&memcmp((void*)(image+WarriorSyncSite),WarriorSyncExpected,8))ok=false;
 DWORD protection=0,paintProtection=0,iconProtection=0,unused=0;bool changed=false,paintChanged=false,iconChanged=false,reloadChanged=false,initChanged=false,syncChanged=false,rollback=true;
 if(ok&&(blackRed||recipeEnabled[GuitarVariant]||addonIconRequested)){ok=VirtualProtect((void*)(image+IconSite),5,0x40,&iconProtection)!=0;if(ok){
  copy((void*)(image+IconSite),iconReplacement,5);iconChanged=true;
  bool flushed=FlushInstructionCache(GetCurrentProcess(),(void*)(image+IconSite),5)!=0;
  bool restored=VirtualProtect((void*)(image+IconSite),5,iconProtection,&unused)!=0;ok=flushed&&restored;
 }}
  DWORD reloadProtection=0;
  if(ok&&wantsWarriorStockCap()){
   ok=VirtualProtect((void*)(image+WarriorReloadSite),7,0x40,&reloadProtection)!=0;
   if(ok){copy((void*)(image+WarriorReloadSite),warriorReloadReplacement,7);reloadChanged=true;
    bool flushed=FlushInstructionCache(GetCurrentProcess(),(void*)(image+WarriorReloadSite),7)!=0;
    bool restored=VirtualProtect((void*)(image+WarriorReloadSite),7,reloadProtection,&unused)!=0;ok=flushed&&restored;
   }
  }
  DWORD initProtection=0,syncProtection=0;
  if(ok&&wantsWarriorStockCap()){
   ok=VirtualProtect((void*)(image+WarriorInitSite),6,0x40,&initProtection)!=0;
   if(ok){copy((void*)(image+WarriorInitSite),warriorInitReplacement,6);initChanged=true;
    bool flushed=FlushInstructionCache(GetCurrentProcess(),(void*)(image+WarriorInitSite),6)!=0;
    bool restored=VirtualProtect((void*)(image+WarriorInitSite),6,initProtection,&unused)!=0;ok=flushed&&restored;
   }
  }
  if(ok&&wantsWarriorStockCap()){
   ok=VirtualProtect((void*)(image+WarriorSyncSite),8,0x40,&syncProtection)!=0;
   if(ok){copy((void*)(image+WarriorSyncSite),warriorSyncReplacement,8);syncChanged=true;
    bool flushed=FlushInstructionCache(GetCurrentProcess(),(void*)(image+WarriorSyncSite),8)!=0;
    bool restored=VirtualProtect((void*)(image+WarriorSyncSite),8,syncProtection,&unused)!=0;ok=flushed&&restored;
   }
  }
  if(ok&&visualHook()){ok=VirtualProtect((void*)(image+PaintSite),5,0x40,&paintProtection)!=0;if(ok){
  copy((void*)(image+PaintSite),paintReplacement,5);paintChanged=true;
  bool flushed=FlushInstructionCache(GetCurrentProcess(),(void*)(image+PaintSite),5)!=0;
  bool restored=VirtualProtect((void*)(image+PaintSite),5,paintProtection,&unused)!=0;ok=flushed&&restored;
 }}
 if(ok){ok=VirtualProtect((void*)(image+Site),5,0x40,&protection)!=0;if(ok){copy((void*)(image+Site),replacement,5);changed=true;
  bool flushed=FlushInstructionCache(GetCurrentProcess(),(void*)(image+Site),5)!=0;
  bool restored=VirtualProtect((void*)(image+Site),5,protection,&unused)!=0;ok=flushed&&restored;}}
  if(!ok&&syncChanged){if(VirtualProtect((void*)(image+WarriorSyncSite),8,0x40,&unused)){
   copy((void*)(image+WarriorSyncSite),WarriorSyncExpected,8);
   bool flushed=FlushInstructionCache(GetCurrentProcess(),(void*)(image+WarriorSyncSite),8)!=0;
   bool restored=VirtualProtect((void*)(image+WarriorSyncSite),8,syncProtection,&unused)!=0;rollback=rollback&&flushed&&restored;
  }else rollback=false;}
  if(!ok&&initChanged){if(VirtualProtect((void*)(image+WarriorInitSite),6,0x40,&unused)){
   copy((void*)(image+WarriorInitSite),WarriorInitExpected,6);
   bool flushed=FlushInstructionCache(GetCurrentProcess(),(void*)(image+WarriorInitSite),6)!=0;
   bool restored=VirtualProtect((void*)(image+WarriorInitSite),6,initProtection,&unused)!=0;rollback=rollback&&flushed&&restored;
  }else rollback=false;}
  if(!ok&&reloadChanged){if(VirtualProtect((void*)(image+WarriorReloadSite),7,0x40,&unused)){
   copy((void*)(image+WarriorReloadSite),WarriorReloadExpected,7);
   bool flushed=FlushInstructionCache(GetCurrentProcess(),(void*)(image+WarriorReloadSite),7)!=0;
   bool restored=VirtualProtect((void*)(image+WarriorReloadSite),7,reloadProtection,&unused)!=0;rollback=flushed&&restored;
  }else rollback=false;}
  if(!ok&&changed){if(VirtualProtect((void*)(image+Site),5,0x40,&unused)){copy((void*)(image+Site),Expected,5);
  bool flushed=FlushInstructionCache(GetCurrentProcess(),(void*)(image+Site),5)!=0;
  bool restored=VirtualProtect((void*)(image+Site),5,protection,&unused)!=0;rollback=flushed&&restored;
 }else rollback=false;}
 if(!ok&&iconChanged){if(VirtualProtect((void*)(image+IconSite),5,0x40,&unused)){
  copy((void*)(image+IconSite),IconExpected,5);
  bool flushed=FlushInstructionCache(GetCurrentProcess(),(void*)(image+IconSite),5)!=0;
  bool restored=VirtualProtect((void*)(image+IconSite),5,iconProtection,&unused)!=0;rollback=rollback&&flushed&&restored;
 }else rollback=false;}
 if(!ok&&paintChanged){if(VirtualProtect((void*)(image+PaintSite),5,0x40,&unused)){
  copy((void*)(image+PaintSite),PaintExpected,5);
  bool flushed=FlushInstructionCache(GetCurrentProcess(),(void*)(image+PaintSite),5)!=0;
  bool restored=VirtualProtect((void*)(image+PaintSite),5,paintProtection,&unused)!=0;rollback=rollback&&flushed&&restored;
 }else rollback=false;}
 bool resumed=closeThreads();if(!rollback||!resumed)return 2;return ok?1:0;
}
}

// The host owns the exact-build gate, pinned module, INI, log and init worker.
// Retain definitions even when recipes are hidden, including legacy weapon 302.
bool InstallSilencedWeapons(unsigned long long gameImage,const SilencedWeaponSettings& settings,void (*logger)(const char*)){
 image=gameImage;logCallback=logger;languageSetting=settings.language;blackRed=settings.blackRed;warriorAmmoMultiplier=settings.warriorAmmoMultiplier;warriorReserveMode=settings.warriorReserveMode;warriorReserveLimit=settings.warriorReserveMagazineLimit;
  for(u32 i=0;i<3;++i)warriorMagazines[i]=settings.warriorMagazineRounds[i];
  freeChiralGuitarShots=settings.freeChiralGuitarShots;
 for(u32 k=0;k<ActiveCount;++k)recipeEnabled[k]=settings.enabled&&settings.recipes[k];
 recipeEnabled[GuitarVariant]=settings.redGuitarEnabled;
 for(u32 k=1;k<ActiveCount;++k)visibleSuppressor[k]=settings.attachments[k-1];
 if(GetModuleHandleW(L"ds2_more_silenced_guns.asi")){
  log("SUPPRESSED_BLOCKED: standalone More Silenced Guns ASI is also loaded. Remove it and restart; one provider is required.");return false;
 }
 if(!prepareHook()){if(cave){VirtualFree(cave,0,0x8000);cave=nullptr;}log("SUPPRESSED_HOOK_BLOCKED: constructor/visual bytes differ or relay allocation failed.");return false;}
 int result=0;for(u32 attempt=0;attempt<20&&!result;++attempt){result=installHook();if(!result)Sleep(50);}
 if(result==2){log("CRITICAL: suppressed weapons rollback/thread resume incomplete; restart game.");return false;}
 if(!result){VirtualFree(cave,0,0x8000);cave=nullptr;log("SUPPRESSED_HOOK_BLOCKED: game already initialized, signature conflict or transaction failed.");return false;}
 log(settings.enabled?"SUPPRESSED_ON: four red suppressed variants enabled; stable IDs 300/301/303/304; hidden legacy 302 retained.":"SUPPRESSED_OFF: fabrication hidden; stable weapon/ammo definitions retained for saved items.");
 if(wantsWarriorStockCap())log("WARRIOR_V195_RESTORE_ON_INIT_SYNC: per-instance finite native ammo ledger restored after game resets; ammo consumed by native reload.");
 if(blackRed||recipeEnabled[GuitarVariant]||addonIconRequested)log("PRIVATE_ICONS_ON: includes independent red Higgs guitar icon; texture upload occurs on menu demand.");
 if(blackRed)log("SUPPRESSED_VISUALS: owned Mech/private red materials and menu images; upload deferred until needed.");
 for(u32 k=1;k<ActiveCount;++k)if(visibleSuppressor[k])logId("ATTACHMENT_ENABLED: waits for player weapon in loaded world; weapon=",Definitions[k].id);
 return true;
}
