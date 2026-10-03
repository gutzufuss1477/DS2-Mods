// Test-only exports. All mutations target memory allocated by the Python host.
// The host is python.exe, so the production DllMain does not enter the DS2 worker.
#include "../src/jump_ramp_unlimited.cpp"

extern "C" __declspec(dllexport) int HarnessPrepare(void* image){
    g_base=(u8*)image;
    return validateBuild()&&validateSites()&&buildCave()?1:0;
}
extern "C" __declspec(dllexport) int HarnessValidateSites(){return validateSites()?1:0;}
extern "C" __declspec(dllexport) int HarnessValidateBuild(){return validateBuild()?1:0;}
extern "C" __declspec(dllexport) int HarnessParseEnabled(const WCHAR* text){
    u32 value=0;return parseUInt(text,0,1,&value)?(int)value:-1;
}
extern "C" __declspec(dllexport) int HarnessToggle(int enabled){
    u8* site=g_base+RVA_STAGE_PATCH;
    if(enabled){
        if(bytesEqual(site,g_patch,5))return 1;
        if(!validateSites())return 0;
        return installPatch()==1?1:0;
    }
    if(bytesEqual(site,VANILLA_STAGE_BYTES,5))return 1;
    if(!bytesEqual(site,g_patch,5))return 0;
    DWORD old=0,tmp=0;
    if(!VirtualProtect(site,5,PAGE_EXECUTE_READWRITE,&old))return 0;
    for(u32 i=0;i<5;i++)site[i]=VANILLA_STAGE_BYTES[i];
    BOOL flushed=FlushInstructionCache(GetCurrentProcess(),site,5);
    BOOL restored=VirtualProtect(site,5,old,&tmp);
    return flushed&&restored?1:0;
}
extern "C" __declspec(dllexport) void* HarnessCave(){return g_cave;}
extern "C" __declspec(dllexport) u32 HarnessCaveSize(){return g_caveCodeSize;}
extern "C" __declspec(dllexport) u32 HarnessCaveProtection(){
    MEMORY_BASIC_INFORMATION_X64 info={};
    if(!VirtualQuery(g_cave,&info,sizeof(info)))return 0;
    return info.Protect;
}
