struct TruckWeaponRawArray {
    u32 count;
    u32 capacity;
    void* entries;
};
struct TruckWeaponStreamingEvents { void** vtable; };

static void* g_truckWeaponSeen[64];
static u32 g_truckWeaponSeenCount=0u;
static bool g_truckWeaponListenerRegistered=false;
static bool g_chiralChargeApplied=false;

static bool truck_weapon_near(float value,float expected,float tolerance) {
    float d=value-expected;
    if (d<0.0f) d=-d;
    return d<=tolerance;
}
static bool truck_weapon_seen(void* resource) {
    for (u32 i=0u;i<g_truckWeaponSeenCount;++i)
        if (g_truckWeaponSeen[i]==resource) return true;
    if (g_truckWeaponSeenCount<64u)
        g_truckWeaponSeen[g_truckWeaponSeenCount++]=resource;
    return false;
}
static HANDLE open_log_append() {
    if (!g_logPath[0] && !build_log_path()) return (HANDLE)(s64)-1;
    return CreateFileW(g_logPath,0x00000004u,
                       FILE_SHARE_READ_VALUE|FILE_SHARE_WRITE_VALUE,0,
                       4u,FILE_ATTRIBUTE_NORMAL_VALUE,0);
}
static void write_u32_decimal(HANDLE log,u32 value) {
    char text[16]; u32 pos=0u,revCount=0u; char rev[16];
    do { rev[revCount++]=(char)('0'+value%10u); value/=10u; } while (value);
    while (revCount) text[pos++]=rev[--revCount];
    text[pos]=0; write_text(log,text);
}
static bool write_resource_float(float* address,float value) {
    if (!address) return false;
    DWORD oldProtect=0u;
    if (!VirtualProtect(address,sizeof(float),PAGE_READWRITE_VALUE,&oldProtect)) return false;
    *address=value;
    DWORD ignored=0u;
    return VirtualProtect(address,sizeof(float),oldProtect,&ignored)!=0;
}
static bool set_resource_float_if_expected(float* address,float expected,float target) {
    const float current=*address;
    if (truck_weapon_near(current,target,0.02f)) return true;
    if (!truck_weapon_near(current,expected,0.05f)) return false;
    return write_resource_float(address,target);
}
static const char* truck_weapon_name(u16 left,u16 right) {
    if (left==118u && right==119u) return "HeavyMachineGun";
    if (left==169u || right==169u) return "Mortar";
    if (left==163u || right==163u) return "ChiralParticleCannon";
    if (left==165u || right==165u) return "MissileLauncher";
    return "Other";
}
static void log_truck_weapon_profile(u8* resource,bool tuned,
                                     float oldUpper,float oldAim,float oldLock,
                                     float oldFire,float oldFireEnd,float oldInterval) {
    HANDLE log=open_log_append();
    if (log==(HANDLE)(s64)-1) return;
    const u16 left=*(u16*)(resource+0x30u);
    const u16 right=*(u16*)(resource+0x32u);
    write_text(log,"truck_weapon_profile name=");
    write_text(log,truck_weapon_name(left,right));
    write_text(log," left="); write_u32_decimal(log,left);
    write_text(log," right="); write_u32_decimal(log,right);
    write_text(log," lower="); write_decimal3(log,*(float*)(resource+0x34u));
    write_text(log," upper="); write_decimal3(log,oldUpper);
    write_text(log," -> "); write_decimal3(log,*(float*)(resource+0x38u));
    write_text(log," aim_deg="); write_decimal3(log,oldAim);
    write_text(log," -> "); write_decimal3(log,*(float*)(resource+0x60u));
    write_text(log," lock="); write_decimal3(log,oldLock);
    write_text(log," -> "); write_decimal3(log,*(float*)(resource+0x64u));
    write_text(log," fire="); write_decimal3(log,oldFire);
    write_text(log," -> "); write_decimal3(log,*(float*)(resource+0x54u));
    write_text(log," fire_end="); write_decimal3(log,oldFireEnd);
    write_text(log," -> "); write_decimal3(log,*(float*)(resource+0x58u));
    write_text(log," interval="); write_decimal3(log,oldInterval);
    write_text(log," -> "); write_decimal3(log,*(float*)(resource+0x5Cu));
    write_text(log,tuned ? " tuned=YES\r\n" : " tuned=NO\r\n");
    CloseHandle(log);
}
static void tune_heavy_machine_gun(u8* resource) {
    const float rangeFactor=g_heavyMachineGunRangeMultiplier;
    const float oldUpper=*(float*)(resource+0x38u);
    const float oldAim=*(float*)(resource+0x60u);
    const float oldLock=*(float*)(resource+0x64u);
    const float oldFire=*(float*)(resource+0x54u);
    const float oldFireEnd=*(float*)(resource+0x58u);
    const float oldInterval=*(float*)(resource+0x5Cu);
    bool ok=true;

    ok=set_resource_float_if_expected((float*)(resource+0x38u),60.0f,60.0f*rangeFactor) && ok;
    ok=set_resource_float_if_expected((float*)(resource+0x3Cu),8.0f,8.0f*rangeFactor) && ok;
    ok=set_resource_float_if_expected((float*)(resource+0x60u),35.0f,
                                      35.0f*g_heavyMachineGunAimSpeedMultiplier) && ok;
    ok=set_resource_float_if_expected((float*)(resource+0x64u),0.5f,
                                      g_heavyMachineGunLockOnSeconds) && ok;
    ok=set_resource_float_if_expected((float*)(resource+0x54u),4.0f,
                                      g_heavyMachineGunFireSeconds) && ok;
    ok=set_resource_float_if_expected((float*)(resource+0x5Cu),2.0f,
                                      g_heavyMachineGunFireIntervalSeconds) && ok;

    u8* longRange=*(u8**)(resource+0x90u);
    if (longRange && *(void***)longRange==(void**)(g_imageBase+0x033ADDC0u)) {
        ok=set_resource_float_if_expected((float*)(longRange+0x24u),100.0f,100.0f*rangeFactor) && ok;
        ok=set_resource_float_if_expected((float*)(longRange+0x28u),80.0f,80.0f*rangeFactor) && ok;
    } else {
        ok=false;
    }
    log_truck_weapon_profile(resource,ok,oldUpper,oldAim,oldLock,
                             oldFire,oldFireEnd,oldInterval);
}
static void tune_mortar(u8* resource) {
    const float oldUpper=*(float*)(resource+0x38u);
    const float oldAim=*(float*)(resource+0x60u);
    const float oldLock=*(float*)(resource+0x64u);
    const float oldFire=*(float*)(resource+0x54u);
    const float oldFireEnd=*(float*)(resource+0x58u);
    const float oldInterval=*(float*)(resource+0x5Cu);
    bool ok=true;

    ok=set_resource_float_if_expected((float*)(resource+0x38u),120.0f,
                                      120.0f*g_mortarRangeMultiplier) && ok;
    ok=set_resource_float_if_expected((float*)(resource+0x60u),35.0f,
                                      35.0f*g_mortarAimSpeedMultiplier) && ok;
    ok=set_resource_float_if_expected((float*)(resource+0x64u),1.5f,
                                      g_mortarLockOnSeconds) && ok;
    ok=set_resource_float_if_expected((float*)(resource+0x58u),1.0f,
                                      g_mortarFireEndSeconds) && ok;
    ok=set_resource_float_if_expected((float*)(resource+0x5Cu),3.0f,
                                      g_mortarFireIntervalSeconds) && ok;

    // Keep native 45 m minimum distance, FireSecond=0 and -1 precision sentinel.
    log_truck_weapon_profile(resource,ok,oldUpper,oldAim,oldLock,
                             oldFire,oldFireEnd,oldInterval);
}

