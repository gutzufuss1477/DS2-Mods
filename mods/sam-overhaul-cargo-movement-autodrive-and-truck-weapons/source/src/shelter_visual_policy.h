#pragma once
// Sam Overhaul / DS2 Steam 1.10.89.0
// Odradek component/resource "OverrideSize" can be doubled by the engine
// when its separate draw-instance is created. The renderer *diameter* must
// equal 2x protected 8m radius, NOT 4x the radius.
// Reject arbitrary sizes rather than modifying unrelated visual effects.
namespace sam_shelter_visual {
static inline bool within_tolerance(float a,float b){
 return a>b-0.06f && a<b+0.06f;
}
static inline bool accept_renderer_correction(float actual,float radius){
 if(radius<4.0f || radius>16.0f)return false;
 const float expectedDiameter=2.0f*radius;
 return within_tolerance(actual,expectedDiameter)
      || within_tolerance(actual,radius)
      || within_tolerance(actual,4.0f)
      || within_tolerance(actual,2.0f*expectedDiameter);
}
static inline float renderer_diameter(float radius){
 return 2.0f*radius;
}
}
