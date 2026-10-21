#pragma once
#include "core.hpp"
namespace silenced {
constexpr u32 SuppressorHash=code("MESH_Suppressor");
// A generic child entity carries only the donor suppressor. Its skeleton and
// mesh bindings stay together; the parent weapon's resource graph is never edited.
struct alignas(16) AttachmentGraph {
 alignas(8) u8 child[0x50],entity[0xB8];
 alignas(16) u8 mover[0x130];
 alignas(8) u8 skinned[0xC8],data[0xB8],model[0xA0];
 alignas(16) u8 part[0xE0];
 u64 components[2],partRefs[1];u32 names[1];i32 parents[1];
};
inline void attachmentArray(void*p,u32 offset,u32 count,const void*values){
 at<u32>(p,offset)=count;at<u32>(p,offset+4)=count;at<u64>(p,offset+8)=u64(values);
}
inline void attachmentIdentity(void*p,u32 kind,u32 variant=1){
 identity(p,variant,kind);
 // +0xC belongs to the native streaming manager, not to the serialized asset.
 // A copied nonzero value can route our allocation into the donor's backend.
 at<u32>(p,0xC)=0;
}
inline bool attachmentEligible(i32 variant,u64 flags,u64 parentType,bool renderReady,bool existing){
 return variant>=1&&variant<i32(ActiveCount)&&(flags&(1ull<<27))&&(flags&(1ull<<28))&&
        !(flags&(1ull<<45))&&parentType==0x434B600&&renderReady&&!existing;
}
// The three fresh resources must first receive their native constructors.
// The caller validates and pins every native donor before publishing the graph.
inline bool attachmentGraph(AttachmentGraph&g,const void*skinned,const void*data,
 const void*model,const void*part,const void*helperLink,u32 variant=1){
 if(variant<1||variant>=ActiveCount||!skinned||!data||!model||!part||!helperLink||!at<u64>(g.child,0)||
    !at<u64>(g.entity,0)||!at<u64>(g.mover,0))return false;
 if(at<u64>(skinned,0x60)!=u64(data)||at<u64>(data,0x20)!=u64(model)||
    at<u32>(skinned,0x28)||at<u32>(skinned,0x38)||at<u32>(skinned,0x50)||
    at<u32>(skinned,0x70)||at<u32>(skinned,0x98)||at<u64>(skinned,0xA8)||at<u64>(skinned,0xB8)||
    !at<u64>(skinned,0x68)||!at<u64>(skinned,0x90)||
    at<u32>(data,0x28)||at<u32>(data,0x60)||at<u32>(data,0x78)||at<u32>(data,0xAC)||
    at<u32>(part,0xD0)!=SuppressorHash||at<u32>(part,0x80)||!at<u64>(part,0x20)||
    !at<u64>(model,0x48)||!at<u64>(model,0x50)||
    !(at<u64>(helperLink,0)|at<u64>(helperLink,8)))return false;
 copy(g.skinned,skinned,sizeof(g.skinned));copy(g.data,data,sizeof(g.data));
 copy(g.model,model,sizeof(g.model));copy(g.part,part,sizeof(g.part));
 void*resources[]={g.child,g.entity,g.mover,g.skinned,g.data,g.model,g.part};
 for(u32 i=0;i<7;++i)attachmentIdentity(resources[i],60+i,variant);
 g.components[0]=u64(g.skinned);g.components[1]=u64(g.data);
 g.partRefs[0]=u64(g.part);g.names[0]=SuppressorHash;g.parents[0]=-1;
 at<u64>(g.child,0x20)=u64(g.entity);at<u64>(g.child,0x28)=u64(g.mover);
 attachmentArray(g.entity,0x48,2,g.components);g.entity[0x3C]=1;
 at<u64>(g.skinned,0x60)=u64(g.data);g.skinned[0x80]=1;
 at<u64>(g.data,0x20)=u64(g.model);
 attachmentArray(g.data,0x88,1,g.names);attachmentArray(g.data,0x98,1,g.parents);
 at<u32>(g.data,0xA8)=1;
 attachmentArray(g.model,0x20,1,g.partRefs);attachmentArray(g.model,0x38,1,g.partRefs);
 at<u32>(g.model,0x90)=1;g.part[0xD4]=0;
 copy(g.mover+0x80,helperLink,80);
 // Fit the AR collar to the actual weapon's animated muzzle helper. The MG
 // keeps its verified transform; the handgun uses a proportional smaller can.
 // Shotgun/handgun placement still requires a test through their animations.
 float scale=variant==3?0.65f:1.f;
 for(u32 i=0;i<16;++i)at<float>(g.mover,0x30+4*i)=(i%5==0)?(i==15?1.f:scale):0.f;
 at<float>(g.mover,0x64)=-0.444f*scale;at<float>(g.mover,0x68)=-0.065f*scale;
 return true;
}
}
