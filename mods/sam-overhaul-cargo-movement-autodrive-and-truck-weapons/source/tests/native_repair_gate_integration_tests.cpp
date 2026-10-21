// Isolated end-to-end test: real production dev24 installer, near relay,
// MASM mid-function hook and return to original baggage repair branches.
// No DS2 process, saves, game configuration, installed ASI or game files touched.
#include <windows.h>
#include <stdint.h>
#include <cstdio>
#include <cstring>
#include "../src/native_jump_rel32.h"

extern "C" bool SamConstructionRepairGateInstall(void*,const wchar_t*,HANDLE);
extern "C" void SamConstructionRepairGateRegister(void*,void*,void*);
extern "C" void SamConstructionRepairGatePoll();
extern "C" int SamGateNativeTestInvoke(void*,void*);
extern "C" volatile LONG SamGateRegistrationCount;
extern "C" uint32_t SamGateRadiusBits;

static unsigned ok=0,failed=0;
static void check(bool b,const char* label){
 std::printf("%s %s\n",b?"PASS":"FAIL",label);
 if(b)++ok;else ++failed;
}
static void storePointer(void* base,unsigned off,uintptr_t v){
 std::memcpy((unsigned char*)base+off,&v,sizeof(v));
}
static uint8_t* makeMockImage(){
 constexpr SIZE_T size=0x0B292000u;
 constexpr uintptr_t siteOffset=0x11B0698u;
 uint8_t* base=(uint8_t*)VirtualAlloc(nullptr,size,MEM_RESERVE,PAGE_NOACCESS);
 if(!base)return nullptr;
 uint8_t* page=(uint8_t*)VirtualAlloc(base+(siteOffset&~(uintptr_t)0xFFFu),
             4096,MEM_COMMIT,PAGE_READWRITE);
 if(!page){VirtualFree(base,0,MEM_RELEASE);return nullptr;}
 const uint8_t mockBytes[]={
  0xF3,0x0F,0x1E,0xFA,    // ENDBR64 native mock function entry
  0x41,0x80,0x7E,0x70,0x00, // CMP byte ptr [r14+0x70],0
  0x0F,0x84,0xC5,0x00,0x00,0x00, // original JE to "skip"
  0x49,0x8B,0x76,0x48,   // exact DS2 followup bytes
  0xB8,0x65,0x00,0x00,0x00,0xC3  // continue: mov eax,101; ret
 };
 // Commit the original DS2 native RepairSpray list-pointer page, too.
 uint8_t* registryPage=(uint8_t*)VirtualAlloc(base+(0x623EAD8u&~(uintptr_t)0xFFFu),
             4096,MEM_COMMIT,PAGE_READWRITE);
 if(!registryPage){VirtualFree(base,0,MEM_RELEASE);return nullptr;}
 uint8_t* site=base+siteOffset;
 std::memcpy(site-9,mockBytes,sizeof(mockBytes));
 const uint8_t skip[]={0xB8,0xCA,0x00,0x00,0x00,0xC3}; // mov eax,202; ret
 std::memcpy(site+6+0xC5,skip,sizeof(skip));
 DWORD prior=0;
 if(!VirtualProtect(page,4096,PAGE_EXECUTE_READ,&prior)){
  VirtualFree(base,0,MEM_RELEASE);return nullptr;
 }
 FlushInstructionCache(GetCurrentProcess(),page,4096);
 return base;
}
static bool writeIni(wchar_t (&filename)[MAX_PATH]){
 wchar_t tmp[MAX_PATH]={};
 if(!GetTempPathW(MAX_PATH,tmp))return false;
 if(!GetTempFileNameW(tmp,L"rgt",0,filename))return false;
 HANDLE out=CreateFileW(filename,GENERIC_WRITE,FILE_SHARE_READ,nullptr,
                      CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);
 if(out==INVALID_HANDLE_VALUE)return false;
 const char bytes[]="[TimefallShelterRange]\r\nEnabled=1\r\nRangePercent=200\r\n";
 DWORD count=0;
 const bool good=WriteFile(out,bytes,sizeof(bytes)-1,&count,nullptr)!=0 &&
                   count==sizeof(bytes)-1;
 CloseHandle(out);
 return good;
}
int main(){
 constexpr uintptr_t kSite=0x11B0698u;
 uint8_t* image=makeMockImage();
 if(!image){std::printf("FAIL MockVirtualImage %lu\n",GetLastError());return 2;}
 wchar_t path[MAX_PATH]={};
 if(!writeIni(path)){std::printf("FAIL WriteTempINI\n");VirtualFree(image,0,MEM_RELEASE);return 3;}
 alignas(16) unsigned char source[0x100]={};
 alignas(16) unsigned char resource[0x60]={};
 alignas(16) unsigned char owner[0xC0]={};
 alignas(16) unsigned char unknownSource[0x100]={};
 alignas(16) unsigned char sourceRegistry[0x580]={};
 alignas(16) unsigned char members[0x70]={};
 uintptr_t nativeRows[2]={(uintptr_t)source,0u};
 InitializeSRWLock((PSRWLOCK)(sourceRegistry+0x60u));
 storePointer(image,0x623EAD8u,(uintptr_t)sourceRegistry);
 storePointer(sourceRegistry,0x548u,(uintptr_t)nativeRows);
 const unsigned nativeCount=1u;
 std::memcpy(sourceRegistry+0x540u,&nativeCount,4);
 storePointer(resource,0,(uintptr_t)image+0x3296670u);
 const float nativeTargetRadius=8.6f;
 std::memcpy(resource+0x20,&nativeTargetRadius,4);
 storePointer(owner,0,(uintptr_t)image+0x3119BC8u);
 storePointer(owner,0xA8u,(uintptr_t)members);
 storePointer(members,0x28u,(uintptr_t)source);
 storePointer(source,0,(uintptr_t)image+0x3297208u);
 storePointer(source,0x30,(uintptr_t)resource);
 storePointer(source,0x48,(uintptr_t)owner);
 storePointer(unknownSource,0,(uintptr_t)image+0x3297208u);
 storePointer(unknownSource,0x30,(uintptr_t)resource);
 storePointer(unknownSource,0x48,(uintptr_t)owner);
 // Explicitly use the actual game RVA offsets, not a synthetic rewritten path.
 auto run=[&](void* src){return SamGateNativeTestInvoke(src,image+kSite-9u);};
 check(run(source)==202,"original DS2 branch skips inactive native contact");
 source[0x70]=1;
 check(run(source)==101,"original DS2 branch allows active native contact");
 source[0x70]=0;

 const bool installed=SamConstructionRepairGateInstall((void*)image,path,GetStdHandle(STD_OUTPUT_HANDLE));
 check(installed,"PRODUCTION installer accepted exact native instruction signatures");
 const uint32_t expected215RadiusBits=0x4109999Au; // 8.6f
 check(SamGateRadiusBits==expected215RadiusBits,"native baggage repair target matches 215pct, separate from 200pct visual");
 if(!installed){DeleteFileW(path);VirtualFree(image,0,MEM_RELEASE);return 4;}
 const uint8_t* patch=image+kSite;
 check(patch[0]==0xE9 && patch[5]==0x90,"production E9 and trailing NOP installed");
 uint8_t instruction[6]={};
 std::memcpy(instruction,patch,6);
 const uintptr_t relay=native_jump_rel32::decodeTarget((uintptr_t)patch,instruction);
 check(relay!=(uintptr_t)patch-1 && relay!=(uintptr_t)patch+5,
       "decoded E9 points to distinct relay code");
 const uint8_t* relayBytes=(const uint8_t*)relay;
 check(relayBytes[0]==0xFF && relayBytes[1]==0x25 &&
       relayBytes[2]==0 && relayBytes[3]==0 &&
       relayBytes[4]==0 && relayBytes[5]==0,
       "production relay contains FF25 RIP-relative jmp");
 check(SamGateRegistrationCount==0,"no source registered until active shelter validation");
 check(run(source)==202,"unregistered inactive source retains vanilla branch");
 source[0x70]=1;
 check(run(source)==101,"unregistered active source retains vanilla branch");
 source[0x70]=0;

 SamConstructionRepairGateRegister(source,resource,owner);
 check(SamGateRegistrationCount==1,"registered active shelter source published");
 check(run(source)==101,"registered source bypasses native contact, continues real native code");
 check(run(unknownSource)==202,"otherwise identical unregistered source uses vanilla skip");
 unknownSource[0x70]=1;
 check(run(unknownSource)==101,"unregistered active source still continues");
 unknownSource[0x70]=0;
 storePointer(source,0x30,0);
 check(run(source)==202,"live invalidated source resource pointer fails closed");
 storePointer(source,0x30,(uintptr_t)resource);
 check(run(source)==101,"restored registered resource resumes contact bypass");
 storePointer(source,0x48,0);
 check(run(source)==202,"live invalidated owner pointer fails closed");
 storePointer(source,0x48,(uintptr_t)owner);
 check(run(source)==101,"restored source owner resumes contact bypass");
 SamConstructionRepairGatePoll();
 check(run(source)==101,"native registry poll retains a verified active source");
 const float originalRingRadius=8.0f;
 std::memcpy(resource+0x20,&originalRingRadius,4);
 Sleep(1400);
 SamConstructionRepairGatePoll();
 check(run(source)==202,"8m source cannot retain dev26 8.6m scoped-gate permission");
 std::memcpy(resource+0x20,&nativeTargetRadius,4);
 SamConstructionRepairGateRegister(source,resource,owner);
 check(run(source)==101,"new 8.6m source is admitted after validation");

 // Simulates a 14-second alt-tab where native game object updates STOP,
 // but native repair-source entries remain loaded. dev23 would erase the
 // source after 13 seconds even though it was still valid in the native table.
 Sleep(14100);
 SamConstructionRepairGatePoll();
 check(run(source)==101,"source survives >13s inactive-game pause without contact expiry");
 check(SamGateRegistrationCount==1,"native source registry remains bounded after pause");

 // Simulate genuine stream-out; the native registry no longer contains the
 // repair component. Only this (or changing native source identity) should
 // remove the permitted source, returning to vanilla.
 nativeRows[0]=(uintptr_t)unknownSource;
 Sleep(1400);
 SamConstructionRepairGatePoll();
 check(run(source)==202,"native source removal immediately revokes enlarged-gate eligibility");
 nativeRows[0]=(uintptr_t)source;
 SamConstructionRepairGateRegister(source,resource,owner);
 check(run(source)==101,"newly loaded matching source is re-registered on shelter scan");
 Sleep(1400);
 SamConstructionRepairGatePoll();
 check(run(source)==101,"valid reloaded source survives native registry liveness check");

 const uint8_t* untouched=image+kSite-5u;
 const uint8_t expectedCmp[]={0x41,0x80,0x7E,0x70,0x00};
 check(std::memcmp(untouched,expectedCmp,sizeof(expectedCmp))==0,
       "production patch preserves original native CMP");
 check(std::memcmp(image+kSite+6,"\x49\x8B\x76\x48",4)==0,
       "production patch preserves original continue bytes");
 DeleteFileW(path);
 VirtualFree(image,0,MEM_RELEASE);
 std::printf("NATIVE_DEV26_END_TO_END_INTEGRATION %s (%u checks passed)\n",
             failed?"FAILED":"PASSED",ok);
 return failed?1:0;
}
