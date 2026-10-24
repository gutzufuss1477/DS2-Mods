#pragma once
#include "visual_archive_api.h"
namespace visual_archive {
inline bool digest(const char*a,const char*b){if(!a||!b)return false;for(unsigned i=0;i<64;++i)if(!a[i]||a[i]!=b[i])return false;return !a[64]&&!b[64];}
inline const DS2VisualArchiveV1* skeleton(){
 HMODULE module=GetModuleHandleW(L"ds2_atlas_exoskeleton_addon.asi");
 using Query=const DS2VisualArchiveV1*(*)();auto q=module?(Query)GetProcAddress(module,"DS2CraftingVisualArchiveV1"):nullptr;
 if(!q)return nullptr;const auto*p=q();
 if(!p||p->size!=sizeof(*p)||p->version!=1||!p->nativeFileOpen||
    !digest(p->archiveSHA256,coffin_archive_profiles::Exoskeleton.sourceSha)||
    !digest(p->linksSHA256,coffin_archive_profiles::Exoskeleton.sourceLinksSha))return nullptr;
 return p;
}
}
