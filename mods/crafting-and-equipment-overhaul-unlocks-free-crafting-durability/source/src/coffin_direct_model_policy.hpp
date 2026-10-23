#pragma once
#include "coffin_direct_assets.hpp"
#include "coffin_vehicle_color_policy.hpp"
namespace coffin_direct {
using namespace redcoffin;
constexpr u64 AddRowSite=0x33DE6Bull,AddRowTarget=0x22B800ull;
constexpr u8 AddRowBytes[5]={0xE8,0x90,0xD9,0xEE,0xFF};
constexpr u64 BindRowTarget=0x22E410ull;
struct Candidate {u64 owner=0,original=0,replacement=0;int slot=-1;};
template<class R>bool id(R&read,u64 p,u64 lo,u64 hi){
 u64 a=0,b=0;return p>=0x10000ull&&read(p+0x10,a)&&a==lo&&read(p+0x18,b)&&b==hi;
}
template<class R>bool privateParts(R&read,u64 group,u64 (&parts)[2]){
 u32 n=0,cap=0,gid=0;u64 rows=0;
 if(group<0x10000ull||!read(group+0x40,n)||n!=coffin_direct_assets::Count||
    !read(group+0x44,cap)||cap!=n||!read(group+0x64,gid)||gid!=coffin_direct_assets::Group||
    !read(group+0x48,rows)||rows<0x10000ull)return false;
 for(u32 i=0;i<2;++i){
  const auto&p=coffin_direct_assets::Parts[i];u64 mesh=0,bounds=0,physics=1;u32 name=0;
  if(!read(rows+8ull*p.index,parts[i])||!id(read,parts[i],p.newLo,p.newHi)||
     !read(parts[i]+0x0c,gid)||gid!=coffin_direct_assets::Group||
     !read(parts[i]+0xD0,name)||name!=p.name||
     !read(parts[i]+0x20,mesh)||mesh<0x10000ull||
     !read(parts[i]+0x38,bounds)||bounds<0x10000ull||
     !read(parts[i]+0x40,physics)||physics)return false;
 }
 return parts[0]!=parts[1];
}
inline bool selected(bool armed,u64 now,u64 started,u64 owner,u64 tracked,
                     const u64*previous,u32 count){
 return armed&&started&&now>=started&&now-started<=RedManufactureWindowMs&&
        (!tracked||tracked==owner)&&newlyCreated(owner,previous,count);
}
template<class R>Candidate inspect(R&read,u64 image,u64 skin,u64 row,const u64 (&parts)[2]){
 Candidate c;u64 vt=0,back=0,resource=0,art=0,model=0,list=0;
 u32 count=0,partName=0;
 if(image<0x10000ull||skin<0x10000ull||row<0x10000ull||
    !read(skin+0x48,c.owner)||c.owner<0x10000ull||
    !read(c.owner,vt)||vt!=image+CoffinVehicleVtableRva||
    !read(c.owner+0xC8,back)||back!=skin||
    !read(skin+0x30,resource)||!id(read,resource,NativeCoffinSkinned.lo,NativeCoffinSkinned.hi)||
    !read(c.owner+0xD8,art)||art<0x10000ull||
    !read(art+0x48,back)||back!=c.owner||
    !read(art+0x30,resource)||!id(read,resource,NativeCoffinArt.lo,NativeCoffinArt.hi)||
    !read(resource+0x20,model)||!id(read,model,NativeCoffinArtModel.lo,NativeCoffinArtModel.hi)||
    !read(model+0x38,count)||count!=NativeCoffinArtPartCount||
    !read(model+0x40,list)||list<0x10000ull||
    !read(row+0xC8,c.original)||!read(c.original+0xD0,partName))return {};
 // Validate BOTH source parts before allowing either replacement. Other
 // physics/accessory rows and every original resource remain untouched.
 u32 found=0;
 for(u32 i=0;i<count;++i){
  u64 part=0;if(!read(list+8ull*i,part))return {};
  for(u32 j=0;j<2;++j){
   const auto&p=coffin_direct_assets::Parts[j];
   if(!id(read,part,p.oldLo,p.oldHi))continue;
   u64 oldVt=0,newVt=0,physics=1;u32 name=0;
   if((found&(1u<<j))||!read(part,oldVt)||!read(parts[j],newVt)||newVt!=oldVt||
      !read(part+0xD0,name)||name!=p.name||!read(part+0x40,physics)||physics)return {};
   found|=1u<<j;
   if(part==c.original&&partName==p.name){c.slot=int(j);c.replacement=parts[j];}
  }
 }
 return found==3&&c.slot>=0?c:Candidate{};
}
// Dispatch native add exactly once; native resource assignment owns refcounts.
template<class Bind,class Add>u64 dispatch(const Candidate&c,u64 skin,u64 row,u8 initialize,Bind&bind,Add&add){
 if(c.replacement&&c.slot>=0&&c.slot<2)bind(row,c.replacement);
 return add(skin,row,initialize);
}
}
