// Optional bounded observation of the native mount sequence.
// ABI verified against the exact EXE disassembly, not decompiler prototypes:
// 10047D0 consumes RCX and returns AL; 100C400/1120D60/F9A390 consume RCX.
// E15D30 consumes RCX and loads its delta time from player+7544 itself.
// 1F6BDE0 consumes RCX/RDX/R8B/R9/[entry RSP+28] and returns RAX.
// E0E230 and the generic phase callbacks are deliberately not intercepted.

typedef BYTE (__fastcall* MtFrontendFn)(void*);
typedef void (__fastcall* MtVoidFn)(void*);
typedef UINT64 (__fastcall* MtReserveFn)(void*, void*, BYTE, void*, BYTE);
static MtFrontendFn g_mtFrontend = 0;
static MtVoidFn g_mtActivate = 0, g_mtDeactivate = 0, g_mtPhase5 = 0, g_mtRideOn = 0;
static MtReserveFn g_mtReserve = 0;
static volatile LONG g_mtFrontendCalls = 0, g_mtFrontendLogs = 0;
static volatile LONG g_mtActivationLogs = 0, g_mtDeactivationLogs = 0;
static volatile LONG g_mtPhaseCalls = 0, g_mtPhaseLogs = 0, g_mtRideOnLogs = 0, g_mtReserveLogs = 0;
static volatile LONG g_mtLastFrontend = -1;
static volatile UINT64 g_mtLastPhase = ~0ull, g_mtLastRideOn = ~0ull;

static UINT64 mt_qword(void* object, SIZE_T offset) {
    return object && readable_range((BYTE*)object + offset, sizeof(UINT64))
        ? *(UINT64*)((BYTE*)object + offset) : ~0ull;
}
static UINT32 mt_dword(void* object, SIZE_T offset) {
    return object && readable_range((BYTE*)object + offset, sizeof(UINT32))
        ? *(UINT32*)((BYTE*)object + offset) : 0xffffffffu;
}
static UINT32 mt_byte(void* object, SIZE_T offset) {
    return object && readable_range((BYTE*)object + offset, 1)
        ? ((BYTE*)object)[offset] : 0xffu;
}
static void* mt_pointer(void* object, SIZE_T offset) {
    UINT64 value = mt_qword(object, offset);
    return value == ~0ull ? 0 : (void*)value;
}
static UINT64 mt_rva(UINT64 address) {
    UINT64 base = (UINT64)g_executable;
    return address >= base && address < base + EXPECTED_IMAGE_SIZE ? address - base : ~0ull;
}
static UINT64 mt_mix(UINT64 seed, UINT64 value) {
    return (seed ^ value) * 0x100000001B3ull;
}
static void mt_header(LogBuffer* m, const char* stage) {
    log_init(m); log_prefix(m); log_text(m, "MOUNT TRACE "); log_text(m, stage);
    log_text(m, " qpidHits="); log_uint(m, (UINT64)__atomic_load_n(&g_v151QpidHits, __ATOMIC_ACQUIRE));
    log_text(m, " inhibitBB="); log_uint(m, v181_mount_inhibit_signal() ? 1 : 0);
}
static void mt_plugin(LogBuffer* m, void* plugin) {
    log_text(m, " plugin="); log_uint(m, (UINT64)plugin);
    log_text(m, " active8="); log_uint(m, mt_byte(plugin, 8));
    log_text(m, " disableA="); log_uint(m, mt_byte(plugin, 0xA));
    log_text(m, " target220="); log_uint(m, mt_qword(plugin, 0x220));
    void* player = mt_pointer(plugin, 0x28);
    log_text(m, " gate739D="); log_uint(m, mt_byte(player, 0x739D));
    void* collector = 0; void* entity = 0; void* vehicle = 0;
    bool coffin = v178_live_ridevehicle_collector_is_coffin(plugin, &collector, &entity, &vehicle);
    log_text(m, " collectorCoffin="); log_uint(m, coffin ? 1 : 0);
    log_text(m, " collectorFlags218="); log_uint(m, mt_dword(collector, 0x218));
    log_text(m, " vehicle="); log_uint(m, (UINT64)vehicle);
}
static void mt_finish(LogBuffer* m) {
    log_text(m, "\r\n"); append_log(m);
}

