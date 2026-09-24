// DS2 High-Density Backpack Modules v1.1.0
// Target: DS2.exe v1.10.89.0
//
// Release based on user-validated Test5; see docs/VALIDATION.md.
#include "backpack_policy.h"

extern "C" {
int _fltused=0;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned long long u64;
typedef signed long long s64;
typedef unsigned long DWORD;
typedef int BOOL;
typedef unsigned long long SIZE_T;
typedef void* HANDLE;
typedef void* HMODULE;
typedef void* LPVOID;
typedef const void* LPCVOID;

#ifndef WINAPI
#define WINAPI __stdcall
#endif
#ifndef FASTCALL
#define FASTCALL __fastcall
#endif

typedef DWORD (WINAPI *LPTHREAD_START_ROUTINE_X64)(LPVOID);

__declspec(dllimport) HMODULE WINAPI GetModuleHandleW(const wchar_t* moduleName);
__declspec(dllimport) DWORD WINAPI GetModuleFileNameW(HMODULE module, wchar_t* path, DWORD capacity);
__declspec(dllimport) BOOL WINAPI DisableThreadLibraryCalls(HMODULE module);
__declspec(dllimport) HANDLE WINAPI CreateThread(LPVOID attributes, SIZE_T stackSize, LPTHREAD_START_ROUTINE_X64 start, LPVOID parameter, DWORD flags, DWORD* threadId);
__declspec(dllimport) BOOL WINAPI CloseHandle(HANDLE handle);
__declspec(dllimport) void WINAPI Sleep(DWORD milliseconds);
__declspec(dllimport) HANDLE WINAPI GetCurrentProcess(void);
__declspec(dllimport) BOOL WINAPI VirtualProtect(LPVOID address, SIZE_T size, DWORD newProtect, DWORD* oldProtect);
__declspec(dllimport) LPVOID WINAPI VirtualAlloc(LPVOID address, SIZE_T size, DWORD allocationType, DWORD protect);
__declspec(dllimport) BOOL WINAPI VirtualFree(LPVOID address, SIZE_T size, DWORD freeType);
__declspec(dllimport) BOOL WINAPI FlushInstructionCache(HANDLE process, LPCVOID address, SIZE_T size);
__declspec(dllimport) HANDLE WINAPI CreateFileW(const wchar_t* name, DWORD desiredAccess, DWORD shareMode, LPVOID securityAttributes, DWORD creationDisposition, DWORD flagsAndAttributes, HANDLE templateFile);
__declspec(dllimport) BOOL WINAPI WriteFile(HANDLE file, LPCVOID buffer, DWORD bytesToWrite, DWORD* bytesWritten, LPVOID overlapped);
__declspec(dllimport) HANDLE WINAPI CreateMutexW(LPVOID attributes, BOOL initialOwner, const wchar_t* name);
__declspec(dllimport) DWORD WINAPI GetLastError(void);

}

static const DWORD DLL_PROCESS_ATTACH_VALUE = 1u;
static const DWORD PAGE_EXECUTE_READ_VALUE = 0x20u;
static const DWORD PAGE_EXECUTE_READWRITE_VALUE = 0x40u;
static const DWORD MEM_COMMIT_VALUE = 0x1000u;
static const DWORD MEM_RESERVE_VALUE = 0x2000u;
static const DWORD MEM_RELEASE_VALUE = 0x8000u;
static const DWORD GENERIC_WRITE_VALUE = 0x40000000u;
static const DWORD FILE_APPEND_DATA_VALUE = 0x00000004u;
static const DWORD FILE_SHARE_READ_VALUE = 0x1u;
static const DWORD FILE_SHARE_WRITE_VALUE = 0x2u;
static const DWORD CREATE_ALWAYS_VALUE = 2u;
static const DWORD OPEN_ALWAYS_VALUE = 4u;
static const DWORD FILE_ATTRIBUTE_NORMAL_VALUE = 0x80u;
static const DWORD ERROR_ALREADY_EXISTS_VALUE = 183u;

static const u32 EXPECTED_PE_TIMESTAMP = 0x6A3DAE46u;
static const u32 EXPECTED_SIZE_OF_IMAGE = 0x0B292000u;
static const u32 EXPECTED_PE_SIGNATURE = 0x00004550u;
static const u32 MAX_PATH_CHARS = 1024u;

static const u32 NATIVE_REFERENCE_CHECK_RVA = 0x00B406F0u;
static const u32 SHAPE_LOOKUP_RVA = 0x00BAD600u;
static const u32 REFERENCE_CHECK_CALLSITE_RVA = 0x00B43043u;
static const u32 MANAGER_POINTER_RVA = 0x0623E4E0u;
static const u32 MANAGER_LOAD_INSTRUCTION_RVA = 0x00B42ED1u;
static const u32 GRID_OFFSET = 0x000022D8u;
static const u32 STAGED_X_OFFSET = 0x000045E4u;
static const u32 STAGED_Y_OFFSET = 0x000045E8u;
static const u32 OCCUPANCY_FLAG_IMMEDIATE_RVA = 0x00B404BCu;
static const u8 EXPECTED_NATIVE_OCCUPANCY_FLAG = 0x40u;
static const u32 CONFLICT_BRANCH_RVA = 0x01525D18u;
static const u8 EXPECTED_NATIVE_CONFLICT_BRANCH[2] = {0x74u, 0x0Cu};


static const u32 CANDIDATE_X_OFFSET = 0x8u;
static const u32 CANDIDATE_Y_OFFSET = 0xCu;
static const u32 SHAPE_EXTENT_A_OFFSET = 0x24u;
static const u32 SHAPE_EXTENT_B_OFFSET = 0x28u;
static const s32 INVALID_COORDINATE_SENTINEL = -1;

struct Patch {
    u32 rva;
    u8 length;
    u8 original[8];
    u8 replacement[8];
    const char* group;
    const char* label;
};

