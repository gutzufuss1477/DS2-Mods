#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>
#include <cstring>
int wmain(int argc,wchar_t** argv) {
    if(argc!=2)return 2;
    HMODULE mod=LoadLibraryW(argv[1]);if(!mod){std::printf("FAIL LoadLibrary %lu\n",GetLastError());return 3;}
    using Version=const char*(*)();using State=unsigned(*)();
    auto version=reinterpret_cast<Version>(GetProcAddress(mod,"SamOverhaulVersion"));
    auto state=reinterpret_cast<State>(GetProcAddress(mod,"SamOverhaulFootprintsState"));
    if(!version || !state || std::strcmp(version(),"1.1.0-dev.28"))return 4;
    Sleep(1800);
    if(state()!=0){std::printf("FAIL unsupported host activated footprint integration: %u\n",state());return 5;}
    std::puts("PASS combined ASI LoadLibrary, exports and unsupported-host guard; no game execution");
    // Let the process end; never unload a module while a startup worker may still run.
    return 0;
}