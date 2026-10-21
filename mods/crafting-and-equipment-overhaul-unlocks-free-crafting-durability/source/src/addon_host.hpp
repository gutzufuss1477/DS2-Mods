#pragma once
#include "crafting_addon_api.h"
namespace addon_host {
using namespace crafting_addon;
bool merge(const Byte*,U32,Byte**,U32*);
void finish(bool,Byte*);
bool requested();
void worn(void*,MeshNext);
void hanging(void*,MeshNext);
void* catalogue(void*,CatalogueNext);
U64 icon(U64,const char*const*,IconNext);
}