static const Patch PATCHES[] = {
    {0x01524D19u,3,{0x83,0xC3,0x02},{0x83,0xC3,0x08},"accessory_menu","remove_index_eight_slots"},

    {0x01528EDBu,5,{0xBA,0x02,0x00,0x00,0x00},{0xBA,0x08,0x00,0x00,0x00},"accessory_menu","eight_slots"},

    {0x01E1B63Bu,3,{0x83,0xF9,0x02},{0x83,0xF9,0x08},"charm_iteration","likes_eight_ids"},
    // Only these verified gameplay consumers scan the virtual eight-ID list.
    {0x00DACA8Fu,3,{0x83,0xFB,0x02},{0x83,0xFB,0x08},"charm_iteration","eight_ids"},
    {0x00E5205Au,3,{0x83,0xFF,0x02},{0x83,0xFF,0x08},"charm_iteration","eight_ids"},
    {0x00E6C61Au,3,{0x83,0xF9,0x02},{0x83,0xF9,0x08},"charm_iteration","eight_ids"},
    {0x01058FDEu,3,{0x83,0xF9,0x02},{0x83,0xF9,0x08},"charm_iteration","eight_ids"},
    {0x019C374Fu,3,{0x83,0xFA,0x02},{0x83,0xFA,0x08},"charm_iteration","eight_ids"},
    {0x01A6FF1Du,3,{0x83,0xFA,0x02},{0x83,0xFA,0x08},"charm_iteration","eight_ids"},
    {0x01D14C21u,3,{0x83,0xF9,0x02},{0x83,0xF9,0x08},"charm_iteration","eight_ids"},
    // Reference collection must use the same logical cell as commit.
    {0x00B3FD69u,3,{0x2B,0x4E,0x28},{0x83,0xE9,0x01},"reference_collection","one_cell_0"},
    {0x00B3FD73u,3,{0x2B,0x46,0x24},{0x83,0xE8,0x01},"reference_collection","one_cell_1"},
    {0x00B3FD7Bu,3,{0x8B,0x46,0x28},{0x6A,0x01,0x58},"reference_collection","one_cell_2"},
    {0x00B3FD81u,3,{0x2B,0x4E,0x24},{0x83,0xE9,0x01},"reference_collection","one_cell_3"},
    {0x00B3FD96u,3,{0x8B,0x4E,0x28},{0x6A,0x01,0x59},"reference_collection","one_cell_4"},
    {0x00B3FD9Fu,3,{0x8B,0x46,0x24},{0x6A,0x01,0x58},"reference_collection","one_cell_5"},
    {0x00B3FDADu,3,{0x8B,0x4E,0x24},{0x6A,0x01,0x59},"reference_collection","one_cell_6"},
    {0x00B3FDBBu,3,{0x2B,0x46,0x28},{0x83,0xE8,0x01},"reference_collection","one_cell_7"},
    // 0x00B40090 - replacement-conflict collection: one logical anchor
    {0x00B400F9u,3,{0x2B,0x48,0x28,0x00},{0x83,0xE9,0x01,0x00},"replacement_collection","orientation3_sub_x_by_1"},
    {0x00B40102u,4,{0x41,0x2B,0x41,0x24},{0x83,0xE8,0x01,0x90},"replacement_collection","orientation3_sub_y_by_1"},
    {0x00B40112u,3,{0x2B,0x48,0x24,0x00},{0x83,0xE9,0x01,0x00},"replacement_collection","orientation2_sub_x_by_1"},
    {0x00B4011Au,3,{0x8B,0x40,0x28,0x00},{0x6A,0x01,0x58,0x00},"replacement_collection","orientation2_load_y_1"},
    {0x00B40122u,3,{0x8B,0x48,0x28,0x00},{0x6A,0x01,0x59,0x00},"replacement_collection","orientation1_load_x_1"},
    {0x00B4012Au,3,{0x8B,0x40,0x24,0x00},{0x6A,0x01,0x58,0x00},"replacement_collection","orientation1_load_y_1"},
    {0x00B40137u,3,{0x8B,0x48,0x24,0x00},{0x6A,0x01,0x59,0x00},"replacement_collection","orientation0_load_x_1"},
    {0x00B40145u,4,{0x41,0x2B,0x41,0x28},{0x83,0xE8,0x01,0x90},"replacement_collection","orientation0_sub_y_by_1"},

    // 0x00B40310 - final grid validation/commit: one logical anchor
    {0x00B40389u,3,{0x2B,0x48,0x28,0x00},{0x83,0xE9,0x01,0x00},"grid_validate_commit","orientation3_sub_x_by_1"},
    {0x00B4038Fu,3,{0x2B,0x50,0x24,0x00},{0x83,0xEA,0x01,0x00},"grid_validate_commit","orientation3_sub_y_by_1"},
    {0x00B4039Fu,3,{0x8B,0x50,0x28,0x00},{0x6A,0x01,0x5A,0x00},"grid_validate_commit","orientation2_load_y_1"},
    {0x00B403B3u,3,{0x2B,0x48,0x24,0x00},{0x83,0xE9,0x01,0x00},"grid_validate_commit","orientation2_sub_x_by_1"},
    {0x00B403C4u,3,{0x8B,0x48,0x28,0x00},{0x6A,0x01,0x59,0x00},"grid_validate_commit","orientation1_load_x_1"},
    {0x00B403CDu,3,{0x8B,0x50,0x24,0x00},{0x6A,0x01,0x5A,0x00},"grid_validate_commit","orientation1_load_y_1"},
    {0x00B403E3u,3,{0x8B,0x48,0x24,0x00},{0x6A,0x01,0x59,0x00},"grid_validate_commit","orientation0_load_x_1"},
    {0x00B403FDu,3,{0x2B,0x50,0x28,0x00},{0x83,0xEA,0x01,0x00},"grid_validate_commit","orientation0_sub_y_by_1"}
};

