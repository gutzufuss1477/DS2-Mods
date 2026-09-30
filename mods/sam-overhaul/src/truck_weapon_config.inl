static bool parse_truck_float(const wchar_t* text,float minimum,float maximum,float& value) {
    if (!text) return false;
    const wchar_t* p=text;
    while (sam_autodrive::Space(*p)) ++p;
    if (*p==L'+') ++p;
    u32 whole=0u,fraction=0u,divisor=1u,digits=0u;
    bool any=false;
    while (*p>=L'0' && *p<=L'9') {
        if (++digits>8u) return false;
        whole=whole*10u+(u32)(*p++-L'0');
        if (whole>1000u) return false;
        any=true;
    }
    if (*p==L'.' || *p==L',') {
        ++p;
        u32 fractionalDigits=0u;
        while (*p>=L'0' && *p<=L'9') {
            if (++fractionalDigits>6u) return false;
            fraction=fraction*10u+(u32)(*p++-L'0');
            divisor*=10u;
            any=true;
        }
    }
    while (sam_autodrive::Space(*p)) ++p;
    if (!any || (*p && *p!=L';' && *p!=L'#')) return false;
    const float parsed=(float)whole+(float)fraction/(float)divisor;
    if (parsed<minimum || parsed>maximum) return false;
    value=parsed;
    return true;
}
static float read_truck_float(const wchar_t* section,const wchar_t* key,
                              const wchar_t* defaultText,float defaultValue,
                              float minimum,float maximum) {
    wchar_t text[64];
    const DWORD n=GetPrivateProfileStringW(
        section,key,defaultText,text,64u,g_iniPath);
    float value=defaultValue;
    if (n>=63u || !parse_truck_float(text,minimum,maximum,value)) return defaultValue;
    return value;
}
static u32 read_truck_percent(const wchar_t* section,const wchar_t* key,
                              u32 defaultValue,u32 minimum,u32 maximum) {
    u32 value=GetPrivateProfileIntW(section,key,(int)defaultValue,g_iniPath);
    if (value<minimum) value=minimum;
    if (value>maximum) value=maximum;
    return value;
}
static void load_truck_weapon_config() {
    static const wchar_t MG[]=L"TruckHeavyMachineGun";
    static const wchar_t MORTAR[]=L"TruckMortar";
    static const wchar_t CHIRAL[]=L"TruckChiralCannon";
    static const wchar_t MISSILE[]=L"TruckMissileLauncher";

    g_tuneHeavyMachineGun=GetPrivateProfileIntW(MG,L"Enabled",1,g_iniPath)!=0u;
    g_tuneMortar=GetPrivateProfileIntW(MORTAR,L"Enabled",1,g_iniPath)!=0u;
    g_tuneChiralParticleCannon=GetPrivateProfileIntW(CHIRAL,L"Enabled",1,g_iniPath)!=0u;
    g_tuneMissileLauncher=GetPrivateProfileIntW(MISSILE,L"Enabled",1,g_iniPath)!=0u;

    const u32 mgRange=read_truck_percent(MG,L"RangePercent",175u,100u,300u);
    const u32 mortarRange=read_truck_percent(MORTAR,L"RangePercent",175u,100u,300u);
    const u32 chiralRange=read_truck_percent(CHIRAL,L"RangePercent",175u,100u,300u);
    const u32 missileRange=read_truck_percent(MISSILE,L"RangePercent",175u,100u,300u);

    const u32 mgAim=read_truck_percent(MG,L"AimSpeedPercent",250u,100u,500u);
    const u32 mortarAim=read_truck_percent(MORTAR,L"AimSpeedPercent",250u,100u,500u);
    const u32 chiralAim=read_truck_percent(CHIRAL,L"AimSpeedPercent",250u,100u,500u);
    const u32 missileAim=read_truck_percent(MISSILE,L"AimSpeedPercent",250u,100u,500u);

    g_heavyMachineGunRangeMultiplier=(float)mgRange/100.0f;
    g_mortarRangeMultiplier=(float)mortarRange/100.0f;
    g_chiralParticleCannonRangeMultiplier=(float)chiralRange/100.0f;
    g_missileLauncherRangeMultiplier=(float)missileRange/100.0f;
    g_heavyMachineGunAimSpeedMultiplier=(float)mgAim/100.0f;
    g_mortarAimSpeedMultiplier=(float)mortarAim/100.0f;
    g_chiralParticleCannonAimSpeedMultiplier=(float)chiralAim/100.0f;
    g_missileLauncherAimSpeedMultiplier=(float)missileAim/100.0f;

    g_heavyMachineGunLockOnSeconds=read_truck_float(
        MG,L"LockOnSeconds",L"0.20",0.20f,0.05f,5.0f);
    g_heavyMachineGunFireSeconds=read_truck_float(
        MG,L"BurstSeconds",L"1.50",1.50f,0.05f,10.0f);
    g_heavyMachineGunFireIntervalSeconds=read_truck_float(
        MG,L"CooldownSeconds",L"0.50",0.50f,0.05f,5.0f);

    g_mortarLockOnSeconds=read_truck_float(
        MORTAR,L"LockOnSeconds",L"0.40",0.40f,0.05f,5.0f);
    g_mortarFireEndSeconds=read_truck_float(
        MORTAR,L"RecoverySeconds",L"0.40",0.40f,0.0f,5.0f);
    g_mortarFireIntervalSeconds=read_truck_float(
        MORTAR,L"CooldownSeconds",L"0.75",0.75f,0.05f,5.0f);

    g_chiralParticleCannonLockOnSeconds=read_truck_float(
        CHIRAL,L"LockOnSeconds",L"0.20",0.20f,0.05f,5.0f);
    g_chiralParticleCannonFireEndSeconds=read_truck_float(
        CHIRAL,L"RecoverySeconds",L"0.40",0.40f,0.0f,5.0f);
    g_chiralParticleCannonFireIntervalSeconds=read_truck_float(
        CHIRAL,L"CooldownSeconds",L"1.00",1.00f,0.05f,5.0f);
    g_chiralParticleCannonChargeSeconds=read_truck_float(
        CHIRAL,L"ChargeSeconds",L"0.75",0.75f,0.05f,5.0f);

    g_missileLauncherLockOnSeconds=read_truck_float(
        MISSILE,L"LockOnSeconds",L"0.40",0.40f,0.05f,5.0f);
    g_missileLauncherFireEndSeconds=read_truck_float(
        MISSILE,L"RecoverySeconds",L"0.40",0.40f,0.0f,5.0f);
    g_missileLauncherFireIntervalSeconds=read_truck_float(
        MISSILE,L"CooldownSeconds",L"0.75",0.75f,0.05f,5.0f);
}
