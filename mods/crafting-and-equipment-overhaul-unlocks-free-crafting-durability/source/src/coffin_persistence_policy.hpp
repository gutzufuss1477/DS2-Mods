#pragma once
#include "coffin_session_policy.hpp"
#include "sha256.hpp"
// Metadata belongs to the mod. The native 0x1d0-byte save record is only read.
namespace coffin_persistence {
using namespace craft;
constexpr u32 RecordSize=0x1d0,Capacity=16384,LoadedCapacity=256;
constexpr u64 Magic=0x3156524346325344ull; // DS2FCRV1
struct Entry {u8 digest[32]={};u64 cargo=0;u32 red=0,reserved=0;};
struct Header {u64 magic=Magic;u32 version=1,count=0;u8 digest[32]={};};
static_assert(sizeof(Entry)==48&&sizeof(Header)==48);
inline bool validCargo(u64 id){return id&&id!=~u64(0);}
inline bool bytesEqual(const u8*a,const u8*b,u32 n){for(u32 i=0;i<n;++i)if(a[i]!=b[i])return false;return true;}
inline Entry fingerprint(const u8*record){
 Entry e;u32 key=0;for(u32 i=0;i<4;++i)key|=u32(record[0x11c+i])<<(8*i);
 if(key!=0x2a889790u)return e;
 for(u32 i=0;i<8;++i)e.cargo|=u64(record[0xd0+i])<<(8*i);
 if(!validCargo(e.cargo))return {};
 Sha256 sha;sha.update(record,RecordSize);sha.finish(e.digest);return e;
}
inline bool same(const Entry&a,const Entry&b){return validCargo(a.cargo)&&a.cargo==b.cargo&&bytesEqual(a.digest,b.digest,32);}
inline int find(const Entry*entries,u32 n,const Entry&e){if(n>Capacity)return -1;for(u32 i=0;i<n;++i)if(same(entries[i],e))return int(i);return -1;}
// Normal records cannot inherit a red marker even if native bytes collide.
// A conflict is retained as native, rather than recoloring an unrelated item.
inline bool put(Entry*entries,u32&n,Entry e,bool red){
 if(!validCargo(e.cargo)||n>Capacity)return false;
 const int at=find(entries,n,e);e.red=red?1:0;
 if(at>=0){if(entries[at].red==e.red)return false;entries[at].red=e.red;return true;}
 if(!red||n==Capacity)return false;entries[n++]=e;return true;
}
inline Header seal(const Entry*entries,u32 n){Header h;h.count=n;Sha256 sha;sha.update(entries,n*sizeof(Entry));sha.finish(h.digest);return h;}
inline bool valid(const Header&h,const Entry*entries,u64 size){
 if(h.magic!=Magic||h.version!=1||h.count>Capacity||size!=sizeof(Header)+u64(h.count)*sizeof(Entry))return false;
 const auto expected=seal(entries,h.count);if(!bytesEqual(h.digest,expected.digest,32))return false;
 for(u32 i=0;i<h.count;++i){if(!validCargo(entries[i].cargo)||entries[i].red>1||entries[i].reserved)return false;}
 return true;
}
// Saved registry rows hold QPID before their live owner pointer is populated.
// The measured QPID persists across restart; the live 128-bit GUID does not.
template<class R>u64 worldCargo(R&read,u64 image,u64 vehicle){
 u64 vt=0,pid=0,manager=0,rows=0,result=0;u32 count=0;
 if(!read(vehicle,vt)||vt!=image+redcoffin::CoffinVehicleVtableRva||
    !read(vehicle+0x320,pid)||!validCargo(pid)||!read(image+0x623ea48,manager)||manager<0x10000||
    !read(manager+0x24488,count)||count>LoadedCapacity||!read(manager+0x24490,rows)||(!rows&&count))return 0;
 for(u32 i=0;i<count;++i){u64 own=0,saved=0,record=0,id=0;
  const u64 p=rows+u64(i)*0x28;
  if(!read(p+8,own)||!read(p+0x18,saved))return 0;
  if(own!=vehicle&&saved!=pid)continue;
  if((own&&own!=vehicle)||result||!read(p,record)||!read(p+0x10,id)||
     coffin_session::recordCargo(read,image,record)!=id)return 0;
  result=id;
 }
 return result;
}
}