static const u32 PATCH_COUNT = (u32)(sizeof(PATCHES) / sizeof(PATCHES[0]));
static const u8 EXPECTED_REFERENCE_CHECK_CALL[5] = {0xE8,0xA8,0xD6,0xFF,0xFF};
static const u8 EXPECTED_MANAGER_LOAD[7] = {0x48,0x8B,0x3D,0x08,0xB6,0x6F,0x05};
static const u8 EXPECTED_REFERENCE_CHECK_PREFIX[8] = {0x48,0x89,0x6C,0x24,0x18,0x56,0x48,0x83};
static const u8 EXPECTED_SHAPE_LOOKUP_PREFIX[22] = {
    0x4C,0x8B,0x1D,0xA9,0x0F,0x69,0x05,0x8D,0x4A,0xEC,0x45,
    0x33,0xC0,0x4D,0x63,0x53,0x20,0x45,0x85,0xD2,0x7E,0x32
};
static const u8 EXPECTED_NATIVE_REFERENCE_SHAPE_READS[8][3] = {
    {0x2B,0x4E,0x28}, {0x2B,0x46,0x24}, {0x8B,0x46,0x28},
    {0x2B,0x4E,0x24}, {0x8B,0x4E,0x28}, {0x8B,0x46,0x24},
    {0x8B,0x4E,0x24}, {0x2B,0x46,0x28}
};
static const u32 NATIVE_REFERENCE_SHAPE_READ_RVAS[8] = {
    0x00B3FD69u,0x00B3FD73u,0x00B3FD7Bu,0x00B3FD81u,
    0x00B3FD96u,0x00B3FD9Fu,0x00B3FDADu,0x00B3FDBBu
};

static u8* g_imageBase = 0;
static HANDLE g_mutex = 0;
static volatile long g_patchApplied = 0;
static volatile u32 g_eventSequence = 0u;
static volatile u32 g_wrapperHits = 0u;
static wchar_t g_logPath[MAX_PATH_CHARS];
static volatile long g_ready = 0;

typedef u8 (FASTCALL *ReferenceCheckFn)(u8* candidate);
typedef u8* (FASTCALL *ShapeLookupFn)(LPVOID unused, u32 type);
static ReferenceCheckFn g_originalReferenceCheck = 0;
static ShapeLookupFn g_shapeLookup = 0;

static bool bytes_equal(const void* first, const void* second, SIZE_T count) {
    const u8* a = (const u8*)first;
    const u8* b = (const u8*)second;
    for (SIZE_T i = 0; i < count; ++i) if (a[i] != b[i]) return false;
    return true;
}

static void copy_bytes(void* destination, const void* source, SIZE_T count) {
    u8* out = (u8*)destination;
    const u8* in = (const u8*)source;
    for (SIZE_T i = 0; i < count; ++i) out[i] = in[i];
}

static u32 ascii_length(const char* text) {
    if (!text) return 0u;
    u32 length = 0u;
    while (text[length]) ++length;
    return length;
}

static void write_text(HANDLE file, const char* text) {
    if (!file || file == (HANDLE)(s64)-1 || !text) return;
    DWORD written = 0u;
    const u32 length = ascii_length(text);
    if (length) WriteFile(file, text, length, &written, 0);
}

static char hex_digit(u8 value) {
    value &= 0xFu;
    return value < 10u ? (char)('0' + value) : (char)('A' + (value - 10u));
}

static void append_text(char* buffer, u32 capacity, u32* position, const char* text) {
    if (!buffer || !position || !text || capacity == 0u) return;
    for (u32 i = 0u; text[i] && *position + 1u < capacity; ++i) buffer[(*position)++] = text[i];
    buffer[*position] = 0;
}

static void append_hex_u32(char* buffer, u32 capacity, u32* position, u32 value) {
    append_text(buffer, capacity, position, "0x");
    for (int shift = 28; shift >= 0 && *position + 1u < capacity; shift -= 4) buffer[(*position)++] = hex_digit((u8)(value >> shift));
    buffer[*position] = 0;
}

static void append_byte_hex(char* buffer, u32 capacity, u32* position, u8 value) {
    if (*position + 2u >= capacity) return;
    buffer[(*position)++] = hex_digit((u8)(value >> 4u));
    buffer[(*position)++] = hex_digit(value);
    buffer[*position] = 0;
}

static void append_u32_decimal(char* buffer, u32 capacity, u32* position, u32 value) {
    char temporary[16];
    u32 count = 0u;
    do { temporary[count++] = (char)('0' + (value % 10u)); value /= 10u; } while (value && count < 16u);
    while (count && *position + 1u < capacity) buffer[(*position)++] = temporary[--count];
    buffer[*position] = 0;
}

static void append_s32_decimal(char* buffer, u32 capacity, u32* position, s32 value) {
    if (value < 0) {
        append_text(buffer, capacity, position, "-");
        append_u32_decimal(buffer, capacity, position, (u32)(-(s64)value));
    } else {
        append_u32_decimal(buffer, capacity, position, (u32)value);
    }
}

static bool build_log_path() {
    DWORD length = GetModuleFileNameW((HMODULE)0, g_logPath, MAX_PATH_CHARS);
    if (length == 0u || length >= MAX_PATH_CHARS) return false;
    u32 slash = 0u;
    for (u32 i = 0u; i < length; ++i) if (g_logPath[i] == L'\\' || g_logPath[i] == L'/') slash = i + 1u;
    static const wchar_t fileName[] = L"DS2_HighDensityBackpackModules.log";
    u32 output = slash;
    for (u32 i = 0u; fileName[i]; ++i) {
        if (output + 1u >= MAX_PATH_CHARS) return false;
        g_logPath[output++] = fileName[i];
    }
    g_logPath[output] = 0;
    return true;
}

static HANDLE open_log_create() {
    if (!build_log_path()) return (HANDLE)(s64)-1;
    return CreateFileW(g_logPath, GENERIC_WRITE_VALUE, FILE_SHARE_READ_VALUE | FILE_SHARE_WRITE_VALUE, 0,
                       CREATE_ALWAYS_VALUE, FILE_ATTRIBUTE_NORMAL_VALUE, 0);
}

