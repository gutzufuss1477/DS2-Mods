#include "../../src/weapons/core.hpp"
#include "../../src/weapons/appearance.hpp"
#include "../../src/weapons/attachment.hpp"
#include "../../src/weapons/pistol.hpp"
#include "../../src/weapons/menu_icon.hpp"
using namespace silenced;
extern "C" int _fltused=0;
extern "C" void* memcpy(void*d,const void*s,u64 n){auto*a=(volatile u8*)d;auto*b=(const volatile u8*)s;while(n--)*a++=*b++;return d;}
#define EXPORT extern "C" __declspec(dllexport)
EXPORT u32 BundleSize(){return sizeof(Bundle);}
EXPORT bool Prepare(void*out,u32 k,const Sources*s,bool enabled,bool german){return prepare(*(Bundle*)out,k,*s,enabled,german);}
EXPORT void* Part(void*out,u32 part,u32 index){auto&b=*(Bundle*)out;switch(part){
 case 0:return b.weapon;case 1:return b.list;case 2:return b.bag;case 3:return b.recipe;case 4:return b.name;case 5:return b.description;
 case 6:return index<MaxTraits?b.traits[index]:nullptr;case 7:return index<MaxAmmo?b.ammo[index]:nullptr;
 default:return nullptr;}}
EXPORT void Language(void*out,u32 k,bool german){language(*(Bundle*)out,k,german);}
EXPORT void Fabrication(void*out,bool enabled){fabrication(*(Bundle*)out,enabled);}
EXPORT int MaterialVariant(const Bundle*const*bundles,u64 weapon){return materialVariant(bundles,weapon);}
EXPORT const MaterialDonor* MaterialSource(u32 variant){return materialDonor(variant);}
EXPORT u32 MaterialMarkerHash(){return MaterialMarker;}
EXPORT bool MaterialMarked(const void*entries,u32 count){return materialMarked(entries,count);}

EXPORT bool MenuIcon(void*b,u32 variant,const void*donor){return menuIcon(*(Bundle*)b,variant,donor);}
EXPORT u32 AttachmentSize(){return sizeof(AttachmentGraph);}
EXPORT void* AttachmentPart(void*out,u32 i){auto&g=*(AttachmentGraph*)out;
 void*p[]={g.child,g.entity,g.mover,g.skinned,g.data,g.model,g.part};return i<7?p[i]:nullptr;}
EXPORT bool BuildAttachment(void*out,const void*s,const void*d,const void*m,const void*p,const void*h){
 return attachmentGraph(*(AttachmentGraph*)out,s,d,m,p,h);}
EXPORT bool AttachmentEligible(i32 k,u64 f,u64 p,bool r,bool e){return attachmentEligible(k,f,p,r,e);}
EXPORT u32 PistolSize(){return sizeof(PistolGraph);}
EXPORT void* PistolPart(void*p,u32 i){auto&g=*(PistolGraph*)p;void*v[]={g.set,g.color,g.gpu,g.entries};return i<4?v[i]:nullptr;}
EXPORT bool BuildPistol(void*g,const void*s,const void*e,const void*c,const void*h,u64 r,u64 d){return pistolGraph(*(PistolGraph*)g,s,e,c,h,r,d);}
EXPORT i32 CustomMenuIcon(const char*s,u32 n,u32 h){return customMenuIcon(s,n,h);}
EXPORT bool SetPrivateIcon(void*b,u32 k,const char*s){return assignPrivateIcon(*(Bundle*)b,k,s);}
EXPORT u32 PrivateIconSize(){return sizeof(PrivateIconGraph);}
EXPORT void* PrivateIconPart(void*p,u32 i){auto&g=*(PrivateIconGraph*)p;void*v[]={g.ui,g.texture,g.gpu};return i<3?v[i]:nullptr;}
EXPORT bool BuildPrivateIcon(void*g,u32 slot,const void*u,const void*h,u64 r,u64 d){return privateIconGraph(*(PrivateIconGraph*)g,slot,u,h,r,d);}

EXPORT bool BuildAttachmentForVariant(void*g,u32 k,const void*s,const void*d,const void*m,const void*p,const void*h){return attachmentGraph(*(AttachmentGraph*)g,s,d,m,p,h,k);}
EXPORT bool OwnWeapon(u16 id){return ownWeapon(id);}
EXPORT bool OwnAmmo(u16 id){return ownAmmo(id);}

EXPORT bool Craftable(u32 k){return craftable(k);}
EXPORT bool GermanLanguage(u32 setting,const char*referenceName){return germanLanguage(setting,referenceName);}