static BYTE __fastcall mt_frontend(void* plugin) {
    BYTE result = g_mtFrontend(plugin);
    LONG call = __atomic_fetch_add(&g_mtFrontendCalls, 1, __ATOMIC_ACQ_REL);
    bool changed = __atomic_exchange_n(&g_mtLastFrontend, (LONG)result, __ATOMIC_ACQ_REL) != result;
    if (changed || call % 600 == 0) {
        if (__atomic_fetch_add(&g_mtFrontendLogs, 1, __ATOMIC_ACQ_REL) < 256) {
            LogBuffer m; mt_header(&m, "FRONTEND");
            log_text(&m, " result="); log_uint(&m, result);
            mt_plugin(&m, plugin); mt_finish(&m);
        }
    }
    return result;
}
static void __fastcall mt_activate(void* plugin) {
    UINT32 before = mt_byte(plugin, 8);
    g_mtActivate(plugin);
    if (__atomic_fetch_add(&g_mtActivationLogs, 1, __ATOMIC_ACQ_REL) >= 128) return;
    LogBuffer m; mt_header(&m, "ACTIVATE");
    log_text(&m, " activeBefore="); log_uint(&m, before);
    mt_plugin(&m, plugin); mt_finish(&m);
}
static void __fastcall mt_deactivate(void* plugin) {
    UINT32 activeBefore = mt_byte(plugin, 8), disabledBefore = mt_byte(plugin, 0xA);
    UINT64 targetBefore = mt_qword(plugin, 0x220);
    g_mtDeactivate(plugin);
    if (__atomic_fetch_add(&g_mtDeactivationLogs, 1, __ATOMIC_ACQ_REL) >= 128) return;
    LogBuffer m; mt_header(&m, "DEACTIVATE");
    log_text(&m, " activeBefore="); log_uint(&m, activeBefore);
    log_text(&m, " disableBefore="); log_uint(&m, disabledBefore);
    log_text(&m, " targetBefore="); log_uint(&m, targetBefore);
    mt_plugin(&m, plugin); mt_finish(&m);
}

static void __fastcall mt_phase5(void* state) {
    // Log after native dispatch. All incoming game parameters are consumed
    // before any observer work; do not read the plugin array on every frame.
    g_mtPhase5(state);
    LONG call = __atomic_fetch_add(&g_mtPhaseCalls, 1, __ATOMIC_ACQ_REL);
    if (call % 120 != 0) return;
    void* entries = mt_pointer(state, 0xAA8);
    if (!entries || !readable_range(entries, 42 * sizeof(void*))) return;
    void* plugin = 0;
    for (UINT32 i = 1; i < 42; ++i) {
        void* candidate = ((void**)entries)[i];
        if (mt_qword(candidate, 0) == (UINT64)g_executable + RVA_RIDEVEHICLE_PLUGIN_VTABLE) {
            plugin = candidate; break;
        }
    }
    void* player = mt_pointer(state, 0x298);
    void* owner = mt_pointer(state, 0x30);
    void* mover = mt_pointer(owner, 0xC0);
    UINT64 moverVt = mt_rva(mt_qword(mover, 0));
    UINT32 gate = mt_byte(player, 0x739D), suppress = mt_byte(state, 0xAB0);
    UINT64 sig = mt_mix((UINT64)state, moverVt);
    sig = mt_mix(sig, (UINT64)plugin);
    sig = mt_mix(sig, gate | (suppress << 8) | (mt_byte(plugin, 8) << 16) | (mt_byte(plugin, 0xA) << 24));
    sig = mt_mix(sig, __atomic_load_n(&g_v151QpidHits, __ATOMIC_ACQUIRE) > 0 ? 1u : 0u);
    bool changed = __atomic_exchange_n(&g_mtLastPhase, sig, __ATOMIC_ACQ_REL) != sig;
    if (!changed && call % 1800 != 0) return;
    if (__atomic_fetch_add(&g_mtPhaseLogs, 1, __ATOMIC_ACQ_REL) >= 256) return;
    LogBuffer m; mt_header(&m, "PHASE5");
    log_text(&m, " state="); log_uint(&m, (UINT64)state);
    log_text(&m, " moverVtRva="); log_uint(&m, moverVt);
    log_text(&m, " stateGate739D="); log_uint(&m, gate);
    log_text(&m, " suppressAB0="); log_uint(&m, suppress);
    mt_plugin(&m, plugin); mt_finish(&m);
}
static void __fastcall mt_rideon(void* state) {
    UINT32 before = mt_dword(state, 0x198);
    void* plugin = mt_pointer(state, 0x190);
    UINT64 sig = mt_mix((UINT64)state, before);
    sig = mt_mix(sig, mt_qword(plugin, 0x220));
    bool changed = __atomic_exchange_n(&g_mtLastRideOn, sig, __ATOMIC_ACQ_REL) != sig;
    g_mtRideOn(state);
    UINT32 after = mt_dword(state, 0x198);
    if (!changed && before == after) return;
    if (__atomic_fetch_add(&g_mtRideOnLogs, 1, __ATOMIC_ACQ_REL) >= 128) return;
    LogBuffer m; mt_header(&m, "RIDEON6");
    log_text(&m, " stageBefore="); log_uint(&m, before);
    log_text(&m, " stageAfter="); log_uint(&m, after);
    mt_plugin(&m, plugin); mt_finish(&m);
}
static UINT64 __fastcall mt_reserve(void* vehicle, void* owner, BYTE acquire, void* context, BYTE fifth) {
    UINT64 flags = mt_qword(vehicle, 0x98);
    UINT32 state = mt_dword(vehicle, 0x4E4), occupied = mt_dword(vehicle, 0x4FC);
    UINT64 result = g_mtReserve(vehicle, owner, acquire, context, fifth);
    if (__atomic_fetch_add(&g_mtReserveLogs, 1, __ATOMIC_ACQ_REL) >= 128) return result;
    LogBuffer m; mt_header(&m, "RESERVE");
    log_text(&m, " vehicle="); log_uint(&m, (UINT64)vehicle);
    log_text(&m, " vehicleVtRva="); log_uint(&m, mt_rva(mt_qword(vehicle, 0)));
    log_text(&m, " acquire="); log_uint(&m, acquire);
    log_text(&m, " preBit9="); log_uint(&m, (flags & 0x200) ? 1 : 0);
    log_text(&m, " preState4E4="); log_uint(&m, state);
    log_text(&m, " preReserved4FC="); log_uint(&m, occupied);
    log_text(&m, " result="); log_uint(&m, result);
    mt_finish(&m); return result;
}