static HANDLE open_log_append() {
    return CreateFileW(g_logPath, FILE_APPEND_DATA_VALUE, FILE_SHARE_READ_VALUE | FILE_SHARE_WRITE_VALUE, 0,
                       OPEN_ALWAYS_VALUE, FILE_ATTRIBUTE_NORMAL_VALUE, 0);
}

static bool validate_pe() {
    if (!g_imageBase || *(volatile const u16*)g_imageBase != 0x5A4Du) return false;
    const u32 peOffset = *(volatile const u32*)(g_imageBase + 0x3Cu);
    if (peOffset < 0x40u || peOffset > 0x1000u) return false;
    u8* nt = g_imageBase + peOffset;
    if (*(volatile const u32*)nt != EXPECTED_PE_SIGNATURE) return false;
    return *(volatile const u32*)(nt + 8u) == EXPECTED_PE_TIMESTAMP &&
           *(volatile const u32*)(nt + 0x50u) == EXPECTED_SIZE_OF_IMAGE;
}

static void log_patch_bytes(HANDLE log, const char* prefix, const Patch& patch, const u8* bytes) {
    char line[320];
    u32 position = 0u;
    append_text(line, sizeof(line), &position, prefix);
    append_text(line, sizeof(line), &position, "_");
    append_text(line, sizeof(line), &position, patch.group);
    append_text(line, sizeof(line), &position, "_");
    append_text(line, sizeof(line), &position, patch.label);
    append_text(line, sizeof(line), &position, "_rva=");
    append_hex_u32(line, sizeof(line), &position, patch.rva);
    append_text(line, sizeof(line), &position, " bytes=");
    for (u32 i = 0u; i < patch.length; ++i) {
        if (i) append_text(line, sizeof(line), &position, " ");
        append_byte_hex(line, sizeof(line), &position, bytes[i]);
    }
    append_text(line, sizeof(line), &position, "\r\n");
    write_text(log, line);
}

static bool get_shape_extents(u8 type, s32* extentA, s32* extentB) {
    if (!g_shapeLookup || !extentA || !extentB) return false;
    u8* shape = g_shapeLookup((LPVOID)0, (u32)type);
    if (!shape) return false;
    const s32 a = *(volatile const s32*)(shape + SHAPE_EXTENT_A_OFFSET);
    const s32 b = *(volatile const s32*)(shape + SHAPE_EXTENT_B_OFFSET);
    if (a <= 0 || b <= 0 || a > 16 || b > 16) return false;
    *extentA = a;
    *extentB = b;
    return true;
}

// Exact equivalent of the native full-shape cell walk in DS2.exe+0xB3FD10.
static void force_local_candidate_rejection(u8* candidate) {
    if (!candidate) return;
    *(volatile s32*)(candidate + CANDIDATE_X_OFFSET) = INVALID_COORDINATE_SENTINEL;
    *(volatile s32*)(candidate + CANDIDATE_Y_OFFSET) = INVALID_COORDINATE_SENTINEL;
}

static void log_anchor_event(const char* eventName, u8 type, u32 componentId,
                             u16 orientation, s32 extentA, s32 extentB,
                             s32 fromX, s32 fromY, u32 occupantId,
                             s32 toX, s32 toY, u32 referenceResult) {
    HANDLE log = open_log_append();
    if (log == (HANDLE)(s64)-1) return;
    char line[640];
    u32 position = 0u;
    append_text(line, sizeof(line), &position, eventName);
    append_text(line, sizeof(line), &position, " seq=");
    append_u32_decimal(line, sizeof(line), &position, ++g_eventSequence);
    append_text(line, sizeof(line), &position, " type=");
    append_hex_u32(line, sizeof(line), &position, (u32)type);
    append_text(line, sizeof(line), &position, " component_id=");
    append_hex_u32(line, sizeof(line), &position, componentId);
    append_text(line, sizeof(line), &position, " orientation=");
    append_u32_decimal(line, sizeof(line), &position, (u32)orientation);
    append_text(line, sizeof(line), &position, " extent_a=");
    append_s32_decimal(line, sizeof(line), &position, extentA);
    append_text(line, sizeof(line), &position, " extent_b=");
    append_s32_decimal(line, sizeof(line), &position, extentB);
    append_text(line, sizeof(line), &position, " from_x=");
    append_s32_decimal(line, sizeof(line), &position, fromX);
    append_text(line, sizeof(line), &position, " from_y=");
    append_s32_decimal(line, sizeof(line), &position, fromY);
    append_text(line, sizeof(line), &position, " occupant_id=");
    append_hex_u32(line, sizeof(line), &position, occupantId);
    append_text(line, sizeof(line), &position, " to_x=");
    append_s32_decimal(line, sizeof(line), &position, toX);
    append_text(line, sizeof(line), &position, " to_y=");
    append_s32_decimal(line, sizeof(line), &position, toY);
    append_text(line, sizeof(line), &position, " reference_result=");
    append_u32_decimal(line, sizeof(line), &position, referenceResult);
    append_text(line, sizeof(line), &position, "\r\n");
    write_text(log, line);
    CloseHandle(log);
}

// Logical placement and native visual footprints are deliberately independent.
static bool relocate(u8* grid, u8* candidate) {
    if (!grid || !candidate) return false;
    u32 ids[30];
    for (u32 i=0; i<30; ++i) ids[i]=*(u32*)(grid+i*0x120+4);
    const u32 id=*(u32*)(candidate+4);
    if (!id || id==0xFFFFFFFFu) return false;
    const s32 x=*(s32*)(candidate+8), y=*(s32*)(candidate+12);
    const int index=backpack::anchor(ids,id,x,y);
    if (index<0) { force_local_candidate_rejection(candidate); return false; }
    *(s32*)(candidate+8)=index%5;
    *(s32*)(candidate+12)=index/5;
    u8* manager=*(u8**)(g_imageBase+MANAGER_POINTER_RVA);
    if (manager && grid==manager+GRID_OFFSET) {
        *(s32*)(manager+STAGED_X_OFFSET)=index%5;
        *(s32*)(manager+STAGED_Y_OFFSET)=index/5;
    }
    if (index%5!=x || index/5!=y) {
        if (g_eventSequence<128) log_anchor_event("AUTO_ANCHOR_1X1",*candidate,id,
            *(u16*)(candidate+0x1A),1,1,x,y,0,index%5,index/5,1);
    }
    return true;
}
static u8 FASTCALL auto_anchor_reference_check(u8* candidate) {
    if (!g_ready) return g_originalReferenceCheck(candidate);
    u8* manager=*(u8**)(g_imageBase+MANAGER_POINTER_RVA);
    if (manager && !relocate(manager+GRID_OFFSET,candidate)) return 0;
    return g_originalReferenceCheck(candidate);
}
using GridFn=u8* (FASTCALL *)(u8*,u8*,u8*,u8);
static u8* FASTCALL auto_anchor_grid(u8* grid,u8* result,u8* candidate,u8 validateOnly) {
    if (g_ready) relocate(grid,candidate);
    return ((GridFn)(g_imageBase+0xB40310))(grid,result,candidate,validateOnly);
}

