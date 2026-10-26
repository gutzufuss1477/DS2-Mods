#pragma once
// Additive peer ABI; does not change Crafting HostV1/AddonV1 or Router1.
// N02 remains the sole resource/visibility owner when both addons are installed.
// Boot runtime versions may change while this ABI and the exact asset contract stay fixed.
struct AtlasBootCompanionV1 {
 unsigned size,version,nativeLayout,atlasBootId;
 const char* graphPayloadSHA256;
 void* storageOpenShim;
 int (*prepareMaterials)();
 void (*syncMaterials)(unsigned long long art,int active);
 int (*isPassive)();
};
static_assert(sizeof(AtlasBootCompanionV1)==56,"Boot companion ABI V1 x64");
using AtlasBootCompanionQuery=const AtlasBootCompanionV1*(*)(unsigned version);
