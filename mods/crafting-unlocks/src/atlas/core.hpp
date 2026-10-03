#pragma once
namespace craft {
using u8=unsigned char; using u16=unsigned short; using u32=unsigned int;
using u64=unsigned long long; using i32=int;
}
namespace equipment {
using namespace craft;
template<class T> T& at(void* p, u32 offset) { return *reinterpret_cast<T*>(static_cast<u8*>(p)+offset); }
template<class T> const T& at(const void* p, u32 offset) { return *reinterpret_cast<const T*>(static_cast<const u8*>(p)+offset); }
constexpr u8 BootId=103, SkeletonId=104;
constexpr u8 BootVisualId=20, SkeletonVisualId=26; // Pizza Baker / ordinary Boost Lv.3
constexpr float BootParams[6]={1.0f,0.5f,1.2f,0.5f,0.5f,0.0f};
constexpr float NativeBootParams[6]={0.0f,1.0f,1.0f,1.0f,1.0f,0.0f};
constexpr u32 BootBag=0x3B0ECB3E, SkeletonBag=0x4D918ADD;
inline bool targetSkeleton(const void* p) {
 return p && at<u8>(p,0x20)==SkeletonId && at<u8>(p,0x22)==6 && at<u8>(p,0x23)==1;
}
inline bool bootParametersCompatible(const float* values) {
 for(u32 i=0;i<6;++i) if(values[i]!=NativeBootParams[i] && values[i]!=BootParams[i]) return false;
 return true;
}
// Called after native capacity contributions, before weight ratios/overload are calculated.
// Extra flags are deliberately written AFTER the native mutually exclusive capacity paths.
inline bool combineLoading(void* out, const void* item, float battle, float boost, float bokka) {
 if(!out || !targetSkeleton(item) || at<u8>(item,0x21)!=2 || !at<u8>(out,0x6c) || at<u32>(out,0x80)!=2) return false;
 if(!(battle>=0 && battle<=1000 && boost>=0 && boost<=1000 && bokka>=0 && bokka<=1000)) return false;
 const u8 powered=at<u8>(out,0x6d);
 float best=battle;
 if(powered) { if(boost>best) best=boost; if(bokka>best) best=bokka; }
 const float delta=best-battle;
 const u32 offsets[]={0u,4u,0x4cu,0x50u};
 for(u32 offset : offsets) at<float>(out,offset)+=delta;
 at<u8>(out,0x66)=1; at<u8>(out,0x69)=1;
 at<u8>(out,0x68)=powered; at<u8>(out,0x6a)=powered;
 at<u32>(out,0x78)=2; at<u32>(out,0x7c)=2;
 return true;
}
}
