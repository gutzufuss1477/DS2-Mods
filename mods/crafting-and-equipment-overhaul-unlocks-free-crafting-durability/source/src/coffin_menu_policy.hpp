#pragma once
#include "core.hpp"
namespace coffin_menu {
using namespace craft;
constexpr u32 RecipeKey=0x0184CF05u,BaggageKey=0x2A889790u;
template<class R,class Own>bool selectedInstance(R&read,Own&own,u64 image,u64 instance){
 u64 vt=0,manager=0,rows=0,recipe=0,bag=0;u32 flags=0,count=0,key=0;u8 usage=0,caseType=0;
 return own(instance)&&read(instance,vt)&&vt==image+0x31FF530&&read(instance+8,flags)&&
  read(image+0x623E540,manager)&&manager>=0x10000&&read(manager+8,count)&&count>0&&count<=4096&&
  (flags&0xfff)<count&&read(manager+0x10,rows)&&rows>=0x10000&&
  read(rows+8ull*(flags&0xfff),recipe)&&recipe>=0x10000&&
  read(recipe+0x20,key)&&key==RecipeKey&&read(recipe+0x52,usage)&&usage==9&&
  read(recipe+0x28,bag)&&bag>=0x10000&&read(bag+0x44,key)&&key==BaggageKey&&
  read(bag+0x49,caseType)&&caseType==18;
}
template<class R,class Own>bool selectedRow(R&read,Own&own,u64 image,u64 row){
 u64 instance=0;u32 key=0;
 return row>=0x10000&&read(row,instance)&&read(row+12,key)&&key==RecipeKey&&selectedInstance(read,own,image,instance);
}
struct alignas(16) IconGraph {alignas(8)u8 ui[0x38],texture[0x70],gpu[0x188];};
template<class T>T& field(void*p,u32 off){return *(T*)((u8*)p+off);}
template<class T>const T& field(const void*p,u32 off){return *(const T*)((const u8*)p+off);}
inline bool iconGraph(IconGraph&g,const void*ui,const void*texture,const void*gpu,u64 resource,u64 descriptor){
 if(!ui||!texture||!gpu||!resource||!descriptor||
    field<u64>(g.ui,0)!=field<u64>(ui,0)||field<u64>(g.texture,0)!=field<u64>(texture,0)||
    field<u64>(g.gpu,0)!=field<u64>(gpu,0)||
    field<u64>(ui,16)!=0x464e26649a54d138ull||field<u64>(ui,24)!=0x73899fb820edf499ull||
    field<u8>(ui,0x20)||field<u32>(ui,0x24)!=256||field<u32>(ui,0x28)!=160||field<u8>(gpu,0x29)!=0x4b)return false;
 field<u32>(g.ui,8)=1;field<u32>(g.ui,12)=0;
 field<u64>(g.ui,16)=0x4A084EB4AA9E0C15ull;field<u64>(g.ui,24)=0x8C629092572FD489ull;
 field<u32>(g.texture,8)=1;field<u32>(g.texture,12)=0;
 field<u64>(g.texture,16)=0x4F6F1ACB87E2F55Dull;field<u64>(g.texture,24)=0xD47026B1DA270EBBull;
 field<u32>(g.ui,0x24)=256;field<u32>(g.ui,0x28)=160;
 field<u64>(g.ui,0x30)=u64(g.texture);field<u64>(g.texture,0x20)=u64(g.gpu);
 for(u32 i=0;i<12;++i)g.gpu[0x28+i]=((const u8*)gpu)[0x28+i];
 field<u32>(g.gpu,8)=1;field<u32>(g.gpu,0x10)=2;
 field<u32>(g.gpu,0x2c)=0x20000000u|512|(320<<15);
 field<u32>(g.gpu,0x30)=1;field<u32>(g.gpu,0x34)=0;g.gpu[0x58]=2;
 field<u64>(g.gpu,0x80)=resource;field<u64>(g.gpu,0x90)=descriptor;
 return true;
}
}
