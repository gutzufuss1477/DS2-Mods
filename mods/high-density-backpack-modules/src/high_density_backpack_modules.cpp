// DS2 High-Density Backpack Modules v1.0.0
// Target: DS2.exe v1.10.89.0
//
// Stage-5 evidence confirms:
// - DS2.exe+0xBAD600 resolves the backpack component definition for the type
//   supplied in EDX.
// - Native logical shape dimensions are signed dwords at definition+0x24 and
//   definition+0x28.
// - Candidate coordinates are at +0x08/+0x0C and orientation at +0x1A.
// - The native reference scan at 0xB3FD10 walks the complete shape in a 5x6
//   grid, while one grid cell remains the authoritative owner of one component.
//
// The release keeps the complete native shape only for boundary validation,
// but reduces replacement collection and final grid ownership to one logical
// cell. If the selected anchor is occupied or the complete shape would cross a
// native boundary, the staged component is relocated to the nearest free (or
// self-owned while moving) full-shape-valid anchor before native commit.

extern "C" {

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

static const DWORD DLL_PROCESS_DETACH_VALUE = 0u;
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

static const s32 GRID_WIDTH = 5;
static const s32 GRID_HEIGHT = 6;
static const u32 GRID_CELL_STRIDE = 0x120u;
static const u32 GRID_CELL_ID_OFFSET = 0x4u;

static const u32 CANDIDATE_TYPE_OFFSET = 0x0u;
static const u32 CANDIDATE_ID_OFFSET = 0x4u;
static const u32 CANDIDATE_X_OFFSET = 0x8u;
static const u32 CANDIDATE_Y_OFFSET = 0xCu;
static const u32 CANDIDATE_ORIENTATION_OFFSET = 0x1Au;
static const u32 SHAPE_EXTENT_A_OFFSET = 0x24u;
static const u32 SHAPE_EXTENT_B_OFFSET = 0x28u;
static const s32 INVALID_COORDINATE_SENTINEL = -1;

struct Patch {
    u32 rva;
    u8 length;
    u8 original[4];
    u8 replacement[4];
    const char* group;
    const char* label;
};

static const Patch PATCHES[] = {
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
static const u32 PATCH_RANGE_START_RVA = 0x00B400F9u;
static const u32 PATCH_RANGE_END_RVA = 0x00B40400u;

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
static u8 g_originalCallBytes[5];
static u8* g_relay = 0;

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

static void append_hex_u64(char* buffer, u32 capacity, u32* position, u64 value) {
    append_text(buffer, capacity, position, "0x");
    for (int shift = 60; shift >= 0 && *position + 1u < capacity; shift -= 4) buffer[(*position)++] = hex_digit((u8)(value >> shift));
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

static u32 read_cell_id(const u8* grid, s32 x, s32 y) {
    if (!grid || x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) return 0xFFFFFFFFu;
    const u32 index = (u32)(x + y * GRID_WIDTH);
    return *(volatile const u32*)(grid + GRID_CELL_ID_OFFSET + (u64)index * GRID_CELL_STRIDE);
}

static bool anchor_is_available(const u8* grid, s32 x, s32 y, u32 componentId) {
    const u32 occupant = read_cell_id(grid, x, y);
    return occupant == 0u || occupant == componentId;
}

static s32 absolute_s32(s32 value) { return value < 0 ? -value : value; }
static u16 normalize_orientation(u16 orientation) { return orientation <= 3u ? orientation : 0u; }

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
static bool full_shape_in_bounds(s32 anchorX, s32 anchorY, u16 orientation, s32 extentA, s32 extentB) {
    const u16 normalized = normalize_orientation(orientation);
    s32 endX = anchorX;
    s32 endY = anchorY;
    s32 stepX = 1;
    s32 stepY = 1;

    if (normalized == 1u) {
        endX = anchorX + extentB;
        endY = anchorY + extentA;
    } else if (normalized == 2u) {
        endX = anchorX - extentA;
        endY = anchorY + extentB;
        stepX = -1;
    } else if (normalized == 3u) {
        endX = anchorX - extentB;
        endY = anchorY - extentA;
        stepX = -1;
        stepY = -1;
    } else {
        endX = anchorX + extentA;
        endY = anchorY - extentB;
        stepY = -1;
    }

    u32 walked = 0u;
    for (s32 x = anchorX; x != endX; x += stepX) {
        if (x < 0 || x >= GRID_WIDTH) return false;
        for (s32 y = anchorY; y != endY; y += stepY) {
            if (y < 0 || y >= GRID_HEIGHT) return false;
            if (++walked > 256u) return false;
        }
    }
    return walked != 0u;
}

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

// Replaces only the call at DS2.exe+0xB43043.
static u8 FASTCALL auto_anchor_reference_check(u8* candidate) {
    ReferenceCheckFn original = g_originalReferenceCheck;
    if (!original || !candidate || !g_imageBase) return original ? original(candidate) : 0u;

    u8* manager = *(u8**)(g_imageBase + MANAGER_POINTER_RVA);
    if (!manager) return original(candidate);
    u8* grid = manager + GRID_OFFSET;

    const u8 type = *(volatile const u8*)(candidate + CANDIDATE_TYPE_OFFSET);
    const u32 componentId = *(volatile const u32*)(candidate + CANDIDATE_ID_OFFSET);
    const s32 originalX = *(volatile const s32*)(candidate + CANDIDATE_X_OFFSET);
    const s32 originalY = *(volatile const s32*)(candidate + CANDIDATE_Y_OFFSET);
    const u16 orientation = normalize_orientation(*(volatile const u16*)(candidate + CANDIDATE_ORIENTATION_OFFSET));
    const u32 intendedOccupant = read_cell_id(grid, originalX, originalY);

    s32 extentA = 0;
    s32 extentB = 0;
    if (!get_shape_extents(type, &extentA, &extentB)) {
        log_anchor_event("AUTO_ANCHOR_SHAPE_LOOKUP_FAILED_NATIVE_FALLBACK", type, componentId,
                         orientation, extentA, extentB, originalX, originalY,
                         intendedOccupant, originalX, originalY, 0u);
        return original(candidate);
    }

    if (++g_wrapperHits == 1u) {
        log_anchor_event("AUTO_ANCHOR_WRAPPER_FIRST_HIT", type, componentId,
                         orientation, extentA, extentB, originalX, originalY,
                         intendedOccupant, originalX, originalY, 0u);
    }

    if (componentId == 0u || componentId == 0xFFFFFFFFu) {
        log_anchor_event("AUTO_ANCHOR_INVALID_COMPONENT_ID_NATIVE_FALLBACK", type, componentId,
                         orientation, extentA, extentB, originalX, originalY,
                         intendedOccupant, originalX, originalY, 0u);
        return original(candidate);
    }

    const bool selectedAnchorAvailable = anchor_is_available(grid, originalX, originalY, componentId);
    const bool selectedShapeInBounds = full_shape_in_bounds(originalX, originalY, orientation, extentA, extentB);

    if (selectedAnchorAvailable && selectedShapeInBounds) {
        const u8 nativeResult = original(candidate);
        if (nativeResult) return nativeResult;
        log_anchor_event("AUTO_ANCHOR_NATIVE_REFERENCE_UNEXPECTED_REJECT", type, componentId,
                         orientation, extentA, extentB, originalX, originalY,
                         intendedOccupant, originalX, originalY, (u32)nativeResult);
        force_local_candidate_rejection(candidate);
        return 0u;
    }

    log_anchor_event(selectedAnchorAvailable ?
                     "AUTO_ANCHOR_SELECTED_FULL_SHAPE_OUT_OF_BOUNDS" :
                     "AUTO_ANCHOR_SELECTED_ANCHOR_OCCUPIED_OR_OUTSIDE_GRID",
                     type, componentId, orientation, extentA, extentB,
                     originalX, originalY, intendedOccupant,
                     originalX, originalY, 0u);

    for (s32 distance = 0; distance <= (GRID_WIDTH - 1) + (GRID_HEIGHT - 1); ++distance) {
        for (s32 y = 0; y < GRID_HEIGHT; ++y) {
            for (s32 x = 0; x < GRID_WIDTH; ++x) {
                if (absolute_s32(x - originalX) + absolute_s32(y - originalY) != distance) continue;
                if (x == originalX && y == originalY) continue;
                if (!anchor_is_available(grid, x, y, componentId)) continue;
                if (!full_shape_in_bounds(x, y, orientation, extentA, extentB)) continue;

                *(volatile s32*)(candidate + CANDIDATE_X_OFFSET) = x;
                *(volatile s32*)(candidate + CANDIDATE_Y_OFFSET) = y;
                const u8 nativeResult = original(candidate);
                if (!nativeResult) continue;

                *(volatile s32*)(manager + STAGED_X_OFFSET) = x;
                *(volatile s32*)(manager + STAGED_Y_OFFSET) = y;
                log_anchor_event("AUTO_ANCHOR_REMAP", type, componentId,
                                 orientation, extentA, extentB,
                                 originalX, originalY, intendedOccupant,
                                 x, y, (u32)nativeResult);
                return nativeResult;
            }
        }
    }

    // Returning false alone is insufficient after the 1x1 commit patch. The
    // local sentinel ensures the native validator rejects exactly this commit.
    force_local_candidate_rejection(candidate);
    log_anchor_event("AUTO_ANCHOR_NO_FREE_FULL_SHAPE_VALID_CELL_REJECTED", type, componentId,
                     orientation, extentA, extentB,
                     originalX, originalY, intendedOccupant,
                     INVALID_COORDINATE_SENTINEL, INVALID_COORDINATE_SENTINEL, 0u);
    return 0u;
}

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
    memory[0] = 0x48u;
    memory[1] = 0xB8u;
    write_u64_le(memory + 2u, (u64)destination);
    memory[10] = 0xFFu;
    memory[11] = 0xE0u;
    FlushInstructionCache(GetCurrentProcess(), memory, 12u);
    DWORD oldProtect = 0u;
    VirtualProtect(memory, pageSize, PAGE_EXECUTE_READ_VALUE, &oldProtect);
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

static bool validate_static_context(HANDLE log) {
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
            write_text(log, "native_reference_scan_unmodified=FAIL\r\n");
            return false;
        }
    }
    write_text(log, "native_reference_scan_unmodified=PASS\r\n");

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

static void restore_footprint_originals_unprotected() {
    for (u32 i = 0u; i < PATCH_COUNT; ++i) copy_bytes(g_imageBase + PATCHES[i].rva, PATCHES[i].original, PATCHES[i].length);
}

static bool apply_footprint_patches(HANDLE log) {
    u8* rangeStart = g_imageBase + PATCH_RANGE_START_RVA;
    const SIZE_T rangeSize = (SIZE_T)(PATCH_RANGE_END_RVA - PATCH_RANGE_START_RVA);
    DWORD oldProtect = 0u;
    if (!VirtualProtect(rangeStart, rangeSize, PAGE_EXECUTE_READWRITE_VALUE, &oldProtect)) {
        write_text(log, "status=FOOTPRINT_VIRTUALPROTECT_FAILED\r\n");
        return false;
    }
    for (u32 i = 0u; i < PATCH_COUNT; ++i) copy_bytes(g_imageBase + PATCHES[i].rva, PATCHES[i].replacement, PATCHES[i].length);
    FlushInstructionCache(GetCurrentProcess(), rangeStart, rangeSize);

    bool verified = true;
    for (u32 i = 0u; i < PATCH_COUNT; ++i) {
        if (!bytes_equal(g_imageBase + PATCHES[i].rva, PATCHES[i].replacement, PATCHES[i].length)) {
            verified = false;
            log_patch_bytes(log, "verify_failed", PATCHES[i], g_imageBase + PATCHES[i].rva);
            break;
        }
    }
    if (!verified) {
        restore_footprint_originals_unprotected();
        FlushInstructionCache(GetCurrentProcess(), rangeStart, rangeSize);
    }
    DWORD ignored = 0u;
    VirtualProtect(rangeStart, rangeSize, oldProtect, &ignored);
    if (!verified) return false;

    for (u32 i = 0u; i < PATCH_COUNT; ++i) log_patch_bytes(log, "patched", PATCHES[i], PATCHES[i].replacement);
    return true;
}

static bool install_reference_check_detour(HANDLE log) {
    u8* callSite = g_imageBase + REFERENCE_CHECK_CALLSITE_RVA;
    u8* destination = (u8*)&auto_anchor_reference_check;
    u8* branchDestination = destination;

    copy_bytes(g_originalCallBytes, callSite, 5u);
    if (!rel32_fits(callSite + 5u, destination)) {
        g_relay = allocate_near_relay(callSite, destination);
        if (!g_relay) {
            write_text(log, "status=NEAR_RELAY_ALLOCATION_FAILED\r\n");
            return false;
        }
        branchDestination = g_relay;
        write_text(log, "detour_mode=NEAR_RELAY\r\n");
    } else {
        write_text(log, "detour_mode=DIRECT_REL32\r\n");
    }

    u8 replacement[5];
    build_call_bytes(replacement, callSite, branchDestination);
    DWORD oldProtect = 0u;
    if (!VirtualProtect(callSite, 5u, PAGE_EXECUTE_READWRITE_VALUE, &oldProtect)) {
        write_text(log, "status=DETOUR_VIRTUALPROTECT_FAILED\r\n");
        return false;
    }
    copy_bytes(callSite, replacement, 5u);
    FlushInstructionCache(GetCurrentProcess(), callSite, 5u);
    const bool verified = bytes_equal(callSite, replacement, 5u);
    DWORD ignored = 0u;
    VirtualProtect(callSite, 5u, oldProtect, &ignored);
    if (!verified) {
        write_text(log, "status=DETOUR_VERIFY_FAILED\r\n");
        return false;
    }

    char line[320];
    u32 position = 0u;
    append_text(line, sizeof(line), &position, "reference_check_callsite_rva=");
    append_hex_u32(line, sizeof(line), &position, REFERENCE_CHECK_CALLSITE_RVA);
    append_text(line, sizeof(line), &position, " wrapper_address=");
    append_hex_u64(line, sizeof(line), &position, (u64)destination);
    append_text(line, sizeof(line), &position, " branch_destination=");
    append_hex_u64(line, sizeof(line), &position, (u64)branchDestination);
    append_text(line, sizeof(line), &position, "\r\n");
    write_text(log, line);
    return true;
}

static void restore_all() {
    if (!g_patchApplied || !g_imageBase) return;

    u8* rangeStart = g_imageBase + PATCH_RANGE_START_RVA;
    const SIZE_T rangeSize = (SIZE_T)(PATCH_RANGE_END_RVA - PATCH_RANGE_START_RVA);
    DWORD oldProtect = 0u;
    if (VirtualProtect(rangeStart, rangeSize, PAGE_EXECUTE_READWRITE_VALUE, &oldProtect)) {
        restore_footprint_originals_unprotected();
        FlushInstructionCache(GetCurrentProcess(), rangeStart, rangeSize);
        DWORD ignored = 0u;
        VirtualProtect(rangeStart, rangeSize, oldProtect, &ignored);
    }

    u8* callSite = g_imageBase + REFERENCE_CHECK_CALLSITE_RVA;
    if (VirtualProtect(callSite, 5u, PAGE_EXECUTE_READWRITE_VALUE, &oldProtect)) {
        copy_bytes(callSite, g_originalCallBytes, 5u);
        FlushInstructionCache(GetCurrentProcess(), callSite, 5u);
        DWORD ignored = 0u;
        VirtualProtect(callSite, 5u, oldProtect, &ignored);
    }

    if (g_relay) {
        VirtualFree(g_relay, 0u, MEM_RELEASE_VALUE);
        g_relay = 0;
    }
    g_patchApplied = 0;
}

static DWORD WINAPI worker_thread(LPVOID) {
    Sleep(1200u);
    g_imageBase = (u8*)GetModuleHandleW((const wchar_t*)0);
    if (g_imageBase) {
        g_originalReferenceCheck = (ReferenceCheckFn)(g_imageBase + NATIVE_REFERENCE_CHECK_RVA);
        g_shapeLookup = (ShapeLookupFn)(g_imageBase + SHAPE_LOOKUP_RVA);
    }

    HANDLE log = open_log_create();
    if (log == (HANDLE)(s64)-1) return 1u;
    write_text(log, "DS2 High-Density Backpack Modules v1.0.0\r\n");
    write_text(log, "mode=RELEASE_HIGH_DENSITY_BACKPACK_MODULES\r\n");
    write_text(log, "target=DS2.exe v1.10.89.0\r\n");
    write_text(log, "expected_sha256=BF3D1C665545930BC850D8F5DF486F7395885BB729D4FD408FDB03390DE0765B\r\n");
    write_text(log, "shape_lookup_rva=0x00BAD600\r\n");
    write_text(log, "shape_extent_offsets=0x24,0x28\r\n");
    write_text(log, "native_reference_scan=UNMODIFIED\r\n");
    write_text(log, "native_conflict_branch=UNMODIFIED\r\n");
    write_text(log, "anchor_policy=NEAREST_FREE_OR_SELF_FULL_SHAPE_VALID\r\n");
    write_text(log, "no_valid_anchor_policy=FORCE_LOCAL_NATIVE_REJECT\r\n");

    if (!validate_pe()) {
        write_text(log, "status=BASELINE_REJECTED\r\n");
        CloseHandle(log);
        return 2u;
    }
    write_text(log, "baseline_check=PASS\r\n");

    if (!validate_static_context(log)) {
        write_text(log, "status=STATIC_CONTEXT_REJECTED\r\n");
        CloseHandle(log);
        return 3u;
    }

    if (!apply_footprint_patches(log)) {
        write_text(log, "status=FOOTPRINT_PATCH_FAILED\r\n");
        CloseHandle(log);
        return 4u;
    }

    if (!install_reference_check_detour(log)) {
        u8* rangeStart = g_imageBase + PATCH_RANGE_START_RVA;
        const SIZE_T rangeSize = (SIZE_T)(PATCH_RANGE_END_RVA - PATCH_RANGE_START_RVA);
        DWORD oldProtect = 0u;
        if (VirtualProtect(rangeStart, rangeSize, PAGE_EXECUTE_READWRITE_VALUE, &oldProtect)) {
            restore_footprint_originals_unprotected();
            FlushInstructionCache(GetCurrentProcess(), rangeStart, rangeSize);
            DWORD ignored = 0u;
            VirtualProtect(rangeStart, rangeSize, oldProtect, &ignored);
        }
        if (g_relay) {
            VirtualFree(g_relay, 0u, MEM_RELEASE_VALUE);
            g_relay = 0;
        }
        write_text(log, "status=DETOUR_INSTALL_FAILED_RESTORED\r\n");
        CloseHandle(log);
        return 5u;
    }

    g_patchApplied = 1;
    write_text(log, "status=PATCH_APPLIED\r\n");
    write_text(log, "replacement_collection_footprint=1x1\r\n");
    write_text(log, "grid_validate_commit_footprint=1x1\r\n");
    write_text(log, "stage5_shape_bounds_guard=ACTIVE\r\n");
    write_text(log, "same_selected_anchor_auto_relocated=YES\r\n");
    write_text(log, "out_of_bounds_visual_shape_auto_relocated=YES\r\n");
    write_text(log, "grid_dimensions=5x6\r\n");
    CloseHandle(log);
    return 0u;
}

extern "C" BOOL WINAPI DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH_VALUE) {
        DisableThreadLibraryCalls(module);
        g_mutex = CreateMutexW(0, 0, L"Local\\DS2_HighDensityBackpackModules_v1_0_0");
        if (!g_mutex || GetLastError() == ERROR_ALREADY_EXISTS_VALUE) return 1;
        HANDLE thread = CreateThread(0, 0, worker_thread, 0, 0, 0);
        if (thread) CloseHandle(thread);
    } else if (reason == DLL_PROCESS_DETACH_VALUE) {
        if (!reserved) restore_all();
        if (g_mutex) {
            CloseHandle(g_mutex);
            g_mutex = 0;
        }
    }
    return 1;
}
