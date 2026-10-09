#pragma once
// Shared source for native 6-byte detour: JMP rel32 occupies exactly 5 bytes;
// the sixth byte is padding. The displacement is ALWAYS relative to site+5.
#include <stdint.h>
namespace native_jump_rel32 {
static constexpr unsigned kJmpBytes=5u;
static constexpr unsigned kPatchedBytes=6u;
static inline bool build(uintptr_t site,uintptr_t destination,uint8_t (&out)[6]){
 if(site<0x10000u || destination<0x10000u)return false;
 const int64_t distance=(int64_t)destination-(int64_t)(site+kJmpBytes);
 if(distance<(-2147483647LL-1LL)||distance>2147483647LL)return false;
 const uint32_t bits=(uint32_t)(int32_t)distance;
 out[0]=0xE9u;
 for(unsigned i=0;i<4u;++i)out[i+1u]=(uint8_t)(bits>>(8u*i));
 out[5]=0x90u;
 // Catch the specific dev22 off-by-one error in source and mock tests.
 const uintptr_t resolved=(uintptr_t)((int64_t)(site+kJmpBytes)+(int64_t)(int32_t)bits);
 return resolved==destination;
}
static inline uintptr_t decodeTarget(uintptr_t site,const uint8_t (&patch)[6]){
 uint32_t raw=0;
 for(unsigned i=0;i<4u;++i)raw|=((uint32_t)patch[i+1u])<<(8u*i);
 return (uintptr_t)((int64_t)(site+kJmpBytes)+(int64_t)(int32_t)raw);
}
}
