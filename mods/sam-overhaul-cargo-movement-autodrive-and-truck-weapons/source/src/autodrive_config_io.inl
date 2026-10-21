// User-facing seconds setting. The old test-build key is accepted as a fallback.
static void load_autodrive_config() {
    wchar_t seconds[64],legacySeconds[64],legacyMultiplier[64];

    const DWORD n=GetPrivateProfileStringW(
        L"AutoDrive",L"ActivationSeconds",L"\x01",seconds,64u,g_iniPath);
    const bool present=!(n==1u && seconds[0]==1);
    if (present) {
        g_autoDriveSettings=sam_autodrive::ResolveSettings(
            n>=63u ? L"" : seconds,true,0u,false);
        return;
    }

    const DWORD oldSeconds=GetPrivateProfileStringW(
        L"Movement",L"AutoDriveActivationSeconds",L"\x01",
        legacySeconds,64u,g_iniPath);
    const bool oldSecondsPresent=!(oldSeconds==1u && legacySeconds[0]==1);
    if (oldSecondsPresent) {
        g_autoDriveSettings=sam_autodrive::ResolveSettings(
            oldSeconds>=63u ? L"" : legacySeconds,true,0u,false);
        return;
    }

    const DWORD old=GetPrivateProfileStringW(
        L"Movement",L"AutoDriveActivationMultiplier",L"\x01",
        legacyMultiplier,64u,g_iniPath);
    const bool oldPresent=!(old==1u && legacyMultiplier[0]==1);
    const u32 factor=GetPrivateProfileIntW(
        L"Movement",L"AutoDriveActivationMultiplier",0,g_iniPath);
    g_autoDriveSettings=sam_autodrive::ResolveSettings(
        L"",false,factor,oldPresent);
}
static void write_decimal3(HANDLE log,float value) {
    const u32 milli=(u32)(value*1000.0f+0.5f);
    u32 whole=milli/1000u, pos=0u, count=0u;
    char text[16],reverse[10];
    do { reverse[count++]=(char)('0'+whole%10u); whole/=10u; } while (whole);
    while (count) text[pos++]=reverse[--count];
    text[pos++]='.'; text[pos++]=(char)('0'+(milli/100u)%10u);
    text[pos++]=(char)('0'+(milli/10u)%10u); text[pos++]=(char)('0'+milli%10u);
    text[pos]=0; write_text(log,text);
}
