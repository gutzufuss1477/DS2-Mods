#pragma once
#include "preview.hpp"
namespace equipment {
constexpr u8 SourceBootId=11, SourceSkeletonId=21;
constexpr u32 SourceBootRecipe=0x6E7D4315, SourceSkeletonRecipe=0x7DDFDB62;
constexpr u32 SourceBootBag=0x45711B80, SourceSkeletonBag=0x56D383F7;
constexpr u32 SourceBootList=0x4B48C47B, SourceSkeletonList=0x58EA5C0C;
// Equipment catalogue IDs occupy their own namespace; these are NOT EDSItemId.
constexpr u32 SourceBootListId=1000001, SourceSkeletonListId=1000011;
constexpr u32 BootListId=1000103, SkeletonListId=1000104;
struct AtlasSources {
 u64 item[2]={},recipe[2]={},visual[2]={};
 bool conflict=false;
 u32 visited=0;
};
constexpr AtlasSources EmptyAtlasSources;
static_assert(EmptyAtlasSources.item[0]==0 && EmptyAtlasSources.item[1]==0 &&
              EmptyAtlasSources.recipe[0]==0 && EmptyAtlasSources.recipe[1]==0,"Default discovery state must be empty");
struct DepotPlan {
 u64 depot=0,original=0,replacement=0;
 i32 count=0,capacity=0;
};
inline bool prepareDepotCopy(DepotPlan& plan,u64 depot,u32 extra,u64(*allocate)(u64),bool(*copy)(u64,void*,u64)){
 if(!depot||!extra||extra>8||plan.replacement)return false;
 const i32 n=at<i32>((void*)depot,0x38),cap=at<i32>((void*)depot,0x3C);
 const u64 source=at<u64>((void*)depot,0x40);
 if(n<0||cap<n||n>100000||(!source&&n))return false;
 plan.depot=depot;plan.original=source;plan.count=n;plan.capacity=n+extra;
 plan.replacement=allocate(u64(plan.capacity)*8);
 if(!plan.replacement)return false;
 return !n||copy(plan.original,(void*)plan.replacement,u64(n)*8);
}
// Publish a separately allocated copy. The old pointer table is deliberately
// retained until process exit: early-startup native realloc needs a TLS arena
// that does not exist here (0.2.1 crash at DS2+E1936).
inline bool publishDepot(DepotPlan& p){
 if(!p.depot||!p.replacement||p.count<0||p.capacity<p.count||
    at<i32>((void*)p.depot,0x38)!=p.count||at<u64>((void*)p.depot,0x40)!=p.original)return false;
 at<u64>((void*)p.depot,0x40)=p.replacement;
 at<i32>((void*)p.depot,0x3C)=p.capacity;
 return true;
}
// Stable identities: never derive these from localized display strings.
constexpr u32 nameCode(const char* s) {
 u32 n=0;while(*s){n^=u8(*s++);for(u32 i=0;i<8;++i)n=(n>>1)^((n&1)?0x82F63B78u:0);}
 return n&0x7FFFFFFFu;
}
constexpr u32 BootRecipe=nameCode("DS2Mod.Atlas.Boots.Recipe.v1");
constexpr u32 SkeletonRecipe=nameCode("DS2Mod.Atlas.Skeleton.Recipe.v1");
constexpr u32 BootList=nameCode("DS2Mod.Atlas.Boots.Equipment.v1");
constexpr u32 SkeletonList=nameCode("DS2Mod.Atlas.Skeleton.Equipment.v1");
constexpr char BootNameDe[]="ATLAS-Stiefel", BootNameEn[]="ATLAS Boots";
constexpr char SkeletonNameDe[]="ATLAS-Skelett", SkeletonNameEn[]="ATLAS Skeleton";
constexpr char BootDescriptionEn[]="ATLAS equipment: secure footing, high impact absorption, stronger kicks and quiet steps. Weight: 0.2 kg. Durability: 3400.";
constexpr char SkeletonDescriptionEn[]="ATLAS equipment combines Battle, Boost and Bokka effects at level 3: fast movement, enhanced jumps, terrain assistance and an electromagnetic shield. Uses battery. Extra carrying capacity: up to 180 kg with power, 100 kg without power. Weight: 4.0 kg. Durability: 20000. Cargo size: L.";
struct AtlasBundle {
 alignas(8) u8 item[0xA0],list[0xC0],bag[0x88],recipe[0xB0],name[0x38];
 float bootParameters[6];
 PreviewBundle preview;
};
inline void resourceIdentity(void* p,u8 id,u8 kind){
 at<u32>(p,8)=1; // permanent pin; native owners receive additional references
 at<u64>(p,0x10)=0x4A634CBDCE790000ull|(u64(kind)<<8)|id;
 at<u64>(p,0x18)=0xB7AE21D89446A581ull;
}
inline void setText(void* resource,const char* text){
 u32 n=0;while(text[n])++n;at<u64>(resource,0x20)=u64(text);at<u32>(resource,0x28)=n;
}
inline void atlasFabrication(AtlasBundle& b,bool enabled){
 // Usage=None excludes these keys from both the native fabrication list and
 // Crafting Overhaul's special-recipe allowlist. Saved item identities persist.
 b.recipe[0x52]=enabled?1:0;
}
inline void atlasLanguage(AtlasBundle& b,bool german){
 bool boot=b.item[0x20]==BootId;
 setText(b.name,boot?(german?BootNameDe:BootNameEn):(german?SkeletonNameDe:SkeletonNameEn));
 setText(b.preview.description,boot?(german?BootDescription:BootDescriptionEn):(german?SkeletonDescription:SkeletonDescriptionEn));
}
// Borrow only presentation fields. In particular, keep the real item ID, level,
// category, subtype, equipment-list graph and all gameplay values unchanged.
inline bool atlasVisuals(AtlasBundle& b,const u8* donor,const u8* donorList){
 const bool boot=b.item[0x20]==BootId;
 if((!boot && b.item[0x20]!=SkeletonId) || donor[0x20]!=(boot?BootVisualId:SkeletonVisualId) ||
    donor[0x22]!=(boot?5:6) || (!boot && (donor[0x21]!=2 || donor[0x23]!=2)))return false;
 at<u64>(b.item,0x38)=at<u64>(donor,0x38);
 previewCopy(b.item+0x78,donor+0x78,0x20); // entity / helper UUID references
 at<u64>(b.list,0x50)=at<u64>(donorList,0x50);
 at<u32>(b.list,0x58)=at<u32>(donorList,0x58);
 return true;
}
// All input objects are immutable. Their owners are pinned by the runtime before
// publishing copies, retaining shared model, icon, language and recipe-cost data.
inline bool prepareAtlas(AtlasBundle& b,bool boot,const u8* item,const u8* list,const u8* bag,const u8* recipe,
 const u8* name,const u8* description,const u8* chart,const u8 (&rows)[6][0x30],bool german){
 const u8 id=boot?BootId:SkeletonId,sourceId=boot?SourceBootId:SourceSkeletonId;
 if(item[0x20]!=sourceId || item[0x22]!=(boot?5:6) || item[0x23]!=(boot?5:1) ||
    at<u32>(list,0x40)!=(boot?SourceBootListId:SourceSkeletonListId) || at<u32>(list,0x44)!=(boot?SourceBootList:SourceSkeletonList) ||
    at<u32>(bag,0x44)!=(boot?SourceBootBag:SourceSkeletonBag) ||
    at<u32>(recipe,0x20)!=(boot?SourceBootRecipe:SourceSkeletonRecipe))return false;
 if(!preparePreview(b.preview,id,description,chart,rows))return false;
 previewCopy(b.item,item,sizeof(b.item));previewCopy(b.list,list,sizeof(b.list));
 previewCopy(b.bag,bag,sizeof(b.bag));previewCopy(b.recipe,recipe,sizeof(b.recipe));previewCopy(b.name,name,sizeof(b.name));
 resourceIdentity(b.item,id,1);resourceIdentity(b.list,id,2);resourceIdentity(b.bag,id,3);
 resourceIdentity(b.recipe,id,4);resourceIdentity(b.name,id,5);resourceIdentity(b.preview.description,id,6);
 resourceIdentity(b.preview.chart,id,7);
 for(u8 i=0;i<6;++i)resourceIdentity(b.preview.parameters[i],id,8+i);
 b.item[0x20]=id;
 at<u64>(b.item,0x28)=u64(b.name);at<u64>(b.item,0x30)=u64(b.preview.description);at<u64>(b.item,0x40)=u64(b.list);
 if(boot){
  previewCopy(b.bootParameters,BootParams,sizeof(BootParams));
  at<u32>(b.item,0x68)=6;at<u32>(b.item,0x6C)=6;at<u64>(b.item,0x70)=u64(b.bootParameters);
 }else{b.item[0x21]=2;at<float>(b.item,0x58)=3.0f;}
 at<u32>(b.list,0x40)=boot?BootListId:SkeletonListId;at<u32>(b.list,0x44)=boot?BootList:SkeletonList;
 at<u64>(b.list,0x20)=u64(b.name);at<u64>(b.list,0x28)=u64(b.preview.description);
 at<u64>(b.list,0x30)=0;at<u64>(b.list,0x38)=0;
 at<u64>(b.list,0x98)=u64(b.preview.chart);at<u64>(b.list,0xA0)=0;
 // NameCode identifies saved baggage independently of language and source item.
 at<u32>(b.bag,0x44)=boot?BootBag:SkeletonBag;
 at<u64>(b.bag,0x20)=u64(b.name);at<u64>(b.bag,0x28)=u64(b.preview.description);
 at<u64>(b.bag,0x30)=0;at<u64>(b.bag,0x38)=0;at<u64>(b.bag,0x50)=u64(b.list);
 at<float>(b.bag,0x60)=boot?0.2f:4.0f;at<u32>(b.bag,0x64)=boot?3400:20000;at<u32>(b.bag,0x6C)=boot?3400:20000;
 at<u32>(b.recipe,0x20)=boot?BootRecipe:SkeletonRecipe;at<u64>(b.recipe,0x28)=u64(b.bag);
 b.recipe[0x30]=0;at<u64>(b.recipe,0x38)=0;at<u64>(b.recipe,0x40)=0;at<u64>(b.recipe,0x48)=0;
 b.recipe[0x52]=1;b.recipe[0x74]=0; // normal crafting, no inherited DLC/fact gate
 atlasLanguage(b,german);return true;
}
}
