#pragma once
// The only data mutation is DSGameBaggageListItem::TypeOfVolume (+0x4D).
namespace craft {
using u8=unsigned char; using u16=unsigned short; using u32=unsigned int;
using u64=unsigned long long; using i32=int;
}
namespace smallcargo {
using namespace craft;
constexpr u32 HookRva=0xB6DA86, CollectorRva=0xB7F890;
constexpr u32 BagVtableRva=0x32084F8, ManagerRva=0x623E540;
constexpr u32 ModelRva=0x11DCB00, AppearanceRva=0x11DCDC0;
constexpr u8 ExpectedModel[]={0x40,0x53,0x48,0x83,0xEC,0x20};
constexpr u8 ExpectedAppearance[]={0x48,0x89,0x5C,0x24,0x08};
constexpr u8 ExpectedCall[]={0xE8,0x05,0x1E,0x01,0x00};
constexpr u8 ExeDigest[]={0xbf,0x3d,0x1c,0x66,0x55,0x45,0x93,0x0b,0xc8,0x50,0xd8,0xf5,0xdf,0x48,0x6f,0x73,0x95,0x88,0x5b,0xb7,0x29,0xd4,0xfd,0x40,0x8f,0xdb,0x03,0x39,0x0d,0xe0,0x76,0x5b};
constexpr u32 MaxBags=65536;
struct Array {i32 count,capacity;u8** data;};
static_assert(sizeof(Array)==16,"native pointer array");
enum class Decision:u32 {Small,Convert,Special};
inline bool functionalEntity(const u8* bag) {
 const u32 id=u32(bag[0x44])|(u32(bag[0x45])<<8)|(u32(bag[0x46])<<16)|(u32(bag[0x47])<<24);
 // These are complete functional entities, not interchangeable container skins.
 // Cart requires DSHoveringCartLowComponent (RTTI 0x436ED90): the native
 // constructor stores it in manager's cart row +48, then 11CE82F dereferences it.
 // Fixed identities select EnemyCart, BBPodPrototype and GhostMech gun entities.
 return bag[0x49]==8 || bag[0x7C]!=0 || id==0x0D3FCCFD || id==0x5D1D2FEB ||
        id==0x4111FB17 || id==0x3041BD9B || id==0x5E576A19;
}
inline Decision decide(const u8* bag) {
 const u8 volume=bag[0x4D],kind=bag[0x49],attribute=bag[0x48];
 // Human/Coffinboard and actual non-container entities use distinct mechanics.
 if(volume>6 || attribute==4 || kind==5 || kind==6 || kind==11 || kind==18 || functionalEntity(bag))
  return Decision::Special;
 return volume==0?Decision::Small:Decision::Convert;
}
template<class Char> inline bool isGamePath(const Char* path,u32 length) {
 if(!path||length<7)return false;
 u32 start=length;while(start&&path[start-1]!='\\'&&path[start-1]!='/')--start;
 if(length-start!=7)return false;
 const char expected[]="ds2.exe";
 for(u32 i=0;i<7;++i){u32 c=u32(path[start+i]);if(c>='A'&&c<='Z')c+='a'-'A';if(c!=u32(expected[i]))return false;}
 return true;
}
inline bool transform(u8* bag) {
 if(decide(bag)!=Decision::Convert)return false;
 bag[0x4D]=0;
 return true;
}
inline bool visualView(const u8* bag,u8* view) {
 if(!bag || bag[0x4D]!=0 || decide(bag)==Decision::Special)return false;
 for(u32 i=0;i<0x88;++i)view[i]=bag[i];
 // A temporary non-owning lookup view, never inserted into the catalogue:
 // erase fixed-model identity, choose Normal/S, omit content/animal special cases.
 for(u32 i=0x44;i<0x48;++i)view[i]=0;
 view[0x49]=0;view[0x7C]=0;
 for(u32 i=0x50;i<0x58;++i)view[i]=0;
 return true;
}
using SelectModel=void*(*)(void*,const u8*);
inline void* model(void* manager,const u8* bag,SelectModel original) {
 alignas(16) u8 view[0x88];
 return original(manager,visualView(bag,view)?view:bag);
}
using SelectAppearance=void*(*)(void*,const u8*);
inline void* appearance(void* context,const u8* instance,SelectAppearance original) {
 const u8* bag=*(const u8* const*)(instance+0x38);
 alignas(16) u8 view[0x88];
 if(!visualView(bag,view))return original(context,instance);
 // The canonical Normal/S branch only reads +38 and +E8 from the instance;
 // its C0/C8 special equipment pointers must be null. No owned object is copied.
 alignas(16) u8 proxy[0x160]={};
 *(const u8**)(proxy+0x38)=view;
 for(u32 i=0xE8;i<0xEC;++i)proxy[i]=instance[i];
 return original(context,proxy);
}
struct Counts {u32 total=0,changed=0,small=0,special=0;};
using Read=bool(*)(u64,void*,u32);
using Writable=bool(*)(void*,u32);
// The collector has just finished on the native initialization thread. Validate
// the entire table before touching any resource; no heap sweep or polling.
inline bool patchArray(Array* array,u64 image,Read rd,Writable writable,Counts& out) {
 Array a={};
 if(!rd(u64(array),&a,sizeof(a)) || a.count<0 || a.count>i32(MaxBags) ||
    a.capacity<a.count || (a.count&&!a.data))return false;
 for(i32 i=0;i<a.count;++i){
  u8* p=nullptr;u8 bytes[0x88];u64 vt=0;
  if(!rd(u64(a.data)+8ull*i,&p,8)||!p||!rd(u64(p),bytes,sizeof(bytes)))return false;
  for(u32 j=0;j<8;++j)vt|=u64(bytes[j])<<(j*8);
  if(vt!=image+BagVtableRva || bytes[0x48]>5 || bytes[0x49]>19 || bytes[0x4D]>8)return false;
  if(decide(bytes)==Decision::Convert&&!writable(p+0x4D,1))return false;
 }
 Counts result;result.total=u32(a.count);
 for(i32 i=0;i<a.count;++i){
  u8* p=a.data[i];
  switch(decide(p)){
   case Decision::Convert:transform(p);++result.changed;break;
   case Decision::Small:++result.small;break;
   case Decision::Special:++result.special;break;
  }
 }
 out=result;return true;
}
}
