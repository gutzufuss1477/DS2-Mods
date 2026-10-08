#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <cstdio>
#include <string>
#include <cstring>
#include "../src/footprints/footprint_api.h"
#include "../src/footprints/footprint_core.h"
static unsigned checks=0;
static void Check(bool good,const char* label) {
    ++checks;if(!good){std::fprintf(stderr,"FAIL %s\n",label);ExitProcess(1);}
}
static void Save(const std::wstring& file,const char* contents) {
    HANDLE h=CreateFileW(file.c_str(),GENERIC_WRITE,0,nullptr,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);
    Check(h!=INVALID_HANDLE_VALUE,"create fixture");DWORD wrote=0;
    Check(WriteFile(h,contents,static_cast<DWORD>(std::strlen(contents)),&wrote,nullptr)!=0,"write fixture");CloseHandle(h);
}
int main() {
    wchar_t temp[MAX_PATH];Check(GetTempPathW(MAX_PATH,temp)>0,"temporary path");
    struct Case{const char* ini;bool enabled;bool invalid;};
    const Case cases[]={
      {"[Movement]\nLandingRollWithBackpack=1\n[AutoDrive]\nActivationSeconds=2.0\n",false,false},
      {"[Footprints]\nHideFootprints=0\n",false,false},
      {"[Footprints]\nHideFootprints=1\n",true,false},
      {"[footprints]\nhidefootprints=1\n",true,false},
      {"[Footprints]\nHideFootprints=\n",false,true},
      {"[Footprints]\nHideFootprints=-1\n",false,true},
      {"[Footprints]\nHideFootprints=2\n",false,true},
      {"[Footprints]\nHideFootprints=true\n",false,true},
      {"[Footprints]\nHideFootprints=01\n",false,true},
      {"[Footprints]\nHideFootprints=111111111111111111111111111111\n",false,true},
      {"[Footprints]\nObserveOnly=0\nEnabled=1\n",false,false},
      {"[Footprints]\nHideFootprints=\"1\"\n",true,false}
    };
    for(unsigned i=0;i<sizeof(cases)/sizeof(cases[0]);++i) {
        std::wstring file=std::wstring(temp)+L"ds2-sam-footprints-"+std::to_wstring(GetCurrentProcessId())+L"-"+std::to_wstring(i)+L".ini";
        Save(file,cases[i].ini);bool invalid=false;
        Check(sam_footprints::ReadEnabled(file.c_str(),&invalid)==cases[i].enabled,"INI enabled interpretation");
        Check(invalid==cases[i].invalid,"INI invalid interpretation");
        bool result=SamFootprintsInstall(nullptr,file.c_str(),nullptr);
        Check(result==!cases[i].enabled,"disabled is a no-op; enabled rejects unknown executable");
        Check(SamOverhaulFootprintsState()==(cases[i].enabled?5u:(cases[i].invalid?3u:1u)),"integration state");
        Check(footprint::original==nullptr && !footprint::suppress.load(),"no hook/suppression on rejected/disabled host");
        Check(DeleteFileW(file.c_str())!=0,"fixture cleanup");
    }
    bool invalid=true;
    Check(!sam_footprints::ReadEnabled(nullptr,&invalid) && !invalid,"null config defaults off");
    Check(!sam_footprints::ReadEnabled(L"",&invalid) && !invalid,"empty path defaults off");
    std::wstring missing=std::wstring(temp)+L"ds2-nonexistent-config-"+std::to_wstring(GetCurrentProcessId())+L".ini";
    Check(!sam_footprints::ReadEnabled(missing.c_str(),&invalid) && !invalid,"missing INI defaults off");
    Check(SamFootprintsInstall(nullptr,missing.c_str(),nullptr),"missing INI installs no hook");
    Check(SamOverhaulFootprintsState()==1,"disabled state for old installation");
    Check(MH_Initialize()==MH_OK,"guarded cases never initialized MinHook");
    Check(MH_Uninitialize()==MH_OK,"test library cleanup");
    std::printf("PASS footprints configuration/integration guards: %u checks\n",checks);return 0;
}