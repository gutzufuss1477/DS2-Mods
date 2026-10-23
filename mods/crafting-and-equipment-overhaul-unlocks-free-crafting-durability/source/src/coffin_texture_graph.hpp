#pragma once
#include "weapons/attachment.hpp"
namespace coffintex {
using namespace craft;
constexpr u64 SetId[2]={0xF843F232FB1F56EFull,0xDA06058AAAA404B8ull};  // TextureSet 53868:10
constexpr u64 ColorId[2]={0x1D36EEDFBE0AFA46ull,0xDAA9BA00FF5814A3ull}; // Color Texture 53868:86
constexpr u32 ColorPackingInfo=2167041u;
// Read-only DS2 1.10.89.0 confirms these LIVE RESOURCE VTAB addresses.
// The earlier 0x5E1F410 / 0x5E1F780 were TypeOf RTTI return identifiers,
// NOT the first virtual function pointers of actual TextureSet/Texture objects.
constexpr u32 TextureSetTypeRva=0x341DB80u,ColorTextureTypeRva=0x3119360u,NativeGpuTypeRva=0x33E5D48u;
constexpr u32 MainPart=1872310964u; // MESH_Main
constexpr u32 UniqueSetKind=90u,UniqueColorKind=91u;
constexpr u32 PrivateMarker=silenced::code("DS2Mod.RedCoffin.PrivateAlbedoBound.v1");
struct alignas(16) Graph {
 alignas(8) u8 set[0x70],color[0x70],gpu[0x188],entries[96];
 u32 targetCount=1,targetCapacity=1;
 u32*targetPointer=nullptr;u32 targets[1]={MainPart};
 u32 stringHash=silenced::code("TextureSetBindings"),stringLength=18,stringRefs=1;
 char stringData[19]="TextureSetBindings";const char*binding=nullptr;
};
static_assert(__builtin_offsetof(Graph,color)%8==0 &&
 __builtin_offsetof(Graph,gpu)%8==0,"native texture graph alignment");
template<class Reader> inline u64 findNativeRenderCoffinSet(u64 model,u64 image,Reader rd){
 if(model<0x10000ull)return 0;
 u64 art=0,partModel=0,refs=0;
 i32 n=0;
 if(!rd(model+0x30,art)||art<0x10000ull ||
    !rd(art+0x20,partModel)||partModel<0x10000ull ||
    !rd(partModel+0x20,n)||n!=10 ||
    !rd(partModel+0x28,refs)||refs<0x10000ull)return 0;
 u64 mainPart=0;
 for(i32 i=0;i<n;++i){
  u64 part=0;u32 code=0;
  if(!rd(refs+u64(i)*8,part)||part<0x10000ull ||
     !rd(part+0xD0,code))return 0;
  if(code==coffintex::MainPart)mainPart=part;
 }
 if(!mainPart)return 0;
 u64 lod=0,lodRefs=0; i32 lodCount=0;
 if(!rd(mainPart+0x20,lod)||lod<0x10000ull ||
    !rd(lod+0x68,lodCount)||lodCount<1||lodCount>8||
    !rd(lod+0x70,lodRefs)||lodRefs<0x10000ull)return 0;
 for(i32 level=0;level<lodCount && level<2;++level){
  u64 skinned=0;if(!rd(lodRefs+16ull*u64(level),skinned)||skinned<0x10000ull)continue;
  i32 groupCount=0;u64 groups=0;
  if(!rd(skinned+0xE8,groupCount)||groupCount<1||groupCount>16||
     !rd(skinned+0xF0,groups)||groups<0x10000ull)continue;
  for(i32 sh=0;sh<groupCount;++sh){
   u64 group=0,effect=0;
   if(!rd(groups+u64(sh)*8ull,group)||group<0x10000ull||
      !rd(group+0x20,effect)||effect<0x10000ull)continue;
   i32 setCount=0;u64 setRows=0;
   if(!rd(effect+0x20,setCount)||setCount<1||setCount>16||
      !rd(effect+0x28,setRows)||setRows<0x10000ull)continue;
   for(i32 s=0;s<setCount;++s){
    const u64 set=setRows+40ull*u64(s);
    i32 techniqueCount=0;u64 techniques=0;
    if(!rd(set,techniqueCount)||techniqueCount<1||techniqueCount>32||
       !rd(set+8,techniques)||techniques<0x10000ull)continue;
    for(i32 t=0;t<techniqueCount;++t){
     u64 technique=techniques+160ull*u64(t);
     i32 bindingCount=0;u64 bindings=0;
     if(!rd(technique+0x10,bindingCount)||bindingCount<1||bindingCount>128||
        !rd(technique+0x18,bindings)||bindings<0x10000ull)continue;
     for(i32 b=0;b<bindingCount;++b){
      u64 binding=bindings+40ull*u64(b);
      u64 texture=0;if(!rd(binding+0x10,texture)||texture<0x10000ull)continue;
      for(u32 k=0;k<2;++k){
       u64 candidate=texture;
       if(k==1 && (!rd(texture+0x28,candidate)||candidate<0x10000ull))continue;
       u64 low=0,high=0,vt=0;
       if(!rd(candidate,vt)||vt!=image+coffintex::TextureSetTypeRva ||
          !rd(candidate+0x10,low)||!rd(candidate+0x18,high))continue;
       if(low==coffintex::SetId[0]&&high==coffintex::SetId[1]){
        // A model-specific exact identity proves this source came from the
        // actual Coffinboard material, not an arbitrary global resource pool.
        return candidate;
       }
      }
     }
    }
   }
  }
 }
 return 0;
}
inline bool validUUID(const void*p,const u64 uuid[2]){
 return p&&silenced::at<u64>(p,0x10)==uuid[0]&&silenced::at<u64>(p,0x18)==uuid[1];
}
inline bool sourceValid(const void*set,const void*entries,const void*color,
 const void*gpu,u64 exe){
 if(!set||!entries||!color||!gpu||exe<0x10000ull)return false;
 if(silenced::at<u64>(set,0)!=exe+TextureSetTypeRva||
    silenced::at<u64>(color,0)!=exe+ColorTextureTypeRva||
    silenced::at<u64>(gpu,0)!=exe+NativeGpuTypeRva)return false;
 if(!validUUID(set,SetId)||!validUUID(color,ColorId)||
    silenced::at<u32>(set,0x20)!=4||silenced::at<u64>(set,0x28)!=u64(entries)||
    silenced::at<u64>(entries,32)!=u64(color)||
    silenced::at<u32>(entries,24)!=ColorPackingInfo||
    silenced::at<u64>(color,0x20)!=u64(gpu)||
    !silenced::at<u64>(set,0x68))return false;
 return true;
}
inline bool clone(Graph&g,const void*set,const void*entries,const void*color,
 const void*gpu,u64 gameBase,u64 d3dResource,u64 descriptor){
 if(!d3dResource||!descriptor||!silenced::at<u64>(g.color,0)||
    !silenced::at<u64>(g.gpu,0)||
    !sourceValid(set,entries,color,gpu,gameBase))return false;
 silenced::copy(g.set,set,sizeof(g.set));
 silenced::copy(g.entries,entries,sizeof(g.entries));
 silenced::attachmentIdentity(g.set,UniqueSetKind,0);
 silenced::attachmentIdentity(g.color,UniqueColorKind,0);
 // The original's streamed mip pointer and loader state are not copied
 // into the independent private resource. Other texture channels are
 // borrowed READ-ONLY from the pinned original TextureSet.
 for(u32 i=0x30;i<0x48;++i)g.set[i]=0;
 silenced::attachmentArray(g.set,0x20,4,g.entries);
 silenced::at<u64>(g.entries,32)=u64(g.color);
 silenced::at<u64>(g.color,0x20)=u64(g.gpu);
 silenced::at<u64>(g.color,0x28)=u64(g.set);
 silenced::copy(g.gpu+0x28,(const u8*)gpu+0x28,12);
 silenced::at<u32>(g.gpu,8)=1;
 silenced::at<u32>(g.gpu,0x10)=2;
 silenced::at<u32>(g.gpu,0x30)=(silenced::at<u32>(gpu,0x30)&0xFE000000u)|1;
 silenced::at<u32>(g.gpu,0x34)=0;
 silenced::at<u32>(g.gpu,0x6C)=0;
 g.gpu[0x58]=2;
 silenced::at<u64>(g.gpu,0x80)=d3dResource;
 silenced::at<u64>(g.gpu,0x90)=descriptor;
 g.targetCount=1;g.targetCapacity=1;
 g.targets[0]=MainPart;g.targetPointer=g.targets;
 g.stringHash=silenced::code("TextureSetBindings");
 g.stringLength=18;g.stringRefs=1;
 constexpr char text[]="TextureSetBindings";
 for(u32 i=0;i<sizeof(text);++i)g.stringData[i]=text[i];
 g.binding=g.stringData;
 return true;
}
}
