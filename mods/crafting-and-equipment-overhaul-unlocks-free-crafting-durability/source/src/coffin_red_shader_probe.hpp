#pragma once
// Private DS2 v1.10.89.0 Red Coffin Board SHADER experiment.
// No duplicate game resources are registered. The extra fabrication entry uses
// the engine's original native Cofﬁnboard item and shared physics class.
// This probe requires an exact user selection and native vehicle-registration event.
// Blood Red is native DSUIColorPaletteCustomizeResource 56:7211, ShaderColorIndex=15.
namespace redcoffin {
using u8=unsigned char;using u32=unsigned int;using u64=unsigned long long;
constexpr u32 NativeCoffinBaggage=0x2A889790u;
constexpr u32 MenuNativeEarlyFlag=0x8000u;
constexpr u32 PaletteVariable=0x3A3714EDu; // DS2 native visual variation parameter
constexpr float BloodRedShaderIndex=15.f; // Verified native palette for Blood Red
constexpr u32 MainModelNameHash=1872310964u; // MESH_Main
constexpr u32 AuxiliaryModelNameHash=1333138349u;
constexpr u32 MaxMenuTab=14u,MaxVisiblePerTab=0x200u,MenuTabStride=0x201u;
struct GpuResourceId {u64 lo,hi;};
constexpr GpuResourceId NativeCoffinSkinned={
  0x2440021B730FEA81ull,0x4BAAA8F22F33E282ull // exact native UUID 730fea81-021b-2440-82e2-332ff2a8aa4b
};
constexpr GpuResourceId NativeCoffinArt={
  0x464E56312BD95748ull,0x92C85B76898D7F8Full // exact native UUID 2bd95748-5631-464e-8f7f-8d89765bc892
};
constexpr GpuResourceId NativeCoffinArtModel={
  0xB94811B3CCE77E73ull,0x962828DD3160EAB4ull // exact native UUID cce77e73-11b3-b948-b4ea-6031dd282896
};
static_assert(NativeCoffinBaggage==0x2A889790u,"native board identity");
inline bool selectedAlias(u64 selectedCell,const u64 *aliasAddresses,u32 n,u32 capacity){
 if(!selectedCell||!aliasAddresses||n>capacity||capacity>1024)return false;
 for(u32 i=0;i<n;++i)if(aliasAddresses[i] && selectedCell==aliasAddresses[i])return true;
 return false;
}
inline bool permittedTabAndIndex(int tabType,int selected){
 return tabType>=0&&tabType<int(MaxMenuTab)&&selected>=0&&selected<int(MaxVisiblePerTab);
}
inline u32 gridIndex(int tabType,int selected){
 return u32(tabType)*MenuTabStride+u32(selected);
}
inline bool pinnedModelResource(u64 lo,u64 hi){
 return (lo==NativeCoffinSkinned.lo&&hi==NativeCoffinSkinned.hi)||
        (lo==NativeCoffinArt.lo&&hi==NativeCoffinArt.hi)||
        (lo==NativeCoffinArtModel.lo&&hi==NativeCoffinArtModel.hi);
}
// Entire native resource identity: original movable Coffinboard, not mission-locked cargo.
inline bool originalCoffinBaggage(u32 id,u8 caseType,u8 country,u32 mission){
 return id==NativeCoffinBaggage && caseType==18u &&
        country==0u && mission==0u;
}
inline bool nativeRedPalette(u32 hash,float index,u32 partName){
 return hash==PaletteVariable&&index==BloodRedShaderIndex&&
   (partName==MainModelNameHash||partName==AuxiliaryModelNameHash);
}
}
