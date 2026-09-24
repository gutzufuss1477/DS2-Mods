#pragma once
#include "core.hpp"
// Additional supported scope: known native module/cover/charm recipe NameCodes.
// Never broad-enable every Usage=5 entry: patches and unknown recipes remain native.
namespace craft {
enum class BackpackKind : u8 { None, Module, Cover, Charm };
struct BackpackItem {u32 key; BackpackKind kind;};
constexpr BackpackItem BackpackItems[] = {
 {0x2F256E9Fu,BackpackKind::Module}, // Extra Battery Lv.1
 {0x08605E71u,BackpackKind::Module}, // Extra Battery Lv.2
 {0x1B30AD85u,BackpackKind::Module}, // Extra Battery Lv.3
 {0x695B2E86u,BackpackKind::Module}, // Extra Battery Lv.4
 {0x5797B553u,BackpackKind::Module}, // Mini Extra Battery
 {0x3C759D6Bu,BackpackKind::Module}, // Utility Pouch
 {0x4E1E1E68u,BackpackKind::Module}, // Grenade Pouch
 {0x3C87F6E8u,BackpackKind::Module}, // Large Grenade Pouch
 {0x44352D24u,BackpackKind::Module}, // Spare Ammo Container
 {0x684D9203u,BackpackKind::Module}, // Large Spare Ammo Container Lv.1
 {0x1A261100u,BackpackKind::Module}, // Large Spare Ammo Container Lv.2
 {0x5DBC861Fu,BackpackKind::Module}, // Stabilizer Lv.1
 {0x0976E2F4u,BackpackKind::Module}, // Stabilizer Lv.2
 {0x7B1D61F7u,BackpackKind::Module}, // Stabilizer Lv.3
 {0x68BFF980u,BackpackKind::Module}, // Chiral Anti-Gravity Device
 {0x7BEF0A74u,BackpackKind::Module}, // Auto Revive Device
 {0x09848977u,BackpackKind::Module}, // Electric Shock Absorber
 {0x25FC3650u,BackpackKind::Module}, // Back Protector
 {0x4EEC75EBu,BackpackKind::Module}, // Solar Generator
 {0x365EAE27u,BackpackKind::Cover}, // Backpack Cover Lv.1
 {0x7A0BDD72u,BackpackKind::Cover}, // Backpack Cover Lv.2
 {0x780F7FE2u,BackpackKind::Charm}, // Cryptobiote
 {0x0A64FCE1u,BackpackKind::Charm}, // Love Knot
 {0x19340F15u,BackpackKind::Charm}, // Bell
 {0x6B5F8C16u,BackpackKind::Charm}, // Chiral Tea
 {0x3F95E8FDu,BackpackKind::Charm}, // Quokka
 {0x4DFE6BFEu,BackpackKind::Charm}, // Porter
 {0x5EAE980Au,BackpackKind::Charm}, // Electrobiote
 {0x2CC51B09u,BackpackKind::Charm}, // Boxing Kangaroos
};
constexpr u32 BackpackItemCount=sizeof(BackpackItems)/sizeof(BackpackItems[0]);
inline BackpackKind backpack_kind(u32 key) {
 for (const auto& item:BackpackItems) if(item.key==key) return item.kind;
 return BackpackKind::None;
}
inline Skip backpack_eligibility(const Settings&s,const Recipe&r) {
 if(!selected(s,r.key)) return Skip::NotSelected;
 if(r.usage!=5 || backpack_kind(r.key)==BackpackKind::None) return Skip::UnsupportedPath;
 if(!r.valid||!r.resource||!r.baggage) return Skip::MissingResource;
 if(r.dlc&1) return Skip::DlcSpecial;
 if(r.mission) return Skip::Quest;
 if(r.duplicate) return Skip::Duplicate;
 return Skip::None;
}
inline Build build_backpack(const Settings&s,const Recipe*r,u32 rn,
                            const Instance*native,u32 nn,Instance*out) {
 Build b={false,0,0,0,0};
 if(rn>MaxRecipes||nn>MaxMenu||(!native&&nn)||(!r&&rn)||!out) return b;
 // Original entries always win: a zero rule never relocks native progression.
 for(u32 i=0;i<nn;++i) out[b.count++]=native[i];
 for(u32 i=0;i<rn;++i) {
  if(backpack_eligibility(s,r[i])!=Skip::None) continue;
  bool present=false;
  for(u32 j=0;j<b.count;++j)
   if((out[j].flags&0xFFF)==(r[i].instance.flags&0xFFF)){present=true;break;}
  if(present) continue;
  ++b.eligible;
  if(b.count==MaxMenu){++b.overLimit;continue;}
  out[b.count]=r[i].instance;
  out[b.count++].flags|=0x8000u; // private working copy only
  ++b.added;
 }
 if(b.overLimit){b.count=nn;b.added=0;return b;}
 b.ok=true;return b;
}
}
