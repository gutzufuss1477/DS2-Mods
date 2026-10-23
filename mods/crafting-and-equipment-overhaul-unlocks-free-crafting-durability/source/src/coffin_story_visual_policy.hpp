#pragma once
// DS2 v1.10.89.0 - visual-only native coffin resource selection.
// ModelType 30 remains the gameplay/vehicle owner; ModelType 31 is used only
// for the first new, validated red board's baggage-view renderer.
namespace coffin_story {
using U8=unsigned char;
using U32=unsigned int;
using U64=unsigned long long;
constexpr U32 NativeBaggageKey=0x2A889790u;
constexpr U8 CoffinCase=18,NormalRegion=0,PersonalAttribute=1;
constexpr U8 CoffinboardModelType=30,StoryCoffinBaggageModelType=31;
constexpr U32 StoryVisualCallsiteRva=0x1236CC6u;
constexpr U32 ModelSelectorRva=0x11DCB00u;
constexpr U8 ExpectedVisualCallsite[5]={0xE8,0x35,0x5E,0xFA,0xFF};
constexpr U64 NativeBaggageEntityUUID[2]={0x5640AC84C641A1ABull,0x808A4CF30D811582ull}; // 499:87142
constexpr U64 StoryBaggageEntityUUID[2]={0x5B400D8DDA732838ull,0x35AF14AEA031DA80ull}; // 8542:46
constexpr U64 StoryBaggageVisualUUID[2]={0x534B082C9E07915Dull,0xF3B1EF47B6A8448Full}; // 8542:64
constexpr U64 StoryWindowMilliseconds=90000ull;
inline bool knownNativeBag(U32 id,U8 attribute,U8 casetype,U8 region,U32 missionId){
 return id==NativeBaggageKey&&attribute==PersonalAttribute&&
        casetype==CoffinCase&&region==NormalRegion&&missionId==0;
}
inline bool knownOriginalEntity(U64 low,U64 high){
 return low==NativeBaggageEntityUUID[0]&&high==NativeBaggageEntityUUID[1];
}
inline bool knownStoryEntity(U64 low,U64 high){
 return low==StoryBaggageEntityUUID[0]&&high==StoryBaggageEntityUUID[1];
}
inline bool selectStoryOnlyForNewVisual(bool armed,bool validBag,bool firstBuild,
 bool stockDefault,bool storyResourceValid,bool inWindow){
 return armed&&validBag&&firstBuild&&stockDefault&&storyResourceValid&&inWindow;
}
}