static void tune_chiral_particle_cannon(u8* resource) {
    const float rangeFactor=g_chiralParticleCannonRangeMultiplier;
    const float oldUpper=*(float*)(resource+0x38u);
    const float oldAim=*(float*)(resource+0x60u);
    const float oldLock=*(float*)(resource+0x64u);
    const float oldFire=*(float*)(resource+0x54u);
    const float oldFireEnd=*(float*)(resource+0x58u);
    const float oldInterval=*(float*)(resource+0x5Cu);
    bool ok=true;

    ok=set_resource_float_if_expected((float*)(resource+0x38u),50.0f,
                                      50.0f*rangeFactor) && ok;
    ok=set_resource_float_if_expected((float*)(resource+0x60u),32.0f,
                                      32.0f*g_chiralParticleCannonAimSpeedMultiplier) && ok;
    ok=set_resource_float_if_expected((float*)(resource+0x64u),0.5f,
                                      g_chiralParticleCannonLockOnSeconds) && ok;
    ok=set_resource_float_if_expected((float*)(resource+0x58u),1.0f,
                                      g_chiralParticleCannonFireEndSeconds) && ok;
    ok=set_resource_float_if_expected((float*)(resource+0x5Cu),4.0f,
                                      g_chiralParticleCannonFireIntervalSeconds) && ok;

    u8* longRange=*(u8**)(resource+0x90u);
    if (longRange && *(void***)longRange==(void**)(g_imageBase+0x033ADDC0u)) {
        // Native long-range override is 10 / 200 / -1. Keep minimum and sentinel.
        ok=set_resource_float_if_expected((float*)(longRange+0x24u),200.0f,
                                          200.0f*rangeFactor) && ok;
    } else {
        ok=false;
    }

    log_truck_weapon_profile(resource,ok,oldUpper,oldAim,oldLock,
                             oldFire,oldFireEnd,oldInterval);
}
static void tune_missile_launcher(u8* resource) {
    const float oldUpper=*(float*)(resource+0x38u);
    const float oldAim=*(float*)(resource+0x60u);
    const float oldLock=*(float*)(resource+0x64u);
    const float oldFire=*(float*)(resource+0x54u);
    const float oldFireEnd=*(float*)(resource+0x58u);
    const float oldInterval=*(float*)(resource+0x5Cu);
    bool ok=true;

    ok=set_resource_float_if_expected((float*)(resource+0x38u),40.0f,
                                      40.0f*g_missileLauncherRangeMultiplier) && ok;
    ok=set_resource_float_if_expected((float*)(resource+0x60u),35.0f,
                                      35.0f*g_missileLauncherAimSpeedMultiplier) && ok;
    ok=set_resource_float_if_expected((float*)(resource+0x64u),2.0f,
                                      g_missileLauncherLockOnSeconds) && ok;
    ok=set_resource_float_if_expected((float*)(resource+0x58u),1.0f,
                                      g_missileLauncherFireEndSeconds) && ok;
    ok=set_resource_float_if_expected((float*)(resource+0x5Cu),2.0f,
                                      g_missileLauncherFireIntervalSeconds) && ok;

    // Keep native 12 m minimum, FireSecond=0 and -1 precision sentinel.
    log_truck_weapon_profile(resource,ok,oldUpper,oldAim,oldLock,
                             oldFire,oldFireEnd,oldInterval);
}