#include "backpack_visuals.inl"

// Runs inside the native per-module visual update, with the owning info and
// slot supplied by the leaf relay. Only render output changes: local/inherited
// mesh flags, entities, inventory and effect counters remain native.
static bool g_equipmentPreviewReported=false;
static bool g_gameplayBackpackReported=false;
static void FASTCALL module_visual_update(u8* model,u8 shadow,u8* info,u64 slotOffset) {
    using ShadowFn=void (FASTCALL *)(u8*,u8);
    ((ShadowFn)(g_imageBase+0x33B480))(model,shadow);
    if (!g_ready || !info || slotOffset>=30*0x60 || slotOffset%0x60 || !model) return;
    u8* entity=*(u8**)(info+0x45F8+slotOffset);
    if (!entity || *(u8**)(entity+0xD8)!=model) return;
    const s32 context=visual_backpack_context(info);
    if (context<0) return;
    VisualLayout& layout=visual_layout(info);
    const u32 slot=(u32)(slotOffset/0x60);
    const bool visible=(layout.packed.shown & (1u<<slot))!=0;
    if (visible && (!(layout.positioned&(1u<<slot)) || layout.models[slot]!=model)) {
        using Position=void (FASTCALL *)(u8*,u8*);
        ((Position)(g_imageBase+0xB3A860))(info,info+0x45B0+slotOffset);
        layout.positioned|=1u<<slot;layout.models[slot]=model;
    }
    const s32 count=*(s32*)(model+0x50);
    u8* nodes=*(u8**)(model+0x58);
    if (!nodes || count<0 || count>2048) return;
    using RenderFn=void (FASTCALL *)(u8*,s32,u8);
    for (s32 n=0;n<count;++n) {
        u8* node=nodes+n*0x30;
        u8* owner=*(u8**)(node+8);
        const s32 mesh=*(s32*)(node+0x28);
        if (owner && mesh>=0) {
            u8* renderer=*(u8**)(owner+0xC8);
            if (renderer) ((RenderFn)(g_imageBase+0x22A8D0))(renderer,mesh,(u8)(visible && node[0x2D]));
        }
    }
    if (context==3 && !g_equipmentPreviewReported) {
        g_equipmentPreviewReported=true;
        HANDLE log=open_log_append();
        write_text(log,"EQUIPMENT_PREVIEW_VISIBILITY=ACTIVE; render_mesh_only\r\n");
        if (log!=(HANDLE)(s64)-1) CloseHandle(log);
    }
    if (context==4 && !g_gameplayBackpackReported) {
        g_gameplayBackpackReported=true;
        HANDLE log=open_log_append();
        write_text(log,"GAMEPLAY_BACKPACK_VISIBILITY=ACTIVE; live_player_lookup\r\n");
        if (log!=(HANDLE)(s64)-1) CloseHandle(log);
    }
    if (!visible && ++g_wrapperHits<=8) {
        HANDLE log=open_log_append();
        write_text(log,"EXCESS_MODULE_MESH_HIDDEN; logical module retained\r\n");
        if (log!=(HANDLE)(s64)-1) CloseHandle(log);
    }
}

#include "backpack_charms.inl"
#include "backpack_menu.inl"

static bool rel32_fits(const u8* instructionNext, const u8* destination) {
    const s64 difference = (s64)((u64)destination - (u64)instructionNext);
    return difference >= (s64)-2147483648LL && difference <= (s64)2147483647LL;
}

static void write_u64_le(u8* output, u64 value) {
    for (u32 i = 0u; i < 8u; ++i) output[i] = (u8)(value >> (i * 8u));
}

static u8* finalize_relay_allocation(u8* memory, u8* callSite, u8* destination, u64 pageSize) {
    if (!memory) return 0;
    if (!rel32_fits(callSite + 5u, memory)) {
        VirtualFree(memory, 0u, MEM_RELEASE_VALUE);
        return 0;
    }
    // RIP-indirect jump preserves RAX, which is live at inline pointer hooks.
    memory[0]=0xFF; memory[1]=0x25;
    for (u32 i=2;i<6;++i) memory[i]=0;
    write_u64_le(memory+6,(u64)destination);
    FlushInstructionCache(GetCurrentProcess(),memory,14);
    DWORD oldProtect = 0u;
    if (!VirtualProtect(memory, pageSize, PAGE_EXECUTE_READ_VALUE, &oldProtect)) {
        VirtualFree(memory,0,MEM_RELEASE_VALUE); return 0;
    }
    return memory;
}

