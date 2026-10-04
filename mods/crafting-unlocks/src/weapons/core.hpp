#pragma once
namespace craft {
using u8=unsigned char;using u16=unsigned short;using u32=unsigned int;
using i32=int;using u64=unsigned long long;
}
namespace silenced {
using namespace craft;
template<class T>T& at(void*p,u32 off){return *reinterpret_cast<T*>(static_cast<u8*>(p)+off);}
template<class T>const T& at(const void*p,u32 off){return *reinterpret_cast<const T*>(static_cast<const u8*>(p)+off);}
inline void copy(void*d,const void*s,u32 n){auto*a=(u8*)d;auto*b=(const u8*)s;while(n--)*a++=*b++;}
constexpr u32 code(const char*s){u32 n=0;while(*s){n^=u8(*s++);for(u32 i=0;i<8;++i)n=(n>>1)^((n&1)?0x82F63B78u:0);}return n&0x7FFFFFFFu;}
constexpr u32 ActiveCount=4,LegacyVariant=4,Count=5,MaxAmmo=4,MaxTraits=4;
constexpr bool craftable(u32 k){return k<ActiveCount;}
// Resolve automatic language once from the standard assault rifle. Other
// German weapon names do not necessarily begin with their English category.
inline bool germanLanguage(u32 setting,const char*referenceName){
 if(setting)return setting==1;
 if(!referenceName)return false;
 constexpr char prefix[]="Sturm";
 for(u32 i=0;i<sizeof(prefix)-1;++i)if(referenceName[i]!=prefix[i])return false;
 return true;
}
struct Definition {
 u16 source,id;u32 recipe,bag,listId,listCode,newListId,newRecipe,newBag,newList;
 u16 ammo[MaxAmmo],soundDonors[MaxAmmo];u32 ammoCount,primaryBehavior;
 const char*nameEn;const char*nameDe;const char*descriptionEn;const char*descriptionDe;
};
constexpr Definition Definitions[]={
 {18,300,0x7FACC8DC,0x585E2C8F,5,0x3237D11E,900300,
  code("DS2Mod.Silenced.AssaultL2.Recipe.v1"),code("DS2Mod.Silenced.AssaultL2.Baggage.v1"),code("DS2Mod.Silenced.AssaultL2.Weapon.v1"),
  {16,17},{39,40},2,1,"Suppressed Assault Rifle [MP] Lv.2","Schallgedämpftes Sturmgewehr [MZ] St.2",
  "Assault Rifle Lv.2 with suppressed primary fire. Original weapon performance and grenade launcher retained. Impacts and explosions can still alert enemies.",
  "Sturmgewehr St.2 mit schallgedaempftem Hauptfeuer. Waffenleistung und Granatwerfer entsprechen der Vorlage. Einschlaege und Explosionen koennen Gegner alarmieren."},
 {47,301,0x1C6095EF,0x49F9F2BF,353,0x23900F2E,900301,
  code("DS2Mod.Silenced.MachineGunL2.Recipe.v1"),code("DS2Mod.Silenced.MachineGunL2.Baggage.v1"),code("DS2Mod.Silenced.MachineGunL2.Weapon.v1"),
  {122,123,321},{39,40,40},3,1,"Suppressed Machine Gun [MP] Lv.2","Schallgedämpftes Maschinengewehr [MZ] St.2",
  "Machine Gun Lv.2 with suppressed fire and original weapon performance. Uses existing weapon assets and suppressed rifle audio. Bullet impacts can still alert enemies.",
  "Maschinengewehr St.2 mit schallgedaempftem Feuer und urspruenglicher Waffenleistung. Verwendet vorhandene Modelle und gedaempften Gewehrsound. Einschlaege koennen Gegner alarmieren."},
 {35,303,0x22843177,0x0576D524,55,0x6F1F28B5,900303,
  code("DS2Mod.Silenced.ShotgunL2.Recipe.v1"),code("DS2Mod.Silenced.ShotgunL2.Baggage.v1"),code("DS2Mod.Silenced.ShotgunL2.Weapon.v1"),
  {69,70},{39,40},2,2,"Suppressed Shotgun [MP] Lv.2","Schallgedämpfte Schrotflinte [MZ] St.2",
  "Shotgun Lv.2 with suppressed primary fire, existing suppressed rifle audio and original weapon performance. The underbarrel grenade launcher remains unchanged. Impacts and explosions can still alert enemies.",
  "Schrotflinte St.2 mit schallgedaempftem Hauptfeuer, vorhandenem gedaempften Gewehrsound und urspruenglicher Waffenleistung. Der Unterlauf-Granatwerfer bleibt unveraendert. Einschlaege und Explosionen koennen Gegner alarmieren."},
 {42,304,0x7623068F,0x51D1E2DC,1002,0x3BB81F4D,900304,
  code("DS2Mod.Silenced.BigBore.Recipe.v1"),code("DS2Mod.Silenced.BigBore.Baggage.v1"),code("DS2Mod.Silenced.BigBore.Weapon.v1"),
  {113,114},{107,108},2,1,"Suppressed Big-Bore Handgun [MP]","Schallgedämpfte Großkaliber-Handfeuerwaffe [MZ]",
  "Big-bore handgun with suppressed fire and original weapon performance. Uses existing suppressed handgun audio. Bullet impacts and visible attacks can still alert enemies.",
  "Großkaliber-Handfeuerwaffe mit schallgedaempftem Feuer und urspruenglicher Waffenleistung. Verwendet vorhandenen gedaempften Pistolensound. Einschlaege und sichtbare Angriffe koennen Gegner alarmieren."},
 {40,302,0x17187678,0x30EA922B,1000,0x5A836FBA,900302,
  code("DS2Mod.Silenced.HandgunL1.Recipe.v1"),code("DS2Mod.Silenced.HandgunL1.Baggage.v1"),code("DS2Mod.Silenced.HandgunL1.Weapon.v1"),
  {101,102,99,100},{107,108,107,108},4,1,"Suppressed Machine Pistol [MP] Lv.1","Schallgedämpfte Maschinenpistole [MZ] St.1",
  "The basic Machine Pistol with suppressed fire and original weapon performance. Bullet impacts and visible attacks can still alert enemies.",
  "Die einfache Maschinenpistole mit schallgedaempftem Feuer und urspruenglicher Waffenleistung. Einschlaege und sichtbare Angriffe koennen Gegner alarmieren."}
};
// Legacy weapon 302 is still referenced by startup inventory/profile data.
// Keep its old resources readable without publishing its fabrication recipe.
constexpr u32 resourceSlot(u32 k){return k==LegacyVariant?2:(k<2?k:k+1);}
constexpr u16 ammoId(u32 k,u32 i){return u16(600+resourceSlot(k)*MaxAmmo+i);}
inline bool ownWeapon(u16 id){for(u32 k=0;k<Count;++k)if(Definitions[k].id==id)return true;return false;}
inline bool ownAmmo(u16 id){for(u32 k=0;k<Count;++k)for(u32 i=0;i<Definitions[k].ammoCount;++i)if(ammoId(k,i)==id)return true;return false;}
inline void identity(void*p,u32 k,u32 kind){
 at<u32>(p,8)=1;
 at<u64>(p,0x10)=0x4B58A127E5310000ull|(u64(kind)<<8)|resourceSlot(k);
 at<u64>(p,0x18)=0x93CC28BE714D062Full;
}
inline void text(void*p,const char*s){u32 n=0;while(s[n])++n;at<u64>(p,0x20)=u64(s);at<u32>(p,0x28)=n;}
struct alignas(16) Bundle {
 alignas(8)u8 weapon[0x320],list[0xC0],bag[0x88],recipe[0xB0],name[0x38],description[0x38];
 // DSWeaponTrait embeds Vec3 at +0xD0; native trait instances are 16-byte aligned.
 alignas(16)u8 traits[MaxTraits][0x120];
 alignas(8)u8 ammo[MaxAmmo][0x390];
 u64 traitRefs[MaxTraits];u16 ammoIds[MaxAmmo];u32 traitsCount,primary;
};
static_assert(__builtin_offsetof(Bundle,traits)%16==0 && sizeof(Bundle::traits[0])%16==0,"Native trait SIMD alignment");
struct Sources {
 const u8*weapon;const u8*list;const u8*bag;const u8*recipe;const u8*name;const u8*description;
 const u8*traits[MaxTraits];const u8*ammo[MaxAmmo];const u8*soundDonors[MaxAmmo];
 u32 traitsCount,primary;
};
inline void language(Bundle&b,u32 k,bool german){const auto&d=Definitions[k];text(b.name,german?d.nameDe:d.nameEn);text(b.description,german?d.descriptionDe:d.descriptionEn);}
inline void fabrication(Bundle&b,bool enabled){b.recipe[0x52]=enabled?1:0;}
// All pointer ranges are validated by the caller before reaching this pure builder.
inline bool prepare(Bundle&b,u32 k,const Sources&s,bool enabled,bool german){
 if(k>=Count||!s.weapon||!s.list||!s.bag||!s.recipe||!s.name||!s.description)return false;
 const auto&d=Definitions[k];
 if(at<u16>(s.weapon,0x20)!=d.source||at<u32>(s.list,0x40)!=d.listId||at<u32>(s.list,0x44)!=d.listCode||
    at<u32>(s.bag,0x44)!=d.bag||at<u32>(s.recipe,0x20)!=d.recipe||
    at<u64>(s.weapon,0x28)!=u64(s.list)||at<u64>(s.bag,0x50)!=u64(s.list)||at<u64>(s.recipe,0x28)!=u64(s.bag)||
    !s.traitsCount||s.traitsCount>MaxTraits||s.primary>=s.traitsCount)return false;
 for(u32 i=0;i<s.traitsCount;++i)if(!s.traits[i])return false;
 const auto*primary=s.traits[s.primary];
 // Gun or Shotgun according to its exact source; never the underbarrel launcher.
 if(at<u32>(primary,0x30)!=d.primaryBehavior||primary[0x34]||at<u32>(primary,0x28)!=0||at<u32>(primary,0x58)!=d.ammoCount)return false;
 for(u32 i=0;i<d.ammoCount;++i)if(!s.ammo[i]||!s.soundDonors[i]||
    at<u16>(s.ammo[i],0x20)!=d.ammo[i]||at<u16>(s.soundDonors[i],0x20)!=d.soundDonors[i]||
    !at<u64>(s.soundDonors[i],0x300))return false;
 copy(b.weapon,s.weapon,sizeof(b.weapon));copy(b.list,s.list,sizeof(b.list));copy(b.bag,s.bag,sizeof(b.bag));
 copy(b.recipe,s.recipe,sizeof(b.recipe));copy(b.name,s.name,sizeof(b.name));copy(b.description,s.description,sizeof(b.description));
 identity(b.weapon,k,1);identity(b.list,k,2);identity(b.bag,k,3);identity(b.recipe,k,4);identity(b.name,k,5);identity(b.description,k,6);
 at<u16>(b.weapon,0x20)=d.id;at<u64>(b.weapon,0x28)=u64(b.list);at<u64>(b.weapon,0x30)=0;
 at<u64>(b.weapon,0x2D0)=u64(b.name);at<u64>(b.weapon,0x2D8)=u64(b.description);
 at<u64>(b.weapon,0x2F0)=u64(b.description);
 b.traitsCount=s.traitsCount;b.primary=s.primary;
 for(u32 i=0;i<s.traitsCount;++i){copy(b.traits[i],s.traits[i],0x120);identity(b.traits[i],k,10+i);b.traitRefs[i]=u64(b.traits[i]);}
 at<u32>(b.weapon,0x40)=s.traitsCount;at<u32>(b.weapon,0x44)=s.traitsCount;at<u64>(b.weapon,0x48)=u64(b.traitRefs);
 b.traits[s.primary][0x37]=1;
 for(u32 i=0;i<d.ammoCount;++i){
  copy(b.ammo[i],s.ammo[i],0x390);identity(b.ammo[i],k,20+i);b.ammoIds[i]=ammoId(k,i);
  at<u16>(b.ammo[i],0x20)=b.ammoIds[i];
  // Damage, recoil, magazine, projectile and firing stimulus stay native.
  at<u64>(b.ammo[i],0x300)=at<u64>(s.soundDonors[i],0x300);
  // Native SG rifle ammo uses the same suppressed graph in both sound slots.
  // Do likewise only for our shotgun: its native "supressed" graph
  // still sounded normal in-game. Every primary shot now uses the proven SG
  // donor, even through a caller that requests the standard slot directly.
  if(d.primaryBehavior==2)at<u64>(b.ammo[i],0x2F8)=at<u64>(s.soundDonors[i],0x300);
 }
 at<u64>(b.traits[s.primary],0x60)=u64(b.ammoIds);at<u32>(b.traits[s.primary],0x5C)=d.ammoCount;
 at<u32>(b.list,0x40)=d.newListId;at<u32>(b.list,0x44)=d.newList;
 at<u64>(b.list,0x20)=u64(b.name);at<u64>(b.list,0x28)=u64(b.description);
 at<u64>(b.list,0x30)=0;at<u64>(b.list,0x38)=0;at<u64>(b.list,0x68)=u64(b.description);at<u64>(b.list,0x70)=0;
 // Baggage ID is not a weapon-list identity: keep the native sentinel (zero).
 at<u32>(b.bag,0x44)=d.newBag;at<u64>(b.bag,0x50)=u64(b.list);
 at<u64>(b.bag,0x20)=u64(b.name);at<u64>(b.bag,0x28)=u64(b.description);at<u64>(b.bag,0x30)=0;at<u64>(b.bag,0x38)=0;
 at<u32>(b.recipe,0x20)=d.newRecipe;at<u64>(b.recipe,0x28)=u64(b.bag);
 b.recipe[0x30]=0;at<u64>(b.recipe,0x38)=0;at<u64>(b.recipe,0x40)=0;at<u64>(b.recipe,0x48)=0;
 b.recipe[0x74]=0;b.recipe[0x72]=0;b.recipe[0xA8]=0;b.recipe[0xA9]=0;
 fabrication(b,craftable(k)&&enabled);language(b,k,german);return true;
}
}
