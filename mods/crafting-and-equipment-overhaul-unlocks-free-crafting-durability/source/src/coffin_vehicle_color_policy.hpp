#pragma once
#include "coffin_red_shader_probe.hpp"
namespace redcoffin {
// MESH_Main -> LodMesh 53868:5 -> SkinnedMesh 53868:138 ->
// ShadingGroup 53868:209 -> RenderEffect 53868:50 ->
// ShaderFloat3 0x58E13A70, default (1,1,1). Unlike the palette scalar,
// this variable IS bound to the visible outer body material with albedo
// TextureSet 53868:10 ("deng_cbrd_def_v00_clr.psd").
constexpr u32 NativeBodyTintVariable=0x58E13A70u;
constexpr float RedBodyTint[3]={1.0f,0.12f,0.13f};
constexpr u32 CoffinVehicleVtableRva=0x33AE468u;
constexpr u32 NativeVehicleUpdateRva=0x1F66E10u;
constexpr u32 CoffinNativeUpdateCallsiteRva=0x1F79699u;
constexpr u32 NativeCoffinArtPartCount=10u;
constexpr u32 MaxPreviousCoffins=64u;
constexpr u32 MaximumWorldCoffins=128u;
constexpr u32 MaximumShaderEntries=256u;
constexpr u64 RedManufactureWindowMs=90000ull;
inline bool newlyCreated(u64 vehicle,const u64*previous,u32 count) {
 if(!vehicle||!previous||count>MaxPreviousCoffins)return false;
 for(u32 i=0;i<count;++i)if(previous[i]==vehicle)return false;
 return true;
}
inline bool validModel(u64 lo,u64 hi,u32 parts,bool hasMain) {
 return lo==NativeCoffinArt.lo&&hi==NativeCoffinArt.hi&&
        parts==NativeCoffinArtPartCount&&hasMain;
}
inline bool rgbMaterialRecorded(u32 part,u32 variable,float r,float g,float b){
 return part==MainModelNameHash&&variable==NativeBodyTintVariable&&
        r>0.99f&&r<1.01f&&
        g>0.11f&&g<0.13f&&
        b>0.12f&&b<0.14f;
}
inline bool shadeRecorded(u32 part,u32 variable,float value) {
 return part==MainModelNameHash&&variable==PaletteVariable&&
        value>14.99f&&value<15.01f;
}
}