static bool tune_chiral_particle_cannon_charge() {
    if (!g_tuneChiralParticleCannon) return true;
    if (g_chiralChargeApplied) return true;
    if (!g_imageBase) return false;

    u8* database=*(u8**)(g_imageBase+0x0623FA50u);
    if (!database) return false;

    const u32 ammoCount=*(u32*)(database+0x40u);
    u8** ammoEntries=*(u8***)(database+0x48u);
    if (!ammoEntries || ammoCount==0u || ammoCount>4096u) return false;

    for (u32 i=0u;i<ammoCount;++i) {
        u8* ammo=ammoEntries[i];
        if (!ammo) continue;
        if (*(u16*)(ammo+0x20u)!=275u) continue;

        if (*(void***)ammo!=(void**)(g_imageBase+0x033C4E80u)) return false;

        float* chargeSeconds=(float*)(ammo+0x80u);
        const float oldCharge=*chargeSeconds;
        const bool ok=set_resource_float_if_expected(
            chargeSeconds,3.0f,g_chiralParticleCannonChargeSeconds);

        HANDLE log=open_log_append();
        if (log!=(HANDLE)(s64)-1) {
            write_text(log,"chiral_cannon_charge ammo_id=275 ds_ammo_parameter_offset=0x80 ");
            write_text(log,"charge_seconds="); write_decimal3(log,oldCharge);
            write_text(log," -> "); write_decimal3(log,*chargeSeconds);
            write_text(log,ok ? " tuned=YES\r\n" : " tuned=NO\r\n");
            CloseHandle(log);
        }
        if (ok) g_chiralChargeApplied=true;
        return ok;
    }
    return false;
}