static bool mt_patch_slot(BYTE* base, UINT32 cellRva, void* wrapper) {
    void** slot = (void**)(base + cellRva);
    DWORD oldProtection = 0;
    if (!VirtualProtect(slot, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtection)) return false;
    __atomic_store_n(slot, wrapper, __ATOMIC_RELEASE);
    DWORD ignored = 0; VirtualProtect(slot, sizeof(void*), oldProtection, &ignored);
    return *slot == wrapper;
}
static bool install_mount_trace(HMODULE executable) {
    BYTE* base = (BYTE*)executable;
    const UINT32 cells[] = {0x325E170u, 0x325E1B0u, 0x325E1B8u, 0x325B7C8u};
    const UINT32 targets[] = {0x10047D0u, 0x100C400u, 0x1120D60u, 0xF9A390u};
    for (UINT32 i = 0; i < 4; ++i) {
        if (!readable_range(base + cells[i], sizeof(void*)) || *(void**)(base + cells[i]) != base + targets[i]) return false;
    }
    static const BYTE prologue[16] = {0x40,0x56,0x48,0x83,0xEC,0x30,0x48,0x8B,0x81,0x98,0x02,0x00,0x00,0x48,0x8B,0xF1};
    BYTE* phase = base + 0xE15D30u;
    if (!readable_range(phase, 16) || !bytes_equal(phase, prologue, 16)) return false;
    BYTE* reserveCall = base + 0xF9AD51u;
    int displacement = 0;
    if (!v151_validate_call(reserveCall, base + 0x1F6BDE0u, &displacement)) return false;
    BYTE* reserveRelay = (BYTE*)allocate_call_relay_near(reserveCall, (void*)&mt_reserve);
    if (!reserveRelay) return false;
    BYTE* trampoline = (BYTE*)VirtualAlloc(0, 32, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!trampoline) return false;
    static const BYTE jump[6] = {0xFF,0x25,0,0,0,0};
    memcpy(trampoline, prologue, 16); memcpy(trampoline + 16, jump, 6);
    UINT64 continuation = (UINT64)(phase + 16); memcpy(trampoline + 22, &continuation, 8);
    FlushInstructionCache((HANDLE)(INT64)-1, trampoline, 30);
    g_mtFrontend = (MtFrontendFn)(base + targets[0]);
    g_mtActivate = (MtVoidFn)(base + targets[1]);
    g_mtDeactivate = (MtVoidFn)(base + targets[2]);
    g_mtRideOn = (MtVoidFn)(base + targets[3]);
    g_mtPhase5 = (MtVoidFn)trampoline;
    g_mtReserve = (MtReserveFn)(base + 0x1F6BDE0u);
    if (!mt_patch_slot(base, cells[0], (void*)&mt_frontend) ||
        !mt_patch_slot(base, cells[1], (void*)&mt_activate) ||
        !mt_patch_slot(base, cells[2], (void*)&mt_deactivate) ||
        !mt_patch_slot(base, cells[3], (void*)&mt_rideon)) return false;
    BYTE replacement[16] = {0xFF,0x25,0,0,0,0,0,0,0,0,0,0,0,0,0x90,0x90};
    UINT64 wrapper = (UINT64)&mt_phase5; memcpy(replacement + 6, &wrapper, 8);
    if (!freeze_for_patch(phase, 16)) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(phase, 16, PAGE_EXECUTE_READWRITE, &oldProtection)) { unfreeze_after_patch(); return false; }
    memcpy(phase, replacement, 16); FlushInstructionCache((HANDLE)(INT64)-1, phase, 16);
    DWORD ignored = 0; VirtualProtect(phase, 16, oldProtection, &ignored); unfreeze_after_patch();
    return v151_patch_call(reserveCall, reserveRelay, displacement);
}
