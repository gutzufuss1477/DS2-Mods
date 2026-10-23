#pragma once
// Pure, fail-closed in-memory transformation of DS2 1.10.89.0's ORIGINAL StreamingGraphResource.
// It reproduces the validated Warrior 9676 + Higgs 2797 graph without changing the original file.
// All binary offsets are from the SHA-pinned original ac0c824e... streaming_graph.core.
namespace vgraph {
using u8 = unsigned char;
using u32 = unsigned int;
constexpr u32 OriginalSize=28334985u, PatchedSize=28363389u;
constexpr u32 Group499=18589873u, Subgroups=23634861u, SubgroupsBytes=2215380u;
constexpr u32 OriginalCount=553844u, ParentStart=14248u, ParentCount=2365u;
constexpr u32 ChildA=9676u, ChildB=2797u, ChildC=8542u;
constexpr u32 AppendA=(ParentCount+1u)*4u,AppendB=(ParentCount+2u)*4u;
constexpr u32 AppendC=(ParentCount+3u)*4u,AppendBytes=AppendA+AppendB+AppendC;
constexpr u32 EndSubgroups=Subgroups+SubgroupsBytes;
constexpr u32 ChildOffset=Subgroups+4u+ParentStart*4u;
static_assert(OriginalSize+AppendBytes==PatchedSize,"Native file-size invariant");
inline u32 get32(const u8*p){return u32(p[0])|(u32(p[1])<<8)|(u32(p[2])<<16)|(u32(p[3])<<24);}
inline void set32(u8*p,u32 x){for(u32 i=0;i<4;++i)p[i]=u8(x>>(8*i));}
inline void bytes(u8*dst,const u8*src,u32 n){for(u32 i=0;i<n;++i)dst[i]=src[i];}
inline bool originalShape(const u8*p,u32 n){
 if(!p||n!=OriginalSize||get32(p+8)!=n-16u||get32(p+Group499)!=499u||
    get32(p+Group499+16)!=ParentStart||get32(p+Group499+20)!=ParentCount||
    get32(p+Subgroups)!=OriginalCount||EndSubgroups+16>=n)return false;
 for(u32 i=0;i<ParentCount;++i){
  const u32 v=get32(p+ChildOffset+i*4);
  if(v==ChildA||v==ChildB||v==ChildC)return false;
 }
 return true;
}
inline bool build(const u8*old,u32 n,u8*out,u32 capacity){
 if(!out||capacity<PatchedSize||!originalShape(old,n))return false;
 bytes(out,old,EndSubgroups);
 const u32 first=EndSubgroups,second=first+AppendA,third=second+AppendB,segment=ParentCount*4;
 bytes(out+first,old+ChildOffset,segment);
 set32(out+first+segment,ChildA);
 bytes(out+second,old+ChildOffset,segment);
 set32(out+second+segment,ChildA);
 set32(out+second+segment+4u,ChildB);
 bytes(out+third,old+ChildOffset,segment);
 set32(out+third+segment,ChildA);
 set32(out+third+segment+4u,ChildB);
 set32(out+third+segment+8u,ChildC);
 bytes(out+EndSubgroups+AppendBytes,old+EndSubgroups,n-EndSubgroups);
 set32(out+8u,get32(old+8)+AppendBytes);
 set32(out+Subgroups,OriginalCount+ParentCount*3u+6u);
 set32(out+Group499+16u,OriginalCount+ParentCount*2u+3u);
 set32(out+Group499+20u,ParentCount+3u);
 return get32(out+Subgroups)==560945u&&get32(out+Group499+16u)==558577u&&
        get32(out+Group499+20u)==2368u;
}
}
