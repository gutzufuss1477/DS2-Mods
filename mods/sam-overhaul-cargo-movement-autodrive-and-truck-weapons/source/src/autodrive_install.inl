// Included after the shared Sam Overhaul memory/code helpers.
static bool install_autodrive_activation_patch(HANDLE log) {
    write_text(log,"autodrive_config_source="); write_text(log,g_autoDriveSettings.source);
    write_text(log,"\r\nautodrive_activation_seconds=");
    write_decimal3(log,g_autoDriveSettings.seconds); write_text(log,"\r\n");
    if (g_autoDriveSettings.invalid)
        write_text(log,"autodrive_config_warning=INVALID_VALUE_USING_VANILLA_5_SECONDS\r\n");
    if (!g_autoDriveSettings.enabled) {
        write_text(log,"autodrive_activation_patch=VANILLA\r\n");
        return true;
    }
    u8* site = g_imageBase + sam_autodrive::TimerRva;
    // Verify the increment, the unchanged 10-second clamp, and the native store.
    if (!bytes_equal(site,sam_autodrive::NativeContext,29u)) {
        write_text(log,"status=AUTODRIVE_DRIVING_CONTEXT_REJECTED\r\n");
        return false;
    }
    static const u8 thresholdCheck[8] = {
        0xC5,0xF8,0x2F,0x05,0xB7,0x23,0x51,0x01
    };
    static const u8 assistFlagWrite[10] = {
        0x81,0x88,0xE8,0x04,0x00,0x00,0x00,0x01,0x00,0x00
    };
    if (!bytes_equal(g_imageBase+0x01F4F94Du,thresholdCheck,8u) ||
        !bytes_equal(g_imageBase+0x01F4F95Bu,assistFlagWrite,10u) ||
        *(volatile const u32*)(g_imageBase+0x03461D0Cu) != 0x40A00000u) {
        write_text(log,"status=AUTODRIVE_READINESS_CONTEXT_REJECTED\r\n");
        return false;
    }
    g_autoDriveTimerCave = allocate_near_relay(site,site);
    if (!g_autoDriveTimerCave) {
        write_text(log,"status=AUTODRIVE_TIMER_CAVE_ALLOCATION_FAILED\r\n");
        return false;
    }
    u8 code[sam_autodrive::MaxCodeBytes];
    const u32 size = sam_autodrive::BuildTimerCode(code,sizeof(code),
        g_autoDriveSettings.multiplier,(u64)g_autoDriveTimerCave,
        (u64)(site+8u),(u64)&g_autoDriveHookExecuted);
    if (!size || !write_code(g_autoDriveTimerCave,code,size)) {
        VirtualFree(g_autoDriveTimerCave,0u,MEM_RELEASE_VALUE);
        g_autoDriveTimerCave=0;
        write_text(log,"status=AUTODRIVE_TIMER_CODE_FAILED\r\n");
        return false;
    }
    u8 replacement[8];
    build_jump_bytes(replacement,site,g_autoDriveTimerCave);
    replacement[5]=replacement[6]=replacement[7]=0x90u;
    if (!write_code(site,replacement,8u)) {
        // Do not release a relay that a partially installed jump could still use.
        if (write_code(site,sam_autodrive::NativeContext,8u)) {
            VirtualFree(g_autoDriveTimerCave,0u,MEM_RELEASE_VALUE);
            g_autoDriveTimerCave=0;
        }
        write_text(log,"status=AUTODRIVE_TIMER_HOOK_FAILED\r\n");
        return false;
    }
    write_text(log,"autodrive_activation_patch=APPLIED\r\n");
    write_text(log,"autodrive_path=DSPlayerVehicleDriving\r\n");
    write_text(log,"autodrive_timer_increment_rva=0x01F4F917\r\n");
    write_text(log,"autodrive_timer_offset=0x80 delta_register=xmm15\r\n");
    write_text(log,"autodrive_activation_multiplier=");
    write_decimal3(log,g_autoDriveSettings.multiplier); write_text(log,"\r\n");
    write_text(log,"autodrive_native_readiness=5.0 timer_cap=10.0\r\n");
    write_text(log,"autodrive_eligibility_and_reset_conditions=VANILLA\r\n");
    write_text(log,"autodrive_physics_and_speed_limits=UNMODIFIED\r\n");
    write_text(log,"autodrive_old_idle_timer_and_forced_events=REMOVED\r\n");
    return true;
}
