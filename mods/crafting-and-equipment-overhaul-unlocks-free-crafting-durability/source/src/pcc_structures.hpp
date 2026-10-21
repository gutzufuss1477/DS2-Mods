#pragma once
#include "core.hpp"
#include "pcc_identities.generated.hpp"
namespace craft { namespace pcc {
constexpr u64 SiteRva=0x2000ABE, OriginalRva=0x182510;
constexpr u64 BehaviorVtableRva=0x33C98B8, ContextRva=0x33FC0A8, ManagerRva=0x62473C8;
constexpr u8 CallBytes[5]={0xE8,0x4D,0x1A,0x18,0xFE};
constexpr u32 AmmoCount=sizeof(Ammo)/sizeof(Ammo[0]);
template<class R> bool guid(R&read,u64 address,const u8 (&expected)[16]) {
 u8 actual[16];if(!read(address,actual,16))return false;
 for(u32 i=0;i<16;++i)if(actual[i]!=expected[i])return false;return true;
}
template<class T,class R> bool get(R&read,u64 address,T&out){return read(address,&out,sizeof(out));}
// Returns a reviewed identity index, or -1. Read-only: no resources, flags or lists are changed.
template<class R> i32 eligible(R&read,u64 image,u64 manager,u64 context,u64 fact,u64 ammo,u64 behavior) {
 u64 vtable=0,currentManager=0,entity=0,weapon=0,trait=0,ownerRef=0,ammoFact=0;
 u16 weaponId=0,ammoId=0;u8 level=0,detail=0,ownerType=0,type=0,category=0,printer=0;i32 count=0,contentCount=0;
 if(!image||!fact||!ammo||!behavior||context!=image+ContextRva||
    !get(read,image+ManagerRva,currentManager)||!currentManager||manager!=currentManager||
    !get(read,behavior,vtable)||vtable!=image+BehaviorVtableRva||
    !get(read,behavior+0x50,entity)||!entity||!get(read,entity+0x2180,weapon)||!weapon||
    !get(read,weapon+0x20,weaponId)||weaponId<104||weaponId>106||
    !get(read,weapon+0x72,level)||!get(read,weapon+0x3A,detail)||detail!=30||
    !get(read,behavior+0x60,trait)||!trait||
    !get(read,entity+0x338,ownerRef)||ownerRef<0x10020||
    !get(read,ownerRef-0x20+0x327,ownerType)||ownerType!=0||
    !get(read,behavior+0x2E8,count))return -1;
 const TierIdentity&t=Tiers[weaponId-104];
 if(level!=t.level||count!=t.count||!guid(read,weapon+0x10,t.uuid)||!guid(read,trait+0x10,t.trait))return -1;
 // Check the complete native tier order, and that this exact resource belongs to it.
 bool found=false;
 for(i32 i=0;i<count;++i){u16 id=0;u64 resource=0;
  if(!get(read,behavior+0x68+u64(i)*0x28,id)||id!=t.ammo[i]||
     !get(read,behavior+0x70+u64(i)*0x28,resource)||!resource)return -1;
  if(resource==ammo){if(found)return -1;found=true;ammoId=id;}
 }
 if(!found)return -1;
 u32 n=0;while(n<AmmoCount&&Ammo[n].id!=ammoId)++n;if(n==AmmoCount)return -1;
 const AmmoIdentity&a=Ammo[n];u16 resourceId=0;
 if(!get(read,ammo+0x20,resourceId)||resourceId!=a.id||!guid(read,ammo+0x10,a.uuid)||
    !get(read,ammo+0x24,type)||type!=17||!get(read,ammo+0x248,category)||category!=a.category||
    !get(read,ammo+0x24B,printer)||printer!=1||!get(read,ammo+0x378,ammoFact)||ammoFact!=fact||
    !get(read,ammo+0x380,contentCount)||contentCount!=0||!guid(read,fact+0x10,a.fact))return -1;
 return (i32)n;
}
}}
