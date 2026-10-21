// Test production SamShelterRestLabelPoll() on localized assets which were
// loaded before the native streaming listener. Real VirtualQuery/self-ReadProcessMemory
// scanning, exact vtable/UUID, no DS2.exe or game save involved.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdint>
#include <cstring>
#include <cstdio>
extern "C" bool SamShelterRestLabelConfigure(void*,const wchar_t*);
extern "C" int SamShelterRestLabelOnStreamResource(void*);
extern "C" LONG SamShelterRestLabelChangedCount();
extern "C" void SamShelterRestLabelPoll(HANDLE);
extern "C" void SamShelterRestLabelNotifyActiveShelter();
static unsigned passed=0,failed=0;
static void check(bool valid,const char* label){
 ++(valid?passed:failed);
 std::printf("%s %s\n",valid?"PASS":"FAIL",label);
}
static const uint8_t a[16]={0xFC,0x5D,0x04,0x6C,0x18,0x27,0x43,0x60,0x96,0xBB,0x10,0x49,0xDE,0xAE,0xEB,0xAC};
static const uint8_t b[16]={0xAD,0x51,0x6D,0xDE,0x5D,0x04,0x46,0x38,0x9F,0xF4,0xE3,0x6B,0xE4,0xEF,0x33,0x7B};
static void writeText(uint8_t* obj,const uint8_t (&guid)[16],uint8_t* string){
 const uintptr_t vtable=0x140000000ULL+0x3455CC0ULL;
 const uintptr_t text=(uintptr_t)string;
 const uint16_t length=18u;
 std::memcpy(obj,&vtable,8);
 std::memcpy(obj+0x10,guid,16);
 std::memcpy(obj+0x20,&text,8);
 std::memcpy(obj+0x28,&length,2);
 std::memcpy(string,"In Bunker ausruhen",18);
}
int main(){
 const uintptr_t candidates[]={0x02000000ULL,0x03000000ULL,0x04000000ULL,
                                0x08000000ULL,0x10000000ULL};
 uint8_t* memory=nullptr;
 for(uintptr_t base:candidates){
  memory=(uint8_t*)VirtualAlloc((void*)base,4096u,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
  if(memory)break;
 }
 if(!memory){std::printf("FAIL no low-memory mock allocation\n");return 3;}
 check((uintptr_t)memory<0x20000000u,"low committed mock page for preload scan");
 writeText(memory,a,memory+0x70u);
 writeText(memory+0x150u,b,memory+0x1C0u);
 check(std::memcmp(memory+0x70,"In Bunker ausruhen",18)==0 &&
       std::memcmp(memory+0x1C0,"In Bunker ausruhen",18)==0,
       "mock native resources remain unmodified before streaming listener");
 wchar_t path[MAX_PATH]={};
 wchar_t folder[MAX_PATH]={};
 if(!GetTempPathW(MAX_PATH,folder) || !GetTempFileNameW(folder,L"pre",0,path))return 4;
 HANDLE handle=CreateFileW(path,GENERIC_WRITE,FILE_SHARE_READ,nullptr,
                            CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);
 if(handle==INVALID_HANDLE_VALUE)return 5;
 const char ini[]="[TimefallShelterRange]\r\nEnabled=1\r\nFixRestPrompt=1\r\n";
 DWORD written=0;
 WriteFile(handle,ini,sizeof(ini)-1,&written,nullptr);
 CloseHandle(handle);
 check(SamShelterRestLabelConfigure((void*)0x140000000ULL,path),
       "preloaded resource scanner enabled under shelter INI");
 check(SamShelterRestLabelChangedCount()==0,"no streaming callback executed");
 SamShelterRestLabelPoll(GetStdHandle(STD_OUTPUT_HANDLE));
 check(SamShelterRestLabelChangedCount()==0,
       "scanner does not run prematurely before an actual shelter is discovered");
 SamShelterRestLabelNotifyActiveShelter();
 SamShelterRestLabelPoll(GetStdHandle(STD_OUTPUT_HANDLE));
 check(SamShelterRestLabelChangedCount()==0,
       "scanner waits for game localization after active shelter is discovered");
 DWORD t0=GetTickCount();
 unsigned polls=0;
 while(SamShelterRestLabelChangedCount()!=2 && polls<250u &&
       (DWORD)(GetTickCount()-t0)<4500u){
  SamShelterRestLabelPoll(GetStdHandle(STD_OUTPUT_HANDLE));
  ++polls;
  Sleep(18);
 }
 check(SamShelterRestLabelChangedCount()==2,
       "PRELOADED RESOURCES patched without native streaming callbacks");
 check(std::memcmp(memory+0x70,"Verschnaufen",12)==0 &&
       std::memcmp(memory+0x1C0,"Verschnaufen",12)==0,
       "both German localized resource strings fixed in existing heap");
 uint16_t lenA=0,lenB=0;
 std::memcpy(&lenA,memory+0x28u,2);
 std::memcpy(&lenB,memory+0x150u+0x28u,2);
 check(lenA==12u && lenB==12u,
       "both native resource text lengths synchronized");
 check(SamShelterRestLabelOnStreamResource(memory)==2 &&
       SamShelterRestLabelOnStreamResource(memory+0x150u)==2,
       "normal streaming path remains idempotent after preload reconciliation");
 check(SamShelterRestLabelChangedCount()==2,
       "source scanner stops once both exact target UUIDs were patched");
 std::printf("NATIVE_PRELOADED_REST_LABEL_TESTS %s (%u passes, %u failures, %u polls, %lums)\n",
             failed?"FAILED":"PASSED",passed,failed,polls,GetTickCount()-t0);
 DeleteFileW(path);
 VirtualFree(memory,0,MEM_RELEASE);
 return failed?1:0;
}
