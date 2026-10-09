// Exercise the exact dev25 production localized-resource patch with native
// 0x38-byte object layout and known German UUIDs. No DS2 process or files used.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>
#include <cstdio>
#include <cstring>
#include <string>

extern "C" bool SamShelterRestLabelConfigure(void*,const wchar_t*);
extern "C" bool SamShelterRestLabelEnabled();
extern "C" int SamShelterRestLabelOnStreamResource(void*);
extern "C" LONG SamShelterRestLabelChangedCount();

static unsigned tested=0,failures=0;
static void check(bool yes,const char* reason){
 ++tested;
 if(yes)std::printf("PASS %s\n",reason);
 else{std::printf("FAIL %s\n",reason);++failures;}
}
static constexpr uintptr_t image=0x140000000ull;
static constexpr uintptr_t txtVT=image+0x3455cc0u;
static const uint8_t restA[16]={0xFC,0x5D,0x04,0x6C,0x18,0x27,0x43,0x60,0x96,0xBB,0x10,0x49,0xDE,0xAE,0xEB,0xAC};
static const uint8_t restB[16]={0xAD,0x51,0x6D,0xDE,0x5D,0x04,0x46,0x38,0x9F,0xF4,0xE3,0x6B,0xE4,0xEF,0x33,0x7B};
struct MockText{
 uint8_t obj[0x38];
 char original[32];
};
static void make(MockText& x,const uint8_t id[16],const char* text){
 std::memset(&x,0,sizeof(x));
 std::memcpy(x.obj,&txtVT,8);
 std::memcpy(x.obj+0x10,id,16);
 const uintptr_t pointer=(uintptr_t)x.original;
 std::memcpy(x.obj+0x20,&pointer,8);
 const uint16_t length=(uint16_t)std::strlen(text);
 std::memcpy(x.obj+0x28,&length,2);
 std::memcpy(x.original,text,length);
 x.original[18]='X';
 x.original[19]='Y';
 x.original[20]='Z';
}
static unsigned sizeOf(const MockText& x){
 uint16_t n=0;
 std::memcpy(&n,x.obj+0x28,2);
 return n;
}
static bool writeIni(wchar_t (&name)[MAX_PATH],const char* settings){
 wchar_t tmp[MAX_PATH]={};
 if(!GetTempPathW(MAX_PATH,tmp)||!GetTempFileNameW(tmp,L"rest",0,name))return false;
 HANDLE handle=CreateFileW(name,GENERIC_WRITE,FILE_SHARE_READ,nullptr,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);
 if(handle==INVALID_HANDLE_VALUE)return false;
 DWORD n=0;
 const DWORD length=(DWORD)std::strlen(settings);
 const bool okay=WriteFile(handle,settings,length,&n,nullptr)!=0&&n==length;
 CloseHandle(handle);
 return okay;
}
int main(){
 static const char* old="In Bunker ausruhen";
 static const char* replacement="Verschnaufen";
 check(std::strlen(old)==18 && std::strlen(replacement)==12,"known original/corrected byte lengths");
 wchar_t ini[MAX_PATH]={};
 const char* settings="[TimefallShelterRange]\r\nEnabled=0\r\nFixRestPrompt=1\r\n";
 if(!writeIni(ini,settings))return 3;
 MockText a={};make(a,restA,old);
 check(!SamShelterRestLabelConfigure((void*)image,ini),"disabled shelter implies text disabled");
 check(!SamShelterRestLabelEnabled(),"disabled shelter streaming label gate off");
 check(SamShelterRestLabelOnStreamResource(a.obj)==0 && sizeOf(a)==18,
       "disabled extension must not affect game text");
 DeleteFileW(ini);
 const char* enableLegacy="[TimefallShelterRange]\r\nEnabled=1\r\n";
 if(!writeIni(ini,enableLegacy))return 3;
 check(SamShelterRestLabelConfigure((void*)image,ini),"legacy INI missing FixRestPrompt uses enabled default");
 check(SamShelterRestLabelEnabled(),"feature enabled for loaded native text resources");
 check(SamShelterRestLabelOnStreamResource(a.obj)==1,"first native RestInShelter UUID patched");
 check(sizeOf(a)==12 && std::memcmp(a.original,replacement,12)==0 && a.original[12]==0,
       "native length, UTF8 string, null termination exactly correct");
 check(a.original[18]=='X'&&a.original[19]=='Y'&&a.original[20]=='Z',
       "do not touch outside original 18-byte native buffer");
 check(SamShelterRestLabelChangedCount()==1,"first translation counted");
 check(SamShelterRestLabelOnStreamResource(a.obj)==2 && SamShelterRestLabelChangedCount()==1,
       "already patched loaded resource remains idempotent");

 MockText b={};make(b,restB,old);
 check(SamShelterRestLabelOnStreamResource(b.obj)==1 && sizeOf(b)==12,
       "second native RestInShelter UUID patched");
 check(SamShelterRestLabelChangedCount()==2,"two separate German UUIDs counted");

 MockText other={};uint8_t unrelated[16]={};
 make(other,unrelated,old);
 check(SamShelterRestLabelOnStreamResource(other.obj)==0 && sizeOf(other)==18,
       "other German generic bunker/rest localized resource untouched");
 MockText english={};make(english,restA,"Rest in Shelter");
 check(SamShelterRestLabelOnStreamResource(english.obj)==0 && sizeOf(english)==15,
       "other locales not rewritten");
 MockText wrongText={};make(wrongText,restB,"Completely unrelated");
 check(SamShelterRestLabelOnStreamResource(wrongText.obj)==0,
       "expected German original bytes mandatory");
 MockText wrongClass={};make(wrongClass,restA,old);
 const uintptr_t unrelatedVT=image+0x3455cd0u;
 std::memcpy(wrongClass.obj,&unrelatedVT,8);
 check(SamShelterRestLabelOnStreamResource(wrongClass.obj)==0 && sizeOf(wrongClass)==18,
       "strict LocalizedTextResource vtable guard");
 MockText invalidLen={};make(invalidLen,restB,old);
 const uint16_t badSize=500;
 std::memcpy(invalidLen.obj+0x28,&badSize,2);
 check(SamShelterRestLabelOnStreamResource(invalidLen.obj)==0,"strict original length guard");
 MockText nullText={};make(nullText,restA,old);
 const uintptr_t nullVal=0;
 std::memcpy(nullText.obj+0x20,&nullVal,8);
 check(SamShelterRestLabelOnStreamResource(nullText.obj)==0,"null original text pointer rejected");

 auto readonly=(uint8_t*)VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
 if(!readonly)return 4;
 std::memcpy(readonly,old,18);
 MockText protectedObj={};make(protectedObj,restA,old);
 const uintptr_t readonlyPtr=(uintptr_t)readonly;
 std::memcpy(protectedObj.obj+0x20,&readonlyPtr,8);
 DWORD previous=0;
 const bool protect=VirtualProtect(readonly,4096,PAGE_READONLY,&previous)!=0;
 check(protect,"test read-only buffer established");
 if(protect)check(SamShelterRestLabelOnStreamResource(protectedObj.obj)==0,
                  "read-only native resource text never overwritten");
 DWORD restore=0;
 VirtualProtect(readonly,4096,previous,&restore);
 VirtualFree(readonly,0,MEM_RELEASE);

 DeleteFileW(ini);
 const char* explicitOff="[TimefallShelterRange]\r\nEnabled=1\r\nFixRestPrompt=0\r\n";
 if(!writeIni(ini,explicitOff))return 3;
 check(!SamShelterRestLabelConfigure((void*)image,ini),"INI switch explicitly disables text fix");
 MockText notChanged={};make(notChanged,restA,old);
 check(SamShelterRestLabelOnStreamResource(notChanged.obj)==0 &&
       sizeOf(notChanged)==18 && std::memcmp(notChanged.original,old,18)==0,
       "all native text preserved with FixRestPrompt=0");
 check(SamShelterRestLabelChangedCount()==2,"disabled switch does not change count");
 DeleteFileW(ini);

 check(SamShelterRestLabelOnStreamResource(nullptr)==0,"null streamed object safely ignored");
 std::printf("SHELTER_REST_LABEL_NATIVE_TESTS %s (%u checks)\n",
             failures?"FAILED":"PASSED",tested);
 return failures?1:0;
}
