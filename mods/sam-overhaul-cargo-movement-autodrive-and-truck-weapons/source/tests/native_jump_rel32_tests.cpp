#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include "../src/native_jump_rel32.h"
static int count=0;
static bool check(bool condition,const char* label) {
 std::printf("%s %s\n",condition?"PASS":"FAIL",label);
 if(condition)++count;
 return condition;
}
int main() {
 using native_jump_rel32::build;
 using native_jump_rel32::decodeTarget;
 unsigned failures=0;
 uint8_t patch[6]{};
 const uintptr_t site=0x140000000ull;
 auto ok=[&](bool condition,const char* label){
  if(!check(condition,label))++failures;
 };
 const uintptr_t target=site+0x400u;
 ok(build(site,target,patch),"forward E9 build");
 ok(patch[0]==0xE9u && patch[5]==0x90u,"E9 opcode and single NOP");
 ok(decodeTarget(site,patch)==target,"resolve exactly to relay, never relay-1");
 ok(build(site,site-0x400u,patch) && decodeTarget(site,patch)==site-0x400u,
    "negative signed rel32 displacement");
 ok(build(site,site+5u,patch) && patch[1]==0u&&patch[2]==0u&&patch[3]==0u&&patch[4]==0u,
    "zero displacement means fallthrough after FIVE bytes");
 ok(build(site,site+5u+2147483647ull,patch),"maximum positive rel32 accepted");
 ok(build(site,site+5u-2147483648ull,patch),"maximum negative rel32 accepted");
 ok(!build(site,site+5u+2147483648ull,patch),"out-of-range positive rejected");
 ok(!build(site,site+5u-2147483649ull,patch),"out-of-range negative rejected");
 const uintptr_t predictedWrong=(uintptr_t)((int64_t)(site+5u)+
                        (int64_t)(int32_t)(target-(site+6u)));
 ok(predictedWrong==target-1u,"dev22 bug independently reproduces one-byte underrun");

 uint8_t* memory=(uint8_t*)VirtualAlloc(nullptr,4096u,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
 if(!memory){std::printf("FAIL VirtualAlloc %lu\n",GetLastError());return 2;}
 const uintptr_t address=(uintptr_t)memory;
 const uintptr_t codeDestination=(uintptr_t)(memory+0x500);
 // machine code: mov eax, 0x13572468 ; ret
 const uint8_t answer[]={0xB8,0x68,0x24,0x57,0x13,0xC3};
 std::memcpy(memory+0x500,answer,sizeof(answer));
 ok(build(address+0x50u,codeDestination,patch),"live executable jump forward builds");
 std::memcpy(memory+0x50,patch,6);
 ok(build(address+0x600u,codeDestination,patch),"live executable jump backward builds");
 std::memcpy(memory+0x600,patch,6);
 DWORD oldProtect=0;
 if(!VirtualProtect(memory,4096u,PAGE_EXECUTE_READ,&oldProtect)){
  std::printf("FAIL VirtualProtect %lu\n",GetLastError());VirtualFree(memory,0,MEM_RELEASE);return 3;
 }
 FlushInstructionCache(GetCurrentProcess(),memory,4096);
 using Fn=int(*)();
 if(!failures) {
  const int forward=((Fn)(memory+0x50))();
  const int backward=((Fn)(memory+0x600))();
  ok(forward==0x13572468,"real CPU E9 rel32 forward dispatch returns correct result");
  ok(backward==0x13572468,"real CPU E9 rel32 backward dispatch returns correct result");
 }
 VirtualFree(memory,0,MEM_RELEASE);
 std::printf("NATIVE_JUMP_REL32_TESTS %s (%d passing checks)\n",failures?"FAILED":"PASSED",count);
 return failures?1:0;
}