static void inspect_truck_weapon_resource(void* object) {
    if (!object) return;
    void** vtable=*(void***)object;
    if (vtable!=(void**)(g_imageBase+0x033AE180u)) return;

    u8* resource=(u8*)object;
    const u16 left=*(u16*)(resource+0x30u);
    const u16 right=*(u16*)(resource+0x32u);
    const bool firstObservation=!truck_weapon_seen(resource);

    if (left==118u && right==119u && g_tuneHeavyMachineGun) {
        tune_heavy_machine_gun(resource);
        return;
    }
    if ((left==169u || right==169u) && g_tuneMortar) {
        tune_mortar(resource);
        return;
    }
    if ((left==163u || right==163u) && g_tuneChiralParticleCannon) {
        tune_chiral_particle_cannon(resource);
        return;
    }
    if ((left==165u || right==165u) && g_tuneMissileLauncher) {
        tune_missile_launcher(resource);
        return;
    }
    if (firstObservation &&
        (left==118u || right==119u || left==163u || left==165u || left==169u ||
         right==163u || right==165u || right==169u)) {
        log_truck_weapon_profile(resource,false,
                                 *(float*)(resource+0x38u),
                                 *(float*)(resource+0x60u),
                                 *(float*)(resource+0x64u),
                                 *(float*)(resource+0x54u),
                                 *(float*)(resource+0x58u),
                                 *(float*)(resource+0x5Cu));
    }
}
static void FASTCALL truck_weapon_on_finish_load(TruckWeaponStreamingEvents*,
                                                  const TruckWeaponRawArray* objects) {
    if (g_tuneChiralParticleCannon && !g_chiralChargeApplied)
        tune_chiral_particle_cannon_charge();
    if (!objects || !objects->entries || objects->count==0u || objects->count>65536u) return;
    void** entries=(void**)objects->entries;
    for (u32 i=0u;i<objects->count;++i) {
        // The original Sam Overhaul streaming listener already observes native
        // resource loads. Reuse it instead of installing an additional hook.
        // Non-LocalizedTextResource objects cost one vtable comparison.
        const int changed=SamShelterRestLabelOnStreamResource(entries[i]);
        if (changed==1) {
            HANDLE log=open_log_append();
            if(log!=(HANDLE)(s64)-1){
                write_text(log,"shelter_rest_label=NATIVE_REST_TEXT_PATCHED original_action_preserved\r\n");
                CloseHandle(log);
            }
        }
        inspect_truck_weapon_resource(entries[i]);
    }
}
static void FASTCALL truck_weapon_on_before_unload(TruckWeaponStreamingEvents*,
                                                   const TruckWeaponRawArray*) {}
static void FASTCALL truck_weapon_on_load_asset(TruckWeaponStreamingEvents*,
                                                const TruckWeaponRawArray*) {}

static void* g_truckWeaponListenerVtable[3]={
    (void*)&truck_weapon_on_finish_load,
    (void*)&truck_weapon_on_before_unload,
    (void*)&truck_weapon_on_load_asset
};
static TruckWeaponStreamingEvents g_truckWeaponListener={g_truckWeaponListenerVtable};

