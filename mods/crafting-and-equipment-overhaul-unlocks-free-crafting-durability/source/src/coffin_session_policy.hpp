#pragma once
#include "core.hpp"
#include "coffin_preview_policy.hpp"
namespace coffin_session {
using namespace craft;
constexpr u32 Capacity=256;
struct Identity {u64 owner=0,lo=0,hi=0;};
template<class R>u64 recordCargo(R&read,u64 image,u64 record){
 u64 uid=0,bag=0,vt=0;u32 key=0;
 if(record<0x10000||!read(record,vt)||vt!=image+0x327DA68||
    !read(record+0x38,bag)||!read(bag+0x44,key)||key!=0x2A889790u||
    !read(record+0x18,uid)||uid==~u64(0))return 0;
 return uid;
}
template<class R>Identity identity(R&read,u64 image,u64 owner){
 Identity a{};u64 vt=0;
 if(owner>=0x10000&&read(owner,vt)&&vt==image+redcoffin::CoffinVehicleVtableRva&&
    read(owner+0x10,a.lo)&&read(owner+0x18,a.hi)&&(a.lo||a.hi))a.owner=owner;
 return a;
}
inline bool same(const Identity&a,const Identity&b){
 return a.owner&&a.owner==b.owner&&a.lo==b.lo&&a.hi==b.hi;
}
template<class R>u64 cargoOwner(R&read,u64 image,u64 cargo){
 if(!cargo||cargo==~u64(0))return 0;
 u64 manager=0,rows=0,result=0;u32 count=0;
 if(!read(image+0x623EA48,manager)||manager<0x10000||
    !read(manager+0x24488,count)||count>Capacity||
    !read(manager+0x24490,rows)||(!rows&&count))return 0;
 for(u32 i=0;i<count;++i){u64 uid=0,owner=0,record=0,bag=0,recordId=0;u32 key=0;
  const u64 p=rows+0x28ull*i;
  if(!read(p+0x10,uid))return 0;
  if(uid!=cargo)continue;
  if(result||!read(p,record)||!read(record+0x18,recordId)||recordId!=cargo||
     !read(record+0x38,bag)||!read(bag+0x44,key)||key!=0x2A889790u||
     !read(p+8,owner)||owner<0x10000)return 0;
  result=owner;
 }
 return result;
}
template<class R>u64 previewCargo(R&read,u64 image,u64 owner){
 u64 manager=0,result=0;if(!owner||!read(image+0x623EA48,manager)||manager<0x10000)return 0;
 for(u32 list=0;list<2;++list){u64 rows=0;u32 count=0,off=list?0x24258:0x24240;
  if(!read(manager+off,count)||count>4096||!read(manager+off+8,rows)||(!rows&&count))continue;
  for(u32 i=0;i<count;++i){u64 view=0,entity=0,id=0,bag=0;u32 key=0;
   if(!read(rows+8ull*i,view)||view<0x10000||!read(view+0xF8,entity)||entity!=owner+0x20||
      !read(view+0x18,id)||!id||id==~u64(0)||!read(view+0x38,bag)||
      !read(bag+0x44,key)||key!=0x2A889790u)continue;
   if(result&&result!=id)return 0;
   result=id;
  }
 }
 return result;
}
}
