#pragma once
#include "attachment.hpp"
namespace silenced {
// Big-bore handgun (hag6); the retired machine pistol (hag5) is untouched.
constexpr u64 PistolSetId[]={0x3B4D047E67854C25ull,0xB26415E927A95590ull};
constexpr u64 PistolColorId[]={0x243A7CEAC58FA041ull,0x3B7869852956F98Aull}; // 499:40075
struct alignas(16) PistolGraph {
 alignas(8) u8 set[0x70],color[0x70],gpu[0x188],entries[72];
 u32 targetCount=0,targetCapacity=16;u32*targetPointer=nullptr;u32 targets[16]={};
 u32 stringHash=code("TextureSetBindings"),stringLength=18,stringRefs=1;
 char stringData[19]="TextureSetBindings";const char*binding=nullptr;
};
inline bool pistolGraph(PistolGraph&g,const void*set,const void*entries,const void*color,const void*gpu,u64 resource,u64 descriptor){
 if(!set||!entries||!color||!gpu||!resource||!descriptor||!at<u64>(g.gpu,0))return false;
 if(at<u64>(set,16)!=PistolSetId[0]||at<u64>(set,24)!=PistolSetId[1]||at<u32>(set,32)!=3||
    at<u64>(color,16)!=PistolColorId[0]||at<u64>(color,24)!=PistolColorId[1]||
    at<u64>(entries,24+8)!=u64(color)||at<u32>(entries,24)!=807473409||
    at<u8>(gpu,0x29)!=0x42||!at<u64>(set,0x68))return false;
 copy(g.set,set,0x70);copy(g.entries,entries,72);
 attachmentIdentity(g.set,70,3);attachmentIdentity(g.color,71,3);
 // Streaming ownership belongs to the original assets. Our color is resident
 // on its own committed allocation and never joins that streaming graph.
 for(u32 i=0x30;i<0x48;++i)g.set[i]=0;
 attachmentArray(g.set,0x20,3,g.entries);at<u64>(g.entries,32)=u64(g.color);
 at<u64>(g.color,0x20)=u64(g.gpu);at<u64>(g.color,0x28)=u64(g.set);
 copy(g.gpu+0x28,(const u8*)gpu+0x28,12);
 at<u32>(g.gpu,8)=1;at<u32>(g.gpu,0x10)=2;
 at<u32>(g.gpu,0x30)=(at<u32>(gpu,0x30)&0xFE000000u)|1;
 at<u32>(g.gpu,0x34)=0;at<u32>(g.gpu,0x6C)=0;
 // Native IsLoaded (0x21124D0) checks this byte. Publish only after the fence.
 g.gpu[0x58]=2;
 at<u64>(g.gpu,0x80)=resource;at<u64>(g.gpu,0x90)=descriptor;
 g.targetPointer=g.targets;g.binding=g.stringData;
 return true;
}
static_assert(__builtin_offsetof(PistolGraph,stringData)-__builtin_offsetof(PistolGraph,stringHash)==12,"Native String header");
}