static u64 truck_weapon_resolve_rip(u64 instruction,u32 operandOffset) {
    const s32 relative=*(const s32*)(instruction+operandOffset);
    return instruction+(u64)operandOffset+4ull+(s64)relative;
}
static bool truck_weapon_is_text_section(const u8* name) {
    return name[0]=='.' && name[1]=='t' && name[2]=='e' && name[3]=='x' && name[4]=='t';
}
static u64 find_streaming_manager_global() {
    if (!g_imageBase) return 0ull;
    const u32 peOffset=*(u32*)(g_imageBase+0x3Cu);
    u8* nt=g_imageBase+peOffset;
    const u16 sectionCount=*(u16*)(nt+6u);
    const u16 optionalSize=*(u16*)(nt+20u);
    if (!sectionCount || sectionCount>96u) return 0ull;
    u8* section=nt+24u+optionalSize;
    u8* textStart=0;
    u32 textSize=0u;
    for (u16 i=0u;i<sectionCount;++i,section+=40u) {
        if (truck_weapon_is_text_section(section)) {
            textSize=*(u32*)(section+8u);
            textStart=g_imageBase+*(u32*)(section+12u);
            break;
        }
    }
    if (!textStart || textSize<0x1000u) return 0ull;
    static const u8 signature[28]={
        0x48,0x89,0x05,0,0,0,0,0xE8,0,0,0,0,0x33,0xD2,0x41,0xB8,
        0xF8,0x0A,0x00,0x00,0x48,0x8B,0xC8,0x48,0x8B,0xD8,0xE8,0
    };
    static const u8 mask[28]={
        1,1,1,0,0,0,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0
    };
    for (u32 i=0u;i+28u<=textSize;++i) {
        bool match=true;
        for (u32 j=0u;j<28u;++j) {
            if (mask[j] && textStart[i+j]!=signature[j]) { match=false; break; }
        }
        if (match) return truck_weapon_resolve_rip((u64)(textStart+i),3u);
    }
    return 0ull;
}
static bool install_truck_weapon_streaming_listener(HANDLE log) {
    // Keep listener enabled when only TimefallShelterRange/FixRestPrompt
    // is on, even if every truck-weapon tuning feature is disabled.
    if (!g_tuneHeavyMachineGun && !g_tuneMortar &&
        !g_tuneChiralParticleCannon && !g_tuneMissileLauncher &&
        !SamShelterRestLabelEnabled()) {
        write_text(log,"truck_weapon_tuning=DISABLED\r\n");
        return true;
    }
    const u64 globalAddress=find_streaming_manager_global();
    if (!globalAddress) {
        write_text(log,"truck_weapon_tuning=FAILED streaming_global_not_found\r\n");
        return false;
    }
    void** managerGlobal=(void**)globalAddress;
    void* manager=0;
    for (u32 i=0u;i<200u;++i) {
        manager=*managerGlobal;
        if (manager) break;
        Sleep(50u);
    }
    if (!manager) {
        write_text(log,"truck_weapon_tuning=FAILED streaming_manager_not_ready\r\n");
        return false;
    }
    void* streamingSystem=*(void**)((u8*)manager+0x578u);
    if (!streamingSystem) {
        write_text(log,"truck_weapon_tuning=FAILED streaming_system_missing\r\n");
        return false;
    }
    void** vtable=*(void***)streamingSystem;
    if (!vtable || !vtable[3]) {
        write_text(log,"truck_weapon_tuning=FAILED listener_api_missing\r\n");
        return false;
    }
    typedef void (FASTCALL *AddListenerFn)(void*,void*);
    ((AddListenerFn)vtable[3])(streamingSystem,&g_truckWeaponListener);
    g_truckWeaponListenerRegistered=true;
    write_text(log,"truck_weapon_tuning=LISTENER_REGISTERED fast_vtable_filter=TRUE\r\n");
    if(SamShelterRestLabelEnabled())
        write_text(log,"shelter_rest_label=STREAMING_LISTENER_REGISTERED\r\n");
    if (!tune_chiral_particle_cannon_charge())
        write_text(log,"chiral_cannon_charge=WAITING_FOR_AMMO_TABLE\r\n");
    write_text(log,"heavy_machine_gun_ids=118/119 mortar_id=169 chiral_cannon_id=163 missile_launcher_id=165\r\n");
    write_text(log,"heavy_machine_gun_range_multiplier=");
    write_decimal3(log,g_heavyMachineGunRangeMultiplier); write_text(log,"\r\n");
    write_text(log,"heavy_machine_gun_aim_multiplier=");
    write_decimal3(log,g_heavyMachineGunAimSpeedMultiplier); write_text(log,"\r\n");
    write_text(log,"heavy_machine_gun_lock_seconds=");
    write_decimal3(log,g_heavyMachineGunLockOnSeconds); write_text(log,"\r\n");
    write_text(log,"heavy_machine_gun_fire_seconds=");
    write_decimal3(log,g_heavyMachineGunFireSeconds); write_text(log,"\r\n");
    write_text(log,"heavy_machine_gun_fire_interval_seconds=");
    write_decimal3(log,g_heavyMachineGunFireIntervalSeconds); write_text(log,"\r\n");
    write_text(log,"mortar_range_multiplier=");
    write_decimal3(log,g_mortarRangeMultiplier); write_text(log,"\r\n");
    write_text(log,"mortar_aim_multiplier=");
    write_decimal3(log,g_mortarAimSpeedMultiplier); write_text(log,"\r\n");
    write_text(log,"mortar_lock_seconds=");
    write_decimal3(log,g_mortarLockOnSeconds); write_text(log,"\r\n");
    write_text(log,"mortar_fire_end_seconds=");
    write_decimal3(log,g_mortarFireEndSeconds); write_text(log,"\r\n");
    write_text(log,"mortar_fire_interval_seconds=");
    write_decimal3(log,g_mortarFireIntervalSeconds); write_text(log,"\r\n");
    write_text(log,"chiral_cannon_range_multiplier=");
    write_decimal3(log,g_chiralParticleCannonRangeMultiplier); write_text(log,"\r\n");
    write_text(log,"chiral_cannon_aim_multiplier=");
    write_decimal3(log,g_chiralParticleCannonAimSpeedMultiplier); write_text(log,"\r\n");
    write_text(log,"chiral_cannon_lock_seconds=");
    write_decimal3(log,g_chiralParticleCannonLockOnSeconds); write_text(log,"\r\n");
    write_text(log,"chiral_cannon_fire_end_seconds=");
    write_decimal3(log,g_chiralParticleCannonFireEndSeconds); write_text(log,"\r\n");
    write_text(log,"chiral_cannon_fire_interval_seconds=");
    write_decimal3(log,g_chiralParticleCannonFireIntervalSeconds); write_text(log,"\r\n");
    write_text(log,"chiral_cannon_charge_seconds=");
    write_decimal3(log,g_chiralParticleCannonChargeSeconds); write_text(log,"\r\n");
    write_text(log,"missile_launcher_range_multiplier=");
    write_decimal3(log,g_missileLauncherRangeMultiplier); write_text(log,"\r\n");
    write_text(log,"missile_launcher_aim_multiplier=");
    write_decimal3(log,g_missileLauncherAimSpeedMultiplier); write_text(log,"\r\n");
    write_text(log,"missile_launcher_lock_seconds=");
    write_decimal3(log,g_missileLauncherLockOnSeconds); write_text(log,"\r\n");
    write_text(log,"missile_launcher_fire_end_seconds=");
    write_decimal3(log,g_missileLauncherFireEndSeconds); write_text(log,"\r\n");
    write_text(log,"missile_launcher_fire_interval_seconds=");
    write_decimal3(log,g_missileLauncherFireIntervalSeconds); write_text(log,"\r\n");
    return true;
}
