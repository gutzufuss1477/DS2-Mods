#pragma once
#include "core.hpp"
// Private static experiment: append a second *identical* native Coffinboard menu Instance.
// Never register any new DSGameCatalogueListItem, baggage, commodity, or world resource.
// The engine sees the original catalogue instance for EITHER visible choice.
namespace craft {
constexpr u32 CoffinMenuNativeKey=0x0184CF05u;
constexpr u32 CoffinMenuNativeBaggage=0x2A889790u;
struct CoffinMenuAliasResult {bool added;u32 originalIndex;u32 catalogueIndex;};
inline CoffinMenuAliasResult addNativeCoffinMenuAlias(const Recipe*r,u32 nr,
          const Instance*existing,u32 n,Instance*out,u32 capacity,u8 facilityMask){
 CoffinMenuAliasResult result={false,0,0};
 if(!r||!existing||!out||!n||n>=capacity||capacity>MaxMenu||nr>MaxRecipes||
    !(facilityMask&2))return result;
 i32 found=-1;
 for(u32 i=0;i<nr;++i)if(r[i].key==CoffinMenuNativeKey){
  if(found>=0||r[i].baggageKey!=CoffinMenuNativeBaggage||r[i].caseType!=18||
     r[i].usage!=9||r[i].dlc!=0||r[i].mission||r[i].duplicate||
     !r[i].valid||!r[i].resource||!r[i].baggage||!r[i].instance.vtable)
   return result;
  found=(i32)i;
 }
 if(found<0)return result;
 const Instance&v=r[found].instance;
 i32 source=-1;
 for(u32 i=0;i<n;++i)if((existing[i].flags&0xFFF)==(v.flags&0xFFF)){
  // When early-unlocked by this mod, build() sets the 0x8000 menu flag
  // on the original engine catalogue instance. That is the normal v1.9.12
  // test scenario: the coffin is not yet in the 35 vanilla unlocked entries.
  if(source>=0||existing[i].vtable!=v.vtable ||
    (existing[i].flags!=v.flags && existing[i].flags!=(v.flags|0x8000u)))
   return result;
  source=(i32)i;
 }
 if(source<0)return result;
 out[n]=existing[source]; // byte-identical underlying native catalogue identity
 result.added=true;result.originalIndex=(u32)source;result.catalogueIndex=(u32)found;
 return result;
}
}
