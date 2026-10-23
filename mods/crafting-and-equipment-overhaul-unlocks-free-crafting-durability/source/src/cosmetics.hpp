#pragma once
#include "core.hpp"
namespace craft {
// Verified native Usage=8 entries, not arbitrary character/NPC models.
// Identity is recipe NameCode + baggage NameCode + native customization category.
struct CosmeticItem {u32 key, baggageKey; u16 category;};
constexpr CosmeticItem CosmeticItems[] = {
 {0x2BF80B63u,0x0C0AEF30u,8}, // „Why Me?“-Kappe
 {0x59938860u,0x7E616C33u,8}, // Drawbridge-Kappe
 {0x4AC37B94u,0x6D319FC7u,8}, // Weihnachtsmannmütze
 {0x38A8F897u,0x1F5A1CC4u,6}, // „Ludens-Maske“-Sonnenbrille
 {0x6C629C7Cu,0x4B90782Fu,6}, // „Sam“-Sonnenbrille
 {0x710F5581u,0x48700DB9u,10}, // Schwarze Tarnung
 {0x625FA675u,0x5B20FE4Du,10}, // Anzug des Pizzabäckers
 {0x7A1AE505u,0x4365BD3Du,11}, // Normal
 {0x0364D682u,0x3A1B8EBAu,10}, // Wüstengelb
 {0x57AEB269u,0x6ED1EA51u,10}, // Schnee
 {0x25C5316Au,0x1CBA6952u,10}, // KI-Tarnung
 {0x3695C29Eu,0x0FEA9AA6u,10}, // Vierecke
 {0x44FE419Du,0x7D8119A5u,10}, // Wald
 {0x1AED7DB9u,0x23922581u,10}, // Wüstenrot
 {0x6886FEBAu,0x51F9A682u,10}, // Wüstengrün
 {0x10342576u,0x294B7D4Eu,10}, // Normal
 {0x1E091F7Fu,0x39FBFB2Cu,9}, // Otterkapuze
 {0x0D59EC8Bu,0x2AAB08D8u,9}, // Kamelkapuze
 {0x7F326F88u,0x58C08BDBu,7}, // Goldene Maske
 {0x212153ACu,0x06D3B7FFu,6}, // Schutzbrille
 {0x534AD0AFu,0x74B834FCu,7}, // Rote Maske
 {0x385A9314u,0x1FA87747u,8}, // Mütze des Pizzabäckers
 {0x4A311017u,0x6DC3F444u,8}, // Chiralspinne-Hut
 {0x5961E3E3u,0x7E9307B0u,8}, // Chiralnagetier-Hut
 {0x2B0A60E0u,0x0CF884B3u,8}, // Nousagi Hat
 {0x7FC0040Bu,0x5832E058u,8}, // Strickmütze
 {0x735E5747u,0x54ACB314u,11}, // Kohlenstoff
 {0x47B9FFDEu,0x604B1B8Du,11}, // Holzmaserung
 {0x541B67A9u,0x73E983FAu,11}, // Schillernd
 {0x33626D54u,0x14908907u,11}, // Kariert
 {0x20C0F523u,0x07321170u,11}, // Sticker
 {0x0B373C92u,0x2CC5D8C1u,11}, // Leder
 {0x0DAB8708u,0x2A59635Bu,6}, // Heartmans Brille (Normal)
 {0x1EFB74FCu,0x390990AFu,6}, // Heartmans herzförmige Brille
 {0x6C90F7FFu,0x4B6213ACu,6}, // Heartmans sternförmige Brille
 {0x3283CBDBu,0x15712F88u,6}, // Heartmans Brille (Blende)
 {0x40E848D8u,0x671AAC8Bu,6}, // Deadmans Brille
 {0x0CBD3B8Du,0x2B4FDFDEu,6}, // PINO-01
 {0x7ED6B88Eu,0x59245CDDu,6}, // TTL-02
 {0x6D864B7Au,0x4A74AF29u,7}, // Yōtei-Maske
 {0x0396BD01u,0x3AE9E539u,10}, // Junji Ito
 {0x71FD3E02u,0x4882663Au,10}, // Rettung
 {0x1FEDC879u,0x381F2C2Au,8}, // KJP-Kappe
 {0x69B87D72u,0x50C7254Au,14}, // Dollmans Outfit
 {0x2A1CDC65u,0x0DEE3836u,8}, // ROG-Kappe
 {0x58775F66u,0x7F85BB35u,8}, // Neils Bandana
 {0x06646342u,0x21968711u,8}, // Blaues Bandana
 {0x740FE041u,0x53FD0412u,8}, // Rotes Bandana
 {0x1F1FA3FAu,0x38ED47A9u,8}, // Erfolgsbandana
 {0x62ADCDF6u,0x5BD295CEu,10}, // ROG
 {0x10C64EF5u,0x29B916CDu,10}, // Verknüpfen
 {0x440C2A1Eu,0x7D737226u,10}, // Tag der Toten
};
constexpr u32 CosmeticItemCount=sizeof(CosmeticItems)/sizeof(CosmeticItems[0]);
struct CosmeticCandidate {Recipe recipe; u16 category;};
inline const CosmeticItem* cosmetic_item(u32 key){
 for(const auto& i:CosmeticItems)if(i.key==key)return &i;return nullptr;
}
inline Skip cosmetic_eligibility(const Settings&s,const CosmeticCandidate&c){
 const Recipe&r=c.recipe;
 if(!s.cosmeticsEnabled||!selected(s,r.key))return Skip::NotSelected;
 const CosmeticItem*i=cosmetic_item(r.key);
 if(r.usage!=8||!i||i->baggageKey!=r.baggageKey||i->category!=c.category)return Skip::UnsupportedPath;
 if(!r.valid||!r.resource||!r.baggage||r.caseType!=0||!r.instance.vtable)return Skip::MissingResource;
 if(r.dlc&1)return Skip::DlcSpecial;
 if(r.mission)return Skip::Quest;
 if(r.duplicate)return Skip::Duplicate;
 return Skip::None;
}
inline Build build_cosmetics(const Settings&s,const CosmeticCandidate*r,u32 rn,
                             const Instance*native,u32 nn,Instance*out){
 Build b={false,0,0,0,0};
 if(rn>CosmeticItemCount||nn>MaxMenu||(!native&&nn)||(!r&&rn)||!out)return b;
 for(u32 j=0;j<nn;++j)out[b.count++]=native[j];
 for(u32 i=0;i<rn;++i){
  if(cosmetic_eligibility(s,r[i])!=Skip::None)continue;
  const Recipe&q=r[i].recipe;bool present=false;
  for(u32 j=0;j<b.count;++j)if((out[j].flags&0xFFF)==(q.instance.flags&0xFFF)){present=true;break;}
  if(present)continue;
  ++b.eligible;
  if(b.count==MaxMenu){++b.overLimit;continue;}
  out[b.count]=q.instance;out[b.count].padding=0;
  out[b.count++].flags|=0x8000u; // Native producer's working-copy bit. No master flag write.
  ++b.added;
 }
 if(b.overLimit){b.count=nn;b.added=0;return b;}
 b.ok=true;return b;
}
inline bool cosmetic_colors_enabled(const Settings&s){
 return s.enabled&&s.all&&s.cosmeticsEnabled&&s.cosmeticsColors;
}
inline bool cosmetic_color_override(bool enabled,u32 owner,u32 caller){
 return enabled&&owner!=0&&owner==caller;
}
}
