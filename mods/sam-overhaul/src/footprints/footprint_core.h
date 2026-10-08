#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <intrin.h>
#include <atomic>
#include <array>
#include <cstdint>
#include <cstring>
#include "MinHook.h"

namespace footprint {
constexpr uintptr_t AppendRva=0x2250210, InstanceVtRva=0x33F57A8, ResourceVtRva=0x33F53F8;
constexpr unsigned char Prologue[]={0x48,0x8B,0xC4,0x48,0x89,0x58,0x18,0x48,0x89,0x70,0x20,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57,0x48,0x81,0xEC,0x10,0x01,0x00,0x00};
struct Id {uint64_t lo,hi;};
// Observed raw UUIDs; match only alongside verified native vtables and executable hash.
constexpr Id Targets[]={
 {0x7346C5FEC6C57B13ULL,0x74E314CC9B9A54A9ULL},
 {0x1348FB900EF353C0ULL,0x91661A228DFEBB9AULL},
 {0x604D06B848E87FC4ULL,0x99E56E0FCB7EE181ULL}};
constexpr const char* Names[]={"road-footstep","player-decal-0","player-decal-1","player-decal-extra"};
inline uintptr_t base=0;
inline std::atomic<bool> suppress{false};
inline std::atomic<uint64_t> total{0},passed{0},matched{0},blocked{0},invalid{0},overflow{0};
// Exact x64 forwarding boundary from current executable call at RVA 0x225496D.
// First 4 args are opaque pointers; arguments 5..12 retain original stack slots.
using Append=void(__fastcall*)(uintptr_t,uintptr_t,const void*,const void*,const void*,float,const void*,int,int,uint8_t,uint32_t,float);
inline Append original=nullptr;
struct Record {
 std::atomic<unsigned> state{0}; // 0 empty, 1 initialization, 2 immutable identity ready
 uintptr_t instance=0,resource=0,returnAddress=0; Id id{}; std::atomic<int> target{-1};
 std::atomic<uint64_t> seen{0},blocked{0};
};
inline std::array<Record,64> records{};
// Kernel-validated reads avoid raising access violations in the game's crash handler.
// Diagnostic implementation deliberately favours safety over pointer-cache speed.
inline bool Read(uintptr_t a,void* out,size_t n) noexcept {
 if(a<0x10000 || a>UINTPTR_MAX-n)return false;
 SIZE_T got=0;
 return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<const void*>(a),out,n,&got) && got==n;
}
template<class T> bool Value(uintptr_t a,T& out) noexcept{return Read(a,&out,sizeof out);}
inline int Match(Id id) noexcept {
 for(int i=0;i<3;++i)if(id.lo==Targets[i].lo && id.hi==Targets[i].hi)return i;
 return -1;
}
// Prefer stable, observed UUIDs even before world resources initialize. Additionally
// accept exact membership in the engine's verified footprint-resource fields.
inline int MatchResource(uintptr_t resource,Id id) noexcept {
 int fixed=Match(id);if(fixed>=0)return fixed;
 uintptr_t mgr=0,mvt=0,res=0,rvt=0,arr=0,foot=0;uint32_t count=0;
 if(!Value(base+0x623F9E8,mgr) || !Value(mgr,mvt) || mvt!=base+0x33A6638 ||
  !Value(mgr+0x38,res) || !Value(res,rvt) || rvt!=base+0x33A6D20)return -1;
 if(Value(res+0x20,foot) && foot==resource)return 0;
 if(!Value(res+0x28,count) || count>16 || !Value(res+0x30,arr))return -1;
 for(uint32_t i=0;i<count;++i){uintptr_t item=0;if(!Value(arr+8*i,item))return -1;if(item==resource)return i<2?static_cast<int>(i)+1:3;}
 return -1;
}
inline bool Identity(uintptr_t i,uintptr_t& r,Id& id) noexcept {
 uintptr_t iv=0,rv=0;
 return Value(i,iv) && iv==base+InstanceVtRva && Value(i+0xA8,r) &&
  Value(r,rv) && rv==base+ResourceVtRva && Value(r+0x10,id);
}
inline Record* Track(uintptr_t i,uintptr_t resource,Id id,int target,uintptr_t ret) noexcept {
 for(auto& r:records){
  unsigned state=r.state.load(std::memory_order_acquire);
  if(state==2 && r.id.lo==id.lo && r.id.hi==id.hi){r.target.store(target,std::memory_order_relaxed);return &r;}
  if(state==0){
   unsigned expected=0;
   if(r.state.compare_exchange_strong(expected,1,std::memory_order_acq_rel)){
    r.instance=i;r.resource=resource;r.id=id;r.target=target;r.returnAddress=ret;
    r.state.store(2,std::memory_order_release);return &r;
   }
  }
 }
 overflow.fetch_add(1,std::memory_order_relaxed);return nullptr;
}
inline void __fastcall OnAppend(uintptr_t instance,uintptr_t entity,const void* p3,const void* p4,
 const void* p5,float p6,const void* p7,int p8,int p9,uint8_t p10,uint32_t p11,float p12){
 DWORD savedError=GetLastError();
 total.fetch_add(1,std::memory_order_relaxed);
 uintptr_t resource=0;Id id{};int target=-1;Record* record=nullptr;
 if(Identity(instance,resource,id)){
  target=MatchResource(resource,id);record=Track(instance,resource,id,target,reinterpret_cast<uintptr_t>(_ReturnAddress()));
  if(record)record->seen.fetch_add(1,std::memory_order_relaxed);
  if(target>=0)matched.fetch_add(1,std::memory_order_relaxed);
 }else invalid.fetch_add(1,std::memory_order_relaxed);
 if(target>=0 && suppress.load(std::memory_order_relaxed)){
  blocked.fetch_add(1,std::memory_order_relaxed);
  if(record)record->blocked.fetch_add(1,std::memory_order_relaxed);
  SetLastError(savedError);return; // Omit a stamp, not resource allocation/ownership, world state, or save data.
 }
 passed.fetch_add(1,std::memory_order_relaxed);
 SetLastError(savedError);
 original(instance,entity,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12);
}
}