static u8* allocate_near_relay(u8* callSite, u8* destination) {
    const u64 granularity = 0x10000ull;
    const u64 pageSize = 0x1000ull;
    const u64 maxDistance = 0x7FFF0000ull;
    const u64 imageEnd = (u64)g_imageBase + (u64)EXPECTED_SIZE_OF_IMAGE;
    const u64 firstCandidate = (imageEnd + granularity - 1ull) & ~(granularity - 1ull);

    u8* memory = (u8*)VirtualAlloc((LPVOID)firstCandidate, pageSize,
                                   MEM_RESERVE_VALUE | MEM_COMMIT_VALUE,
                                   PAGE_EXECUTE_READWRITE_VALUE);
    memory = finalize_relay_allocation(memory, callSite, destination, pageSize);
    if (memory) return memory;

    const u64 aligned = ((u64)callSite) & ~(granularity - 1ull);
    for (u64 delta = granularity; delta <= maxDistance; delta += granularity) {
        u64 candidates[2] = {aligned + delta, aligned > delta ? aligned - delta : 0ull};
        for (u32 i = 0u; i < 2u; ++i) {
            if (!candidates[i] || candidates[i] == firstCandidate) continue;
            memory = (u8*)VirtualAlloc((LPVOID)candidates[i], pageSize,
                                       MEM_RESERVE_VALUE | MEM_COMMIT_VALUE,
                                       PAGE_EXECUTE_READWRITE_VALUE);
            memory = finalize_relay_allocation(memory, callSite, destination, pageSize);
            if (memory) return memory;
        }
    }
    return 0;
}

static void build_call_bytes(u8 output[5], const u8* callSite, const u8* destination) {
    const s64 difference = (s64)((u64)destination - (u64)(callSite + 5u));
    const s32 displacement = (s32)difference;
    output[0] = 0xE8u;
    output[1] = (u8)(displacement & 0xFF);
    output[2] = (u8)((displacement >> 8) & 0xFF);
    output[3] = (u8)((displacement >> 16) & 0xFF);
    output[4] = (u8)((displacement >> 24) & 0xFF);
}

#include "baseline_signatures.h"
static bool validate_static_context(HANDLE log) {
    for (u32 i=0;i<sizeof(SIGNATURES)/sizeof(SIGNATURES[0]);++i) {
        const BaselineSignature& s=SIGNATURES[i];
        if (!bytes_equal(g_imageBase+s.rva,s.bytes,s.size)) {
            write_text(log,"native_callee_or_context=REJECTED\r\n"); return false;
        }
    }
    if (*(volatile const u8*)(g_imageBase + OCCUPANCY_FLAG_IMMEDIATE_RVA) != EXPECTED_NATIVE_OCCUPANCY_FLAG) {
        write_text(log, "native_occupancy_flag_check=FAIL\r\n");
        return false;
    }
    write_text(log, "native_occupancy_flag_check=PASS\r\n");

    if (!bytes_equal(g_imageBase + CONFLICT_BRANCH_RVA, EXPECTED_NATIVE_CONFLICT_BRANCH, 2u)) {
        write_text(log, "native_conflict_branch_check=FAIL\r\n");
        return false;
    }
    write_text(log, "native_conflict_branch_check=PASS\r\n");

    if (!bytes_equal(g_imageBase + REFERENCE_CHECK_CALLSITE_RVA, EXPECTED_REFERENCE_CHECK_CALL, 5u)) {
        write_text(log, "reference_check_callsite_check=FAIL\r\n");
        return false;
    }
    write_text(log, "reference_check_callsite_check=PASS\r\n");

    if (!bytes_equal(g_imageBase + MANAGER_LOAD_INSTRUCTION_RVA, EXPECTED_MANAGER_LOAD, 7u)) {
        write_text(log, "manager_pointer_context_check=FAIL\r\n");
        return false;
    }
    write_text(log, "manager_pointer_context_check=PASS\r\n");

    if (!bytes_equal(g_imageBase + NATIVE_REFERENCE_CHECK_RVA, EXPECTED_REFERENCE_CHECK_PREFIX, 8u)) {
        write_text(log, "native_reference_check_prefix=FAIL\r\n");
        return false;
    }
    write_text(log, "native_reference_check_prefix=PASS\r\n");

    if (!bytes_equal(g_imageBase + SHAPE_LOOKUP_RVA, EXPECTED_SHAPE_LOOKUP_PREFIX,
                     sizeof(EXPECTED_SHAPE_LOOKUP_PREFIX))) {
        write_text(log, "stage5_shape_lookup_check=FAIL\r\n");
        return false;
    }
    write_text(log, "stage5_shape_lookup_check=PASS\r\n");

    for (u32 i = 0u; i < 8u; ++i) {
        if (!bytes_equal(g_imageBase + NATIVE_REFERENCE_SHAPE_READ_RVAS[i], EXPECTED_NATIVE_REFERENCE_SHAPE_READS[i], 3u)) {
            write_text(log, "reference_scan_baseline=FAIL\r\n");
            return false;
        }
    }
    write_text(log, "reference_scan_baseline=PASS\r\n");

    for (u32 i = 0u; i < PATCH_COUNT; ++i) {
        const u8* current = g_imageBase + PATCHES[i].rva;
        if (!bytes_equal(current, PATCHES[i].original, PATCHES[i].length)) {
            log_patch_bytes(log, "unexpected", PATCHES[i], current);
            return false;
        }
    }
    write_text(log, "one_cell_instruction_checks=PASS\r\n");
    return true;
}

static bool write_code(u8* address,const u8* bytes,u32 count);
static bool apply_footprint_patches(HANDLE log) {
    for (u32 i=0;i<PATCH_COUNT;++i) {
        const Patch& p=PATCHES[i];
        g_patchApplied=(long)i+1;
        if (!write_code(g_imageBase+p.rva,p.replacement,p.length)) return false;
        log_patch_bytes(log,"patched",p,p.replacement);
    }
    return true;
}

