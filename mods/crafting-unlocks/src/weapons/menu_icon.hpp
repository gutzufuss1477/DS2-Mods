#pragma once
#include "attachment.hpp"
namespace silenced {
// Engine String references live at data-16, cached CRC at data-12.
struct NativeIconString {u32 refs,hash,length,capacity;char data[64];};
template<u32 N>constexpr NativeIconString iconString(const char(&name)[N]){
 NativeIconString s={1,code(name),N-1,63,{}};for(u32 i=0;i<N;++i)s.data[i]=name[i];return s;
}
constexpr NativeIconString CustomIconStrings[]={
 iconString("ut_ui_icon_wep_shg0_v02_msg_red"),iconString("ut_ui_icon_wep_hag6_msg_red")
};
constexpr NativeIconString SourceIconStrings[]={
 iconString("ut_ui_icon_wep_shg0_v02"),iconString("ut_ui_icon_wep_hag6")
};
constexpr u32 PrivateIconCount=2;
constexpr u64 IconUuids[PrivateIconCount][2]={
 {0xCD48AD984C1ED9F8ull,0x3E233F140B5E7787ull}, // 56:110242, shotgun Lv.2
 {0x144F8DD1B11F5FA7ull,0x59F1636F947284A3ull}  // 56:75928, big-bore handgun
};
constexpr u32 IconSite=0x17BE390;
constexpr u8 IconExpected[]={0x48,0x89,0x5C,0x24,0x08};
inline i32 customMenuIcon(const char*name,u32 length,u32 hash){
 if(!name)return -1;
 for(u32 k=0;k<PrivateIconCount;++k){const auto&s=CustomIconStrings[k];
  if(length!=s.length||(hash!=s.hash&&hash!=0xFFFFFFFFu))continue;
  bool same=true;for(u32 i=0;i<=length;++i)if(name[i]!=s.data[i]){same=false;break;}
  if(same)return i32(k);
 }
 return -1;
}
inline bool assignPrivateIcon(Bundle&b,u32 variant,const char*name){
 if(variant<2||variant>=ActiveCount||!name||at<u16>(b.weapon,0x20)!=Definitions[variant].id||
    at<u32>(b.list,0x40)!=Definitions[variant].newListId)return false;
 at<u64>(b.list,0x50)=u64(name);return true;
}
struct alignas(16) PrivateIconGraph {alignas(8)u8 ui[0x38],texture[0x70],gpu[0x188];};
inline bool privateIconGraph(PrivateIconGraph&g,u32 slot,const void*ui,const void*gpu,u64 resource,u64 descriptor){
 if(slot>=PrivateIconCount||!ui||!gpu||!resource||!descriptor||!at<u64>(g.ui,0)||!at<u64>(g.texture,0)||!at<u64>(g.gpu,0))return false;
 if(at<u64>(ui,16)!=IconUuids[slot][0]||at<u64>(ui,24)!=IconUuids[slot][1]||at<u8>(ui,0x20)||
    at<u32>(ui,0x24)!=256||at<u32>(ui,0x28)!=160||at<u8>(gpu,0x29)!=0x4B)return false;
 attachmentIdentity(g.ui,72,slot+2);attachmentIdentity(g.texture,73,slot+2);
 copy(g.ui+0x24,(const u8*)ui+0x24,8);at<u64>(g.ui,0x30)=u64(g.texture);
 at<u64>(g.texture,0x20)=u64(g.gpu);
 copy(g.gpu+0x28,(const u8*)gpu+0x28,12);
 at<u32>(g.gpu,8)=1;at<u32>(g.gpu,0x10)=2;
 // Physical 512x320 pixels, native logical icon extent remains 256x160.
 at<u32>(g.gpu,0x2C)=0x20000000u|512|(320<<15);
 at<u32>(g.gpu,0x30)=1;at<u32>(g.gpu,0x34)=0;g.gpu[0x58]=2;
 at<u64>(g.gpu,0x80)=resource;at<u64>(g.gpu,0x90)=descriptor;
 return true;
}
}
