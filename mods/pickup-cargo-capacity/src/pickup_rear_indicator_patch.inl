// DS2 Off-road Pickup rear load indicator scaling, v1.0.2.
// The FIVE in-world display bars are set in FUN_141F935E0.
// Original byte 0x37 counts occupied 16-unit packing areas; the vanilla HUD
// maps 1-2=>bar 1, 3-4=>bar 2, ..., 9-10=>bar 5, thus saturates at 160.
// Never write to the authoritative cargo area. Scale ONLY the temporary
// integer registers used by the 3D material/LED display.
static BYTE* g_indicator_cave = 0;
static const QWORD RVA_INDICATOR_5BARS = 0x01F9369Aull;
static const QWORD RVA_INDICATOR_FLOAT = 0x01F93792ull;

// Emit a relative x64 JMP; source is the actual live instruction address.
static BOOL indicator_jmp(BYTE* buffer, BYTE* source, BYTE* destination) {
    const long long rel = (long long)(QWORD)destination - (long long)(QWORD)(source + 5);
    if (rel < -2147483648LL || rel > 2147483647LL) return 0;
    DWORD bits = (DWORD)rel;
    buffer[0] = 0xE9;
    for (int i = 0; i < 4; ++i) buffer[1 + i] = (BYTE)(bits >> (i * 8));
    return 1;
}

// Reserve the code island outside DS2.exe, close enough for rel32 JMPs.
// Start above the current EXE image (~0x0B200000 RVA).
static BYTE* indicator_alloc_near(BYTE* image_base) {
    const QWORD begin = ((QWORD)image_base + 0x0C000000ull + 0xFFFFull) & ~0xFFFFull;
    for (QWORD step = 0; step < 4096; ++step) {
        LPVOID addr = (LPVOID)(begin + step * 0x10000ull);
        BYTE* block = (BYTE*)VirtualAlloc(addr, 4096, 0x3000u, PAGE_EXECUTE_READWRITE);
        if (block) return block;
    }
    return 0;
}

static BOOL install_indicator_scaling(BYTE* base, int usable_area_count) {
    if (usable_area_count == 10) {
        g_indicator_state = "VANILLA_160_NO_PATCH_NEEDED";
        return 1;
    }
    if (usable_area_count < 10 || usable_area_count > 30) {
        g_indicator_state = "ERROR_INVALID_AREA_COUNT";
        return 0;
    }
    BYTE* first = base + RVA_INDICATOR_5BARS;
    BYTE* second = base + RVA_INDICATOR_FLOAT;
    const BYTE expected_first[6] = {0x0F, 0xB6, 0x48, 0x37, 0x84, 0xC9};
    const BYTE expected_second[9] = {0x0F, 0xB6, 0x70, 0x37, 0x44, 0x0F, 0xB6, 0x70, 0x36};
    if (!bytes_equal(first, expected_first, sizeof(expected_first)) ||
        !bytes_equal(second, expected_second, sizeof(expected_second))) {
        g_indicator_state = "ERROR_ORIGINAL_DISPLAY_BYTES_MISMATCH";
        return 0;
    }

    BYTE* cave = indicator_alloc_near(base);
    if (!cave) {
        g_indicator_state = "ERROR_NEAR_CODE_ALLOCATION";
        return 0;
    }
    const BYTE template1[44] = {
        0x0F,0xB6,0x48,0x37,        // movzx ecx, byte ptr [rax+37]
        0x50,0x52,                  // save rax,rdx
        0x89,0xC8,0x6B,0xC0,0x0A, // eax = occupied_areas * 10
        0x83,0xC0,0x00,             // eax += usable_areas-1 (ceil rounding)
        0xB9,0,0,0,0,               // ecx = usable_area_count
        0x31,0xD2,0xF7,0xF1,        // edx=0; div ecx
        0x83,0xF8,0x0A,0x76,0x05, // cap result to 10
        0xB8,0x0A,0,0,0,
        0x89,0xC1,0x5A,0x58,        // ecx=display; restore rdx,rax
        0x84,0xC9,                  // original test cl,cl for following JZ
        0xE9,0,0,0,0
    };
    const BYTE template2[49] = {
        0x0F,0xB6,0x70,0x37,       // movzx esi, byte ptr [rax+37]
        0x50,0x51,0x52,            // save rax,rcx,rdx
        0x89,0xF0,0x6B,0xC0,0x0A, // eax = occupied_areas * 10
        0x83,0xC0,0x00,
        0xB9,0,0,0,0,              // ecx = usable_area_count
        0x31,0xD2,0xF7,0xF1,
        0x83,0xF8,0x0A,0x76,0x05, // cap result to 10
        0xB8,0x0A,0,0,0,
        0x89,0xC6,0x5A,0x59,0x58, // esi=display; restore registers
        0x44,0x0F,0xB6,0x70,0x36, // original movzx r14d,byte[rax+36]
        0xE9,0,0,0,0
    };
    for (SIZE_T i = 0; i < sizeof(template1); ++i) cave[i] = template1[i];
    for (SIZE_T i = 0; i < sizeof(template2); ++i) cave[0x80 + i] = template2[i];
    cave[13] = (BYTE)(usable_area_count - 1);
    cave[15] = (BYTE)usable_area_count;
    cave[0x80 + 14] = (BYTE)(usable_area_count - 1);
    cave[0x80 + 16] = (BYTE)usable_area_count;

    BYTE hook1[6] = {0xE9,0,0,0,0,0x90};
    BYTE hook2[9] = {0xE9,0,0,0,0,0x90,0x90,0x90,0x90};
    if (!indicator_jmp(cave + 39, cave + 39, first + sizeof(expected_first)) ||
        !indicator_jmp(cave + 0x80 + 44, cave + 0x80 + 44, second + sizeof(expected_second)) ||
        !indicator_jmp(hook1, first, cave) ||
        !indicator_jmp(hook2, second, cave + 0x80)) {
        VirtualFree(cave, 0, 0x8000u);
        g_indicator_state = "ERROR_CAVE_OUTSIDE_REL32_RANGE";
        return 0;
    }
    if (!FlushInstructionCache(GetCurrentProcess(), cave, 4096)) {
        VirtualFree(cave, 0, 0x8000u);
        g_indicator_state = "ERROR_CODE_CACHE_FLUSH";
        return 0;
    }
    DWORD old_protect = 0;
    if (!VirtualProtect(cave, 4096, 0x20u, &old_protect)) {
        VirtualFree(cave, 0, 0x8000u);
        g_indicator_state = "ERROR_CODE_CAVE_PROTECT";
        return 0;
    }
    if (!patch_bytes(first, hook1, sizeof(hook1))) {
        VirtualFree(cave, 0, 0x8000u);
        g_indicator_state = "ERROR_PATCH_DISPLAY_BARS";
        return 0;
    }
    if (!patch_bytes(second, hook2, sizeof(hook2))) {
        if (!patch_bytes(first, expected_first, sizeof(expected_first))) {
            g_indicator_cave = cave; // do not free memory that a remaining hook uses
            g_indicator_state = "ERROR_ROLLBACK_FAILED_CAVE_RETAINED";
        } else {
            VirtualFree(cave, 0, 0x8000u);
            g_indicator_state = "ERROR_PATCH_PERCENT_ROLLED_BACK";
        }
        return 0;
    }
    g_indicator_cave = cave;
    g_indicator_state = "ACTIVE";
    return 1;
}
