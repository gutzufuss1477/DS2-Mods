#pragma once
#include "coffin_direct_model_policy.hpp"
namespace coffin_preview {
using namespace redcoffin;
// Original baggage preview, independently exported and read back in v1953.
constexpr u64 SkinLo=0xFD4E81E8A77B66BCull,SkinHi=0x4F26D00C42CE2580ull;
constexpr u64 ArtLo=0x674A7A4632EFC547ull,ArtHi=0x1258632605045C81ull;
constexpr u64 ModelLo=0x3C49AA2E0176C560ull,ModelHi=0x3BC2538BB2A2A19Aull;
constexpr u64 PartIds[2][2]={{0x1A46E741CCA5FE58ull,0x57A434C37973CEA2ull},{0x7E4E70FAC22C99E7ull,0x8890624662837E84ull}};
struct alignas(16) PartCopy {u8 bytes[0xE0];};
template<class T>T& at(void*p,u32 n){return *(T*)((u8*)p+n);}
template<class T>const T& at(const void*p,u32 n){return *(const T*)((const u8*)p+n);}
inline bool copyGraphics(PartCopy&out,const void*native,const void*story,u32 slot){
 if(!native||!story||slot>1||at<u64>(native,0)!=at<u64>(story,0)||
    at<u64>(native,16)!=PartIds[slot][0]||at<u64>(native,24)!=PartIds[slot][1]||
    at<u64>(story,16)!=coffin_direct_assets::Parts[slot].newLo||at<u64>(story,24)!=coffin_direct_assets::Parts[slot].newHi||
    !at<u64>(story,0x20)||!at<u64>(story,0x38)||at<u32>(native,0x80)||
    at<u32>(native,0xD0)!=coffin_direct_assets::Parts[slot].name)return false;
 for(u32 i=0;i<0xe0;++i)out.bytes[i]=((const u8*)native)[i];
 at<u32>(out.bytes,8)=1;at<u32>(out.bytes,12)=0;
 at<u64>(out.bytes,16)=0x438539BE2367AF10ull+slot;at<u64>(out.bytes,24)=0x9B7D683A76C5C582ull;
 // ONLY geometry/material graph and matching bone bounds. Native collision,
 // parameters, part name and transforms remain byte-for-byte original.
 at<u64>(out.bytes,0x20)=at<u64>(story,0x20);at<u64>(out.bytes,0x38)=at<u64>(story,0x38);
 return true;
}
template<class R>u64 cargoIdentity(R&read,u64 image,u64 redVehicle){
 u64 manager=0,rows=0,id=0;u32 count=0;bool found=false;
 if(!redVehicle||!read(image+0x623EA48,manager)||manager<0x10000||
    !read(manager+0x24488,count)||!count||count>MaximumWorldCoffins||
    !read(manager+0x24490,rows)||rows<0x10000)return 0;
 for(u32 i=0;i<count;++i){u64 owner=0,value=0;
  if(!read(rows+0x28ull*i+8,owner))return 0;
  if(owner!=redVehicle)continue;
  if(found||!read(rows+0x28ull*i+0x10,value)||!value||value==~u64(0))return 0;
  id=value;found=true;
 }
 return id;
}
template<class R>bool viewOwns(R&read,u64 image,u64 owner,u64 cargo){
 u64 manager=0;if(!owner||!cargo||!read(image+0x623EA48,manager)||manager<0x10000)return false;
 // Native pre-load callback establishes +F8 before model initialization.
 // Look in both active and pending baggage views; compare the complete cargo UID.
 for(u32 list=0;list<2;++list){u64 rows=0;u32 count=0;u32 off=list?0x24258:0x24240;
  if(!read(manager+off,count)||count>4096||!read(manager+off+8,rows)||(!rows&&count))continue;
  for(u32 i=0;i<count;++i){u64 view=0,entity=0,id=0,bag=0;u32 key=0;
   if(!read(rows+8ull*i,view)||view<0x10000||!read(view+0xF8,entity)||entity!=owner+0x20||
      !read(view+0x18,id)||id!=cargo||!read(view+0x38,bag)||bag<0x10000||
      !read(bag+0x44,key)||key!=0x2A889790u)continue;
   return true;
  }
 }
 return false;
}
template<class R>coffin_direct::Candidate inspect(R&read,u64 image,u64 skin,u64 row,const u64(&parts)[2],u64 cargo){
 using coffin_direct::id;coffin_direct::Candidate c;u64 vt=0,back=0,res=0,art=0,model=0,list=0;u32 count=0,name=0;
 if(!cargo||skin<0x10000||row<0x10000||!read(skin+0x48,c.owner)||c.owner<0x10000||
    !read(c.owner,vt)||vt!=image+0x3119BC8||!read(c.owner+0xC8,back)||back!=skin||
    !read(skin+0x30,res)||!id(read,res,SkinLo,SkinHi)||
    !read(c.owner+0xD8,art)||art<0x10000||!read(art+0x48,back)||back!=c.owner||
    !read(art+0x30,res)||!id(read,res,ArtLo,ArtHi)||!read(res+0x20,model)||!id(read,model,ModelLo,ModelHi)||
    !read(model+0x38,count)||count!=6||!read(model+0x40,list)||list<0x10000||
    !read(row+0xC8,c.original)||!read(c.original+0xD0,name))return {};
 u32 found=0;
 for(u32 i=0;i<count;++i){u64 p=0;if(!read(list+8ull*i,p))return {};
  for(u32 j=0;j<2;++j){if(!id(read,p,PartIds[j][0],PartIds[j][1]))continue;
   u64 oldVt=0,newVt=0,physics=0,replacementPhysics=0;u32 n=0;const auto&expected=coffin_direct_assets::Parts[j];
   if((found&(1u<<j))||!read(p,oldVt)||!read(parts[j],newVt)||oldVt!=newVt||
      !read(p+0xD0,n)||n!=expected.name||!read(p+0x40,physics)||
      !read(parts[j]+0x40,replacementPhysics)||physics!=replacementPhysics)return {};
   found|=1u<<j;if(p==c.original&&name==expected.name){c.slot=int(j);c.replacement=parts[j];}
  }
 }
 return found==3&&c.replacement&&viewOwns(read,image,c.owner,cargo)?c:coffin_direct::Candidate{};
}
}
