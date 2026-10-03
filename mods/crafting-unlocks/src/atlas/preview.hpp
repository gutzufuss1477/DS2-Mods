#pragma once
#include "core.hpp"
namespace equipment {
struct PreviewRow {u8 type;i32 before,after;};
constexpr PreviewRow BootPreview[6]={{14,4,8},{7,5,8},{13,4,8},{6,16,16},{11,4,8},{15,4,12}};
// Portability stays at the native L-size rating. The base model/packing size
// remains L, even though the equipment's weight has been reduced to 4 kg.
constexpr PreviewRow SkeletonPreview[6]={{16,3,10},{7,12,16},{13,4,16},{6,4,4},{17,10,16},{12,8,12}};
constexpr char BootDescription[]=u8"ATLAS-Ausrüstung: sicherer Halt, hohe Stoßabsorption, stärkere Tritte und leise Schritte. Gewicht: 0,2 kg. Haltbarkeit: 3400.";
constexpr char SkeletonDescription[]=u8"ATLAS-Ausrüstung mit Kampf-, Boost- und Bokka-Effekten auf Stufe 3: schnelle Bewegung, stärkere Sprünge, Geländehilfe und elektromagnetischer Schutz. Verbraucht Akku. Tragkraftbonus: bis zu +180 kg mit Akku, +100 kg ohne Akku. Gewicht: 4,0 kg. Haltbarkeit: 20000. Packgröße: L.";
// These small resource copies retain a permanent reference for process lifetime.
// Game-owned fields get their own references. Thus native resource destructors
// never run on mod-owned storage, including during a level/resource unload.
struct PreviewBundle {
 u64 originalDescription,originalChart;
 u8 itemId;
 alignas(8) u8 description[0x38];
 alignas(8) u8 chart[0x30];
 alignas(8) u8 parameters[6][0x30];
 u64 parameterPointers[6];
 char text[512];
};
static_assert(sizeof(SkeletonDescription)<=512,"Preview description capacity");
inline void previewCopy(void*d,const void*s,u32 n){auto*a=(u8*)d;auto*b=(const u8*)s;for(u32 i=0;i<n;++i)a[i]=b[i];}
inline bool preparePreview(PreviewBundle& out,u8 itemId,const u8* description,const u8* chart,const u8 (&parameters)[6][0x30]){
 if(itemId!=BootId && itemId!=SkeletonId)return false;
 if(at<i32>(chart,0x20)!=6)return false;
 const PreviewRow* rows=itemId==BootId?BootPreview:SkeletonPreview;
 // Require the exact native chart. A conflicting mod is never silently replaced.
 for(u32 i=0;i<6;++i){
  if(at<u8>(parameters[i],0x20)!=rows[i].type || at<i32>(parameters[i],0x24)!=rows[i].before || at<u64>(parameters[i],0x28)!=0)return false;
 }
 out.itemId=itemId;
 previewCopy(out.description,description,0x38);at<u32>(out.description,8)=1;
 const char*text=itemId==BootId?BootDescription:SkeletonDescription;
 const u32 count=itemId==BootId?sizeof(BootDescription)-1:sizeof(SkeletonDescription)-1;
 previewCopy(out.text,text,count+1);
 at<u64>(out.description,0x20)=u64(out.text);at<u32>(out.description,0x28)=count;
 previewCopy(out.chart,chart,0x30);at<u32>(out.chart,8)=1;
 at<u64>(out.chart,0x28)=u64(out.parameterPointers);
 for(u32 i=0;i<6;++i){
  previewCopy(out.parameters[i],parameters[i],0x30);at<u32>(out.parameters[i],8)=1;
  at<i32>(out.parameters[i],0x24)=rows[i].after;
  out.parameterPointers[i]=u64(out.parameters[i]);
 }
 return true;
}
}
