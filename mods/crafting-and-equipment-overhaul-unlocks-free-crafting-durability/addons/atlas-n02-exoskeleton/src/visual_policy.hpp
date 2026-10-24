#pragma once
#include "core.hpp"
namespace chiral {
using namespace craft;
constexpr u8 AtlasId=103, MarkerId=11;
constexpr u32 RowSize=0x28, MaxRows=64;
template<class T>T& at(void*p,u32 offset){return *(T*)((u8*)p+offset);}
template<class T>const T& at(const void*p,u32 offset){return *(const T*)((const u8*)p+offset);}
inline void copy(void*d,const void*s,u32 n){for(u32 i=0;i<n;++i)((u8*)d)[i]=((const u8*)s)[i];}
inline bool same(const void*a,const void*b,u32 n){for(u32 i=0;i<n;++i)if(((const u8*)a)[i]!=((const u8*)b)[i])return false;return true;}
inline bool rowIndices(const u8* rows,u32 count,i32& atlas,i32& donor) {
 atlas=donor=-1;if(!rows||!count||count>MaxRows)return false;
 for(u32 i=0;i<count;++i){
  if(rows[i*RowSize]==AtlasId){if(atlas>=0)return false;atlas=i;}
  if(rows[i*RowSize]==MarkerId){if(donor>=0)return false;donor=i;}
 }
 return atlas>=0&&donor>=0;
}
inline bool makeMarkerRows(u8* out,const u8* rows,u32 count) {
 i32 atlas,donor;if(!out||!rowIndices(rows,count,atlas,donor))return false;
 copy(out,rows,count*RowSize);copy(out+atlas*RowSize,rows+donor*RowSize,RowSize);
 out[atlas*RowSize]=AtlasId;return true;
}
inline bool markerNames(const u32* names,u32 count,bool hanging){
 constexpr u32 worn[]={1063881013,55570813,254575095,856357311,983316578,112869418,991501977,121595601};
 constexpr u32 hung[]={824723485,932823509};
 const u32 n=hanging?2:8;if(!names||count!=n)return false;
 for(u32 i=0;i<n;++i)if(names[i]!=(hanging?hung[i]:worn[i]))return false;
 return true;
}
inline bool absoluteHostRelay(const u8* stub,u64 host,u32 hostSize,u64& function){
 const u8 jump[]={0xff,0x25,0,0,0,0};
 if(!stub||!same(stub,jump,6)||!hostSize)return false;
 function=at<u64>(stub,6);return function>=host&&function-host<hostSize;
}
inline bool relativeJump(u8* out,u64 from,u64 to){
 const long long d=(long long)to-(long long)(from+5);
 if(d<(-2147483647LL-1)||d>2147483647LL)return false;
 out[0]=0xe9;at<i32>(out,1)=i32(d);return true;
}
}
