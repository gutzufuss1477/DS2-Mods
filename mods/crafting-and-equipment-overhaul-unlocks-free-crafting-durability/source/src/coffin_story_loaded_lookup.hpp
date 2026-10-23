#pragma once
#include "coffin_story_visual_policy.hpp"
#include "coffin_direct_assets.hpp"
// Read-only in-process resource-group lookup. Tested against passive DS2
// 1.10.89.0 snapshots (resource group 499 and the streamed 8542 group).
// No world manager table mutation, no newly fabricated game resource.
namespace coffin_story {
constexpr U64 GroupPageAlignment=0x10000ull;
constexpr U64 GroupScanRadius=32ull*1024*1024;
constexpr U32 NormalGroupId=499u,StoryGroupId=8542u;
constexpr U32 NormalGroupResourceCount=124219u,StoryGroupResourceCount=coffin_direct_assets::Count;
constexpr U32 NormalBagIndex=112987u,StoryBagIndex=64u,StoryEntityIndex=46u;
constexpr U32 BaggageVtableRva=0x3278C20u,EntityVtableRva=0x311B4F0u;
constexpr U64 NormalBaggageUUID[2]={0xD64EDCEA577426E6ull,0xAEB289638ABC14A8ull};
struct LoadedStory {
 U64 bag=0,entity=0,normalGroup=0,storyGroup=0;
 constexpr bool ready()const{return bag!=0 && entity!=0&&normalGroup!=0&&storyGroup!=0;}
};
template<class R,class T> bool get(R&reader,U64 where,T &result){
 if(where<0x10000ull||where>0x00007FFFFFFFFFFFull-sizeof(T))return false;
 return reader(where,result);
}
template<class R>bool isGroup(R&read,U64 addr,U32 id,U32 resources,
 U32 index,U64 expectedResource,const U64 uuid[2],U64&table){
 U32 n=0,cap=0,gid=0;
 U64 ptr=0,current=0,low=0,high=0;
 if(!get(read,addr+0x40,n)||n!=resources||
    !get(read,addr+0x44,cap)||cap!=resources||
    !get(read,addr+0x48,ptr)||ptr<0x10000ull||
    !get(read,addr+0x64,gid)||gid!=id||
    !get(read,ptr+8ull*index,current)||current<0x10000ull||
    (expectedResource&&current!=expectedResource)||
    !get(read,current+0x10,low)||!get(read,current+0x18,high)||
    low!=uuid[0]||high!=uuid[1])return false;
 table=ptr;return true;
}
// Exactly one matching group or none. A corrupted/multiple result fails closed.
template<class R>U64 groupNear(R&read,U64 anchor,U32 id,U32 count,
 U32 index,U64 resource,const U64 expectedUuid[2]){
 if(anchor<GroupScanRadius+0x10000ull ||
    anchor>=0x00007FFFFFFFFFFFull-GroupScanRadius)return 0;
 const U64 center=anchor & ~(GroupPageAlignment-1ull);
 U64 chosen=0;
 for(U32 i=0;i<=2u*U32(GroupScanRadius/GroupPageAlignment);++i){
  U64 a=center-GroupScanRadius+U64(i)*GroupPageAlignment;
  U64 table=0;
  if(isGroup(read,a,id,count,index,resource,expectedUuid,table)){
   if(chosen)return 0;
   chosen=a;
  }
 }
 return chosen;
}
template<class R>LoadedStory findLoaded(R&read,U64 executable,U64 nativeBag){
 LoadedStory o={};
 U64 ptr=0,low=0,high=0,vt=0;
 U8 type=0;
 U32 nativeGroup=0;
 if(executable<0x10000ull||nativeBag<0x10000ull ||
    !get(read,nativeBag,vt)||vt!=executable+BaggageVtableRva||
    !get(read,nativeBag+0x0c,nativeGroup)||nativeGroup!=NormalGroupId||
    !get(read,nativeBag+0x10,low)||!get(read,nativeBag+0x18,high)||
    low!=NormalBaggageUUID[0]||high!=NormalBaggageUUID[1]||
    !get(read,nativeBag+0x20,type)||type!=CoffinboardModelType)return o;
 const U64 original=groupNear(read,nativeBag,NormalGroupId,
    NormalGroupResourceCount,NormalBagIndex,nativeBag,NormalBaggageUUID);
 if(!original)return o;
 const U64 scene=groupNear(read,original,StoryGroupId,StoryGroupResourceCount,
    StoryBagIndex,0,StoryBaggageVisualUUID);
 if(!scene)return o;
 if(!get(read,scene+0x48,ptr)||ptr<0x10000ull)return o;
 U64 story=0,entity=0,viaBag=0;
 U32 storyGroup=0,entityGroup=0,parts=0;
 if(!get(read,ptr+8ull*StoryBagIndex,story)||story<0x10000ull||
    !get(read,ptr+8ull*StoryEntityIndex,entity)||entity<0x10000ull||
    !get(read,story,vt)||vt!=executable+BaggageVtableRva||
    !get(read,story+0x0c,storyGroup)||storyGroup!=StoryGroupId||
    !get(read,story+0x20,type)||type!=StoryCoffinBaggageModelType||
    !get(read,story+0x28,viaBag)||viaBag!=entity||
    !get(read,entity,vt)||vt!=executable+EntityVtableRva||
    !get(read,entity+0x0c,entityGroup)||entityGroup!=StoryGroupId||
    !get(read,entity+0x10,low)||!get(read,entity+0x18,high)||
    low!=StoryBaggageEntityUUID[0]||high!=StoryBaggageEntityUUID[1]||
    !get(read,entity+0x48,parts)||parts!=11u||
    !get(read,entity+0x50,viaBag)||viaBag<0x10000ull)return o;
 o.bag=story;o.entity=entity;o.normalGroup=original;o.storyGroup=scene;
 return o;
}
}