struct Hook {
    u32 rva, target;
    void* wrapper;
    bool moduleContext;
    u8* relay;
    u8 original[5];
    bool installed;
    bool tail=false;
};
static Hook HOOKS[] = {

    {0xB43043,0xB406F0,(void*)&auto_anchor_reference_check,false,0,{},false},
    {0xB42E79,0xB40310,(void*)&auto_anchor_grid,false,0,{},false},
    {0xB431A8,0xB40310,(void*)&auto_anchor_grid,false,0,{},false},
    {0xB431D4,0xB40310,(void*)&auto_anchor_grid,false,0,{},false},
    {0xB3D386,0x33B480,(void*)&module_visual_update,true,0,{},false},
    {0xB3D9D7,0xB39FE0,(void*)&backpack_preview_update,false,0,{},false},
    {0xB3CFC6,0xB3E470,(void*)&battery_capacity,false,0,{},false},
    {0xEA8633,0xB3E470,(void*)&battery_capacity,false,0,{},false},
    {0xEA7214,0xB3E470,(void*)&battery_capacity,false,0,{},false},
    {0x17C8F49,0xE84880,(void*)&has_virtual_charm,false,0,{},false},
    {0x15285F7,0x1528E90,(void*)&build_charm_category,false,0,{},false},
    {0x1523706,0x1518520,(void*)&menu_charm_index,false,0,{},false},
    {0x15270F6,0x1518580,(void*)&charm_already_equipped,false,0,{},false},
    {0x1529510,0x1518580,(void*)&charm_already_equipped,false,0,{},false},
    {0x1527139,0x151A5C0,(void*)&set_menu_charm,false,0,{},false},
    {0x15272AC,0x151A5C0,(void*)&set_menu_charm,false,0,{},false,true},
    {0x1524DF6,0xB393E0,(void*)&remove_menu_charms,false,0,{},false},
    {0x15189BA,0xB41A60,(void*)&close_charm_menu,false,0,{},false},
    {0xB3AA9C,0xB3FD10,(void*)&visual_reference,false,0,{},false},
    {0xB3AB46,0xB3FD10,(void*)&visual_reference,false,0,{},false},
};
static const u32 HOOK_COUNT=sizeof(HOOKS)/sizeof(HOOKS[0]);

struct InlineHook {
    u32 rva, length;
    u8 original[8];
    void* wrapper;
    u8* relay;
    bool installed;
};
static InlineHook INLINE_HOOKS[] = {
    {0x01523742u,8,{0x0F,0xB6,0x8C,0x3B,0x98,0x44,0x00,0x00},(void*)&charm_menu_read,0,false},

    {0x01E1B657u,6,{0x48,0x05,0x8C,0x04,0x00,0x00},(void*)&charm_full_snapshot_rax,0,false},
    {0x00DACA7Fu,6,{0x48,0x05,0xFC,0x01,0x00,0x00},(void*)&charm_snapshot_rax,0,false},
    {0x00E52048u,6,{0x48,0x05,0xFC,0x01,0x00,0x00},(void*)&charm_snapshot_rax,0,false},
    {0x00E6C605u,6,{0x48,0x05,0xFC,0x01,0x00,0x00},(void*)&charm_snapshot_rax,0,false},
    {0x01058FC5u,6,{0x48,0x05,0xFC,0x01,0x00,0x00},(void*)&charm_snapshot_rax,0,false},
    {0x019C373Eu,7,{0x48,0x8D,0x88,0xFC,0x01,0x00,0x00},(void*)&charm_snapshot_rcx,0,false},
    {0x01A6FF0Cu,7,{0x48,0x8D,0x88,0xFC,0x01,0x00,0x00},(void*)&charm_snapshot_rcx,0,false},
    // Leave CMP at 1D14C17 intact: the native loop branches back to it.
    {0x01D14C10u,7,{0x41,0x8B,0xCE,0x49,0x0F,0x44,0xC0},(void*)&charm_crypto_rax,0,false},
};
static const u32 INLINE_COUNT=sizeof(INLINE_HOOKS)/sizeof(INLINE_HOOKS[0]);

static bool prepare_hooks(HANDLE log) {
    for (u32 i=0;i<INLINE_COUNT;++i) {
        const InlineHook& h=INLINE_HOOKS[i];
        if (!bytes_equal(g_imageBase+h.rva,h.original,h.length)) return false;
    }
    // Validate all destinations before modifying any game code.
    for (u32 i=0;i<HOOK_COUNT;++i) {
        Hook& h=HOOKS[i];
        build_call_bytes(h.original,g_imageBase+h.rva,g_imageBase+h.target);
        if (h.tail) h.original[0]=0xE9;
        if (!bytes_equal(g_imageBase+h.rva,h.original,5)) {
            write_text(log,"hook_context=REJECTED\r\n"); return false;
        }
    }
    for (u32 i=0;i<HOOK_COUNT;++i) {
        Hook& h=HOOKS[i];
        h.relay=allocate_near_relay(g_imageBase+h.rva,(u8*)h.wrapper);
        if (!h.relay) return false;
        if (h.moduleContext) {
            DWORD previous=0;
            if (!VirtualProtect(h.relay,32,PAGE_EXECUTE_READWRITE_VALUE,&previous)) return false;
            const u8 args[]={0x49,0x89,0xF8,0x49,0x89,0xF1}; // r8=rdi, r9=rsi
            copy_bytes(h.relay,args,sizeof(args));
            h.relay[6]=0xFF; h.relay[7]=0x25;
            for (u32 j=8;j<12;++j) h.relay[j]=0;
            write_u64_le(h.relay+12,(u64)h.wrapper);
            FlushInstructionCache(GetCurrentProcess(),h.relay,20);
            DWORD ignored=0;
            if (!VirtualProtect(h.relay,32,previous,&ignored)) return false;
        }
    }
    for (u32 i=0;i<INLINE_COUNT;++i) {
        InlineHook& h=INLINE_HOOKS[i];
        h.relay=allocate_near_relay(g_imageBase+h.rva,(u8*)h.wrapper);
        if (!h.relay) return false;
    }
    return true;
}
static bool write_code(u8* address,const u8* bytes,u32 count) {
    DWORD previous=0;
    if (!VirtualProtect(address,count,PAGE_EXECUTE_READWRITE_VALUE,&previous)) return false;
    copy_bytes(address,bytes,count);
    FlushInstructionCache(GetCurrentProcess(),address,count);
    const bool ok=bytes_equal(address,bytes,count);
    DWORD ignored=0;
    return VirtualProtect(address,count,previous,&ignored) && ok;
}
static bool install_hooks() {
    for (u32 i=0;i<HOOK_COUNT;++i) {
        Hook& h=HOOKS[i]; u8 bytes[5];
        build_call_bytes(bytes,g_imageBase+h.rva,h.relay);
        if (h.tail) bytes[0]=0xE9;
        // Mark before writing so a protection-restore failure is rolled back too.
        h.installed=true;
        if (!write_code(g_imageBase+h.rva,bytes,5)) return false;
    }
    for (u32 i=0;i<INLINE_COUNT;++i) {
        InlineHook& h=INLINE_HOOKS[i]; u8 bytes[8]={0,0,0,0,0,0x90,0x90,0x90};
        build_call_bytes(bytes,g_imageBase+h.rva,h.relay);
        h.installed=true;
        if (!write_code(g_imageBase+h.rva,bytes,h.length)) return false;
    }
    return true;
}
static bool restore_all() {
    g_ready=0;
    bool ok=true;
    if (g_patchApplied) {
        for (u32 i=0;i<(u32)g_patchApplied;++i)
            if (!write_code(g_imageBase+PATCHES[i].rva,PATCHES[i].original,PATCHES[i].length)) ok=false;
        if (ok) g_patchApplied=0;
    }
    for (u32 i=0;i<HOOK_COUNT;++i) {
        Hook& h=HOOKS[i];
        if (h.installed) {
            if (write_code(g_imageBase+h.rva,h.original,5)) h.installed=false;
            else ok=false;
        }
    }
    for (u32 i=0;i<INLINE_COUNT;++i) {
        InlineHook& h=INLINE_HOOKS[i];
        if (h.installed) {
            if (write_code(g_imageBase+h.rva,h.original,h.length)) h.installed=false;
            else ok=false;
        }
    }
    // Relays stay allocated: a thread may still be returning through a wrapper.
    return ok;
}
struct MemoryInfo {
    void* base; void* allocationBase; DWORD allocationProtect; DWORD padding;
    SIZE_T size; DWORD state,protect,type,padding2;
};
extern "C" __declspec(dllimport) SIZE_T WINAPI VirtualQuery(LPCVOID,MemoryInfo*,SIZE_T);
extern "C" __declspec(dllimport) BOOL WINAPI GetModuleHandleExW(DWORD,const wchar_t*,HMODULE*);

