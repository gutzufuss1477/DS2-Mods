#pragma once
// Frozen Windows x64 ABI. Append fields in a new version; never change V1 layout.
// No C++ objects/allocators cross the boundary. Callbacks must not throw.
namespace crafting_addon {
using U32=unsigned int; using U64=unsigned long long; using Byte=unsigned char;
using MeshNext=void(*)(void*);
using CatalogueNext=void*(*)(void*);
using IconNext=U64(*)(U64,const char*const*);
constexpr U32 Version=1, NativeLayout=0x01108901;
struct HostV1 {
 U32 size,version,nativeLayout,atlasBootId;
 U64 atlasIdentityHigh,atlasEquipmentLow;
};
struct AddonV1 {
 U32 size,version;
 // Called once outside DllMain, before resource loading, on the final HOST graph.
 // Successful output belongs to addon until releaseGraph, including on write failure.
 int (*mergeGraph)(const Byte*,U32,Byte**,U32*);
 void (*releaseGraph)(Byte*);
 // Called after host graph write; publish visual changes only when success==1.
 void (*graphCommitted)(int success);
 // Host has finished its own work; next must be called exactly once.
 void (*worn)(void*,MeshNext);
 void (*hanging)(void*,MeshNext);
 void* (*catalogue)(void*,CatalogueNext);
 U64 (*icon)(U64,const char*const*,IconNext);
};
using QueryAddon=int(*)(const HostV1*,AddonV1*);
static_assert(sizeof(HostV1)==32 && sizeof(AddonV1)==64,"ABI v1 x64 layout");
inline bool validHost(const HostV1*h){return h&&h->size>=sizeof(HostV1)&&h->version==Version&&
 h->nativeLayout==NativeLayout&&h->atlasBootId==103&&h->atlasIdentityHigh==0xB7AE21D89446A581ull&&h->atlasEquipmentLow==0x4A634CBDCE790267ull;}
}
