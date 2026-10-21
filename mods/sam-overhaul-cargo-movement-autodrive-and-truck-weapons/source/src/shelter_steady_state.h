#pragma once
// Pure, portable state matching used by Sam Overhaul's shelter steady-state
// path. Never cache merely by a pointer address: require owner, member-array,
// native Jolt BodyID, trigger/resource and expected dimensions to agree.
// Mismatches always request a full guarded native reconciliation.
#include <stdint.h>
namespace shelter_steady_state {
struct Key {
 uintptr_t shelter;
 uintptr_t owner;
 uintptr_t members;
 uintptr_t odradek;
 uintptr_t odradekResource;
 uintptr_t renderer;
 uintptr_t repair;
 uintptr_t repairResource;
 uintptr_t triggers[2];
 uintptr_t cylinders[2];
 uint32_t ids[2];
};
static inline bool nearly(float current,float expected) {
 return current>expected-0.06f && current<expected+0.06f;
}
static inline bool identity(const Key& saved,const Key& observed){
 if(!saved.shelter || !saved.owner || !saved.members ||
    !saved.odradek || !saved.odradekResource || !saved.renderer ||
    !saved.repair || !saved.repairResource)return false;
 if(saved.shelter!=observed.shelter ||
    saved.owner!=observed.owner ||
    saved.members!=observed.members ||
    saved.odradek!=observed.odradek ||
    saved.odradekResource!=observed.odradekResource ||
    saved.renderer!=observed.renderer ||
    saved.repair!=observed.repair ||
    saved.repairResource!=observed.repairResource)return false;
 for(unsigned i=0;i<2u;++i){
  if(!saved.triggers[i] || !saved.cylinders[i] ||
     saved.ids[i]==0xffffffffu ||
     observed.triggers[i]!=saved.triggers[i] ||
     observed.cylinders[i]!=saved.cylinders[i] ||
     observed.ids[i]!=saved.ids[i])return false;
 }
 return true;
}
static inline bool dimensions(float rain,float coat,float ringDiameter,
                              float ringResource,float ringComponent,
                              float cylinderRadius0,float cylinderHalfHeight0,
                              float cylinderRadius1,float cylinderHalfHeight1,
                              float expectedRain,float expectedCoat){
 return nearly(rain,expectedRain) &&
        nearly(coat,expectedCoat) &&
        nearly(ringDiameter,2.0f*expectedRain) &&
        nearly(ringResource,2.0f*expectedRain) &&
        nearly(ringComponent,2.0f*expectedRain) &&
        nearly(cylinderRadius0,expectedRain) &&
        nearly(cylinderRadius1,expectedRain) &&
        nearly(cylinderHalfHeight0,3.0f*expectedRain/4.0f) &&
        nearly(cylinderHalfHeight1,3.0f*expectedRain/4.0f);
}
}