static bool find_image() {
    g_imageBase=(u8*)GetModuleHandleW(0);
    if (validate_pe()) return true;
    // Some loaders map the supported game image separately from the launcher.
    for (u64 address=0x10000;address<0x00007FFFFFFF0000ull;) {
        MemoryInfo m;
        if (!VirtualQuery((void*)address,&m,sizeof(m))) break;
        const u64 next=(u64)m.base+m.size;
        if (next<=address) break;
        if (m.base==m.allocationBase && m.state==MEM_COMMIT_VALUE &&
            !(m.protect&0x101) && (m.protect&0xEE) && m.size>=0x2000) {
            g_imageBase=(u8*)m.base;
            if (validate_pe()) return true;
        }
        address=next;
    }
    g_imageBase=0; return false;
}
static DWORD WINAPI worker_thread(LPVOID) {
    HANDLE log=open_log_create();
    if (log==(HANDLE)(s64)-1) return 1;
    write_text(log,"DS2 High-Density Backpack Modules v1.1.0\r\n");
    write_text(log,"target=DS2.exe v1.10.89.0; PE timestamp=6A3DAE46\r\n");
    write_text(log,"expected_sha256=BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B\r\n");
    Sleep(1200); // Match the stable loader startup delay.
    bool found=false;
    for (u32 attempt=0;attempt<30;++attempt) {
        if (find_image()) { found=true; break; }
        Sleep(1000);
    }
    if (!found) { write_text(log,"status=BASELINE_REJECTED\r\n"); CloseHandle(log); return 2; }
    g_originalReferenceCheck=(ReferenceCheckFn)(g_imageBase+NATIVE_REFERENCE_CHECK_RVA);
    g_shapeLookup=(ShapeLookupFn)(g_imageBase+SHAPE_LOOKUP_RVA);
    if (!validate_static_context(log) || !prepare_hooks(log)) {
        write_text(log,"status=CONTEXT_REJECTED_NO_PATCHES\r\n"); CloseHandle(log); return 3;
    }
    // Pin the DLL while callbacks can run; live unloading is not supported.
    HMODULE pinned=0;
    if (!GetModuleHandleExW(5,(const wchar_t*)&worker_thread,&pinned)) {
        write_text(log,"status=MODULE_PIN_FAILED_NO_PATCHES\r\n"); CloseHandle(log); return 4;
    }
    initialize_charm_table();
    load_extra_charms();
    if (!install_hooks() || !apply_footprint_patches(log)) {
        write_text(log,restore_all()?"status=INSTALL_FAILED_RESTORED\r\n":"status=INSTALL_FAILED_ROLLBACK_INCOMPLETE\r\n");
        CloseHandle(log); return 5;
    }
    _InterlockedExchange(&g_ready,1);
    write_text(log,"status=PATCH_APPLIED\r\nlogical_grid=5x6; all_module_footprints=1x1\r\n");
    write_text(log,"visibility=NONOVERLAPPING_NATIVE_FOOTPRINTS; render_mesh_only\r\n");
    write_text(log,"charms=EIGHT_EQUIPMENT_SLOTS; visible_slots=2; invisible_slots=6\r\n");
    CloseHandle(log); return 0;
}
extern "C" BOOL WINAPI DllMain(HMODULE module,DWORD reason,LPVOID) {
    if (reason==DLL_PROCESS_ATTACH_VALUE) {
        DisableThreadLibraryCalls(module);
        // Retain the stable version's mutex to exclude duplicate installations.
        g_mutex=CreateMutexW(0,0,L"Local\\DS2_HighDensityBackpackModules_v1_0_0");
        if (!g_mutex || GetLastError()==ERROR_ALREADY_EXISTS_VALUE) return 1;
        HANDLE thread=CreateThread(0,0,worker_thread,0,0,0);
        if (thread) CloseHandle(thread);
    }
    return 1;
}
