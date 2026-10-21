#pragma once
#include "core.hpp"
namespace silenced {
struct IconDonor {u32 listId,nameCode;const char* texture;};
constexpr IconDonor IconDonors[]={
 {5045,1524247433,"ut_ui_icon_wep_asr2_enem_gm2"},
 {5054,991113469,"ut_ui_icon_wep_mag3_v02_gm"}
};
// The caller pins the native list that owns this engine String. Only our
// cloned list changes; rarity, names, and the original icon remain untouched.
inline bool menuIcon(Bundle&b,u32 variant,const void*donor){
 if(variant>=2||!donor)return false;
 const auto&d=IconDonors[variant];
 if(at<u32>(donor,0x40)!=d.listId||at<u32>(donor,0x44)!=d.nameCode||!at<u64>(donor,0x50))return false;
 at<u64>(b.list,0x50)=at<u64>(donor,0x50);return true;
}
// Borrow only native Mech texture substitutions, never enemy weapon behavior.
struct MaterialDonor { u64 uuid[2]; u32 replacements; };
constexpr MaterialDonor MaterialDonors[]={
 {{0xEA4B07DF506ECDC8ull,0x4BED09AD68F68F95ull},7}, // 499:100140, AR Lv.2
 {{0x3E460E8AFBFDFA9Aull,0x8F8C0EB394772CB7ull},3}, // 20719:9, MG Lv.2
 {{0x84411F2D52FEFEBEull,0xF575508052BA2C98ull},6} // 922:0, Shotgun Lv.2
};
constexpr u32 MaterialMarker=code("MoreSilencedGuns_MechMaterials_v1");
inline i32 materialVariant(const Bundle*const*bundles,u64 weapon){
 if(!weapon)return -1;
 for(u32 k=0;k<ActiveCount;++k)if(bundles[k]&&weapon==u64(bundles[k]->weapon))return i32(k);
 return -1;
}
inline const MaterialDonor* materialDonor(u32 variant){return variant<3?&MaterialDonors[variant]:nullptr;}
// Engine-owned controller entries disappear with the model. This avoids a
// process-wide cache of dangling/reused entity addresses.
inline bool materialMarked(const void*entries,u32 count){
 if(!entries||count>256)return false;
 for(u32 i=0;i<count;++i){const u8*p=(const u8*)entries+40*i;
  if(at<u64>(p,0)==(u64(MaterialMarker)<<32)&&at<u32>(p,8)==MaterialMarker&&
     at<float>(p,12)==1.f&&p[28]==1)return true;
 }
 return false;
}
}
