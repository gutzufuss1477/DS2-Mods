#include "../src/autodrive_settings.h"
#include <string>
typedef unsigned int u32;
static wchar_t g_iniPath[MAX_PATH];
static sam_autodrive::Settings g_autoDriveSettings={};
static std::string captured;
static void write_text(HANDLE,const char* text) { captured+=text; }
#include "../src/autodrive_config_io.inl"
static void check_ini(const char* text,float seconds,const char* source,bool invalid) {
    wchar_t temp[MAX_PATH];
    require(GetTempPathW(MAX_PATH,temp)!=0,"temporary directory");
    require(GetTempFileNameW(temp,L"DS2",0,g_iniPath)!=0,"unique test INI");
    HANDLE file=CreateFileW(g_iniPath,GENERIC_WRITE,0,nullptr,CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL,nullptr);
    require(file!=INVALID_HANDLE_VALUE,"open test INI");
    DWORD written=0,bytes=(DWORD)std::strlen(text);
    require(WriteFile(file,text,bytes,&written,nullptr)!=0 && written==bytes,"write test INI");
    CloseHandle(file);
    load_autodrive_config();
    require(g_autoDriveSettings.seconds==seconds,"Windows INI seconds");
    require(std::strcmp(g_autoDriveSettings.source,source)==0,"Windows INI precedence");
    require(g_autoDriveSettings.invalid==invalid,"Windows INI validation");
    require(g_autoDriveSettings.enabled==(seconds<5.0f),"5 seconds disables hook");
    require(DeleteFileW(g_iniPath)!=0,"delete test INI");
}
static void test_settings() {
    using namespace sam_autodrive;
    struct Good { const wchar_t* text; float value; };
    const Good good[]={{L"0.5",0.5f},{L"1.0",1.0f},{L"1.5",1.5f},{L"2",2.0f},
        {L"2.25",2.25f},{L"3.0",3.0f},{L"5.0",5.0f},{L"1,5",1.5f},
        {L" .5 ",0.5f},{L"+2.0",2.0f},{L"2.000000",2.0f},{L"2.0 ; seconds",2.0f}};
    for (const Good& g:good) {
        float value=0.0f;
        require(ParseSeconds(g.text,value) && value==g.value,"parse valid seconds");
    }
    const wchar_t* bad[]={nullptr,L"",L" ",L"0",L"-1",L"0.49",L"0.499999",L"5.000001",
        L"10",L"NaN",L"inf",L"2seconds",L"2.5.0",L"2,5,0",L"1e0",L".",L"+",
        L"1.0000000",L"999999999999999999999",L"0000000000000000000002"};
    for (const wchar_t* text:bad) {
        float value=3.0f;
        require(!ParseSeconds(text,value) && value==3.0f,"reject invalid seconds unchanged");
    }
    Settings s=ResolveSettings(L"2.0",true,10,true);
    require(s.seconds==2.0f && s.multiplier==2.5f && !s.invalid,"seconds override legacy");
    s=ResolveSettings(L"bad",true,10,true);
    require(!s.enabled && s.invalid && s.seconds==5.0f,"invalid new key never uses legacy");
    s=ResolveSettings(nullptr,false,0,false);
    require(s.seconds==2.0f && s.enabled,"new default two seconds");
    for (unsigned int factor=1;factor<=10;++factor) {
        s=ResolveSettings(nullptr,false,factor,true);
        require(s.seconds==5.0f/(float)factor && !s.invalid,"legacy factor compatibility");
    }
    check_ini("[Movement]\r\nAutoDriveActivationSeconds=2.0\r\n",2.0f,"SECONDS",false);
    check_ini("[Movement]\r\nAutoDriveActivationSeconds=1.5\r\n",1.5f,"SECONDS",false);
    check_ini("[Movement]\r\nAutoDriveActivationSeconds=5.0\r\n",5.0f,"SECONDS",false);
    check_ini("[Movement]\r\nAutoDriveActivationSeconds=1,5\r\n",1.5f,"SECONDS",false);
    check_ini("[Movement]\r\nAutoDriveActivationSeconds=2.0\r\nAutoDriveActivationMultiplier=10\r\n",
        2.0f,"SECONDS",false);
    check_ini("[Movement]\r\nAutoDriveActivationSeconds=bad\r\nAutoDriveActivationMultiplier=10\r\n",
        5.0f,"INVALID_SECONDS_VANILLA",true);
    check_ini("[Movement]\r\nAutoDriveActivationSeconds=\r\n",5.0f,"INVALID_SECONDS_VANILLA",true);
    check_ini("[Movement]\r\nAutoDriveActivationSeconds=0\r\n",5.0f,"INVALID_SECONDS_VANILLA",true);
    check_ini("[Movement]\r\nAutoDriveActivationMultiplier=10\r\n",0.5f,"LEGACY_MULTIPLIER",false);
    check_ini("[Movement]\r\nAutoDriveActivationMultiplier=2\r\n",2.5f,"LEGACY_MULTIPLIER",false);
    check_ini("[Movement]\r\nAutoDriveActivationMultiplier=1\r\n",5.0f,"LEGACY_MULTIPLIER",false);
    check_ini("[Movement]\r\nAutoDriveActivationMultiplier=0\r\n",5.0f,"INVALID_LEGACY_VANILLA",true);
    check_ini("[Movement]\r\n",2.0f,"DEFAULT_SECONDS",false);
    captured.clear(); write_decimal3(nullptr,2.0f); require(captured=="2.000","seconds log text");
    captured.clear(); write_decimal3(nullptr,2.5f); require(captured=="2.500","fractional multiplier log text");
    captured.clear(); write_decimal3(nullptr,10.0f); require(captured=="10.000","two-digit log text");
    std::printf("SETTINGS_PASS: parser, real Windows INI, precedence, legacy and invalid fallback.\n");
}
