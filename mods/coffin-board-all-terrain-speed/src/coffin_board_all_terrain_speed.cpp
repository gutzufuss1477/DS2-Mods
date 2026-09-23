// DS2 Coffin Board All-Terrain Speed v1.83.0-network-availability
// Target: DEATH STRANDING 2: ON THE BEACH, Steam 1.10.89.0
//
// Active production strategy:
//   - preserve the proven v1.54 QPID/entity boundary-crossing roots;
//   - keep speed/acceleration/drive-force and true Floating Carrier support;
//   - use the normal DSPlayerRideVehicleActionPlugin Coffin path only;
//   - scope the RideVehicle inhibit exception to a captured live Coffin;
//   - preserve native availability across the Coffin-only network veto;
//   - keep the later native action/target builder authoritative and observe 0x158 only;
//   - leave RideFloater E3/bit20/EDFE10/C8/input experiments uninstalled.
// DS2 must still build the native action 0x158 and enter Player RideVehicle
// state 3 with its own current target; this mod does not fabricate mount state.
extern "C" {
typedef void* HANDLE;
typedef void* HMODULE;
typedef void* HINSTANCE;
typedef void* LPVOID;
typedef const void* LPCVOID;
typedef unsigned long DWORD;
typedef long LONG;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef unsigned int UINT32;
typedef unsigned long long UINT64;
typedef unsigned long long SIZE_T;
typedef long long INT64;
typedef wchar_t WCHAR;
typedef const WCHAR* LPCWSTR;
typedef DWORD (__stdcall *LPTHREAD_START_ROUTINE)(LPVOID);
struct MEMORY_BASIC_INFORMATION_X64 {
    LPVOID BaseAddress;
    LPVOID AllocationBase;
    DWORD AllocationProtect;
    WORD PartitionId;
    SIZE_T RegionSize;
    DWORD State;
    DWORD Protect;
    DWORD Type;
};
struct SYSTEMTIME_X {
    WORD wYear, wMonth, wDayOfWeek, wDay, wHour, wMinute, wSecond, wMilliseconds;
};
struct THREADENTRY32_X {
    DWORD dwSize;
    DWORD cntUsage;
    DWORD th32ThreadID;
    DWORD th32OwnerProcessID;
    LONG tpBasePri;
    LONG tpDeltaPri;
    DWORD dwFlags;
};
__declspec(dllimport) int __stdcall CloseHandle(HANDLE);
__declspec(dllimport) void __stdcall AcquireSRWLockExclusive(void*);
__declspec(dllimport) void __stdcall AcquireSRWLockShared(void*);
__declspec(dllimport) HANDLE __stdcall CreateFileW(LPCWSTR, DWORD, DWORD, LPVOID, DWORD, DWORD, HANDLE);
__declspec(dllimport) HANDLE __stdcall CreateMutexW(LPVOID, int, LPCWSTR);
__declspec(dllimport) HANDLE __stdcall CreateToolhelp32Snapshot(DWORD, DWORD);
__declspec(dllimport) HANDLE __stdcall CreateThread(LPVOID, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, DWORD*);
__declspec(dllimport) int __stdcall DisableThreadLibraryCalls(HMODULE);
__declspec(dllimport) DWORD __stdcall GetLastError();
__declspec(dllimport) void __stdcall GetLocalTime(SYSTEMTIME_X*);
__declspec(dllimport) DWORD __stdcall GetCurrentProcessId();
__declspec(dllimport) DWORD __stdcall GetCurrentThreadId();
__declspec(dllimport) int __stdcall GetThreadContext(HANDLE, void*);
__declspec(dllimport) DWORD __stdcall GetModuleFileNameW(HMODULE, WCHAR*, DWORD);
__declspec(dllimport) HMODULE __stdcall GetModuleHandleW(LPCWSTR);
__declspec(dllimport) unsigned int __stdcall GetPrivateProfileIntW(LPCWSTR, LPCWSTR, int, LPCWSTR);
__declspec(dllimport) HANDLE __stdcall OpenThread(DWORD, int, DWORD);
__declspec(dllimport) void __stdcall ReleaseSRWLockShared(void*);
__declspec(dllimport) void __stdcall ReleaseSRWLockExclusive(void*);
__declspec(dllimport) DWORD __stdcall ResumeThread(HANDLE);
__declspec(dllimport) DWORD __stdcall SetFilePointer(HANDLE, long, long*, DWORD);
__declspec(dllimport) void __stdcall Sleep(DWORD);
__declspec(dllimport) DWORD __stdcall SuspendThread(HANDLE);
__declspec(dllimport) int __stdcall Thread32First(HANDLE, THREADENTRY32_X*);
__declspec(dllimport) int __stdcall Thread32Next(HANDLE, THREADENTRY32_X*);
__declspec(dllimport) LPVOID __stdcall VirtualAlloc(LPVOID, SIZE_T, DWORD, DWORD);
__declspec(dllimport) SIZE_T __stdcall VirtualQuery(LPCVOID, MEMORY_BASIC_INFORMATION_X64*, SIZE_T);
__declspec(dllimport) int __stdcall VirtualProtect(LPVOID, SIZE_T, DWORD, DWORD*);
__declspec(dllimport) int __stdcall FlushInstructionCache(HANDLE, LPCVOID, SIZE_T);
__declspec(dllimport) int __stdcall WriteFile(HANDLE, LPCVOID, DWORD, DWORD*, LPVOID);
}
extern "C" int _fltused = 0;
extern "C" void* memset(void* destination, int value, SIZE_T size) {
    BYTE* output = (BYTE*)destination;
    for (SIZE_T i = 0; i < size; ++i) output[i] = (BYTE)value;
    return destination;
}
extern "C" void* memcpy(void* destination, const void* source, SIZE_T size) {
    BYTE* output = (BYTE*)destination;
    const BYTE* input = (const BYTE*)source;
    for (SIZE_T i = 0; i < size; ++i) output[i] = input[i];
    return destination;
}
#define TRUE 1
#define FALSE 0
#define DLL_PROCESS_ATTACH 1u
#define DLL_PROCESS_DETACH 0u
#define ERROR_ALREADY_EXISTS 183u
#define TH32CS_SNAPTHREAD 0x00000004u
#define THREAD_SUSPEND_RESUME 0x00000002u
#define THREAD_GET_CONTEXT 0x00000008u
#define CONTEXT_CONTROL_X64 0x00100001u
#define MEM_COMMIT 0x1000u
#define MEM_RESERVE 0x2000u
#define PAGE_NOACCESS 0x01u
#define PAGE_READWRITE 0x04u
#define PAGE_WRITECOPY 0x08u
#define PAGE_EXECUTE_READWRITE 0x40u
#define PAGE_EXECUTE_WRITECOPY 0x80u
#define PAGE_GUARD 0x100u
#define FILE_APPEND_DATA 0x00000004u
#define FILE_SHARE_READ 0x00000001u
#define FILE_SHARE_WRITE 0x00000002u
#define OPEN_ALWAYS 4u
#define FILE_ATTRIBUTE_NORMAL 0x80u
#define FILE_END 2u
#define INVALID_HANDLE_VALUE ((HANDLE)(INT64)-1)

static const UINT32 EXPECTED_TIMESTAMP = 0x6A3DAE46u;
static const UINT32 EXPECTED_IMAGE_SIZE = 0x0B292000u;
static const SIZE_T OFF_LAND_TOP_SPEED = 0x44u;
static const SIZE_T OFF_FINAL_GEAR_RATIO = 0x48u;
static const SIZE_T OFF_LAND_BOOST_TOP_SPEED = 0x94u;
static const SIZE_T OFF_WATER_TOP_SPEED = 0x98u;
static const SIZE_T OFF_WATER_BOOST_TOP_SPEED = 0x9Cu;
static const SIZE_T OFF_SLIP_SPEED = 0xD8u;
static const SIZE_T OFF_COFFIN_QPID_BORDER_ESCAPE_SPEED = 0x5Cu;
static const UINT32 COFFIN_PHYSICS_GROUP_ID = 499u;
static const UINT32 COFFIN_PHYSICS_OBJECT_INDEX = 71288u;
static const UINT64 COFFIN_PHYSICS_UUID_LOW = 0x6B4E9446DCF0C387ull;
static const UINT64 COFFIN_PHYSICS_UUID_HIGH = 0x26DBAD5945C463BAull;
static const UINT32 COFFIN_RIDE_CONFIG_GROUP_ID = 31126u;
static const UINT32 COFFIN_RIDE_CONFIG_OBJECT_INDEX = 19173u;
static const UINT64 COFFIN_RIDE_CONFIG_UUID_LOW = 0x0230E666A879EA57ull;
static const UINT64 COFFIN_RIDE_CONFIG_UUID_HIGH = 0x21C573467A45A1A5ull;
static const float COFFIN_QPID_BORDER_ESCAPE_SPEED_NATIVE = 25.0f;
static const float COFFIN_QPID_BORDER_ESCAPE_SPEED_TARGET = 0.1f;
static const UINT32 RVA_STREAMING_SIGNATURE = 0x00693674u;
static const UINT32 RVA_STREAMING_MANAGER_GLOBAL = 0x06266938u;
// Separate world/chiral manager used by the native RideFloater BB/BC gates.
// D9B790/D9B7C0 and FFFCF0 resolve this exact RIP global (NOT the resource StreamingManager above).
static const UINT32 RVA_CHIRAL_WORLD_MANAGER_GLOBAL = 0x0623E9C8u;
static const UINT32 RVA_STREAMING_SYSTEM_VTABLE = 0x034532A0u;
static const UINT32 RVA_STREAMING_ADD_LISTENER = 0x026F6E40u;
static const UINT32 RVA_STREAMING_REMOVE_LISTENER = 0x026F6EE0u;
static const UINT32 RVA_DRIVE_TORQUE_HOOK = 0x0247A431u;
static const UINT32 RVA_COFFIN_LINKED_OBJECT_TYPE_LIMIT = 0x01007D94u;
static const UINT32 RVA_COFFIN_RIDEVEHICLE_ACTION_UPDATE = 0x010047D0u;
static const UINT32 RVA_COFFIN_RIDEVEHICLE_NETWORK_BRANCH = 0x010047EAu;
static const UINT32 RVA_COFFIN_RIDEVEHICLE_NETWORK_CONTINUE = 0x010047F0u;
static const UINT32 RVA_COFFIN_RIDEVEHICLE_NETWORK_REJECT = 0x0100493Eu;
// v1.77: DSPlayerRideVehicleActionPlugin::1006010 has a second direct
// Chiral-network kill switch. At 10060C4 it calls F4D290, which only sets
// plugin+0x0A=1 (request-disable). Scope that one call to the live type-3
// Coffin target instead of touching the global network byte.
static const UINT32 RVA_COFFIN_RIDEVEHICLE_NETWORK_CANCEL_CALL = 0x010060C4u;
static const UINT32 RVA_ACTION_PLUGIN_DISABLE = 0x00F4D290u;
// DSPlayerRideVehicleActionPlugin exact vtable.
static const UINT32 RVA_RIDEVEHICLE_PLUGIN_VTABLE = 0x0325E168u;
static const UINT32 RVA_RIDEVEHICLE_START_THUNK = 0x01004680u; // retained for old dead observer only
static const UINT32 RVA_RIDEVEHICLE_ACTION_BUILDER = 0x010079D0u;
static const UINT32 RVA_COMPONENT_LOOKUP = 0x0011FFA0u;
static const UINT32 RVA_BAGGAGE_CARRIER_COMPONENT_TYPE = 0x0436ECE0u;
static const UINT32 RVA_COFFIN_PRESERVE_CARRIER_HOOK = 0x00F9A807u;
static const UINT32 RVA_COFFIN_PRESERVE_CARRIER_CONTINUE = 0x00F9A81Cu;
static const UINT32 RVA_COFFIN_PRESERVE_CARRIER_SKIP = 0x00F9AC6Bu;
static const UINT32 RVA_RESOLVE_LINKED_CARRIER = 0x00E462F0u;
static const UINT32 RVA_CARRIER_DETACH_EVENT_GATE = 0x00E5C7AAu;
static const UINT32 RVA_CARRIER_DETACH_OTHER_EVENT = 0x00E5C7C2u;
static const UINT32 RVA_CARRIER_DETACH_NATIVE = 0x00E5CDD3u;
static const UINT32 RVA_CARRIER_DETACH_SUPPRESSED = 0x00E5D0A7u;
static const UINT32 RVA_CARRIER_WARNING_NOTIFICATION_GATE = 0x0121D839u;
static const UINT32 RVA_CARRIER_WARNING_NOTIFICATION_CONTINUE = 0x0121D84Au;
static const UINT32 RVA_ENTITY_MESSAGE_DISPATCH = 0x00130C60u;
// Coffin-main-vtable slot +0x108.  This virtual method handles request E3
// locally before the shared action resolver is reached.  Its E3-only branch
// rejects the two network-status bits; all following native validation stays
// intact when that one branch is bypassed.
static const UINT32 RVA_COFFIN_RIDE_FLOATER_ACTION_E3_PREDICATE = 0x01000A70u;
static const UINT32 RVA_COFFIN_RIDE_FLOATER_ACTION_E3_NETWORK_BRANCH = 0x01000B3Eu;
static const UINT32 RVA_COFFIN_RIDE_FLOATER_ACTION_E3_VTABLE_SLOT = 0x0323D610u;
// v1.55 mount-only trace: RideFloater +0x118 is the nested native gate called
// by 1000A70 for request E3. The wrapper is read-only and preserves its result.
static const UINT32 RVA_V155_RIDEFLOATER_GATE118 = 0x011215E0u;
// Coffin-main-vtable slot +0x110.  ED6200 calls this producer after the
// +0x108 availability predicate has accepted an E3 candidate.
static const UINT32 RVA_COFFIN_RIDE_FLOATER_ACTION_E3_RECORD_SLOT = 0x0323D618u;
static const UINT32 RVA_COFFIN_NETWORK_RESOLVER_REJECT_GATE = 0x00ED9B67u;
static const UINT32 RVA_COFFIN_NETWORK_RESOLVER_CONTINUE = 0x00ED9B79u;
static const UINT32 RVA_COFFIN_NETWORK_RESOLVER_FALSE = 0x00ED9BE9u;
// ED9C80 advances the per-request state cursor immediately after EDFE10
// returns.  At this point RBX is the current state entry and RSI is its
// request index; a Coffin-only post-producer write can therefore participate
// in the native refresh lifecycle without touching the mount dispatcher.
static const UINT32 RVA_COFFIN_NETWORK_STATE_POST_PRODUCER = 0x00ED9DEBu;
// Outside the native network the refresh bitset can omit E3 entirely.  Hook
// the bitset test so E3 still receives one native EDFE10 refresh pass before
// the Coffin-only post-producer promotion below.
static const UINT32 RVA_COFFIN_NETWORK_STATE_BITSET_TEST = 0x00ED9DB0u;
// The preceding ED9C80 loop is the native action-record producer gate. Its
// E3 bit is absent outside the network, so ED6200 is never reached even when
// the Coffin descriptor and state bitsets are present.
static const UINT32 RVA_COFFIN_NETWORK_ACTION_BITSET_TEST = 0x00ED9D30u;
// Native request-record builder used by the action refresh routine.
static const UINT32 RVA_COFFIN_NETWORK_REGISTER_E3 = 0x00ED6200u;
// Native candidate registration helper called by ED52C0.  This is a
// read-only probe point: it is the exact helper that inserts entries into the
// manager-owned sorted candidate container at +0x2D0.
static const UINT32 RVA_COFFIN_NATIVE_CANDIDATE_REGISTER = 0x00ED4EF0u;
// The candidate container is fed through the generic entity-message helper.
// This hook is optional and filters before logging, so unrelated message
// traffic remains entirely native.
static const UINT32 RVA_COFFIN_NATIVE_CANDIDATE_MESSAGE_DISPATCH = 0x001618C0u;
// ED52C0 dispatches pad-cache/network events and conditionally calls the
// candidate registration helper.  This is a read-only upstream discriminator.
static const UINT32 RVA_COFFIN_NATIVE_EVENT_DISPATCH = 0x00ED52C0u;
static const UINT32 RVA_NATIVE_EVENT_ACTION_REGISTRY_GLOBAL = 0x0623EA48u;
// Exact RTTI-backed cache vtable for DSPlayerPadCacheInfoCart<DSBaggageInfo,2>.
// Slots +0x18/+0x20 are the two native cache insertion methods identified by
// the offline decompilation; the probe below observes those methods directly.
static const UINT32 RVA_COFFIN_BAGGAGE_INFO_CACHE_VTABLE = 0x0324B4F8u;
static const UINT32 RVA_COFFIN_BAGGAGE_INFO_VTABLE = 0x0327CC68u;
static const UINT32 RVA_COFFIN_BAGGAGE_INFO_REFRESH_SLOT = 0x18u;
static const UINT32 RVA_COFFIN_BAGGAGE_INFO_REFRESH = 0x011C14B0u;
// Across all 193 native Coffin cache-admission samples from the current
// v1.10.89.0 runtime, +0x08 was stable at this exact value.  Use it together
// with the RTTI-confirmed DSBaggageInfo vtable to scope the persistent bit20
// experiment to the real Coffin candidate rather than all baggage objects.
static const UINT64 COFFIN_BAGGAGE_Q08_SIGNATURE = 0x0300660000000033ull;
static const UINT32 COFFIN_BAGGAGE_D4_MOUNT_BIT = 0x00100000u;
// v1.40-dev19: exact event handler that owns candidate+0xD4 bit20.
// The first 16 bytes are position-independent and safe to replay in a trampoline.
static const UINT32 RVA_COFFIN_BIT20_EVENT_HANDLER = 0x01221CB0u;
static const UINT32 COFFIN_BIT20_EVENT_ID = 0x1BE9DE97u;
static const UINT32 RVA_COFFIN_BAGGAGE_INFO_CACHE_RESET = 0x00EE3F70u;
static const UINT32 RVA_COFFIN_BAGGAGE_INFO_CACHE_RESET_SLOT = 0x00u;
// Slot +0x10 is the true native admission boundary. EDB900 reaches it only
// after its physical-query, type and registry filters have accepted a DSBaggageInfo.
static const UINT32 RVA_COFFIN_BAGGAGE_INFO_CACHE_ADMIT_SLOT = 0x10u;
static const UINT32 RVA_COFFIN_BAGGAGE_INFO_CACHE_ADMIT = 0x00EE5B60u;
static const UINT32 RVA_COFFIN_BAGGAGE_INFO_CACHE_INSERT_SLOT = 0x18u;
static const UINT32 RVA_COFFIN_BAGGAGE_INFO_CACHE_INSERT_EXPLICIT_SLOT = 0x20u;
static const UINT32 RVA_COFFIN_BAGGAGE_INFO_CACHE_INSERT = 0x00EE5A20u;
static const UINT32 RVA_COFFIN_BAGGAGE_INFO_CACHE_INSERT_EXPLICIT = 0x00EE5AE0u;

// RTTI-confirmed CoffinBoardCheckPhysicsCollisionFilter.  Its slot +0x18 is
// the Coffin Board's collision predicate.  DS2 also ships an explicit
// PhysicsCollisionIgnoreChiralWallFilter whose slot +0x18 rejects collision
// category 0x16.  The wrapper below preserves the Coffin predicate for every
// object except that exact engine-defined Chiral-Wall category.
static const UINT32 RVA_COFFIN_COLLISION_FILTER_VTABLE = 0x0325DFA8u;
static const UINT32 RVA_COFFIN_COLLISION_FILTER_PREDICATE_SLOT = 0x18u;
static const UINT32 RVA_COFFIN_COLLISION_FILTER_PREDICATE = 0x01331490u;
static const UINT32 RVA_PHYSICS_COLLISION_IGNORE_CHIRAL_WALL_VTABLE = 0x0328A458u;
static const UINT32 RVA_PHYSICS_COLLISION_IGNORE_CHIRAL_WALL_PREDICATE = 0x0128F930u;
static const UINT32 RVA_RESOLVE_PHYSICS_COLLISION_OBJECT = 0x0011EA10u;
static const UINT32 PHYSICS_COLLISION_TYPE_CHIRAL_WALL = 0x16u;
static const UINT32 PHYSICS_COLLISION_TYPE_VEHICLE_STOPPER = 0x1Bu;
static const SIZE_T OFF_COFFIN_ACTION_MANAGER = 0x60u;
static const SIZE_T OFF_COFFIN_MANAGER_TABLES = 0x25068u;
static const SIZE_T OFF_COFFIN_TABLES_CANDIDATE_CURSOR = 0x73A0u;
static const SIZE_T OFF_COFFIN_MANAGER_RECORDS = 0x48870u;
static const SIZE_T COFFIN_REQUEST_RECORD_STRIDE = 0x4D0u;
static const UINT32 COFFIN_NETWORK_E3_REQUEST = 0xE3u;
static const UINT32 RVA_COFFIN_MOUNT_VFUNC = 0x0111F9F0u;
static const UINT32 RVA_COFFIN_RIDE_FLOATER_ACTION_VTABLE = 0x0323D508u;
static const UINT32 RVA_COFFIN_RIDE_FLOATER_ACTION_C8_SLOT = 0x0323D5D0u;
static const UINT32 RVA_COFFIN_RIDE_FLOATER_MOUNT_ENTRY = 0x00FFFCF0u;
static const UINT32 RVA_COFFIN_ACTION_DISPATCH = 0x0111F4B0u;
// The Coffin registration callback builds a 0x40-byte request descriptor on
// its stack and passes it to F64E80. Its E3 descriptor flag (word 7, bit 3)
// is normally zero, so ED9C80 never reaches ED6200 for this action. The hook
// sets only that local Coffin descriptor bit and preserves the original
// r9/edx/vzeroupper sequence.
static const UINT32 RVA_COFFIN_E3_DESCRIPTOR_FLAG_HOOK = 0x00FF7CC6u;
static const UINT32 RVA_COFFIN_RIDE_FLOATER_MOUNT_NETWORK_GATE = 0x00FFFD03u;
static const UINT32 RVA_COFFIN_DRIVE_NETWORK_GATE = 0x00F8A79Du;
// RTTI correction: F8DB60/F8DC51 belongs to DSPlayerRideFloaterKickState,
// not FallingState.  The true FallingState entry is F8BD90.
static const UINT32 RVA_COFFIN_KICK_NETWORK_GATE = 0x00F8DC51u;
static const UINT32 RVA_COFFIN_DRIVE_STATE_ENTRY = 0x00F8A750u;
static const UINT32 RVA_COFFIN_FALLING_STATE_ENTRY = 0x00F8BD90u;
static const UINT32 RVA_COFFIN_KICK_STATE_ENTRY = 0x00F8DB60u;
// v1.29: true per-frame RideFloater Drive virtual and the EndState motion
// virtual identified by the RTTI/vtable audit.  These are passive probes only.
static const UINT32 RVA_COFFIN_DRIVE_STATE_TICK = 0x00F8A930u;
static const UINT32 RVA_COFFIN_RIDE_UPDATE = 0x01001BD0u;
static const UINT32 RVA_COFFIN_RIDE_UPDATE_CALLS[4] = {
    0x00F8A94Du, 0x00F8BE74u, 0x00F8DD43u, 0x00F8E857u
};
// v1.61: 1001BD0 itself resolves the currently ridden vehicle with 1783C0.
// Reuse that exact native lookup read-only to remember the real DSVehicleCoffin
// entity before dismount; this avoids all outside-network broadphase discovery.
static const UINT32 RVA_V161_ACTION_VEHICLE_RESOLVE = 0x001783C0u;
static const UINT32 RVA_V161_COFFIN_VEHICLE_VTABLE = 0x033AE468u;
static const UINT32 RVA_V161_BAGGAGE_COMPONENT_VTABLE = 0x0327C440u;
static const UINT32 RVA_V161_BAGGAGE_COMPONENT_ALT_VTABLE = 0x0327CEA0u;
// v1.67: immediate object-path recovery.  DSVehicleCoffin::vftable +0x180 is
// exactly FUN_141F76D80, the same Coffin method whose internal second-QPID branch
// is already used by the frozen v1.54 crossing fix.  The hook is pass-through and
// only remembers `this`; it does not alter the function's return/state.
static const UINT32 RVA_V167_COFFIN_UPDATE_SLOT = 0x180u;
static const UINT32 RVA_V167_COFFIN_UPDATE_NATIVE = 0x01F76D80u;
// ED9C80 has one direct call to EDB900 at EDA12B.  Wrapping this call preserves
// EDB900 natively and gives us its live reference-position parameter for an
// immediate native EE5B60 fallback when the outside cache is empty.
static const UINT32 RVA_V167_EDB900_CALL = 0x00EDA12Bu;
static const UINT32 RVA_V167_EDB900_NATIVE = 0x00EDB900u;
// v1.69: native component lookup with the actual DSBaggageComponent type key.
// FUN_1411AF150 returns &DAT_14436C9F0 and the adjacent FUN_1411AF160
// installs DSBaggageComponent::vftable.  DAT_144499F00 is AirWallEffect.
static const UINT32 RVA_V169_COMPONENT_LOOKUP = 0x0011FFA0u;
static const UINT32 RVA_V169_BAGGAGE_COMPONENT_TYPE = 0x0436C9F0u;
// v1.63 direct-interaction root: RideFloater::FFFBC0 consumes the real
// player-system packet and calls 11CA860 immediately before the native
// E5B3F0(state=4,target=packet+0x620) path.  We scope only this single call.
static const UINT32 RVA_V163_DIRECT_INTERACTION = 0x00FFFBC0u;
static const UINT32 RVA_V163_INTERACTION_GATE_CALL = 0x00FFFBDEu;
static const UINT32 RVA_V163_INTERACTION_GATE_NATIVE = 0x011CA860u;
static const UINT32 RVA_COFFIN_ENDSTATE_MOTION = 0x00F8A990u;
// Same RideFloater action layout as the native 4C producer.  This routine
// consumes StreamingManager+0x2264BC/+0x2264C0 and converts it to a mode before
// calling 10112650, so it is the strongest current candidate for directional
// border slowdown.
static const UINT32 RVA_COFFIN_BORDER_MODE_CONTROLLER = 0x0100FF80u;
// Native upstream producer that enumerates Coffin/Baggage candidates and emits
// event 0x4C87C18F at 100A7AB.  ED52C0 can subsequently route that event to
// ED4EF0 and the real E3 candidate container.
static const UINT32 RVA_COFFIN_4C_PRODUCER = 0x0100A300u;
static const UINT32 RVA_COFFIN_4C_CANDIDATE_CHECK = 0x010065A0u;
static const UINT32 RVA_COFFIN_4C_CHECK_RETURN_FALSE_PROBE = 0x0100A604u;
static const UINT32 RVA_COFFIN_4C_CHECK_RETURN_TRUE_PROBE = 0x0100A68Du;
// v1.30: EDA7xx rebuilds the fixed RideFloater/Baggage spatial candidate list.
// By EDAB2F both native spatial-query passes and the list merge are complete;
// RDI is the same root object whose +0x12D98 count is consumed by 100A300.
static const UINT32 RVA_COFFIN_SPATIAL_LIST_POST = 0x00EDAB2Fu;
// Immediately after the native Coffin torque write (and after our existing
// drive-force multiplier), the live factors are still in XMM6/XMM12.  This
// passive probe tells us whether the border slowdown is an input/torque clamp
// rather than a physics collision or state transition.
static const UINT32 RVA_COFFIN_DRIVE_TORQUE_POST = 0x0247A43Fu;
// v1.31: Coffin EndState hard-border correction gate. When the computed
// separation exceeds 3.0 the native state enters F8AF63..F8AFEF and calls
// FD5090 to reposition/stop the rider. Skip only that correction block.
static const UINT32 RVA_COFFIN_ENDSTATE_BORDER_CORRECTION_GATE = 0x00F8AF5Du;
static const UINT32 RVA_COFFIN_BORDER_SIGNAL_SETTER = 0x00D9B790u;
static const UINT32 RVA_COFFIN_MOUNT_SIGNAL_SETTER = 0x00D9B7C0u;
static const UINT32 RVA_COFFIN_F3_DESCRIPTOR_REMOVE_CALL = 0x010E06A0u;
static const UINT32 RVA_COFFIN_DRIVE_FORCED_END_GATE = 0x00F8A7CAu;
static const UINT32 RVA_COFFIN_QPID_ESCAPE_STATE_A = 0x00F8AD82u;
static const UINT32 RVA_COFFIN_QPID_ESCAPE_STATE_B = 0x00F97BFFu;
static const UINT32 RVA_COFFIN_QPID_ESCAPE_STATE_A_ENTRY = 0x00F8AD50u;
static const UINT32 RVA_COFFIN_QPID_ESCAPE_STATE_B_ENTRY = 0x00F97BD0u;
static const UINT32 RVA_COFFIN_QPID_ESCAPE_STATE_A_VTABLE_SLOT = 0x0325B928u;
static const UINT32 RVA_COFFIN_QPID_ESCAPE_STATE_B_VTABLE_SLOT = 0x0325BE38u;
static const UINT32 RVA_COFFIN_PHYSICS_VTABLE = 0x033AD0B0u;
static const UINT32 RVA_COFFIN_PROGRESSIVE_DECAY_ARM_BRANCH = 0x01F41E37u;
static const UINT32 RVA_COFFIN_SPATIAL_GATE_CALL = 0x00EDA046u;
static const UINT32 RVA_COFFIN_SPATIAL_GATE_NATIVE = 0x00E9E6B0u;
static const UINT32 RVA_COFFIN_PHYSICS_ATTENUATION_VFUNC = 0x01F457A0u;
static const UINT32 RVA_COFFIN_PHYSICS_ATTENUATION_SLOT = 0x50u;
static const UINT32 RVA_COFFIN_CANDIDATE_FILTER_BLOCK = 0x00EDA26Du;
static const UINT32 RVA_COFFIN_STOPSTATE_5DC_A = 0x01F4FA8Cu;
static const UINT32 RVA_COFFIN_STOPSTATE_61B = 0x01F4FAB5u;
static const UINT32 RVA_COFFIN_STOPSTATE_5DC_B = 0x01F4FC1Bu;
static const UINT32 RVA_COFFIN_CANDIDATE_ACCEPT = 0x00EDA27Bu;
static const UINT32 RVA_COFFIN_CANDIDATE_REJECT = 0x00EDA28Cu;
// Coffin physics helper 1F50F10 builds three query descriptors.  The one at
// +0x100? is explicitly collision layer 0x2D (engine table: "vs Chiral Wall").
// After the dedicated query at 1F51630, a positive hit takes 1F51639 and marks
// the current Coffin sample blocked.  v1.26 changes only that two-byte branch
// so a hit from this one Chiral-Wall query is ignored; all other Coffin physics
// queries and terrain/vehicle collision paths stay native.
static const UINT32 RVA_COFFIN_CHIRAL_QUERY_DESCRIPTOR = 0x01F51029u;
static const UINT32 RVA_COFFIN_CHIRAL_QUERY_RESULT_CONTEXT = 0x01F51630u;
static const UINT32 RVA_COFFIN_CHIRAL_QUERY_RESULT_BRANCH = 0x01F51637u;
// RTTI-confirmed DSVehicleCollisionListener. Slot +0x08 is the main vehicle
// collision callback. The native callback resolves the collision object via
// 1F7A180 and reads collision tag (object+0x50)&0x7F. The global tag table maps
// 0x16 = "Chiral_Wall", 0x1B = "Vehicle_stopper", and 0x4E =
// "Vehicle_stopper_without_coffinboard". Vanilla already returns 0 for tag
// 0x4E when vehicle type +0x490 == 3. v1.46 extends that exact Coffin-only
// exemption to tags 0x16 and 0x1B; every other collision stays native.
static const UINT32 RVA_VEHICLE_COLLISION_LISTENER_VTABLE = 0x033AE850u;
static const UINT32 RVA_VEHICLE_COLLISION_LISTENER_CALLBACK_SLOT = 0x08u;
static const UINT32 RVA_VEHICLE_COLLISION_LISTENER_CALLBACK = 0x01F7B2F0u;
static const UINT32 RVA_VEHICLE_COLLISION_RESOLVE_OBJECT = 0x01F7A180u;
static const UINT32 RVA_VEHICLE_COLLISION_NO_COFFIN_GATE = 0x01F7BB59u;
static const SIZE_T OFF_VEHICLE_COLLISION_LISTENER_VEHICLE = 0x18u;
static const SIZE_T OFF_VEHICLE_TYPE = 0x490u;
static const SIZE_T OFF_VEHICLE_STATE_FLAGS_CE4 = 0xCE4u;
static const SIZE_T OFF_VEHICLE_STATE_FLAGS_4E8 = 0x4E8u;
static const SIZE_T OFF_VEHICLE_STATE_FLAGS_4EC = 0x4ECu;
static const SIZE_T OFF_COLLISION_OBJECT_LAYER = 0x50u;
static const UINT32 VEHICLE_TYPE_COFFIN_BOARD = 3u;
static const UINT32 RVA_COFFIN_VFUNC_0 = 0x01F430B0u;
static const UINT32 RVA_COFFIN_VFUNC_1 = 0x01F4E160u;
static const UINT32 RVA_COFFIN_VFUNC_2 = 0x01F480A0u;
static const UINT32 RVA_COFFIN_VFUNC_3 = 0x01F49350u;
static const SIZE_T OFF_STREAMING_GROUP_LOCK = 0xA8u;
static const SIZE_T OFF_STREAMING_GROUP_COUNT = 0x150288u;
static const SIZE_T OFF_STREAMING_GROUP_TABLE = 0x150290u;
static const SIZE_T STREAMING_GROUP_ENTRY_STRIDE = 0x21u;
static const SIZE_T OFF_LOADED_GROUP_OBJECTS = 0x20u;

static HMODULE g_module = 0;
static HMODULE g_executable = 0;
static HANDLE g_mutex = 0;
static WCHAR g_iniPath[512];
static WCHAR g_logPath[512];
static int g_enabled = 1;
static int g_speedPercent = 500;
static int g_accelerationPercent = 400;
static int g_driveForcePercent = 2000;
static int g_gearRatioPercent = 20;
static int g_allowFloatingCarrier = 1;
static int g_enableNetworkTraversal = 0;
static int g_networkTraversalRequested = 0;
static int g_outsideNetworkMountRequested = 0;
static int g_enableOutsideNetworkMount = 0;
typedef BYTE (__fastcall* CoffinSpatialGateFn)(void*, UINT32, UINT32);
static CoffinSpatialGateFn g_nativeCoffinSpatialGate = 0;
static volatile LONG g_spatialGateOverrideCount = 0;
static int g_progressiveDecayPatchInstalled = 0;
static int g_spatialGatePatchInstalled = 0;
static int g_coffinPhysicsAttenuationHookInstalled = 0;
static int g_coffinCandidateFilterHookInstalled = 0;
static volatile LONG g_coffinCandidateFilterOverrideCount = 0;
static volatile LONG g_coffinCandidateFilterHitCount = 0;
static volatile LONG g_coffinCandidateSeenCount = 0;
static volatile LONG g_nonCoffinCandidateLogCount = 0;
static int g_coffinDirectStopStatePatchInstalled = 0;
typedef void (__fastcall* CoffinPhysicsAttenuationFn)(void*, float);
static CoffinPhysicsAttenuationFn g_nativeCoffinPhysicsAttenuation = 0;
static int g_enableNetworkTrace = 0;
static int g_enableMountCallerTrace = 0;
static int g_enableNetworkStateTrace = 0;
static int g_enableQpidBorderEscapeClamp = 0;
static int g_enableCoffinE3Descriptor = 1;
static int g_enableNativeE3Producer = 0;
static int g_enableCoffinCandidateCursorTrace = 0;
static int g_enableCoffinCandidateInsertTrace = 0;
static int g_enableCoffinCacheInsertTrace = 0;
static int g_testClearActivity5D0Bit2 = 0;
static int g_testHoldRideScalar = 0;
static int g_enableActivity5D0XrefScan = 1;
static volatile UINT64 g_lastCoffinActivity = 0;
static volatile LONG g_activity5D0MaskEvents = 0;
static UINT64 g_callbackGroups = 0;
static UINT64 g_callbackObjects = 0;
static UINT64 g_rttiLookups = 0;
static UINT64 g_targetChecks = 0;
static volatile LONG g_listenerRegistered = 0;
static void* g_streamingSystem = 0;
static void (__fastcall* g_removeStreamingListener)(void*, void*) = 0;
static volatile LONG g_complete = 0;
static int g_physicsReady = 0;
// The serialized config group is not a direct StreamingSystem load on the
// live player path. The two exact Coffin state entries below own the live
// pointer and apply the same value without extending listener lifetime.
static int g_qpidBorderConfigReady = 1;
static float g_waterBaseline = 0.0f;
static float g_waterBoostBaseline = 0.0f;
static float g_lastAppliedWater = 0.0f;
static float g_lastAppliedWaterBoost = 0.0f;
static float g_gearRatioBaseline = 0.0f;
static float g_lastAppliedGearRatio = 0.0f;
static void* g_seenPhysicsResource = 0;
static void* g_seenQpidBorderConfig = 0;
static int g_driveHookInstalled = 0;
static int g_qpidEscapeStateHooksInstalled = 0;
typedef void (__fastcall* CoffinEndStateTickFn)(void*, float);
static CoffinEndStateTickFn g_nativeCoffinEndStateTick = 0;
static volatile LONG g_coffinEndStateTraceCalls = 0;
static int g_coffinEndStateTraceInstalled = 0;
static volatile LONG* g_driveFactorBits = 0;
static int g_carrierGateInstalled = 0;
static int g_carrierLinkPreserveInstalled = 0;
static int g_carrierDetachGuardInstalled = 0;
static int g_carrierWarningGuardInstalled = 0;
static int g_v176RideVehicleRootInstalled = 0;
static int g_v177RideVehicleCancelScopeInstalled = 0;
static volatile LONG g_v177RideVehicleCancelBypasses = 0;
static volatile LONG g_v178RideVehicleCancelCalls = 0;
static int g_v178RideVehicleStartObserverInstalled = 0; // dead in v1.79
static volatile LONG g_v178RideVehicleStartCalls = 0;
typedef void (__fastcall* V177PluginDisableFn)(void*);
static V177PluginDisableFn g_nativeV177PluginDisable = 0;
typedef UINT64 (__fastcall* V178RideVehicleStartFn)(void*);
static V178RideVehicleStartFn g_nativeV178RideVehicleStart = 0;
static int g_v179RideVehicleBuilderObserverInstalled = 0;
static volatile LONG g_v179RideVehicleBuilderLogs = 0;
static volatile LONG g_v181RideVehicleIdleLogs = 0;
static volatile LONG g_v182BuilderCalls = 0;
static volatile LONG g_v182BuilderLogs = 0;
static volatile UINT64 g_v182BuilderSignature = ~0ull;
typedef void (__fastcall* V179RideVehicleBuilderFn)(void*, int, UINT32*);
static V179RideVehicleBuilderFn g_nativeV179RideVehicleBuilder = 0;
#ifdef COFFIN_AB_DIAGNOSTIC
static volatile UINT64 g_diagCandidateVehicle = 0;
static volatile LONG g_diagBuilderInside = 0;
static volatile LONG g_diagBuilderOutside = 0;
#endif
static int g_networkResolverFallbackInstalled = 0;
static int g_networkC8FallbackInstalled = 0;
static int g_coffinNetworkStatePostProducerInstalled = 0;
static int g_coffinNetworkStateBitsetRefreshInstalled = 0;
static int g_coffinNetworkActionBitsetRefreshInstalled = 0;
static int g_coffinE3RecordProducerScopeInstalled = 0;
static int g_networkMountGateInstalled = 0;
static int g_networkDriveEndGuardsInstalled = 0;
static int g_coffinE3DescriptorInstalled = 0;
static int g_networkTraceInstalled = 0;
static int g_coffinChiralQueryBypassInstalled = 0;
typedef BYTE (__fastcall* CoffinC8Fn)(void*, UINT32, void*);
static CoffinC8Fn g_nativeCoffinC8 = 0;
static volatile LONG g_coffinC8PayloadValid = 0;
static void* g_coffinC8PayloadPlugin = 0;
static BYTE g_coffinC8Payload[0x64] = {};
static volatile LONG g_coffinC8PayloadCaptures = 0;
static volatile LONG g_coffinC8PayloadReplays = 0;
static volatile LONG g_coffinC8BridgeMisses = 0;
static int g_coffinE3NetworkPredicateBypassInstalled = 0;
static int g_coffinE3AvailabilityWrapperInstalled = 0;
static int g_coffinMountEntryTraceInstalled = 0;
static int g_coffinE3NetworkPredicateFailureStage = 0;
static DWORD g_coffinE3NetworkPredicateSystemError = 0;
static UINT64 g_coffinE3RuntimeMountEntry = 0;
static UINT64 g_coffinE3RuntimePredicateEntry = 0;
static BYTE g_coffinE3RuntimeBranchBytes[9] = {};
static volatile LONG g_networkTraceE3Calls = 0;
static volatile LONG g_networkTraceE3NativeSuccess = 0;
static volatile LONG g_networkTraceE3NativeFalse = 0;
static volatile LONG g_networkTraceFalseFlags = 0;
static volatile LONG g_networkTraceFirstLogs = 0;
static volatile LONG g_coffinE3AvailabilityCalls = 0;
static volatile LONG g_coffinE3AvailabilityForced = 0;
static volatile LONG g_coffinE3QueryCalls = 0;
static volatile LONG g_coffinE3RecordProducerCalls = 0;
static volatile LONG g_v155ProviderTraceLogs = 0;
static volatile LONG g_v155ProducerEntryLogs = 0;
static volatile LONG g_v155ProfileTraceLogs = 0;
static volatile UINT64 g_v155ProviderLastSignature = 0xFFFFFFFFFFFFFFFFull;
static volatile UINT64 g_v155ProducerLastSignature = 0xFFFFFFFFFFFFFFFFull;
static volatile UINT64 g_v155ProfileLastSignature = 0xFFFFFFFFFFFFFFFFull;
static volatile UINT64 g_v155Gate118Action = 0;
static volatile LONG g_v155Gate118Result = -1;
static volatile UINT64 g_v151ValidatedCoffinAction = 0;
static volatile UINT64 g_v151ValidatedCoffinManager = 0;
static volatile UINT64 g_v151ValidatedCoffinCandidate = 0;
static volatile UINT32 g_v151ValidatedNativeD4 = 0;
static volatile UINT32 g_v151ValidatedNativeManagerFlags = 0;
static volatile LONG g_coffinCandidateRegistrationCalls = 0;
static volatile LONG g_coffinCandidateInsertTraceCalls = 0;
static volatile UINT64 g_coffinCandidateMessageContainer = 0;
static volatile UINT64 g_lastNativeE3SelectedManager = 0;
static volatile UINT64 g_lastNativeE3SelectedCandidate = 0;
static volatile LONG g_lastNativeE3SelectedSequence = -1000000;
static volatile LONG g_nativeE3SelectedCaptures = 0;
static volatile LONG g_nativeE3SelectedRestores = 0;
static volatile LONG g_rideScalarHoldEvents = 0;
static volatile LONG g_rideUpdateScopeEvents = 0;
static int g_rideUpdateCallScopeInstalled = 0;
typedef void (__fastcall* CoffinRideUpdateFn)(void*, float, char);
static CoffinRideUpdateFn g_nativeCoffinRideUpdate = 0;
// ED7920 clears manager+0x230 before selecting from the live network-object
// array.  Keep only the last object the game itself selected for this same
// manager.  This is a cache of a native object, never an allocation or a
// fabricated object graph; it is used only while the native list is empty.
[[maybe_unused]] static volatile LONG g_coffinE3RegistrationInProgress = 0;
static volatile LONG g_coffinE3RegistrationLogCalls = 0;
static volatile LONG g_coffinMountEntryCalls = 0;
static volatile LONG g_coffinActionDispatchCalls = 0;
static int g_coffinActionDispatchProbeInstalled = 0;
static int g_coffinCandidateRegistrationTraceInstalled = 0;
static int g_coffinCandidateInsertTraceInstalled = 0;
static int g_coffinEventDispatchTraceInstalled = 0;
static int g_coffinCacheInsertTraceInstalled = 0;
static int g_coffinBaggageD4PersistInstalled = 0;
static volatile LONG g_coffinBaggageD4PersistCalls = 0;
static volatile LONG g_coffinBaggageD4PersistWrites = 0;
// v1.71: bounded revival of dev16's proven persistent D4 mount eligibility.
// The transaction is armed only for one RTTI/signature-confirmed DSBaggageInfo
// after a real boundary/QPID hit (or a real native descriptor 0xB fallback),
// persists through native refreshes, and is restored when RideFloater DriveEntry
// proves the mount completed. No manager selection, target or ride state is fabricated.
static volatile LONG g_v171MountTxnActive = 0;
static volatile UINT64 g_v171MountTxnCandidate = 0;
static volatile UINT64 g_v171MountTxnManager = 0;
static volatile UINT32 g_v171MountTxnOriginalD4 = 0;
static volatile LONG g_v171MountTxnRefreshes = 0;
static volatile LONG g_v171MountTxnArms = 0;
static volatile LONG g_v171MountTxnRestores = 0;
static volatile LONG g_v171MountTxnLogs = 0;
static int g_v171DriveEntryRestoreInstalled = 0;
static const LONG V171_MOUNT_TXN_REFRESH_LIMIT = 8192;
static void* g_coffinBaggageInfoVtableAddress = 0;
typedef void (__fastcall* CoffinBaggageRefreshFn)(void*, void*);
static CoffinBaggageRefreshFn g_nativeCoffinBaggageRefresh = 0;
// v1.40-dev19 native bit20 handler gate test.
typedef void (__fastcall* CoffinBit20EventHandlerFn)(void*, void*);
static CoffinBit20EventHandlerFn g_nativeCoffinBit20EventHandler = 0;
static int g_coffinBit20EventHandlerProbeInstalled = 0;
static volatile LONG g_coffinBit20EventHandlerCalls = 0;
static volatile LONG g_coffinEventDispatchCalls = 0;
static volatile LONG g_coffinCacheResetCalls = 0;
static volatile LONG g_coffinCacheAdmitCalls = 0;
static volatile LONG g_coffinCacheInsertCalls = 0;
static volatile LONG g_coffinCacheExplicitInsertCalls = 0;
static volatile LONG g_coffinCollisionFilterCalls = 0;
static volatile LONG g_coffinChiralWallBypassCalls = 0;
static int g_coffinChiralWallBypassInstalled = 0;
static volatile LONG g_mainVehicleCollisionCalls = 0;
static volatile LONG g_mainChiralCollisionBypasses = 0;
static int g_mainVehicleCollisionBypassInstalled = 0;
static volatile LONG g_coffinDirectStateCalls[7] = {};
static UINT32 g_coffinDirectStateLastBB[7] = {0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu};
static UINT32 g_coffinDirectStateLastBC[7] = {0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu};
static UINT32 g_coffinDirectStateLastType[7] = {0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu};
static UINT32 g_coffinDirectStateLastId[7] = {0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu};
static UINT32 g_coffinDirectStateLastState[7] = {0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu};
static volatile LONG g_coffinBorderSignalCalls = 0;
static volatile LONG g_coffinMountSignalCalls = 0;
static volatile LONG g_coffin4CCandidateCheckCalls = 0;
typedef void* (__fastcall *Coffin4CCandidateCheckFn)(void*, void*, void*, UINT64);
static Coffin4CCandidateCheckFn g_nativeCoffin4CCandidateCheck = 0;
static int g_coffin4CCandidateCheckInstalled = 0;
static int g_coffinBorderSignalTraceInstalled = 0;
static int g_coffinMountSignalTraceInstalled = 0;
static int g_coffinDirectStateTraceInstalled = 0;
static volatile LONG g_coffinSpatialListTraceCalls = 0;
static volatile LONG g_coffinSpatialLastCount = -1;
static volatile UINT64 g_coffinSpatialLastRoot = 0;
static int g_coffinSpatialListTraceInstalled = 0;
static volatile LONG g_coffinTorqueTraceCalls = 0;
static float g_coffinTorqueLast = 0.0f;
static float g_coffinTorqueLastX6 = -9999.0f;
static float g_coffinTorqueLastX12 = -9999.0f;
static int g_coffinTorqueLastSourceIndex = -9999;
static int g_coffinTorqueTraceInstalled = 0;
// v1.34: passive snapshot of the live Coffin physics instance.  On a positive
// XMM12 downshift we diff the preceding frame against the current frame so the
// producer-side state change can be identified without forcing gear/torque.
static BYTE g_coffinPhysicsSnapshot[0x1000] = {};
static volatile UINT64 g_coffinPhysicsSnapshotObject = 0;
static int g_coffinPhysicsSnapshotValid = 0;
static float g_coffinPhysicsSnapshotLastX12 = -9999.0f;
static volatile LONG g_coffinPhysicsGearDropEvents = 0;
// v1.34: first real DSBaggageInfo admitted by the native pad cache.  This is
// telemetry only; the candidate is never inserted/replayed by the mod.
static volatile UINT64 g_nativeBaggageCandidate = 0;
static volatile UINT64 g_nativeBaggageCache = 0;
static volatile LONG g_nativeBaggageEpochArmed = 1;
static volatile LONG g_nativeBaggageResetLogs = 0;

// v1.60: native-lifecycle Outside-Mount discovery recovery.
// No synthetic candidate is fabricated. We remember only a Coffin candidate
// that DS2 itself resolved from EDB900, and while its physics entry has the
// outside-owner marker (+0x98 != 0) we keep the native DSBaggageInfo mount
// eligibility bit20 alive so ED7920 can derive manager+0x2F0 bit1 itself.
static volatile UINT64 g_v159ResolvedCoffinCandidate = 0;
static volatile UINT64 g_v159ResolvedCoffinEntry = 0;
static volatile UINT64 g_v159ForcedCoffinCandidate = 0;
static volatile UINT64 g_v159ForcedCoffinEntry = 0;
static volatile UINT32 g_v159OriginalCandidateD4 = 0;
static volatile LONG g_v159D4GateLogs = 0;
static volatile LONG g_v159QueryFallbackLogs = 0;
static volatile LONG g_v160ManualRecoveryLogs = 0;
// v1.61 direct engine-object recovery.  The vehicle pointer comes from the
// same action->vehicle resolver called natively by 1001BD0.  The candidate is
// then derived from that vehicle's real DSBaggageComponent (+0x58 info block).
static volatile UINT64 g_v161RideCoffinVehicle = 0;
static volatile UINT64 g_v161RideCoffinHandle = 0xFFFFFFFFFFFFFFFFull;
static volatile UINT64 g_v161LatestBaggageCache = 0;
static volatile DWORD g_v161LatestBaggageCacheThread = 0;
static volatile UINT64 g_v161DirectGateCandidate = 0;
static volatile UINT32 g_v161DirectGateOriginalD4 = 0;
static volatile LONG g_v161RideCaptureLogs = 0;
static volatile LONG g_v162RideProbeLogs = 0;
static volatile LONG g_v161DirectCandidateLogs = 0;
static volatile LONG g_v161DirectReinsertLogs = 0;
static volatile LONG g_v161DirectGateLogs = 0;
// v1.67 immediate Coffin-entity/cache path.
typedef void (__fastcall* V167CoffinUpdateFn)(void*);
typedef void (__fastcall* V167Edb900Fn)(void*, double*, UINT64, char);
typedef void* (__fastcall* V169ComponentLookupFn)(void*, const short*);
static V167CoffinUpdateFn g_nativeV167CoffinUpdate = 0;
static V167Edb900Fn g_nativeV167Edb900 = 0;
static V169ComponentLookupFn g_nativeV169ComponentLookup = 0;
static volatile UINT64 g_v167CoffinVehicle = 0;
static volatile LONG g_v167VehicleCaptureLogs = 0;
static volatile LONG g_v167DirectAdmitLogs = 0;
static volatile LONG g_v167DirectAdmitSuccesses = 0;
static volatile LONG g_v167CoffinUpdateInstalled = 0;
static volatile LONG g_v167Edb900Installed = 0;
static volatile LONG g_v169ComponentLookupLogs = 0;
static volatile LONG g_v151QpidHits = 0;
// v1.32: exact live snapshot of the Chiral/world manager cluster that contains
// the native RideFloater BB/BC state. Previous diagnostics accidentally read
// the resource StreamingManager global, so all old BB/BC=0 observations are invalid.
static BYTE g_chiralClusterLast[0x50] = {0};
static volatile LONG g_chiralClusterInitialized = 0;
static volatile LONG g_chiralClusterLock = 0;
static volatile LONG g_chiralClusterEvents = 0;
// Native mount target captured only after a real successful Coffin mount.
// It is process-local and reused only for the same player context.
static volatile UINT64 g_nativeMountTarget = 0;
static volatile UINT64 g_nativeMountPlayerContext = 0;
static volatile LONG g_nativeMountTargetCaptures = 0;
typedef BYTE (__fastcall* V163InteractionGateFn)(void*);
static V163InteractionGateFn g_nativeV163InteractionGate = 0;
static volatile LONG g_v163InteractionGateCalls = 0;
static volatile LONG g_v163InteractionGateBypasses = 0;
static int g_v163InteractionGateInstalled = 0;

// v1.64: sticky DS2-native Coffin candidate.
// The pointer is accepted only after the game's own DSBaggageInfo cache admits it.
// We also snapshot the two native 3-double vectors passed to EE5B60 so a later
// re-admission uses the exact same native method and native spatial inputs.
static volatile UINT64 g_v164StickyCandidate = 0;
static volatile UINT64 g_v164StickyCache = 0;
static volatile UINT64 g_v164FirstVecBits[3] = {0,0,0};
static volatile UINT64 g_v164SecondVecBits[3] = {0,0,0};
static volatile LONG g_v164StickyCaptureLogs = 0;
static volatile LONG g_v164StickyReinsertLogs = 0;
static volatile LONG g_v164StickyReinsertSuccesses = 0;
static int g_v164ProviderInstalled = 0;
static int g_coffinEndStateBorderCorrectionBypassInstalled = 0;
static volatile UINT64 g_lastCoffinE3UnavailableAction = 0;
static volatile LONG g_coffinDirectMountFallbackCalls = 0;
static UINT32 g_actionDispatchCountScope = 0;
static UINT64 g_actionDispatchCallerScope = 0;
struct CarrierRideLinkState {
    volatile UINT64 carrierEntity;
    volatile UINT64 playerState;
    volatile UINT64 coffinVehicleHandle;
    volatile UINT64 carrierEntityHandle;
    volatile LONG followUpdateCalls;
    volatile LONG publishedCarrierCalls;
    volatile LONG activeCoffinCalls;
    volatile LONG scaledFollowCalls;
    volatile UINT64 lastComponentHandle;
};
__declspec(align(16)) static CarrierRideLinkState g_carrierRideLinkState = {};
static volatile LONG* g_carrierWarningProducerHits = 0;
static volatile LONG* g_carrierWarningSuppressedHits = 0;
static UINT64 g_logLock = 0;
static UINT64 g_targetStateLock = 0;
static HANDLE g_suspendedThreadHandles[512];
static DWORD g_suspendedThreadIds[512];
static UINT32 g_suspendedThreadCount = 0;
__declspec(align(16)) static BYTE g_threadContext[0x4D0];

static float f_abs(float value) { return value < 0.0f ? -value : value; }
static bool f_near(float value, float expected, float tolerance) {
    return f_abs(value - expected) <= tolerance;
}
static float f_max(float a, float b) { return a > b ? a : b; }
static bool patch_complete_acquire() {
    return __atomic_load_n(&g_complete, __ATOMIC_ACQUIRE) != 0;
}
static void publish_patch_complete(bool complete) {
    __atomic_store_n(&g_complete, complete ? 1 : 0, __ATOMIC_RELEASE);
}
static LONG listener_state_acquire() {
    return __atomic_load_n(&g_listenerRegistered, __ATOMIC_ACQUIRE);
}
static void wide_copy(WCHAR* destination, int capacity, const WCHAR* source) {
    if (!destination || capacity <= 0) return;
    int index = 0;
    while (source && source[index] && index < capacity - 1) {
        destination[index] = source[index];
        ++index;
    }
    destination[index] = 0;
}
static WCHAR wide_lower_ascii(WCHAR value) {
    return value >= L'A' && value <= L'Z' ? (WCHAR)(value + (L'a' - L'A')) : value;
}
static bool wide_equal_ignore_case_ascii(const WCHAR* a, const WCHAR* b) {
    if (!a || !b) return a == b;
    while (*a && *b) {
        if (wide_lower_ascii(*a) != wide_lower_ascii(*b)) return false;
        ++a; ++b;
    }
    return *a == 0 && *b == 0;
}
static bool is_main_game_process() {
    WCHAR path[512];
    DWORD length = GetModuleFileNameW(0, path, 511);
    if (length == 0 || length >= 511) return false;
    path[length] = 0;
    const WCHAR* baseName = path;
    for (DWORD i = 0; i < length; ++i) {
        if (path[i] == L'\\' || path[i] == L'/') baseName = &path[i + 1];
    }
    return wide_equal_ignore_case_ascii(baseName, L"DS2.exe");
}
struct LogBuffer { char data[2048]; int length; };
static void log_init(LogBuffer* buffer) { buffer->length = 0; }
static void log_char(LogBuffer* buffer, char value) {
    if (buffer->length < 2047) buffer->data[buffer->length++] = value;
}
static void log_text(LogBuffer* buffer, const char* text) {
    if (!text) return;
    while (*text && buffer->length < 2047) buffer->data[buffer->length++] = *text++;
}
static void log_uint(LogBuffer* buffer, UINT64 value) {
    char temporary[32]; int count = 0;
    if (value == 0) { log_char(buffer, '0'); return; }
    while (value && count < 31) {
        temporary[count++] = (char)('0' + value % 10); value /= 10;
    }
    while (count) log_char(buffer, temporary[--count]);
}
static void log_float2(LogBuffer* buffer, float value) {
    if (value < 0.0f) { log_char(buffer, '-'); value = -value; }
    UINT64 whole = (UINT64)value;
    UINT32 fraction = (UINT32)((value - (float)whole) * 100.0f + 0.5f);
    if (fraction >= 100) { ++whole; fraction -= 100; }
    log_uint(buffer, whole); log_char(buffer, '.');
    log_char(buffer, (char)('0' + (fraction / 10) % 10));
    log_char(buffer, (char)('0' + fraction % 10));
}
static void log_prefix(LogBuffer* buffer) {
    SYSTEMTIME_X time; GetLocalTime(&time);
    log_char(buffer, '['); log_uint(buffer, time.wYear); log_char(buffer, '-');
    if (time.wMonth < 10) log_char(buffer, '0'); log_uint(buffer, time.wMonth); log_char(buffer, '-');
    if (time.wDay < 10) log_char(buffer, '0'); log_uint(buffer, time.wDay); log_char(buffer, ' ');
    if (time.wHour < 10) log_char(buffer, '0'); log_uint(buffer, time.wHour); log_char(buffer, ':');
    if (time.wMinute < 10) log_char(buffer, '0'); log_uint(buffer, time.wMinute); log_char(buffer, ':');
    if (time.wSecond < 10) log_char(buffer, '0'); log_uint(buffer, time.wSecond); log_text(buffer, "] ");
}
static void append_log(LogBuffer* buffer) {
    AcquireSRWLockExclusive(&g_logLock);
    HANDLE file = CreateFileW(g_logPath, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
                              0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (!file || file == INVALID_HANDLE_VALUE) {
        ReleaseSRWLockExclusive(&g_logLock); return;
    }
    SetFilePointer(file, 0, 0, FILE_END);
    DWORD written = 0; WriteFile(file, buffer->data, (DWORD)buffer->length, &written, 0);
    CloseHandle(file); ReleaseSRWLockExclusive(&g_logLock);
}
static void log_line(const char* text) {
    LogBuffer buffer; log_init(&buffer); log_prefix(&buffer); log_text(&buffer, text);
    log_text(&buffer, "\r\n"); append_log(&buffer);
}
static void make_sibling_path(WCHAR* destination, int capacity, const WCHAR* fileName) {
    WCHAR modulePath[512]; DWORD length = GetModuleFileNameW(g_module, modulePath, 511);
    if (length == 0 || length >= 511) { wide_copy(destination, capacity, fileName); return; }
    modulePath[length] = 0; int separator = -1;
    for (int i = 0; modulePath[i]; ++i) if (modulePath[i] == L'\\' || modulePath[i] == L'/') separator = i;
    int output = 0;
    for (int i = 0; i <= separator && output < capacity - 1; ++i) destination[output++] = modulePath[i];
    for (int i = 0; fileName[i] && output < capacity - 1; ++i) destination[output++] = fileName[i];
    destination[output] = 0;
}
static int clamp_speed(int value) { if (value < 100) return 100; if (value > 1000) return 1000; return value; }
static int clamp_accel(int value) { if (value < 100) return 100; if (value > 500) return 500; return value; }
static void read_configuration() {
    make_sibling_path(g_iniPath, 512, L"ds2_coffin_board_all_terrain_speed.ini");
    make_sibling_path(g_logPath, 512, L"ds2_coffin_board_all_terrain_speed.log");
    g_enabled = (int)GetPrivateProfileIntW(L"CoffinBoardAllTerrainSpeed", L"Enabled", 1, g_iniPath);
    g_speedPercent = clamp_speed((int)GetPrivateProfileIntW(L"CoffinBoardAllTerrainSpeed", L"SpeedPercent", 500, g_iniPath));
    g_accelerationPercent = clamp_accel((int)GetPrivateProfileIntW(L"CoffinBoardAllTerrainSpeed", L"AccelerationPercent", 400, g_iniPath));
    g_allowFloatingCarrier = (int)GetPrivateProfileIntW(L"CoffinBoardAllTerrainSpeed", L"AllowFloatingCarrier", 1, g_iniPath) != 0;
    g_networkTraversalRequested = (int)GetPrivateProfileIntW(L"CoffinBoardAllTerrainSpeed", L"EnableNetworkTraversal", 1, g_iniPath) != 0;
    g_outsideNetworkMountRequested = (int)GetPrivateProfileIntW(L"CoffinBoardAllTerrainSpeed", L"EnableOutsideNetworkMount", 1, g_iniPath) != 0;
    // Public switches for the v1.50 statically closed crossing/mount roots.
    g_enableNetworkTraversal = g_networkTraversalRequested;
    g_enableOutsideNetworkMount = g_outsideNetworkMountRequested;
    g_enableNetworkTrace = (int)GetPrivateProfileIntW(L"CoffinBoardAllTerrainSpeed", L"EnableNetworkC8Trace", 0, g_iniPath) != 0;
    g_enableMountCallerTrace = (int)GetPrivateProfileIntW(L"CoffinBoardAllTerrainSpeed", L"EnableMountCallerTrace", 0, g_iniPath) != 0;
    g_enableNetworkStateTrace = (int)GetPrivateProfileIntW(L"CoffinBoardAllTerrainSpeed", L"EnableNetworkStateTrace", 0, g_iniPath) != 0;
    g_enableQpidBorderEscapeClamp = (int)GetPrivateProfileIntW(L"CoffinBoardAllTerrainSpeed", L"EnableQpidBorderEscapeClamp", 0, g_iniPath) != 0;
    g_enableCoffinE3Descriptor = (int)GetPrivateProfileIntW(L"CoffinBoardAllTerrainSpeed", L"EnableCoffinE3Descriptor", 1, g_iniPath) != 0;
    g_enableNativeE3Producer = (int)GetPrivateProfileIntW(L"CoffinBoardAllTerrainSpeed", L"EnableNativeE3Producer", 0, g_iniPath) != 0;
    g_enableCoffinCandidateCursorTrace = (int)GetPrivateProfileIntW(L"CoffinBoardAllTerrainSpeed", L"EnableCoffinCandidateCursorTrace", 0, g_iniPath) != 0;
    g_enableCoffinCandidateInsertTrace = (int)GetPrivateProfileIntW(L"CoffinBoardAllTerrainSpeed", L"EnableCoffinCandidateInsertTrace", 0, g_iniPath) != 0;
    g_enableCoffinCacheInsertTrace = (int)GetPrivateProfileIntW(L"CoffinBoardAllTerrainSpeed", L"EnableCoffinCacheInsertTrace", 0, g_iniPath) != 0;
    g_testClearActivity5D0Bit2 = (int)GetPrivateProfileIntW(L"CoffinBoardAllTerrainSpeed", L"TestClearActivity5D0Bit2", 0, g_iniPath) != 0;
    g_testHoldRideScalar = (int)GetPrivateProfileIntW(L"CoffinBoardAllTerrainSpeed", L"TestHoldRideScalar", 0, g_iniPath) != 0;
    g_enableActivity5D0XrefScan = (int)GetPrivateProfileIntW(L"CoffinBoardAllTerrainSpeed", L"EnableActivity5D0XrefScan", 1, g_iniPath) != 0;

    // v1.50 production defaults: only the statically closed root fixes are
    // controlled by the two public INI switches. Legacy diagnostic/experimental
    // paths remain off and are never installed by the active worker.
    g_enableNetworkTrace = 0;
    g_enableMountCallerTrace = 0;
    g_enableNetworkStateTrace = 0;
    g_enableQpidBorderEscapeClamp = 0;
    g_enableCoffinE3Descriptor = 0;
    g_enableNativeE3Producer = 0;
    g_enableCoffinCandidateCursorTrace = 0;
    g_enableCoffinCandidateInsertTrace = 0;
    g_enableCoffinCacheInsertTrace = 0;
    g_testClearActivity5D0Bit2 = 0;
    g_testHoldRideScalar = 0;
    g_enableActivity5D0XrefScan = 0;
    UINT64 derived = (UINT64)g_speedPercent * (UINT64)g_accelerationPercent / 100u;
    if (derived < 100u) derived = 100u; if (derived > 5000u) derived = 5000u;
    g_driveForcePercent = (int)derived;
    g_gearRatioPercent = 10000 / g_speedPercent;
    if (g_gearRatioPercent < 10) g_gearRatioPercent = 10;
    if (g_gearRatioPercent > 100) g_gearRatioPercent = 100;
}
static bool readable_range(const void* address, SIZE_T bytes) {
    if (!address || bytes == 0) return false;
    MEMORY_BASIC_INFORMATION_X64 information;
    if (!VirtualQuery(address, &information, sizeof(information))) return false;
    if (information.State != MEM_COMMIT || (information.Protect & PAGE_GUARD) ||
        (information.Protect & PAGE_NOACCESS)) return false;
    UINT64 regionStart = (UINT64)information.BaseAddress;
    UINT64 regionEnd = regionStart + (UINT64)information.RegionSize;
    UINT64 requestedStart = (UINT64)address;
    UINT64 requestedEnd = requestedStart + (UINT64)bytes;
    return requestedStart >= regionStart && requestedEnd >= requestedStart && requestedEnd <= regionEnd;
}
static bool writable_range(const void* address, SIZE_T bytes) {
    if (!readable_range(address, bytes)) return false;
    MEMORY_BASIC_INFORMATION_X64 information;
    if (!VirtualQuery(address, &information, sizeof(information))) return false;
    DWORD protection = information.Protect & 0xFFu;
    return protection == PAGE_READWRITE || protection == PAGE_WRITECOPY ||
           protection == PAGE_EXECUTE_READWRITE || protection == PAGE_EXECUTE_WRITECOPY;
}
static int write_float_checked(float* field, float target) {
    if (!field || !readable_range(field, sizeof(float))) return 0;
    if (f_near(*field, target, 0.01f)) return 1;
    DWORD oldProtection = 0; int changedProtection = FALSE;
    if (!writable_range(field, sizeof(float))) {
        changedProtection = VirtualProtect(field, sizeof(float), PAGE_READWRITE, &oldProtection);
        if (!changedProtection) return 0;
    }
    *field = target; bool verified = f_near(*field, target, 0.01f);
    if (changedProtection) { DWORD ignored = 0; VirtualProtect(field, sizeof(float), oldProtection, &ignored); }
    return verified ? 2 : 0;
}
static bool value_is_native_or_compatible(float value, float nativeValue, float target) {
    return f_near(value, nativeValue, 0.05f) || f_near(value, target, 0.05f) || value > target;
}
// v1.33 diagnostic: enumerate executable-code references to displacement +0x5D0.
// This is a static in-memory xref scan only.  It does not patch any candidate.
static void scan_activity_5d0_xrefs(HMODULE executable) {
    if (!g_enableActivity5D0XrefScan || !executable) return;
    BYTE* base = (BYTE*)executable;
    if (!readable_range(base, 0x1000u)) return;
    UINT32 peOffset = *(UINT32*)(base + 0x3Cu);
    if (peOffset > 0x1000u || !readable_range(base + peOffset, 0x108u)) return;
    BYTE* nt = base + peOffset;
    if (*(UINT32*)nt != 0x00004550u) return;
    WORD sectionCount = *(WORD*)(nt + 6u);
    WORD optionalSize = *(WORD*)(nt + 20u);
    BYTE* section = nt + 24u + optionalSize;
    UINT32 total = 0;
    for (WORD si = 0; si < sectionCount && total < 96u; ++si, section += 40u) {
        if (!readable_range(section, 40u)) break;
        UINT32 virtualSize = *(UINT32*)(section + 8u);
        UINT32 virtualAddress = *(UINT32*)(section + 12u);
        UINT32 characteristics = *(UINT32*)(section + 36u);
        if ((characteristics & 0x20000000u) == 0u || virtualSize < 8u) continue;
        BYTE* start = base + virtualAddress;
        if (!readable_range(start, virtualSize)) continue;
        for (UINT32 i = 4u; i + 8u < virtualSize && total < 96u; ++i) {
            if (start[i] != 0xD0u || start[i+1] != 0x05u || start[i+2] != 0x00u || start[i+3] != 0x00u) continue;
            BYTE op = start[i-2u];
            BYTE modrm = start[i-1u];
            BYTE op0 = start[i-3u];
            bool likelyMemoryOperand = (modrm & 0xC0u) == 0x80u;
            bool likelyWrite = likelyMemoryOperand &&
                (op == 0x80u || op == 0x81u || op == 0x83u || op == 0x88u || op == 0x89u ||
                 op == 0xC6u || op == 0xC7u || (op0 == 0x0Fu && (op == 0xABu || op == 0xB3u || op == 0xBBu)));
            LogBuffer m; log_init(&m); log_prefix(&m);
            log_text(&m, "ACTIVITY5D0 XREF #"); log_uint(&m, total);
            log_text(&m, ": rva="); log_uint(&m, (UINT64)(virtualAddress + i));
            log_text(&m, ", op0="); log_uint(&m, op0);
            log_text(&m, ", op="); log_uint(&m, op);
            log_text(&m, ", modrm="); log_uint(&m, modrm);
            log_text(&m, ", likelyWrite="); log_uint(&m, likelyWrite ? 1u : 0u);
            log_text(&m, ", bytes=");
            for (int d = -4; d <= 7; ++d) {
                if (d != -4) log_text(&m, ",");
                log_uint(&m, (UINT64)start[i + d]);
            }
            log_text(&m, ". Static executable scan; code unchanged.\r\n"); append_log(&m);
            ++total;
        }
    }
    LogBuffer m; log_init(&m); log_prefix(&m);
    log_text(&m, "ACTIVITY5D0 XREF SCAN COMPLETE: candidates="); log_uint(&m, total);
    log_text(&m, ".\r\n"); append_log(&m);
}

static bool validate_target_build(HMODULE executable) {
    BYTE* base = (BYTE*)executable;
    if (!readable_range(base, 0x1000)) return false;
    UINT32 peOffset = *(UINT32*)(base + 0x3C);
    if (peOffset > 0x1000 || !readable_range(base + peOffset, 0x108)) return false;
    BYTE* nt = base + peOffset;
    if (*(UINT32*)nt != 0x00004550u) return false;
    if (*(UINT32*)(nt + 8) != EXPECTED_TIMESTAMP) return false;
    BYTE* optional = nt + 24;
    if (*(WORD*)optional != 0x020Bu) return false;
    return *(UINT32*)(optional + 56) == EXPECTED_IMAGE_SIZE;
}
static bool bytes_equal(const BYTE* left, const BYTE* right, SIZE_T size) {
    if (!left || !right) return false;
    for (SIZE_T index = 0; index < size; ++index) if (left[index] != right[index]) return false;
    return true;
}
static bool tracked_thread_id(const DWORD* ids, UINT32 count, DWORD id) {
    for (UINT32 index = 0; index < count; ++index) if (ids[index] == id) return true;
    return false;
}
static void resume_suspended_threads(HANDLE* handles, UINT32 count) {
    while (count) { HANDLE thread = handles[--count]; ResumeThread(thread); CloseHandle(thread); }
}
static bool suspend_other_threads(HANDLE* handles, UINT32* suspendedCount, const BYTE* patchTarget, SIZE_T patchSize) {
    if (!handles || !suspendedCount || !patchTarget || patchSize == 0u) return false;
    *suspendedCount = 0;
    DWORD processId = GetCurrentProcessId(); DWORD currentThreadId = GetCurrentThreadId();
    bool stable = false, unsafeInstructionPointer = false, contextFailure = false;
    for (UINT32 pass = 0; pass < 4u && !stable && !unsafeInstructionPointer && !contextFailure; ++pass) {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        if (snapshot == INVALID_HANDLE_VALUE) break;
        THREADENTRY32_X entry; memset(&entry, 0, sizeof(entry)); entry.dwSize = sizeof(entry);
        bool unresolved = false; UINT32 added = 0;
        if (Thread32First(snapshot, &entry)) {
            do {
                DWORD id = entry.th32ThreadID;
                if (entry.th32OwnerProcessID != processId || id == currentThreadId ||
                    tracked_thread_id(g_suspendedThreadIds, *suspendedCount, id)) continue;
                if (*suspendedCount >= 512u) { unresolved = true; break; }
                HANDLE thread = OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT, FALSE, id);
                if (!thread) { unresolved = true; continue; }
                if (SuspendThread(thread) == 0xFFFFFFFFu) { CloseHandle(thread); unresolved = true; continue; }
                g_suspendedThreadIds[*suspendedCount] = id; handles[*suspendedCount] = thread;
                ++*suspendedCount; ++added;
                memset(g_threadContext, 0, sizeof(g_threadContext));
                *(DWORD*)(g_threadContext + 48u) = CONTEXT_CONTROL_X64;
                if (!GetThreadContext(thread, g_threadContext)) { contextFailure = true; break; }
                UINT64 instructionPointer = *(UINT64*)(g_threadContext + 248u);
                UINT64 patchStart = (UINT64)patchTarget, patchEnd = patchStart + (UINT64)patchSize;
                if (instructionPointer >= patchStart && instructionPointer < patchEnd) { unsafeInstructionPointer = true; break; }
            } while (Thread32Next(snapshot, &entry));
        } else unresolved = true;
        CloseHandle(snapshot);
        stable = !unresolved && !unsafeInstructionPointer && !contextFailure && added == 0u;
    }
    if (!stable) { resume_suspended_threads(handles, *suspendedCount); *suspendedCount = 0; }
    return stable;
}
static bool freeze_for_patch(const BYTE* target, SIZE_T size) {
    bool threadsFrozen = false;
    for (UINT32 attempt = 0; attempt < 50u && !threadsFrozen; ++attempt) {
        g_suspendedThreadCount = 0;
        threadsFrozen = suspend_other_threads(g_suspendedThreadHandles, &g_suspendedThreadCount, target, size);
        if (!threadsFrozen) Sleep(1);
    }
    return threadsFrozen;
}
static void unfreeze_after_patch() {
    resume_suspended_threads(g_suspendedThreadHandles, g_suspendedThreadCount);
    g_suspendedThreadCount = 0;
}

// This is the first type-specific insertion boundary for the native Coffin
// candidate cache.  Both wrappers call the original cache method unchanged;
// they only record whether the game's own DSBaggageInfo cache receives an
// object and how its two-slot count changes.
typedef BYTE (__fastcall* CoffinCacheAdmitFn)(void*, void*, void*, void*);
typedef void (__fastcall* CoffinCacheInsertFn)(void*, void*, double*, double*);
typedef void (__fastcall* CoffinCacheInsertExplicitFn)(void*, int, void*, void*, UINT64, UINT32);
typedef void (__fastcall* CoffinCacheResetFn)(void*);
static CoffinCacheResetFn g_nativeCoffinCacheReset = 0;
static CoffinCacheAdmitFn g_nativeCoffinCacheAdmit = 0;
static CoffinCacheInsertFn g_nativeCoffinCacheInsert = 0;
static CoffinCacheInsertExplicitFn g_nativeCoffinCacheInsertExplicit = 0;

static void log_coffin_cache_insert(const char* kind, void* cache, void* candidate,
                                    UINT32 before, UINT32 after, UINT32 index, UINT32 type,
                                    UINT64 generation, UINT64 callerRva) {
    LogBuffer buffer;
    log_init(&buffer); log_prefix(&buffer); log_text(&buffer, "COFFIN CACHE "); log_text(&buffer, kind);
    log_text(&buffer, ": cache=0x"); log_uint(&buffer, (UINT64)cache);
    log_text(&buffer, ", candidate=0x"); log_uint(&buffer, (UINT64)candidate);
    log_text(&buffer, ", index="); log_uint(&buffer, index);
    log_text(&buffer, ", value="); log_uint(&buffer, type);
    log_text(&buffer, ", generation="); log_uint(&buffer, generation);
    log_text(&buffer, ", callerRva="); log_uint(&buffer, callerRva);
    log_text(&buffer, ", count="); log_uint(&buffer, before);
    log_text(&buffer, "->"); log_uint(&buffer, after); log_text(&buffer, "\r\n");
    append_log(&buffer);
}

static void __fastcall trace_coffin_cache_reset(void* cache) {
    UINT32 before = 0;
    if (cache && readable_range((BYTE*)cache + 0xA8u, sizeof(UINT32))) before = *(UINT32*)((BYTE*)cache + 0xA8u);
    if (g_nativeCoffinCacheReset) g_nativeCoffinCacheReset(cache);
    // v1.61: every call reaches us through the DSBaggageInfo cache vtable.
    // Remember the exact cache instance and thread so the EDB900 query scope
    // can reinsert only into the cache that was just reset on this update.
    if (cache && g_executable && readable_range(cache, sizeof(void*)) &&
        *(void**)cache == (void*)((BYTE*)g_executable + RVA_COFFIN_BAGGAGE_INFO_CACHE_VTABLE)) {
        __atomic_store_n(&g_v161LatestBaggageCache, (UINT64)cache, __ATOMIC_RELEASE);
        __atomic_store_n(&g_v161LatestBaggageCacheThread, GetCurrentThreadId(), __ATOMIC_RELEASE);
    }
    UINT32 after = before;
    if (cache && readable_range((BYTE*)cache + 0xA8u, sizeof(UINT32))) after = *(UINT32*)((BYTE*)cache + 0xA8u);
    LONG call = __atomic_add_fetch(&g_coffinCacheResetCalls, 1, __ATOMIC_RELAXED);
    LONG nonEmptyResetIndex = -1;
    if (before > 0u) nonEmptyResetIndex = __atomic_fetch_add(&g_nativeBaggageResetLogs, 1, __ATOMIC_ACQ_REL);
    if (call <= 32 || (before > 0u && nonEmptyResetIndex < 64)) {
        UINT64 caller = (UINT64)__builtin_return_address(0);
        UINT64 callerRva = g_executable && caller >= (UINT64)g_executable ? caller - (UINT64)g_executable : caller;
        LogBuffer buffer; log_init(&buffer); log_prefix(&buffer); log_text(&buffer, "COFFIN CACHE RESET: cache=0x");
        log_uint(&buffer, (UINT64)cache); log_text(&buffer, ", count="); log_uint(&buffer, before);
        log_text(&buffer, "->"); log_uint(&buffer, after); log_text(&buffer, ", callerRva=");
        log_uint(&buffer, callerRva); log_text(&buffer, "\r\n"); append_log(&buffer);
    }
}

static bool v159_is_genuine_coffin_candidate_early(void* candidate) {
    if (!candidate || !g_executable ||
        !readable_range(candidate, sizeof(void*)) ||
        !readable_range((BYTE*)candidate + 0x08u, sizeof(UINT64))) return false;
    return *(void**)candidate == (void*)((BYTE*)g_executable + RVA_COFFIN_BAGGAGE_INFO_VTABLE) &&
           *(UINT64*)((BYTE*)candidate + 0x08u) == COFFIN_BAGGAGE_Q08_SIGNATURE;
}

static UINT64 v159_entry_owner98(void* entry) {
    if (!entry || !readable_range((BYTE*)entry + 0x98u, sizeof(UINT64))) return 0u;
    return *(UINT64*)((BYTE*)entry + 0x98u);
}

static void v164_capture_native_coffin_candidate(void* cache, void* candidate,
                                                  void* firstVector, void* secondVector,
                                                  BYTE nativeResult, UINT32 cacheCountAfter) {
    if (!g_enableOutsideNetworkMount || nativeResult == 0u || cacheCountAfter == 0u ||
        !cache || !candidate || !firstVector || !secondVector ||
        !v159_is_genuine_coffin_candidate_early(candidate) ||
        !readable_range(firstVector, sizeof(double) * 3u) ||
        !readable_range(secondVector, sizeof(double) * 3u)) {
        return;
    }

    UINT64 firstBits[3] = {};
    UINT64 secondBits[3] = {};
    memcpy(firstBits, firstVector, sizeof(firstBits));
    memcpy(secondBits, secondVector, sizeof(secondBits));

    UINT64 oldCandidate = __atomic_load_n(&g_v164StickyCandidate, __ATOMIC_ACQUIRE);
    UINT64 oldCache = __atomic_load_n(&g_v164StickyCache, __ATOMIC_ACQUIRE);

    for (UINT32 i = 0; i < 3u; ++i) {
        __atomic_store_n(&g_v164FirstVecBits[i], firstBits[i], __ATOMIC_RELEASE);
        __atomic_store_n(&g_v164SecondVecBits[i], secondBits[i], __ATOMIC_RELEASE);
    }
    __atomic_store_n(&g_v164StickyCache, (UINT64)cache, __ATOMIC_RELEASE);
    // Publish candidate last so readers never observe a new pointer with old vectors.
    __atomic_store_n(&g_v164StickyCandidate, (UINT64)candidate, __ATOMIC_RELEASE);

    if (oldCandidate != (UINT64)candidate || oldCache != (UINT64)cache) {
        LONG idx = __atomic_fetch_add(&g_v164StickyCaptureLogs, 1, __ATOMIC_ACQ_REL);
        if (idx < 32) {
            LogBuffer m; log_init(&m); log_prefix(&m);
            log_text(&m, "V164 STICKY NATIVE CAPTURE #"); log_uint(&m, (UINT64)idx);
            log_text(&m, ": candidate="); log_uint(&m, (UINT64)candidate);
            log_text(&m, ", cache="); log_uint(&m, (UINT64)cache);
            log_text(&m, ", qpidHits="); log_uint(&m, (UINT64)__atomic_load_n(&g_v151QpidHits, __ATOMIC_ACQUIRE));
            log_text(&m, ", countAfter="); log_uint(&m, cacheCountAfter);
            log_text(&m, ". Candidate was accepted by native EE5B60; vectors snapshotted.\r\n");
            append_log(&m);
        }
    }
}

static bool v164_reinsert_sticky_candidate_before_provider(void* action) {
    if (!g_enableOutsideNetworkMount || !action || !g_nativeCoffinCacheAdmit || !g_executable ||
        __atomic_load_n(&g_v151QpidHits, __ATOMIC_ACQUIRE) <= 0) return false;

    BYTE* a = (BYTE*)action;
    if (!readable_range(a + OFF_COFFIN_ACTION_MANAGER, sizeof(void*))) return false;
    BYTE* manager = *(BYTE**)(a + OFF_COFFIN_ACTION_MANAGER);
    if (!manager) return false;

    BYTE* cache = manager + 0x228u;
    UINT64 stickyCache = __atomic_load_n(&g_v164StickyCache, __ATOMIC_ACQUIRE);
    BYTE* candidate = (BYTE*)__atomic_load_n(&g_v164StickyCandidate, __ATOMIC_ACQUIRE);
    if (!candidate || stickyCache != (UINT64)cache ||
        !v159_is_genuine_coffin_candidate_early(candidate) ||
        !readable_range(cache, 0xB0u) ||
        *(void**)cache != (void*)((BYTE*)g_executable + RVA_COFFIN_BAGGAGE_INFO_CACHE_VTABLE)) {
        return false;
    }

    UINT32 before = *(UINT32*)(cache + 0xA8u);
    if (before != 0u) return false;

    double first[3] = {};
    double second[3] = {};
    UINT64 firstBits[3] = {}, secondBits[3] = {};
    for (UINT32 i = 0; i < 3u; ++i) {
        firstBits[i] = __atomic_load_n(&g_v164FirstVecBits[i], __ATOMIC_ACQUIRE);
        secondBits[i] = __atomic_load_n(&g_v164SecondVecBits[i], __ATOMIC_ACQUIRE);
    }
    memcpy(first, firstBits, sizeof(first));
    memcpy(second, secondBits, sizeof(second));

    BYTE result = g_nativeCoffinCacheAdmit(cache, candidate, first, second);
    UINT32 after = readable_range(cache + 0xA8u, sizeof(UINT32)) ? *(UINT32*)(cache + 0xA8u) : 0u;
    if (result != 0u && after > 0u) {
        __atomic_fetch_add(&g_v164StickyReinsertSuccesses, 1, __ATOMIC_ACQ_REL);
    }

    LONG idx = __atomic_fetch_add(&g_v164StickyReinsertLogs, 1, __ATOMIC_ACQ_REL);
    if (idx < 64 || (result != 0u && after > 0u)) {
        LogBuffer m; log_init(&m); log_prefix(&m);
        log_text(&m, "V164 STICKY REINSERT #"); log_uint(&m, (UINT64)idx);
        log_text(&m, ": candidate="); log_uint(&m, (UINT64)candidate);
        log_text(&m, ", cache="); log_uint(&m, (UINT64)cache);
        log_text(&m, ", count="); log_uint(&m, before); log_text(&m, "->"); log_uint(&m, after);
        log_text(&m, ", nativeResult="); log_uint(&m, result);
        log_text(&m, ", qpidHits="); log_uint(&m, (UINT64)__atomic_load_n(&g_v151QpidHits, __ATOMIC_ACQUIRE));
        log_text(&m, ". Re-admitted through original EE5B60 before native 1000A70.\r\n");
        append_log(&m);
    }
    return result != 0u && after > 0u;
}

static void v159_restore_candidate_gate_if_inside_or_invalid() {
    BYTE* candidate = (BYTE*)__atomic_load_n(&g_v159ForcedCoffinCandidate, __ATOMIC_ACQUIRE);
    BYTE* entry = (BYTE*)__atomic_load_n(&g_v159ForcedCoffinEntry, __ATOMIC_ACQUIRE);
    if (!candidate) return;

    bool validCandidate = v159_is_genuine_coffin_candidate_early(candidate) &&
                          readable_range(candidate + 0xD4u, sizeof(UINT32));
    UINT64 owner98 = v159_entry_owner98(entry);
    // owner98==0 is the native inside/normal state seen in the proven trace.
    // Invalid runtime objects are simply forgotten; do not write stale memory.
    if (!validCandidate) {
        __atomic_store_n(&g_v159ForcedCoffinCandidate, 0ull, __ATOMIC_RELEASE);
        __atomic_store_n(&g_v159ForcedCoffinEntry, 0ull, __ATOMIC_RELEASE);
        return;
    }
    if (owner98 != 0u) return;

    UINT32 original = __atomic_load_n(&g_v159OriginalCandidateD4, __ATOMIC_ACQUIRE);
    UINT32 current = *(UINT32*)(candidate + 0xD4u);
    UINT32 restored = (current & ~COFFIN_BAGGAGE_D4_MOUNT_BIT) |
                      (original & COFFIN_BAGGAGE_D4_MOUNT_BIT);
    *(UINT32*)(candidate + 0xD4u) = restored;

    LONG idx = __atomic_fetch_add(&g_v159D4GateLogs, 1, __ATOMIC_ACQ_REL);
    if (idx < 32) {
        LogBuffer m; log_init(&m); log_prefix(&m);
        log_text(&m, "V159 D4 GATE RESTORE #"); log_uint(&m, (UINT64)idx);
        log_text(&m, ": candidate="); log_uint(&m, (UINT64)candidate);
        log_text(&m, ", owner98="); log_uint(&m, owner98);
        log_text(&m, ", d4="); log_uint(&m, current);
        log_text(&m, " -> "); log_uint(&m, restored);
        log_text(&m, ". Native inside-state restored.\\r\\n"); append_log(&m);
    }
    __atomic_store_n(&g_v159ForcedCoffinCandidate, 0ull, __ATOMIC_RELEASE);
    __atomic_store_n(&g_v159ForcedCoffinEntry, 0ull, __ATOMIC_RELEASE);
}

static BYTE __fastcall trace_coffin_cache_admit(void* cache, void* candidate,
                                                void* firstVector, void* secondVector) {
    UINT32 before = 0;
    if (cache && readable_range((BYTE*)cache + 0xA8u, sizeof(UINT32))) before = *(UINT32*)((BYTE*)cache + 0xA8u);

    bool v159ForcedNow = false;
    UINT32 v159SavedD4 = 0u;
    BYTE* resolvedCandidate = (BYTE*)__atomic_load_n(&g_v159ResolvedCoffinCandidate, __ATOMIC_ACQUIRE);
    BYTE* resolvedEntry = (BYTE*)__atomic_load_n(&g_v159ResolvedCoffinEntry, __ATOMIC_ACQUIRE);
    UINT64 owner98 = v159_entry_owner98(resolvedEntry);
    if (g_enableOutsideNetworkMount && candidate == resolvedCandidate &&
        owner98 != 0u && v159_is_genuine_coffin_candidate_early(candidate) &&
        readable_range((BYTE*)candidate + 0xD4u, sizeof(UINT32))) {
        UINT32* d4 = (UINT32*)((BYTE*)candidate + 0xD4u);
        v159SavedD4 = *d4;
        if ((v159SavedD4 & COFFIN_BAGGAGE_D4_MOUNT_BIT) == 0u) {
            BYTE* previous = (BYTE*)__atomic_load_n(&g_v159ForcedCoffinCandidate, __ATOMIC_ACQUIRE);
            if (previous && previous != candidate) v159_restore_candidate_gate_if_inside_or_invalid();
            __atomic_store_n(&g_v159OriginalCandidateD4, v159SavedD4, __ATOMIC_RELEASE);
            __atomic_store_n(&g_v159ForcedCoffinCandidate, (UINT64)candidate, __ATOMIC_RELEASE);
            __atomic_store_n(&g_v159ForcedCoffinEntry, (UINT64)resolvedEntry, __ATOMIC_RELEASE);
            *d4 = v159SavedD4 | COFFIN_BAGGAGE_D4_MOUNT_BIT;
            v159ForcedNow = true;

            LONG idx = __atomic_fetch_add(&g_v159D4GateLogs, 1, __ATOMIC_ACQ_REL);
            if (idx < 32) {
                LogBuffer m; log_init(&m); log_prefix(&m);
                log_text(&m, "V159 D4 GATE OPEN #"); log_uint(&m, (UINT64)idx);
                log_text(&m, ": candidate="); log_uint(&m, (UINT64)candidate);
                log_text(&m, ", entry="); log_uint(&m, (UINT64)resolvedEntry);
                log_text(&m, ", owner98="); log_uint(&m, owner98);
                log_text(&m, ", d4="); log_uint(&m, v159SavedD4);
                log_text(&m, " -> "); log_uint(&m, *d4);
                log_text(&m, ". ED7920 will derive manager bit1 natively.\r\n"); append_log(&m);
            }
        }
    }

    BYTE result = g_nativeCoffinCacheAdmit ? g_nativeCoffinCacheAdmit(cache, candidate, firstVector, secondVector) : 0;
    if (v159ForcedNow && result == 0u &&
        readable_range((BYTE*)candidate + 0xD4u, sizeof(UINT32))) {
        *(UINT32*)((BYTE*)candidate + 0xD4u) = v159SavedD4;
        __atomic_store_n(&g_v159ForcedCoffinCandidate, 0ull, __ATOMIC_RELEASE);
        __atomic_store_n(&g_v159ForcedCoffinEntry, 0ull, __ATOMIC_RELEASE);
    }
    UINT32 after = before;
    if (cache && readable_range((BYTE*)cache + 0xA8u, sizeof(UINT32))) after = *(UINT32*)((BYTE*)cache + 0xA8u);

    // v1.64: remember only a Coffin candidate that DS2 itself just admitted.
    // This is capture-only here; replay happens later, immediately before 1000A70,
    // and only after a proven QPID crossing when this exact cache is empty.
    v164_capture_native_coffin_candidate(cache, candidate, firstVector, secondVector, result, after);

    if (result != 0u && after > 0u && candidate &&
        __atomic_exchange_n(&g_nativeBaggageEpochArmed, 0, __ATOMIC_ACQ_REL) != 0) {
        __atomic_store_n(&g_nativeBaggageCandidate, (UINT64)candidate, __ATOMIC_RELEASE);
        __atomic_store_n(&g_nativeBaggageCache, (UINT64)cache, __ATOMIC_RELEASE);
        // The useful cache hit often happens after the startup/inside traffic
        // consumed the small first-N trace windows. Re-arm diagnostics once,
        // at the first game-produced candidate, without altering native state.
        __atomic_store_n(&g_coffinE3QueryCalls, 0, __ATOMIC_RELEASE);
        __atomic_store_n(&g_coffinE3RecordProducerCalls, 0, __ATOMIC_RELEASE);
        __atomic_store_n(&g_nativeE3SelectedCaptures, 0, __ATOMIC_RELEASE);
        __atomic_store_n(&g_nativeE3SelectedRestores, 0, __ATOMIC_RELEASE);
        __atomic_store_n(&g_networkTraceFirstLogs, 0, __ATOMIC_RELEASE);
        __atomic_store_n(&g_coffin4CCandidateCheckCalls, 0, __ATOMIC_RELEASE);
        __atomic_store_n(&g_coffinEventDispatchCalls, 0, __ATOMIC_RELEASE);
        __atomic_store_n(&g_coffinCandidateRegistrationCalls, 0, __ATOMIC_RELEASE);
        __atomic_store_n(&g_coffinCandidateInsertTraceCalls, 0, __ATOMIC_RELEASE);
        __atomic_store_n(&g_coffinMountEntryCalls, 0, __ATOMIC_RELEASE);
        LogBuffer phase; log_init(&phase); log_prefix(&phase);
        log_text(&phase, "COFFIN CACHE PIPELINE ARMED: real native candidate=0x"); log_uint(&phase, (UINT64)candidate);
        log_text(&phase, ", cache=0x"); log_uint(&phase, (UINT64)cache);
        log_text(&phase, ". E3/4C/ED4EF0 diagnostic windows reset; no game state changed.\r\n"); append_log(&phase);
    }
    LONG call = __atomic_add_fetch(&g_coffinCacheAdmitCalls, 1, __ATOMIC_RELAXED);
    if (call <= 64) {
        UINT64 caller = (UINT64)__builtin_return_address(0);
        UINT64 callerRva = g_executable && caller >= (UINT64)g_executable ? caller - (UINT64)g_executable : caller;
        UINT64 qwords[9] = {};
        if (candidate && readable_range(candidate, sizeof(qwords))) memcpy(qwords, candidate, sizeof(qwords));
        UINT64 vtableRva = 0xFFFFFFFFFFFFFFFFull;
        UINT64 base = (UINT64)g_executable;
        if (qwords[0] >= base && qwords[0] < base + EXPECTED_IMAGE_SIZE) vtableRva = qwords[0] - base;
        LogBuffer buffer; log_init(&buffer); log_prefix(&buffer);
        log_text(&buffer, "COFFIN CACHE ADMIT DETAIL: cache=0x"); log_uint(&buffer, (UINT64)cache);
        log_text(&buffer, ", candidate=0x"); log_uint(&buffer, (UINT64)candidate);
        log_text(&buffer, ", vtable=0x"); log_uint(&buffer, qwords[0]);
        log_text(&buffer, ", vtableRva="); log_uint(&buffer, vtableRva);
        log_text(&buffer, ", q08=0x"); log_uint(&buffer, qwords[1]);
        log_text(&buffer, ", q10=0x"); log_uint(&buffer, qwords[2]);
        log_text(&buffer, ", q18=0x"); log_uint(&buffer, qwords[3]);
        log_text(&buffer, ", q20=0x"); log_uint(&buffer, qwords[4]);
        log_text(&buffer, ", q28=0x"); log_uint(&buffer, qwords[5]);
        log_text(&buffer, ", q30=0x"); log_uint(&buffer, qwords[6]);
        log_text(&buffer, ", q38=0x"); log_uint(&buffer, qwords[7]);
        log_text(&buffer, ", q40=0x"); log_uint(&buffer, qwords[8]);
        log_text(&buffer, ", nativeResult="); log_uint(&buffer, (UINT64)result);
        log_text(&buffer, ", count="); log_uint(&buffer, before); log_text(&buffer, "->"); log_uint(&buffer, after);
        log_text(&buffer, ", callerRva="); log_uint(&buffer, callerRva);
        log_text(&buffer, ". Native admission unchanged.\r\n"); append_log(&buffer);
    }
    return result;
}

static void __fastcall trace_coffin_cache_insert(void* cache, void* candidate,
                                                  double* firstVector, double* secondVector) {
    UINT32 before = 0;
    if (cache && readable_range((BYTE*)cache + 0xA8u, sizeof(UINT32))) before = *(UINT32*)((BYTE*)cache + 0xA8u);
    UINT64 generation = 0;
    if (candidate && readable_range((BYTE*)candidate + 8u, sizeof(UINT64))) generation = *(UINT64*)((BYTE*)candidate + 8u);
    if (g_nativeCoffinCacheInsert) g_nativeCoffinCacheInsert(cache, candidate, firstVector, secondVector);
    UINT32 after = before;
    if (cache && readable_range((BYTE*)cache + 0xA8u, sizeof(UINT32))) after = *(UINT32*)((BYTE*)cache + 0xA8u);
    LONG call = __atomic_add_fetch(&g_coffinCacheInsertCalls, 1, __ATOMIC_RELAXED);
    if (call <= 32) {
        UINT32 distanceBits = 0;
        UINT32 index = before < 2u ? before : 0xFFFFFFFFu;
        if (index < 2u && readable_range((BYTE*)cache + 0x20u + index * 4u, sizeof(UINT32))) {
            distanceBits = *(UINT32*)((BYTE*)cache + 0x20u + index * 4u);
        }
        UINT64 caller = (UINT64)__builtin_return_address(0);
        UINT64 callerRva = g_executable && caller >= (UINT64)g_executable ? caller - (UINT64)g_executable : caller;
        log_coffin_cache_insert("INSERT", cache, candidate, before, after, index, distanceBits, generation, callerRva);
    }
}

static void __fastcall trace_coffin_cache_insert_explicit(void* cache, int index, void* candidate,
                                                           void* vector, UINT64 parameter5, UINT32 type) {
    UINT32 before = 0;
    if (cache && readable_range((BYTE*)cache + 0xA8u, sizeof(UINT32))) before = *(UINT32*)((BYTE*)cache + 0xA8u);
    UINT64 generation = 0;
    if (candidate && readable_range((BYTE*)candidate + 8u, sizeof(UINT64))) generation = *(UINT64*)((BYTE*)candidate + 8u);
    if (g_nativeCoffinCacheInsertExplicit) {
        g_nativeCoffinCacheInsertExplicit(cache, index, candidate, vector, parameter5, type);
    }
    UINT32 after = before;
    if (cache && readable_range((BYTE*)cache + 0xA8u, sizeof(UINT32))) after = *(UINT32*)((BYTE*)cache + 0xA8u);
    LONG call = __atomic_add_fetch(&g_coffinCacheExplicitInsertCalls, 1, __ATOMIC_RELAXED);
    if (call <= 32) {
        UINT64 caller = (UINT64)__builtin_return_address(0);
        UINT64 callerRva = g_executable && caller >= (UINT64)g_executable ? caller - (UINT64)g_executable : caller;
        log_coffin_cache_insert("EXPLICIT INSERT", cache, candidate, before, after,
                                index < 0 ? 0xFFFFFFFFu : (UINT32)index, type, generation, callerRva);
    }
}

static bool install_coffin_cache_insert_trace(HMODULE executable) {
    if (!g_enableCoffinCacheInsertTrace) return true;
    BYTE* base = (BYTE*)executable;
    void** vtable = (void**)(base + RVA_COFFIN_BAGGAGE_INFO_CACHE_VTABLE);
    const SIZE_T slotCount = 5u;
    if (!readable_range(vtable, slotCount * sizeof(void*)) ||
        vtable[RVA_COFFIN_BAGGAGE_INFO_CACHE_RESET_SLOT / sizeof(void*)] != (void*)(base + RVA_COFFIN_BAGGAGE_INFO_CACHE_RESET) ||
        vtable[RVA_COFFIN_BAGGAGE_INFO_CACHE_ADMIT_SLOT / sizeof(void*)] != (void*)(base + RVA_COFFIN_BAGGAGE_INFO_CACHE_ADMIT) ||
        vtable[RVA_COFFIN_BAGGAGE_INFO_CACHE_INSERT_SLOT / sizeof(void*)] != (void*)(base + RVA_COFFIN_BAGGAGE_INFO_CACHE_INSERT) ||
        vtable[RVA_COFFIN_BAGGAGE_INFO_CACHE_INSERT_EXPLICIT_SLOT / sizeof(void*)] != (void*)(base + RVA_COFFIN_BAGGAGE_INFO_CACHE_INSERT_EXPLICIT)) {
        return false;
    }
    g_nativeCoffinCacheReset = (CoffinCacheResetFn)vtable[RVA_COFFIN_BAGGAGE_INFO_CACHE_RESET_SLOT / sizeof(void*)];
    g_nativeCoffinCacheAdmit = (CoffinCacheAdmitFn)vtable[RVA_COFFIN_BAGGAGE_INFO_CACHE_ADMIT_SLOT / sizeof(void*)];
    g_nativeCoffinCacheInsert = (CoffinCacheInsertFn)vtable[RVA_COFFIN_BAGGAGE_INFO_CACHE_INSERT_SLOT / sizeof(void*)];
    g_nativeCoffinCacheInsertExplicit = (CoffinCacheInsertExplicitFn)vtable[RVA_COFFIN_BAGGAGE_INFO_CACHE_INSERT_EXPLICIT_SLOT / sizeof(void*)];
    if (!freeze_for_patch((const BYTE*)vtable, slotCount * sizeof(void*))) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(vtable, slotCount * sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtection)) {
        unfreeze_after_patch(); return false;
    }
    vtable[RVA_COFFIN_BAGGAGE_INFO_CACHE_RESET_SLOT / sizeof(void*)] = (void*)&trace_coffin_cache_reset;
    vtable[RVA_COFFIN_BAGGAGE_INFO_CACHE_ADMIT_SLOT / sizeof(void*)] = (void*)&trace_coffin_cache_admit;
    vtable[RVA_COFFIN_BAGGAGE_INFO_CACHE_INSERT_SLOT / sizeof(void*)] = (void*)&trace_coffin_cache_insert;
    vtable[RVA_COFFIN_BAGGAGE_INFO_CACHE_INSERT_EXPLICIT_SLOT / sizeof(void*)] = (void*)&trace_coffin_cache_insert_explicit;
    FlushInstructionCache((HANDLE)(INT64)-1, vtable, slotCount * sizeof(void*));
    bool installed = vtable[RVA_COFFIN_BAGGAGE_INFO_CACHE_RESET_SLOT / sizeof(void*)] == (void*)&trace_coffin_cache_reset &&
                     vtable[RVA_COFFIN_BAGGAGE_INFO_CACHE_ADMIT_SLOT / sizeof(void*)] == (void*)&trace_coffin_cache_admit &&
                     vtable[RVA_COFFIN_BAGGAGE_INFO_CACHE_INSERT_SLOT / sizeof(void*)] == (void*)&trace_coffin_cache_insert &&
                     vtable[RVA_COFFIN_BAGGAGE_INFO_CACHE_INSERT_EXPLICIT_SLOT / sizeof(void*)] == (void*)&trace_coffin_cache_insert_explicit;
    if (!installed) {
        vtable[RVA_COFFIN_BAGGAGE_INFO_CACHE_RESET_SLOT / sizeof(void*)] = (void*)g_nativeCoffinCacheReset;
        vtable[RVA_COFFIN_BAGGAGE_INFO_CACHE_ADMIT_SLOT / sizeof(void*)] = (void*)g_nativeCoffinCacheAdmit;
        vtable[RVA_COFFIN_BAGGAGE_INFO_CACHE_INSERT_SLOT / sizeof(void*)] = (void*)g_nativeCoffinCacheInsert;
        vtable[RVA_COFFIN_BAGGAGE_INFO_CACHE_INSERT_EXPLICIT_SLOT / sizeof(void*)] = (void*)g_nativeCoffinCacheInsertExplicit;
        FlushInstructionCache((HANDLE)(INT64)-1, vtable, slotCount * sizeof(void*));
    }
    DWORD ignored = 0; VirtualProtect(vtable, slotCount * sizeof(void*), oldProtection, &ignored);
    unfreeze_after_patch();
    if (installed) g_coffinCacheInsertTraceInstalled = 1;
    return installed;
}

// Coffin-only Chiral-Wall collision bypass.  This is deliberately lower in
// the stack than the RideFloater state gates: the game has a dedicated
// Exact Coffin-only Chiral-Wall query bypass.  This is materially narrower
// than the v1.25 collision-filter experiment: the descriptor in 1F50F10 is
// literally engine collision layer 0x2D ("vs Chiral Wall"), and the patched
// branch is immediately after the dedicated physics query using that descriptor.
[[maybe_unused]] static bool install_coffin_chiral_query_result_bypass(HMODULE executable) {
    if (!g_enableNetworkTraversal) return true;
    BYTE* base = (BYTE*)executable;
    BYTE* descriptor = base + RVA_COFFIN_CHIRAL_QUERY_DESCRIPTOR;
    BYTE* context = base + RVA_COFFIN_CHIRAL_QUERY_RESULT_CONTEXT;
    BYTE* branch = base + RVA_COFFIN_CHIRAL_QUERY_RESULT_BRANCH;
    static const BYTE expectedDescriptor[7] = {0xC7,0x45,0x10,0x2D,0x00,0x00,0x00};
    static const BYTE expectedContext[9] = {0xE8,0xCB,0xFA,0x4F,0x00,0x85,0xC0,0x7E,0x0A};
    static const BYTE replacement[2] = {0xEB,0x0A};
    if (!readable_range(descriptor, sizeof(expectedDescriptor)) ||
        !bytes_equal(descriptor, expectedDescriptor, sizeof(expectedDescriptor)) ||
        !readable_range(context, sizeof(expectedContext)) ||
        !bytes_equal(context, expectedContext, sizeof(expectedContext)) ||
        branch != context + 7u) return false;
    if (!freeze_for_patch(branch, sizeof(replacement))) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(branch, sizeof(replacement), PAGE_EXECUTE_READWRITE, &oldProtection)) {
        unfreeze_after_patch(); return false;
    }
    memcpy(branch, replacement, sizeof(replacement));
    FlushInstructionCache((HANDLE)(INT64)-1, branch, sizeof(replacement));
    bool installed = bytes_equal(branch, replacement, sizeof(replacement));
    if (!installed) { branch[0] = 0x7E; branch[1] = 0x0A; FlushInstructionCache((HANDLE)(INT64)-1, branch, 2u); }
    DWORD ignored = 0; VirtualProtect(branch, sizeof(replacement), oldProtection, &ignored);
    unfreeze_after_patch();
    if (installed) g_coffinChiralQueryBypassInstalled = 1;
    return installed;
}

// Main DSVehicleCollisionListener bypass. Unlike the earlier query/filter
// experiments, this is the engine's central collision callback for the live
// vehicle instance. It is restricted twice: vehicle type must be the native
// Coffin Board type (3), and the resolved collision layer must be exactly the
// native Chiral Wall layer (0x16). All other collisions execute the original
// callback unchanged.

// v1.72: correlate the live event-side DSBaggageInfo with the independently
// admitted EE5B60 interaction candidate by world position. DS2 can maintain two
// distinct DSBaggageInfo objects for the same physical Coffin, so pointer identity
// is not a valid ownership test. The first 3 doubles at +0x10 are the live world
// position used by the native interaction pipeline.
static bool v172_same_physical_coffin(void* eventCandidate, void* stickyCandidate, double* outDist2) {
    if (outDist2) *outDist2 = -1.0;
    if (!v159_is_genuine_coffin_candidate_early(eventCandidate) ||
        !v159_is_genuine_coffin_candidate_early(stickyCandidate) ||
        !readable_range((BYTE*)eventCandidate + 0x10u, sizeof(double) * 3u) ||
        !readable_range((BYTE*)stickyCandidate + 0x10u, sizeof(double) * 3u)) return false;
    double* a = (double*)((BYTE*)eventCandidate + 0x10u);
    double* b = (double*)((BYTE*)stickyCandidate + 0x10u);
    double dx = a[0] - b[0];
    double dy = a[1] - b[1];
    double dz = a[2] - b[2];
    double d2 = dx*dx + dy*dy + dz*dz;
    if (outDist2) *outDist2 = d2;
    // Interaction/cache snapshots can lag the live component by a few frames.
    // 25 m is tight enough to reject a different Coffin but tolerant of motion.
    return d2 <= 625.0;
}

// v1.72: direct FUN_141221CB0 native-owner scope.  The native function owns
// DSBaggageInfo+D4 bit20. We never write D4 here; only handler+0x12C is opened
// around the native call when the event candidate matches the active Coffin.
static void __fastcall coffin_bit20_event_handler_probe(void* handler, void* eventObject) {
    UINT32 eventId = 0;
    void* candidate = 0;
    UINT64 q08 = 0;
    UINT32 d4Before = 0;
    bool candidateIsCoffin = false;

    if (eventObject && readable_range((BYTE*)eventObject + 0x10u, sizeof(UINT32)))
        eventId = *(UINT32*)((BYTE*)eventObject + 0x10u);
    if (eventId == COFFIN_BIT20_EVENT_ID && readable_range((BYTE*)eventObject + 0xA0u, sizeof(void*)))
        candidate = *(void**)((BYTE*)eventObject + 0xA0u);
    if (candidate && readable_range(candidate, sizeof(void*)) &&
        readable_range((BYTE*)candidate + 0x08u, sizeof(UINT64)) &&
        readable_range((BYTE*)candidate + 0xD4u, sizeof(UINT32))) {
        void* vtable = *(void**)candidate;
        q08 = *(UINT64*)((BYTE*)candidate + 0x08u);
        candidateIsCoffin = (vtable == (void*)((BYTE*)g_executable + RVA_COFFIN_BAGGAGE_INFO_VTABLE) &&
                             q08 == COFFIN_BAGGAGE_Q08_SIGNATURE);
        if (candidateIsCoffin) d4Before = *(UINT32*)((BYTE*)candidate + 0xD4u);
    }

    int state4D8 = 0x7fffffff;
    UINT64 metricObject = 0;
    float metric = -99999.0f;
    UINT32 stateF6 = 0xffffffffu;
    float state12C = -99999.0f;
    bool contextReadable = false;
    bool nativePredicateExcept12C = false;
    bool spatialMatch = false;
    bool opened12C = false;
    double dist2 = -1.0;
    void* sticky = (void*)__atomic_load_n(&g_v164StickyCandidate, __ATOMIC_ACQUIRE);

    if (candidateIsCoffin && handler &&
        readable_range((BYTE*)handler + 0x4D8u, sizeof(int)) &&
        readable_range((BYTE*)handler + 0x8E8u, sizeof(void*)) &&
        readable_range((BYTE*)handler + 0xF6u, sizeof(BYTE)) &&
        readable_range((BYTE*)handler + 0x12Cu, sizeof(float))) {
        state4D8 = *(int*)((BYTE*)handler + 0x4D8u);
        metricObject = *(UINT64*)((BYTE*)handler + 0x8E8u);
        stateF6 = (UINT32)*(BYTE*)((BYTE*)handler + 0xF6u);
        state12C = *(float*)((BYTE*)handler + 0x12Cu);
        if (metricObject && readable_range((void*)(metricObject + 4u), sizeof(float))) {
            metric = *(float*)(metricObject + 4u);
            contextReadable = true;
            nativePredicateExcept12C = (state4D8 != 1 && metric > 0.0f && stateF6 == 4u);
        }
    }

    if (candidateIsCoffin && contextReadable && nativePredicateExcept12C &&
        __atomic_load_n(&g_v151QpidHits, __ATOMIC_ACQUIRE) > 0 && sticky) {
        spatialMatch = v172_same_physical_coffin(candidate, sticky, &dist2);
        if (spatialMatch && state12C > 0.0f) {
            *(float*)((BYTE*)handler + 0x12Cu) = 0.0f;
            opened12C = true;
        }
    }

    if (g_nativeCoffinBit20EventHandler) g_nativeCoffinBit20EventHandler(handler, eventObject);

    if (opened12C && readable_range((BYTE*)handler + 0x12Cu, sizeof(float)))
        *(float*)((BYTE*)handler + 0x12Cu) = state12C;

    if (!candidateIsCoffin || !readable_range((BYTE*)candidate + 0xD4u, sizeof(UINT32))) return;
    UINT32 d4After = *(UINT32*)((BYTE*)candidate + 0xD4u);
    LONG call = __atomic_fetch_add(&g_coffinBit20EventHandlerCalls, 1, __ATOMIC_ACQ_REL);
    if (call < 128 && (opened12C || spatialMatch || call < 12)) {
        LogBuffer m; log_init(&m); log_prefix(&m);
        log_text(&m, "V172 NATIVE BIT20 OWNER #"); log_uint(&m, (UINT64)call);
        log_text(&m, ": candidate="); log_uint(&m, (UINT64)candidate);
        log_text(&m, ", sticky="); log_uint(&m, (UINT64)sticky);
        log_text(&m, ", d4="); log_uint(&m, (UINT64)d4Before); log_text(&m, "->"); log_uint(&m, (UINT64)d4After);
        log_text(&m, ", bit20="); log_uint(&m, (UINT64)((d4Before & COFFIN_BAGGAGE_D4_MOUNT_BIT) != 0u));
        log_text(&m, "->"); log_uint(&m, (UINT64)((d4After & COFFIN_BAGGAGE_D4_MOUNT_BIT) != 0u));
        log_text(&m, ", 4D8="); log_uint(&m, (UINT64)(UINT32)state4D8);
        log_text(&m, ", metric="); log_float2(&m, metric);
        log_text(&m, ", F6="); log_uint(&m, (UINT64)stateF6);
        log_text(&m, ", 12C="); log_float2(&m, state12C);
        log_text(&m, ", dist2="); log_float2(&m, (float)dist2);
        log_text(&m, ", match="); log_uint(&m, (UINT64)spatialMatch);
        log_text(&m, ", opened12C="); log_uint(&m, (UINT64)opened12C);
        log_text(&m, ". D4 remained native-owned.\r\n");
        append_log(&m);
    }
}

static bool install_coffin_bit20_event_handler_probe(HMODULE executable) {
    BYTE* target = (BYTE*)executable + RVA_COFFIN_BIT20_EVENT_HANDLER;
    // Static v7 audit, RVA 0x1221CB0..0x1221CBF.  Exactly four complete,
    // position-independent instructions; continuation begins at JNZ 1221CC0.
    static const BYTE expected[16] = {
        0x40,0x53,
        0x48,0x83,0xEC,0x30,
        0x81,0x7A,0x10,0x97,0xDE,0xE9,0x1B,
        0x48,0x8B,0xD9
    };
    if (!readable_range(target, sizeof(expected)) ||
        !bytes_equal(target, expected, sizeof(expected))) return false;

    BYTE* trampoline = (BYTE*)VirtualAlloc(0, sizeof(expected) + 14u,
                                            MEM_COMMIT | MEM_RESERVE,
                                            PAGE_EXECUTE_READWRITE);
    if (!trampoline) return false;
    memcpy(trampoline, expected, sizeof(expected));
    static const BYTE absoluteJump[6] = {0xFF,0x25,0x00,0x00,0x00,0x00};
    memcpy(trampoline + sizeof(expected), absoluteJump, sizeof(absoluteJump));
    UINT64 continuation = (UINT64)(target + sizeof(expected));
    memcpy(trampoline + sizeof(expected) + sizeof(absoluteJump), &continuation, sizeof(continuation));
    FlushInstructionCache((HANDLE)(INT64)-1, trampoline, sizeof(expected) + 14u);
    g_nativeCoffinBit20EventHandler = (CoffinBit20EventHandlerFn)trampoline;

    BYTE replacement[16] = {
        0xFF,0x25,0x00,0x00,0x00,0x00,
        0,0,0,0,0,0,0,0,
        0x90,0x90
    };
    UINT64 wrapper = (UINT64)&coffin_bit20_event_handler_probe;
    memcpy(replacement + 6u, &wrapper, sizeof(wrapper));

    if (!freeze_for_patch(target, sizeof(replacement))) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(target, sizeof(replacement), PAGE_EXECUTE_READWRITE, &oldProtection)) {
        unfreeze_after_patch();
        return false;
    }
    memcpy(target, replacement, sizeof(replacement));
    FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(replacement));
    bool installed = bytes_equal(target, replacement, sizeof(replacement));
    if (!installed) {
        memcpy(target, expected, sizeof(expected));
        FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(expected));
    }
    DWORD ignored = 0;
    VirtualProtect(target, sizeof(replacement), oldProtection, &ignored);
    unfreeze_after_patch();
    if (installed) g_coffinBit20EventHandlerProbeInstalled = 1;
    return installed;
}

// v1.71: transaction-scoped form of dev16's proven D4 persistence.
// dev16 proved that keeping DSBaggageInfo+D4 bit20 alive lets DS2 reach the
// real outside RideFloater path, but it kept the bit asserted indefinitely.
// Here the bit is armed for one genuine Coffin only after the boundary has
// actually been hit, survives native 11C14B0 refreshes, and is restored as
// soon as the native RideFloater DriveEntry confirms a completed mount.
static bool v171_is_genuine_coffin_baggage(void* candidate) {
    return candidate && g_executable &&
        readable_range(candidate, sizeof(void*)) &&
        readable_range((BYTE*)candidate + 0x08u, sizeof(UINT64)) &&
        readable_range((BYTE*)candidate + 0xD4u, sizeof(UINT32)) &&
        *(void**)candidate == (void*)((BYTE*)g_executable + RVA_COFFIN_BAGGAGE_INFO_VTABLE) &&
        *(UINT64*)((BYTE*)candidate + 0x08u) == COFFIN_BAGGAGE_Q08_SIGNATURE;
}

static void v171_log_mount_txn(const char* label, void* candidate, UINT32 before, UINT32 after, const char* reason) {
    LONG idx = __atomic_fetch_add(&g_v171MountTxnLogs, 1, __ATOMIC_ACQ_REL);
    if (idx >= 96) return;
    LogBuffer m; log_init(&m); log_prefix(&m);
    log_text(&m, "V171 MOUNT TXN "); log_text(&m, label);
    log_text(&m, " #"); log_uint(&m, (UINT64)idx);
    log_text(&m, ": candidate="); log_uint(&m, (UINT64)candidate);
    log_text(&m, ", d4="); log_uint(&m, before); log_text(&m, "->"); log_uint(&m, after);
    log_text(&m, ", refreshes="); log_uint(&m, (UINT64)__atomic_load_n(&g_v171MountTxnRefreshes, __ATOMIC_ACQUIRE));
    if (reason) { log_text(&m, ", reason="); log_text(&m, reason); }
    log_text(&m, ".\r\n"); append_log(&m);
}

static void v171_restore_mount_transaction(const char* reason) {
    if (__atomic_exchange_n(&g_v171MountTxnActive, 0, __ATOMIC_ACQ_REL) == 0) return;
    void* candidate = (void*)__atomic_load_n(&g_v171MountTxnCandidate, __ATOMIC_ACQUIRE);
    UINT32 original = __atomic_load_n(&g_v171MountTxnOriginalD4, __ATOMIC_ACQUIRE);
    UINT32 before = 0, after = 0;
    if (v171_is_genuine_coffin_baggage(candidate)) {
        UINT32* d4 = (UINT32*)((BYTE*)candidate + 0xD4u);
        before = *d4;
        after = (before & ~COFFIN_BAGGAGE_D4_MOUNT_BIT) | (original & COFFIN_BAGGAGE_D4_MOUNT_BIT);
        *d4 = after;
    }
    __atomic_store_n(&g_v171MountTxnCandidate, 0ull, __ATOMIC_RELEASE);
    __atomic_store_n(&g_v171MountTxnManager, 0ull, __ATOMIC_RELEASE);
    __atomic_store_n(&g_v171MountTxnOriginalD4, 0u, __ATOMIC_RELEASE);
    __atomic_store_n(&g_v171MountTxnRefreshes, 0, __ATOMIC_RELEASE);
    __atomic_fetch_add(&g_v171MountTxnRestores, 1, __ATOMIC_ACQ_REL);
    v171_log_mount_txn("RESTORE", candidate, before, after, reason);
}

static void v171_arm_mount_transaction(void* candidate, void* manager, UINT32 nativeD4, const char* reason) {
    if (!v171_is_genuine_coffin_baggage(candidate)) return;
    UINT64 activeCandidate = __atomic_load_n(&g_v171MountTxnCandidate, __ATOMIC_ACQUIRE);
    if (__atomic_load_n(&g_v171MountTxnActive, __ATOMIC_ACQUIRE) != 0) {
        if (activeCandidate == (UINT64)candidate) {
            UINT32* d4 = (UINT32*)((BYTE*)candidate + 0xD4u);
            if ((*d4 & COFFIN_BAGGAGE_D4_MOUNT_BIT) == 0u) {
                UINT32 before = *d4; *d4 = before | COFFIN_BAGGAGE_D4_MOUNT_BIT;
                __atomic_fetch_add(&g_coffinBaggageD4PersistWrites, 1, __ATOMIC_ACQ_REL);
                v171_log_mount_txn("REASSERT", candidate, before, *d4, reason);
            }
            return;
        }
        v171_restore_mount_transaction("candidate-switch");
    }

    __atomic_store_n(&g_v171MountTxnCandidate, (UINT64)candidate, __ATOMIC_RELEASE);
    __atomic_store_n(&g_v171MountTxnManager, (UINT64)manager, __ATOMIC_RELEASE);
    __atomic_store_n(&g_v171MountTxnOriginalD4, nativeD4, __ATOMIC_RELEASE);
    __atomic_store_n(&g_v171MountTxnRefreshes, 0, __ATOMIC_RELEASE);
    __atomic_store_n(&g_v171MountTxnActive, 1, __ATOMIC_RELEASE);
    __atomic_fetch_add(&g_v171MountTxnArms, 1, __ATOMIC_ACQ_REL);

    UINT32* d4 = (UINT32*)((BYTE*)candidate + 0xD4u);
    UINT32 before = *d4;
    if ((before & COFFIN_BAGGAGE_D4_MOUNT_BIT) == 0u) {
        *d4 = before | COFFIN_BAGGAGE_D4_MOUNT_BIT;
        __atomic_fetch_add(&g_coffinBaggageD4PersistWrites, 1, __ATOMIC_ACQ_REL);
    }
    v171_log_mount_txn("ARM", candidate, before, *d4, reason);
}

static void __fastcall coffin_baggage_refresh_d4_persist(void* candidate, void* context) {
    if (g_nativeCoffinBaggageRefresh) g_nativeCoffinBaggageRefresh(candidate, context);
    if (!g_enableOutsideNetworkMount || !v171_is_genuine_coffin_baggage(candidate)) return;

    UINT32* d4 = (UINT32*)((BYTE*)candidate + 0xD4u);
    UINT32 nativeD4 = *d4;
    LONG call = __atomic_fetch_add(&g_coffinBaggageD4PersistCalls, 1, __ATOMIC_ACQ_REL);
    (void)call;

    LONG active = __atomic_load_n(&g_v171MountTxnActive, __ATOMIC_ACQUIRE);
    UINT64 activeCandidate = __atomic_load_n(&g_v171MountTxnCandidate, __ATOMIC_ACQUIRE);
    if (active != 0 && activeCandidate == (UINT64)candidate) {
        LONG refreshes = __atomic_add_fetch(&g_v171MountTxnRefreshes, 1, __ATOMIC_ACQ_REL);
        if ((nativeD4 & COFFIN_BAGGAGE_D4_MOUNT_BIT) == 0u) {
            *d4 = nativeD4 | COFFIN_BAGGAGE_D4_MOUNT_BIT;
            __atomic_fetch_add(&g_coffinBaggageD4PersistWrites, 1, __ATOMIC_ACQ_REL);
            if (refreshes <= 32 || (refreshes % 256) == 0)
                v171_log_mount_txn("REFRESH", candidate, nativeD4, *d4, "native-refresh");
        }
        if (refreshes >= V171_MOUNT_TXN_REFRESH_LIMIT)
            v171_restore_mount_transaction("refresh-timeout");
        return;
    }

    // Closest safe equivalent of dev16: arm only after the proven crossing
    // root has actually observed a QPID boundary hit, and only when vanilla
    // has just cleared bit20 on the genuine Coffin candidate.
    if (active == 0 &&
        __atomic_load_n(&g_v151QpidHits, __ATOMIC_ACQUIRE) > 0 &&
        (nativeD4 & COFFIN_BAGGAGE_D4_MOUNT_BIT) == 0u) {
        v171_arm_mount_transaction(candidate, 0, nativeD4, "post-boundary-refresh");
    }
}

static bool install_coffin_baggage_d4_persist(HMODULE executable) {
    if (!g_enableNetworkTraversal && !g_enableOutsideNetworkMount) return true;

    BYTE* base = (BYTE*)executable;
    void** vtable = (void**)(base + RVA_COFFIN_BAGGAGE_INFO_VTABLE);
    void** slot = (void**)((BYTE*)vtable + RVA_COFFIN_BAGGAGE_INFO_REFRESH_SLOT);
    void* expected = (void*)(base + RVA_COFFIN_BAGGAGE_INFO_REFRESH);

    if (!readable_range(vtable, 4u * sizeof(void*)) ||
        !readable_range(slot, sizeof(void*)) ||
        *slot != expected) return false;

    g_coffinBaggageInfoVtableAddress = (void*)vtable;
    g_nativeCoffinBaggageRefresh = (CoffinBaggageRefreshFn)*slot;

    if (!freeze_for_patch((const BYTE*)slot, sizeof(void*))) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(slot, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtection)) {
        unfreeze_after_patch();
        return false;
    }

    *slot = (void*)&coffin_baggage_refresh_d4_persist;
    FlushInstructionCache((HANDLE)(INT64)-1, slot, sizeof(void*));
    bool installed = *slot == (void*)&coffin_baggage_refresh_d4_persist;
    if (!installed) {
        *slot = (void*)g_nativeCoffinBaggageRefresh;
        FlushInstructionCache((HANDLE)(INT64)-1, slot, sizeof(void*));
    }

    DWORD ignored = 0;
    VirtualProtect(slot, sizeof(void*), oldProtection, &ignored);
    unfreeze_after_patch();

    if (installed) g_coffinBaggageD4PersistInstalled = 1;
    return installed;
}

typedef BYTE (__fastcall* VehicleCollisionListenerFn)(void*, void*);
typedef void* (__fastcall* VehicleCollisionResolveFn)(void*, void*);
static VehicleCollisionListenerFn g_nativeVehicleCollisionListener = 0;
static VehicleCollisionResolveFn g_vehicleCollisionResolveObject = 0;

static BYTE __fastcall coffin_main_vehicle_collision(void* listener, void* collisionEvent) {
    LONG call = __atomic_add_fetch(&g_mainVehicleCollisionCalls, 1, __ATOMIC_RELAXED);
    if (g_enableNetworkTraversal && listener && collisionEvent && g_vehicleCollisionResolveObject) {
        void* vehicle = 0;
        if (readable_range((BYTE*)listener + OFF_VEHICLE_COLLISION_LISTENER_VEHICLE, sizeof(void*))) {
            vehicle = *(void**)((BYTE*)listener + OFF_VEHICLE_COLLISION_LISTENER_VEHICLE);
        }
        if (vehicle &&
            readable_range((BYTE*)vehicle + OFF_VEHICLE_STATE_FLAGS_CE4, sizeof(UINT32)) &&
            readable_range((BYTE*)vehicle + OFF_VEHICLE_STATE_FLAGS_4E8, sizeof(BYTE)) &&
            readable_range((BYTE*)vehicle + OFF_VEHICLE_STATE_FLAGS_4EC, sizeof(UINT32)) &&
            readable_range((BYTE*)vehicle + OFF_VEHICLE_TYPE, sizeof(UINT32))) {
            UINT32 stateCE4 = *(UINT32*)((BYTE*)vehicle + OFF_VEHICLE_STATE_FLAGS_CE4);
            BYTE state4E8 = *(BYTE*)((BYTE*)vehicle + OFF_VEHICLE_STATE_FLAGS_4E8);
            UINT32 state4EC = *(UINT32*)((BYTE*)vehicle + OFF_VEHICLE_STATE_FLAGS_4EC);
            UINT32 vehicleType = *(UINT32*)((BYTE*)vehicle + OFF_VEHICLE_TYPE);

            // Match the native callback's four pre-resolver early-out gates.
            // We only extend the game's own Coffin-specific stopper exemption.
            bool nativeWouldResolve = ((stateCE4 & 0x10u) == 0u) &&
                                      ((stateCE4 & 0x04u) == 0u) &&
                                      ((state4E8 & 0x20u) == 0u) &&
                                      ((state4EC & 0x4000u) == 0u);
            if (vehicleType == VEHICLE_TYPE_COFFIN_BOARD && nativeWouldResolve) {
                void* collisionObject = g_vehicleCollisionResolveObject(listener, collisionEvent);
                if (collisionObject &&
                    readable_range((BYTE*)collisionObject + OFF_COLLISION_OBJECT_LAYER, sizeof(UINT32))) {
                    UINT32 tag = *(UINT32*)((BYTE*)collisionObject + OFF_COLLISION_OBJECT_LAYER) & 0x7Fu;
                    if (tag == PHYSICS_COLLISION_TYPE_CHIRAL_WALL ||
                        tag == PHYSICS_COLLISION_TYPE_VEHICLE_STOPPER) {
                        LONG bypass = __atomic_add_fetch(&g_mainChiralCollisionBypasses, 1, __ATOMIC_RELAXED);
                        if (bypass <= 32) {
                            UINT64 caller = (UINT64)__builtin_return_address(0);
                            UINT64 base = (UINT64)(g_executable ? g_executable : GetModuleHandleW(0));
                            LogBuffer m; log_init(&m); log_prefix(&m);
                            log_text(&m, "BOUNDARY COLLISION BYPASS #"); log_uint(&m, (UINT64)bypass);
                            log_text(&m, ": tag=0x");
                            if (tag == PHYSICS_COLLISION_TYPE_CHIRAL_WALL) {
                                log_text(&m, "16 (Chiral_Wall)");
                            } else {
                                log_text(&m, "1B (Vehicle_stopper)");
                            }
                            log_text(&m, ", vehicleType=3, callerRva=");
                            log_uint(&m, caller >= base ? caller - base : caller);
                            log_text(&m, ". Returned the same ignore result vanilla uses for tag 0x4E on Coffin Board.\r\n");
                            append_log(&m);
                        }
                        return 0;
                    }
                }
            }
        }
    }
    if (call == 1 && g_enableNetworkTraversal) {
        log_line("BOUNDARY ROOT: central DSVehicleCollisionListener active; Coffin ignores only tags 0x16 Chiral_Wall and 0x1B Vehicle_stopper. All other collisions remain native.");
    }
    return g_nativeVehicleCollisionListener ? g_nativeVehicleCollisionListener(listener, collisionEvent) : 0;
}

[[maybe_unused]] static bool install_coffin_main_vehicle_collision_bypass(HMODULE executable) {
    if (!g_enableNetworkTraversal) return true;
    BYTE* base = (BYTE*)executable;
    void** vtable = (void**)(base + RVA_VEHICLE_COLLISION_LISTENER_VTABLE);
    void** slot = (void**)((BYTE*)vtable + RVA_VEHICLE_COLLISION_LISTENER_CALLBACK_SLOT);
    static const BYTE expectedResolver[13] = {
        0x40,0x53,0x48,0x83,0xEC,0x20,0x48,0x8B,0x1A,0x48,0x39,0x59,0x10
    };
    static const BYTE expectedNoCoffinGate[23] = {
        0x41,0x83,0xFD,0x4E,0x75,0x19,0x49,0x8B,0x47,0x18,0x83,0xB8,
        0x90,0x04,0x00,0x00,0x03,0x0F,0x84,0xDA,0xFB,0xFF,0xFF
    };
    if (!readable_range(vtable, 3u * sizeof(void*)) ||
        !readable_range(base + RVA_VEHICLE_COLLISION_RESOLVE_OBJECT, sizeof(expectedResolver)) ||
        !readable_range(base + RVA_VEHICLE_COLLISION_NO_COFFIN_GATE, sizeof(expectedNoCoffinGate)) ||
        *slot != (void*)(base + RVA_VEHICLE_COLLISION_LISTENER_CALLBACK) ||
        !bytes_equal(base + RVA_VEHICLE_COLLISION_RESOLVE_OBJECT, expectedResolver, sizeof(expectedResolver)) ||
        !bytes_equal(base + RVA_VEHICLE_COLLISION_NO_COFFIN_GATE, expectedNoCoffinGate, sizeof(expectedNoCoffinGate))) {
        return false;
    }
    g_nativeVehicleCollisionListener = (VehicleCollisionListenerFn)*slot;
    g_vehicleCollisionResolveObject = (VehicleCollisionResolveFn)(base + RVA_VEHICLE_COLLISION_RESOLVE_OBJECT);
    if (!freeze_for_patch((const BYTE*)slot, sizeof(void*))) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(slot, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtection)) {
        unfreeze_after_patch(); return false;
    }
    *slot = (void*)&coffin_main_vehicle_collision;
    FlushInstructionCache((HANDLE)(INT64)-1, slot, sizeof(void*));
    bool installed = *slot == (void*)&coffin_main_vehicle_collision;
    if (!installed) {
        *slot = (void*)g_nativeVehicleCollisionListener;
        FlushInstructionCache((HANDLE)(INT64)-1, slot, sizeof(void*));
    }
    DWORD ignored = 0;
    VirtualProtect(slot, sizeof(void*), oldProtection, &ignored);
    unfreeze_after_patch();
    if (installed) g_mainVehicleCollisionBypassInstalled = 1;
    return installed;
}

// CoffinBoardCheckPhysicsCollisionFilter and a separate engine filter named
// PhysicsCollisionIgnoreChiralWallFilter.  The latter proves that collision
// category 0x16 is the Chiral Wall and returns false for that category.  Keep
// the Coffin filter's native answer for every other collision.
typedef BYTE (__fastcall* CoffinCollisionPredicateFn)(void*, void*);
typedef void* (__fastcall* ResolvePhysicsCollisionObjectFn)(void*);
static CoffinCollisionPredicateFn g_nativeCoffinCollisionPredicate = 0;
static ResolvePhysicsCollisionObjectFn g_resolvePhysicsCollisionObject = 0;

static BYTE __fastcall coffin_collision_ignore_chiral_wall(void* filter, void* collision) {
    if (!g_nativeCoffinCollisionPredicate) return 0;
    BYTE nativeResult = g_nativeCoffinCollisionPredicate(filter, collision);
    LONG call = __atomic_add_fetch(&g_coffinCollisionFilterCalls, 1, __ATOMIC_RELAXED);
    if (!nativeResult || !g_enableNetworkTraversal || !collision || !g_resolvePhysicsCollisionObject) return nativeResult;

    void* object = g_resolvePhysicsCollisionObject(collision);
    // 0x11EA10 is the same resolver the native Coffin filter itself uses; a
    // non-null return is therefore an engine-owned live collision object.
    // Read the already-native field directly to keep this hot physics path
    // free of VirtualQuery overhead.
    UINT32 category = object ? (*(UINT32*)((BYTE*)object + 0x50u) & 0x7Fu) : 0xFFFFFFFFu;
    if (category == PHYSICS_COLLISION_TYPE_CHIRAL_WALL) {
        LONG bypass = __atomic_add_fetch(&g_coffinChiralWallBypassCalls, 1, __ATOMIC_RELAXED);
        if (bypass <= 32) {
            UINT64 caller = (UINT64)__builtin_return_address(0);
            UINT64 base = (UINT64)(g_executable ? g_executable : GetModuleHandleW(0));
            LogBuffer message; log_init(&message); log_prefix(&message);
            log_text(&message, "COFFIN CHIRAL WALL BYPASS #"); log_uint(&message, (UINT64)bypass);
            log_text(&message, ": collision=0x"); log_uint(&message, (UINT64)collision);
            log_text(&message, ", object=0x"); log_uint(&message, (UINT64)object);
            log_text(&message, ", category=0x16, nativeResult=1, callerRva=");
            log_uint(&message, caller >= base ? caller - base : caller);
            log_text(&message, ". Collision suppressed for Coffin only.\r\n"); append_log(&message);
        }
        return 0;
    }
    if (call <= 16) {
        UINT64 caller = (UINT64)__builtin_return_address(0);
        UINT64 base = (UINT64)(g_executable ? g_executable : GetModuleHandleW(0));
        LogBuffer message; log_init(&message); log_prefix(&message);
        log_text(&message, "COFFIN COLLISION FILTER #"); log_uint(&message, (UINT64)call);
        log_text(&message, ": category="); log_uint(&message, (UINT64)category);
        log_text(&message, ", nativeResult="); log_uint(&message, (UINT64)nativeResult);
        log_text(&message, ", callerRva="); log_uint(&message, caller >= base ? caller - base : caller);
        log_text(&message, ". Native result preserved.\r\n"); append_log(&message);
    }
    return nativeResult;
}

[[maybe_unused]] static bool install_coffin_chiral_wall_collision_bypass(HMODULE executable) {
    if (!g_enableNetworkTraversal) return true;
    BYTE* base = (BYTE*)executable;
    void** coffinVtable = (void**)(base + RVA_COFFIN_COLLISION_FILTER_VTABLE);
    void** ignoreVtable = (void**)(base + RVA_PHYSICS_COLLISION_IGNORE_CHIRAL_WALL_VTABLE);
    void** slot = (void**)((BYTE*)coffinVtable + RVA_COFFIN_COLLISION_FILTER_PREDICATE_SLOT);
    if (!readable_range(coffinVtable, 4u * sizeof(void*)) ||
        !readable_range(ignoreVtable, 4u * sizeof(void*)) ||
        *slot != (void*)(base + RVA_COFFIN_COLLISION_FILTER_PREDICATE) ||
        ignoreVtable[3] != (void*)(base + RVA_PHYSICS_COLLISION_IGNORE_CHIRAL_WALL_PREDICATE)) return false;

    // Validate the engine filter's exact category gate: mov eax,[rbx+50],
    // and eax,7F, cmp al,16, setne al. This protects against silently applying
    // the bypass to a future build where category semantics have changed.
    static const BYTE chiralWallGate[14] = {
        0x8B,0x43,0x50,0x83,0xE0,0x7F,0x3C,0x16,0x0F,0x95,0xC0,0x48,0x8B,0x5C
    };
    BYTE* gate = base + RVA_PHYSICS_COLLISION_IGNORE_CHIRAL_WALL_PREDICATE + 0xF0u;
    if (!readable_range(gate, sizeof(chiralWallGate)) || !bytes_equal(gate, chiralWallGate, sizeof(chiralWallGate))) return false;

    g_nativeCoffinCollisionPredicate = (CoffinCollisionPredicateFn)*slot;
    g_resolvePhysicsCollisionObject = (ResolvePhysicsCollisionObjectFn)(base + RVA_RESOLVE_PHYSICS_COLLISION_OBJECT);
    if (!freeze_for_patch((const BYTE*)slot, sizeof(void*))) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(slot, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtection)) { unfreeze_after_patch(); return false; }
    *slot = (void*)&coffin_collision_ignore_chiral_wall;
    FlushInstructionCache((HANDLE)(INT64)-1, slot, sizeof(void*));
    bool installed = *slot == (void*)&coffin_collision_ignore_chiral_wall;
    if (!installed) *slot = (void*)g_nativeCoffinCollisionPredicate;
    DWORD ignored = 0; VirtualProtect(slot, sizeof(void*), oldProtection, &ignored);
    unfreeze_after_patch();
    if (installed) g_coffinChiralWallBypassInstalled = 1;
    return installed;
}

// Enable the native E3 request-record producer at its source.  The Coffin's
// registration descriptor deliberately carries no bit for request 0xE3
// (word 7, bit 3), so the ED9C80 refresh loop never calls ED6200 and the
// resolver record remains ownerless.  This hook changes only the temporary
// descriptor on the Coffin callback's stack before the original F64E80 call;
// the game's descriptor merge, record construction, generation handling and
// Save-Restore lifecycle remain native.
static bool install_coffin_e3_descriptor_flag(HMODULE executable) {
    if (!g_enableCoffinE3Descriptor) return true;
    BYTE* target = (BYTE*)executable + RVA_COFFIN_E3_DESCRIPTOR_FLAG_HOOK;
    static const BYTE expected[14] = {
        0x41,0xB9,0xFE,0xFF,0xFF,0xFF, // mov r9d,-2
        0xBA,0xBC,0xC0,0x68,0x35,       // mov edx,3568C0BCh
        0xC5,0xF8,0x77                  // vzeroupper
    };
    if (!readable_range(target, sizeof(expected)) || !bytes_equal(target, expected, sizeof(expected))) return false;
    // or dword ptr [rsp+5Ch],8; original r9/edx/vzeroupper; jump back.
    BYTE* cave = (BYTE*)VirtualAlloc(0, 64u, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!cave) return false;
    SIZE_T p = 0;
    static const BYTE setE3[5] = {0x83,0x4C,0x24,0x5C,0x08};
    memcpy(cave + p, setE3, sizeof(setE3)); p += sizeof(setE3);
    memcpy(cave + p, expected, sizeof(expected)); p += sizeof(expected);
    static const BYTE absoluteJump[14] = {0xFF,0x25,0,0,0,0,0,0,0,0,0,0,0,0};
    memcpy(cave + p, absoluteJump, sizeof(absoluteJump));
    UINT64 continuation = (UINT64)(target + sizeof(expected));
    memcpy(cave + p + 6u, &continuation, sizeof(continuation));
    p += sizeof(absoluteJump);
    FlushInstructionCache((HANDLE)(INT64)-1, cave, p);

    BYTE replacement[14] = {0xFF,0x25,0,0,0,0,0,0,0,0,0,0,0,0};
    UINT64 caveAddress = (UINT64)cave;
    memcpy(replacement + 6u, &caveAddress, sizeof(caveAddress));
    if (!freeze_for_patch(target, sizeof(replacement))) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(target, sizeof(replacement), PAGE_EXECUTE_READWRITE, &oldProtection)) {
        unfreeze_after_patch(); return false;
    }
    memcpy(target, replacement, sizeof(replacement));
    FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(replacement));
    bool installed = bytes_equal(target, replacement, sizeof(replacement));
    if (!installed) {
        memcpy(target, expected, sizeof(expected));
        FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(expected));
    }
    DWORD ignored = 0; VirtualProtect(target, sizeof(replacement), oldProtection, &ignored);
    unfreeze_after_patch();
    if (installed) g_coffinE3DescriptorInstalled = 1;
    return installed;
}

// This is deliberately a branch patch, not a resolver result override.
//
// The main Coffin action vtable is the only vtable that points to this method.
// The method first subtracts E3 from EDX, so the bypass below is unreachable
// for every other request.  It merely permits E3 to continue through the
// native predicate after the two network-status bits are observed.  The
// subsequent owner, action-state, movement, and physics validation all run
// exactly as authored by the game.
[[maybe_unused]] static bool install_coffin_e3_network_predicate_bypass(HMODULE executable) {
    if (!g_enableNetworkTraversal && !g_enableOutsideNetworkMount) return true;
    g_coffinE3NetworkPredicateFailureStage = 0;
    g_coffinE3NetworkPredicateSystemError = 0;
    g_coffinE3RuntimeMountEntry = 0;
    g_coffinE3RuntimePredicateEntry = 0;
    memset(g_coffinE3RuntimeBranchBytes, 0, sizeof(g_coffinE3RuntimeBranchBytes));
    BYTE* base = (BYTE*)executable;
    void** vtable = (void**)(base + RVA_COFFIN_RIDE_FLOATER_ACTION_VTABLE);
    void** predicateSlot = (void**)(base + RVA_COFFIN_RIDE_FLOATER_ACTION_E3_VTABLE_SLOT);
    BYTE* predicate = base + RVA_COFFIN_RIDE_FLOATER_ACTION_E3_PREDICATE;
    BYTE* branchContext = base + RVA_COFFIN_RIDE_FLOATER_ACTION_E3_NETWORK_BRANCH - 3u;
    BYTE* branch = branchContext + 3u;
    static const BYTE expectedPredicate[18] = {
        0x40,0x53,0x48,0x83,0xEC,0x20,0x48,0x8B,0xD9,
        0x81,0xEA,0xE3,0x00,0x00,0x00,0x74,0x71,0x83
    };
    static const BYTE expectedBranch[9] = {
        0xF6,0xC1,0x01,0x75,0x76,0x48,0x8B,0x43,0x28
    };
    static const BYTE replacement[2] = {0x90,0x90};
    if (!readable_range(vtable, 34u * sizeof(void*))) {
        g_coffinE3NetworkPredicateFailureStage = 1; return false;
    }
    g_coffinE3RuntimeMountEntry = (UINT64)vtable[1];
    if (!readable_range(predicate, sizeof(expectedPredicate))) {
        g_coffinE3NetworkPredicateFailureStage = 2; return false;
    }
    if (!readable_range(branchContext, sizeof(expectedBranch))) {
        g_coffinE3NetworkPredicateFailureStage = 3; return false;
    }
    memcpy(g_coffinE3RuntimeBranchBytes, branchContext, sizeof(g_coffinE3RuntimeBranchBytes));
    g_coffinE3RuntimePredicateEntry = (UINT64)*predicateSlot;
    if (vtable[1] != (void*)(base + RVA_COFFIN_RIDE_FLOATER_MOUNT_ENTRY)) {
        g_coffinE3NetworkPredicateFailureStage = 4; return false;
    }
    if (predicateSlot != vtable + 33u || *predicateSlot != (void*)predicate) {
        g_coffinE3NetworkPredicateFailureStage = 5; return false;
    }
    if (!bytes_equal(predicate, expectedPredicate, sizeof(expectedPredicate))) {
        g_coffinE3NetworkPredicateFailureStage = 6; return false;
    }
    if (!bytes_equal(branchContext, expectedBranch, sizeof(expectedBranch))) {
        g_coffinE3NetworkPredicateFailureStage = 7; return false;
    }
    // The target is a two-byte, naturally aligned x64 instruction.  An
    // aligned 16-bit store is atomic: executing threads see either the
    // original JNE or two complete NOPs, never a torn instruction.  Keeping
    // every DS2 thread suspended here was less reliable than the patch itself
    // and caused the previous false "anchor" failure at startup.
    if (((UINT64)branch & 1u) != 0u) {
        g_coffinE3NetworkPredicateFailureStage = 8; return false;
    }
    DWORD oldProtection = 0;
    if (!VirtualProtect(branch, sizeof(replacement), PAGE_EXECUTE_READWRITE, &oldProtection)) {
        g_coffinE3NetworkPredicateFailureStage = 9;
        g_coffinE3NetworkPredicateSystemError = GetLastError();
        return false;
    }
    __atomic_store_n((volatile WORD*)branch, (WORD)0x9090u, __ATOMIC_SEQ_CST);
    FlushInstructionCache((HANDLE)(INT64)-1, branch, sizeof(replacement));
    bool installed = bytes_equal(branch, replacement, sizeof(replacement));
    if (!installed) {
        __atomic_store_n((volatile WORD*)branch, (WORD)0x7675u, __ATOMIC_SEQ_CST);
        FlushInstructionCache((HANDLE)(INT64)-1, branch, sizeof(replacement));
    }
    DWORD ignored = 0;
    VirtualProtect(branch, sizeof(replacement), oldProtection, &ignored);
    if (!installed) {
        g_coffinE3NetworkPredicateFailureStage = 10; return false;
    }
    g_coffinE3NetworkPredicateBypassInstalled = 1;
    return true;
}
[[maybe_unused]] static void log_coffin_e3_network_predicate_failure() {
    LogBuffer message; log_init(&message); log_prefix(&message);
    log_text(&message, "ERROR: Coffin E3 predicate inactive; stage=");
    log_uint(&message, (UINT64)g_coffinE3NetworkPredicateFailureStage);
    log_text(&message, ", systemError=");
    log_uint(&message, (UINT64)g_coffinE3NetworkPredicateSystemError);
    log_text(&message, ", runtimeMount=");
    log_uint(&message, g_coffinE3RuntimeMountEntry);
    log_text(&message, ", runtimeE3Slot=");
    log_uint(&message, g_coffinE3RuntimePredicateEntry);
    log_text(&message, ", branchBytes=");
    for (UINT32 index = 0; index < sizeof(g_coffinE3RuntimeBranchBytes); ++index) {
        if (index) log_char(&message, ',');
        log_uint(&message, (UINT64)g_coffinE3RuntimeBranchBytes[index]);
    }
    log_text(&message, ". No network byte was changed.\r\n");
    append_log(&message);
}

static void refresh_coffin_e3_registration(void* action);
static bool coffin_voluntary_dispatch_scope();
typedef UINT32 (__fastcall* CoffinE3AvailabilityFn)(void*, UINT32);
static CoffinE3AvailabilityFn g_nativeCoffinE3Availability = 0;

// ED9C80 passes tables+0x73A0 as the fifth argument to ED6200.  ED6200
// advances past the cursor header and enumerates the following native action
// pointers, invoking their +0x108/+0x110 methods.  Snapshot only this input
// container so the next offline/runtime evidence identifies whether the
// missing Coffin candidate is absent at registration time or rejected later.
static void log_coffin_e3_candidate_cursor(void* manager, LogBuffer* message) {
    if (!manager || !readable_range((BYTE*)manager + 0x25068u, sizeof(void*))) return;
    void* tables = *(void**)((BYTE*)manager + 0x25068u);
    if (!tables || !readable_range((BYTE*)tables + 0x73A0u, sizeof(void*))) return;
    void* cursor = *(void**)((BYTE*)tables + 0x73A0u);
    log_text(message, ", candidateCursor="); log_uint(message, (UINT64)cursor);
    if (!cursor || !readable_range((BYTE*)cursor, 0x150u)) return;
    UINT32 nonNull = 0;
    void* firstCandidate = 0;
    log_text(message, ", cursorHeader="); log_uint(message, (UINT64)*(void**)cursor);
    log_text(message, ", cursorSlots=");
    for (UINT32 index = 0; index < 41u; ++index) {
        void* candidate = *(void**)((BYTE*)cursor + 8u + (SIZE_T)index * sizeof(void*));
        if (!candidate) continue;
        if (!firstCandidate) firstCandidate = candidate;
        if (nonNull) log_char(message, '|');
        log_uint(message, (UINT64)index); log_char(message, ':');
        log_uint(message, (UINT64)candidate); log_char(message, ':');
        UINT64 vtableAddress = 0;
        if (readable_range(candidate, sizeof(void*))) {
            void** vtable = *(void***)candidate;
            if (vtable && readable_range(vtable, sizeof(void*))) vtableAddress = (UINT64)vtable;
        }
        log_uint(message, vtableAddress);
        ++nonNull;
    }
    log_text(message, ", cursorNonNull="); log_uint(message, nonNull);
    UINT64 executableBase = (UINT64)(g_executable ? g_executable : GetModuleHandleW(0));
    if (firstCandidate && readable_range(firstCandidate, sizeof(void*))) {
        void** vtable = *(void***)firstCandidate;
        if (!vtable || !readable_range(vtable, 0x118u)) return;
        UINT64 slot108 = (UINT64)vtable[0x108u / sizeof(void*)];
        UINT64 slot110 = (UINT64)vtable[0x110u / sizeof(void*)];
        log_text(message, ", firstCandidateVtable="); log_uint(message, (UINT64)vtable);
        log_text(message, ", firstVtable108="); log_uint(message, slot108);
        log_text(message, ", firstVtable110="); log_uint(message, slot110);
        log_text(message, ", firstVtable108Rva=");
        log_uint(message, slot108 >= executableBase ? slot108 - executableBase : slot108);
        log_text(message, ", firstVtable110Rva=");
        log_uint(message, slot110 >= executableBase ? slot110 - executableBase : slot110);
    }
}

// v1.55 read-only RideFloater E3 provider trace. 1000A70 is the +0x108
// classifier called by ED6200 before the +0x110 record producer. The nested
// +0x118 callback is wrapped separately so its native result can be reported
// without calling it twice. No return value or game field is changed.
typedef BYTE (__fastcall* V155Gate118Fn)(void*);
static V155Gate118Fn g_nativeV155Gate118 = 0;

static BYTE __fastcall coffin_v155_gate118_trace(void* action) {
    BYTE result = g_nativeV155Gate118 ? g_nativeV155Gate118(action) : 1u;
    __atomic_store_n(&g_v155Gate118Action, (UINT64)action, __ATOMIC_RELEASE);
    __atomic_store_n(&g_v155Gate118Result, (LONG)result, __ATOMIC_RELEASE);
    return result;
}

static UINT64 v155_mix(UINT64 h, UINT64 v) {
    h ^= v + 0x9E3779B97F4A7C15ull + (h << 6) + (h >> 2);
    return h;
}

static UINT32 __fastcall coffin_v155_provider_trace(void* action, UINT32 request) {
    if (!g_nativeCoffinE3Availability) return 0xBu;
    if (request == COFFIN_NETWORK_E3_REQUEST && action) {
        __atomic_store_n(&g_v155Gate118Action, (UINT64)action, __ATOMIC_RELEASE);
        __atomic_store_n(&g_v155Gate118Result, -1, __ATOMIC_RELEASE);
        // v1.64: by provider time the native pad-cache rebuild for this frame
        // has already run. If Outside QPID erased the previously native Coffin
        // candidate, restore only that exact DS2-accepted object before the
        // native provider/selector consumes the cache.
        v164_reinsert_sticky_candidate_before_provider(action);
    }
    UINT32 result = g_nativeCoffinE3Availability(action, request);
    if (request != COFFIN_NETWORK_E3_REQUEST || !action) return result;

    BYTE* a = (BYTE*)action;
    void* player = readable_range(a + 0x28u, sizeof(void*)) ? *(void**)(a + 0x28u) : 0;
    void* owner50 = readable_range(a + 0x50u, sizeof(void*)) ? *(void**)(a + 0x50u) : 0;
    void* owner38 = readable_range(a + 0x38u, sizeof(void*)) ? *(void**)(a + 0x38u) : 0;
    void* manager = readable_range(a + 0x60u, sizeof(void*)) ? *(void**)(a + 0x60u) : 0;
    void* component = (owner50 && readable_range((BYTE*)owner50 + 0x2A8u, sizeof(void*))) ? *(void**)((BYTE*)owner50 + 0x2A8u) : 0;
    void* scalar = (owner38 && readable_range((BYTE*)owner38 + 0x1338u, sizeof(void*))) ? *(void**)((BYTE*)owner38 + 0x1338u) : 0;

    UINT32 p7388 = 0, p7390 = 0;
    int p7548 = -999;
    if (player && readable_range((BYTE*)player + 0x7388u, 0x0Cu)) {
        p7388 = *(UINT32*)((BYTE*)player + 0x7388u);
        p7390 = *(UINT32*)((BYTE*)player + 0x7390u);
    }
    if (player && readable_range((BYTE*)player + 0x7548u, sizeof(int))) p7548 = *(int*)((BYTE*)player + 0x7548u);
    UINT32 comp5E8 = 0;
    if (component && readable_range((BYTE*)component + 0x5E8u, sizeof(UINT32))) comp5E8 = *(UINT32*)((BYTE*)component + 0x5E8u);
    float scalar4 = 999999.0f; BYTE scalarE = 0xFFu;
    if (scalar && readable_range((BYTE*)scalar + 4u, 0x0Bu)) {
        scalar4 = *(float*)((BYTE*)scalar + 4u);
        scalarE = *(BYTE*)((BYTE*)scalar + 0x0Eu);
    }
    LONG gate118 = -1;
    if (__atomic_load_n(&g_v155Gate118Action, __ATOMIC_ACQUIRE) == (UINT64)action)
        gate118 = __atomic_load_n(&g_v155Gate118Result, __ATOMIC_ACQUIRE);

    void* selected = 0; UINT32 count = 0, selectedIndex = 0, flags2F0 = 0;
    if (manager) {
        if (readable_range((BYTE*)manager + 0x230u, sizeof(void*))) selected = *(void**)((BYTE*)manager + 0x230u);
        if (readable_range((BYTE*)manager + 0x2D0u, sizeof(UINT32))) count = *(UINT32*)((BYTE*)manager + 0x2D0u);
        if (readable_range((BYTE*)manager + 0x2D4u, sizeof(UINT32))) selectedIndex = *(UINT32*)((BYTE*)manager + 0x2D4u);
        if (readable_range((BYTE*)manager + 0x2F0u, sizeof(UINT32))) flags2F0 = *(UINT32*)((BYTE*)manager + 0x2F0u);
    }

    UINT64 sig = 0x1551000ull;
    sig = v155_mix(sig, result); sig = v155_mix(sig, p7388 >> 12); sig = v155_mix(sig, p7390 >> 12);
    sig = v155_mix(sig, (UINT32)p7548); sig = v155_mix(sig, (UINT32)(gate118 + 1));
    sig = v155_mix(sig, comp5E8); sig = v155_mix(sig, (UINT32)scalarE); sig = v155_mix(sig, (UINT64)selected);
    sig = v155_mix(sig, count); sig = v155_mix(sig, selectedIndex); sig = v155_mix(sig, flags2F0);
    UINT64 old = __atomic_exchange_n(&g_v155ProviderLastSignature, sig, __ATOMIC_ACQ_REL);
    if (old != sig) {
        LONG idx = __atomic_fetch_add(&g_v155ProviderTraceLogs, 1, __ATOMIC_ACQ_REL);
        if (idx < 96) {
            LogBuffer m; log_init(&m); log_prefix(&m);
            log_text(&m, "V155 PROVIDER E3 #"); log_uint(&m, (UINT64)idx);
            log_text(&m, ": nativeResult="); log_uint(&m, result);
            log_text(&m, ", playerBit12[7388,7390]="); log_uint(&m, (p7388 >> 12) & 1u); log_char(&m, ','); log_uint(&m, (p7390 >> 12) & 1u);
            log_text(&m, ", player7548="); log_uint(&m, (UINT64)(UINT32)p7548);
            log_text(&m, ", gate118="); log_uint(&m, (UINT64)(UINT32)(gate118 < 0 ? 0xFFFFFFFFu : (UINT32)gate118));
            log_text(&m, ", comp5E8="); log_uint(&m, comp5E8);
            log_text(&m, ", scalar4="); log_float2(&m, scalar4);
            log_text(&m, ", scalarE="); log_uint(&m, scalarE);
            log_text(&m, ", manager="); log_uint(&m, (UINT64)manager);
            log_text(&m, ", selected="); log_uint(&m, (UINT64)selected);
            log_text(&m, ", candidateCount="); log_uint(&m, count);
            log_text(&m, ", selectedIndex="); log_uint(&m, selectedIndex);
            log_text(&m, ", manager2F0="); log_uint(&m, flags2F0);
            log_text(&m, ". READ-ONLY; native 1000A70 result preserved.\r\n"); append_log(&m);
        }
    }
    return result;
}

static bool install_v155_provider_trace(HMODULE executable) {
    if (!g_enableOutsideNetworkMount) return true;
    BYTE* base = (BYTE*)executable;
    void** vtable = (void**)(base + RVA_COFFIN_RIDE_FLOATER_ACTION_VTABLE);
    void** slot108 = (void**)((BYTE*)vtable + 0x108u);
    void** slot118 = (void**)((BYTE*)vtable + 0x118u);
    if (!readable_range(vtable, 36u * sizeof(void*)) ||
        *slot108 != (void*)(base + RVA_COFFIN_RIDE_FLOATER_ACTION_E3_PREDICATE) ||
        *slot118 != (void*)(base + RVA_V155_RIDEFLOATER_GATE118)) return false;
    g_nativeCoffinE3Availability = (CoffinE3AvailabilityFn)*slot108;
    g_nativeV155Gate118 = (V155Gate118Fn)*slot118;
    if (!freeze_for_patch((const BYTE*)slot108, 0x18u)) return false;
    DWORD oldProtection = 0; bool ok = false;
    if (VirtualProtect(slot108, 0x18u, PAGE_EXECUTE_READWRITE, &oldProtection)) {
        *slot118 = (void*)&coffin_v155_gate118_trace;
        *slot108 = (void*)&coffin_v155_provider_trace;
        FlushInstructionCache((HANDLE)(INT64)-1, slot108, 0x18u);
        ok = (*slot108 == (void*)&coffin_v155_provider_trace && *slot118 == (void*)&coffin_v155_gate118_trace);
        if (!ok) {
            *slot108 = (void*)g_nativeCoffinE3Availability;
            *slot118 = (void*)g_nativeV155Gate118;
            FlushInstructionCache((HANDLE)(INT64)-1, slot108, 0x18u);
        }
        DWORD ignored = 0; VirtualProtect(slot108, 0x18u, oldProtection, &ignored);
    }
    unfreeze_after_patch();
    return ok;
}

// The native E3 predicate returns 0xB for every unavailable Coffin action,
// including the network-border case.  Keep all native checks and override
// only that result in the Coffin action's own E3 vtable slot.  This avoids
// changing the shared predicate or any global availability bitset.
static UINT32 __fastcall coffin_e3_availability_scope(void* action, UINT32 request) {
    if (!g_nativeCoffinE3Availability) return 0xBu;
    UINT32 result = g_nativeCoffinE3Availability(action, request);
    __atomic_fetch_add(&g_coffinE3AvailabilityCalls, 1, __ATOMIC_ACQ_REL);
    if (request == COFFIN_NETWORK_E3_REQUEST) {
        LONG queryIndex = __atomic_fetch_add(&g_coffinE3QueryCalls, 1, __ATOMIC_ACQ_REL);
        void* manager = 0;
        void* selectedCandidate = 0;
        if (action && readable_range((BYTE*)action + 0x60u, sizeof(void*))) {
            manager = *(void**)((BYTE*)action + 0x60u);
        }
        if (manager && readable_range((BYTE*)manager + 0x230u, sizeof(void*))) {
            selectedCandidate = *(void**)((BYTE*)manager + 0x230u);
        }
        // Static audit: ED7920 is the native owner of manager+0x230. Cache only
        // a candidate that ED7920 itself selected and that already carries the
        // exact candidate+0xD4 bit 20 required later by 1000BD0. This is only a
        // short-lived pointer cache; no object or candidate array is fabricated.
        if (manager && selectedCandidate &&
            readable_range((BYTE*)selectedCandidate + 0xD4u, sizeof(UINT32)) &&
            readable_range((BYTE*)selectedCandidate + 0x08u, sizeof(UINT64))) {
            UINT32 d4 = *(UINT32*)((BYTE*)selectedCandidate + 0xD4u);
            UINT64 handle = *(UINT64*)((BYTE*)selectedCandidate + 0x08u);
            if (handle != 0xFFFFFFFFFFFFFFFFull) {
                __atomic_store_n(&g_lastNativeE3SelectedManager, (UINT64)manager, __ATOMIC_RELEASE);
                __atomic_store_n(&g_lastNativeE3SelectedCandidate, (UINT64)selectedCandidate, __ATOMIC_RELEASE);
                __atomic_store_n(&g_lastNativeE3SelectedSequence, queryIndex, __ATOMIC_RELEASE);
                LONG capture = __atomic_fetch_add(&g_nativeE3SelectedCaptures, 1, __ATOMIC_ACQ_REL);
                if (capture < 16) {
                    LogBuffer c; log_init(&c); log_prefix(&c);
                    log_text(&c, "E3 NATIVE SELECT CAPTURE #"); log_uint(&c, (UINT64)capture);
                    log_text(&c, ": manager="); log_uint(&c, (UINT64)manager);
                    log_text(&c, ", candidate="); log_uint(&c, (UINT64)selectedCandidate);
                    log_text(&c, ", d4="); log_uint(&c, d4);
                    log_text(&c, ", bit20="); log_uint(&c, (UINT64)((d4 & 0x00100000u) != 0u));
                    log_text(&c, ", sequence="); log_uint(&c, (UINT64)(UINT32)queryIndex);
                    log_text(&c, ". Genuine ED7920 selection cached; no candidate field changed here.\r\n"); append_log(&c);
                }
            }
        }
        if (queryIndex < 16) {
            UINT64 caller = (UINT64)__builtin_return_address(0);
            UINT64 executableBase = (UINT64)(g_executable ? g_executable : GetModuleHandleW(0));
            LogBuffer message; log_init(&message); log_prefix(&message);
            log_text(&message, "E3 Coffin candidate query #"); log_uint(&message, (UINT64)queryIndex);
            log_text(&message, ": nativeResult="); log_uint(&message, result);
            log_text(&message, ", callerRva=");
            log_uint(&message, caller >= executableBase ? caller - executableBase : caller);
            void* owner = 0;
            BYTE active = 0;
            BYTE reject = 0;
            UINT32 generation = 0;
            UINT32 sourceGeneration = 0;
            if (manager) {
                BYTE* requestEntry = (BYTE*)manager + (SIZE_T)COFFIN_NETWORK_E3_REQUEST * 0x4D0u;
                BYTE* stateEntry = (BYTE*)manager + (SIZE_T)COFFIN_NETWORK_E3_REQUEST * 0xD8u;
                if (readable_range(requestEntry + 0x48918u, sizeof(void*))) owner = *(void**)(requestEntry + 0x48918u);
                if (readable_range(stateEntry + 0x2518Cu, sizeof(UINT32))) generation = *(UINT32*)(stateEntry + 0x2518Cu);
                if (readable_range(stateEntry + 0x25156u, sizeof(BYTE))) reject = *(BYTE*)(stateEntry + 0x25156u);
                if (readable_range(stateEntry + 0x251E8u, sizeof(BYTE))) active = *(BYTE*)(stateEntry + 0x251E8u);
                if (readable_range(requestEntry + 0x488BCu, sizeof(UINT32))) sourceGeneration = *(UINT32*)(requestEntry + 0x488BCu);
            }
            log_text(&message, ", ownerMatch="); log_uint(&message, (UINT64)(owner == action));
            log_text(&message, ", requiredActive="); log_uint(&message, (UINT64)(active != 0));
            log_text(&message, ", reject="); log_uint(&message, (UINT64)(reject != 0));
            log_text(&message, ", generation="); log_uint(&message, generation);
            log_text(&message, ", sourceGeneration="); log_uint(&message, sourceGeneration);
            UINT32 candidateCount = 0;
            UINT32 candidateCapacity = 0;
            void* candidateStorage = 0;
            UINT32 selectedIndex = 0;
            UINT32 managerFlags = 0;
            UINT64 managerVtable = 0;
            if (manager &&
                readable_range((BYTE*)manager + 0x238u, sizeof(void*)) &&
                readable_range((BYTE*)manager + 0x2D0u, sizeof(UINT32)) &&
                readable_range((BYTE*)manager + 0x2D4u, sizeof(UINT32)) &&
                readable_range((BYTE*)manager + 0x2D8u, sizeof(UINT32)) &&
                readable_range((BYTE*)manager + 0x2F0u, sizeof(UINT32))) {
                candidateStorage = *(void**)((BYTE*)manager + 0x238u);
                candidateCount = *(UINT32*)((BYTE*)manager + 0x2D0u);
                selectedIndex = *(UINT32*)((BYTE*)manager + 0x2D4u);
                candidateCapacity = *(UINT32*)((BYTE*)manager + 0x2D8u);
                managerFlags = *(UINT32*)((BYTE*)manager + 0x2F0u);
                if (readable_range(manager, sizeof(void*))) managerVtable = (UINT64)*(void***)manager;
            }
            if (g_enableCoffinCandidateInsertTrace && manager) {
                __atomic_store_n(&g_coffinCandidateMessageContainer,
                                 (UINT64)((BYTE*)manager + 0x2D0u),
                                 __ATOMIC_RELEASE);
            }
            log_text(&message, ", managerVtable="); log_uint(&message, managerVtable);
            log_text(&message, ", managerFlags2F0="); log_uint(&message, managerFlags);
            log_text(&message, ", candidateCount="); log_uint(&message, candidateCount);
            log_text(&message, ", candidateCapacity="); log_uint(&message, candidateCapacity);
            log_text(&message, ", selectedIndex="); log_uint(&message, selectedIndex);
            log_text(&message, ", candidateStorage="); log_uint(&message, (UINT64)candidateStorage);
            log_text(&message, ", selectedCandidate="); log_uint(&message, (UINT64)selectedCandidate);
            log_coffin_e3_candidate_cursor(manager, &message);
            if (manager && candidateCount > 0 && candidateCount <= 32u &&
                readable_range((BYTE*)manager + 0x238u, (SIZE_T)candidateCount * sizeof(void*))) {
                log_text(&message, ", candidates=");
                for (UINT32 candidateIndex = 0; candidateIndex < candidateCount; ++candidateIndex) {
                    if (candidateIndex) log_char(&message, '|');
                    void* candidate = ((void**)((BYTE*)manager + 0x238u))[candidateIndex];
                    log_uint(&message, (UINT64)candidate);
                }
            }
            log_text(&message, ".\r\n"); append_log(&message);
        }
    }
    if ((g_enableNetworkTraversal || g_enableOutsideNetworkMount || g_enableNativeE3Producer) &&
        request == COFFIN_NETWORK_E3_REQUEST && result == 0xBu) {
        if (g_enableOutsideNetworkMount && action) {
            __atomic_store_n(&g_lastCoffinE3UnavailableAction, (UINT64)action, __ATOMIC_RELEASE);
        }
        if (__atomic_fetch_add(&g_coffinE3AvailabilityForced, 1, __ATOMIC_ACQ_REL) == 0) {
            LogBuffer message; log_init(&message); log_prefix(&message);
            log_text(&message, "E3 availability wrapper forced native unavailable result to available for Coffin; request=");
            log_uint(&message, request); log_text(&message, ".\r\n"); append_log(&message);
        }
        return 4u;
    }
    return result;
}

static bool install_coffin_e3_availability_scope(HMODULE executable) {
    if (!g_enableNetworkTraversal && !g_enableOutsideNetworkMount && !g_enableNativeE3Producer && !g_enableCoffinCandidateCursorTrace) return true;
    BYTE* base = (BYTE*)executable;
    void** vtable = (void**)(base + RVA_COFFIN_RIDE_FLOATER_ACTION_VTABLE);
    void** target = (void**)(base + RVA_COFFIN_RIDE_FLOATER_ACTION_E3_VTABLE_SLOT);
    BYTE* predicate = base + RVA_COFFIN_RIDE_FLOATER_ACTION_E3_PREDICATE;
    static const BYTE expectedPredicate[18] = {
        0x40,0x53,0x48,0x83,0xEC,0x20,0x48,0x8B,0xD9,
        0x81,0xEA,0xE3,0x00,0x00,0x00,0x74,0x71,0x83
    };
    if (!readable_range(vtable, 34u * sizeof(void*)) ||
        !readable_range(target, sizeof(void*)) ||
        vtable[1] != (void*)(base + RVA_COFFIN_RIDE_FLOATER_MOUNT_ENTRY) ||
        *target != (void*)predicate ||
        !readable_range(predicate, sizeof(expectedPredicate)) ||
        !bytes_equal(predicate, expectedPredicate, sizeof(expectedPredicate))) return false;
    g_nativeCoffinE3Availability = (CoffinE3AvailabilityFn)*target;
    if (!freeze_for_patch((const BYTE*)target, sizeof(void*))) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(target, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtection)) {
        unfreeze_after_patch(); return false;
    }
    *target = (void*)&coffin_e3_availability_scope;
    FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(void*));
    bool installed = *target == (void*)&coffin_e3_availability_scope;
    if (!installed) *target = (void*)g_nativeCoffinE3Availability;
    DWORD ignored = 0; VirtualProtect(target, sizeof(void*), oldProtection, &ignored);
    unfreeze_after_patch();
    if (installed) g_coffinE3AvailabilityWrapperInstalled = 1;
    return installed;
}

// The native ED6200 candidate builder calls the Coffin action's +0x110
// producer after the availability slot. Its E3 path has one additional
// network-owned manager gate (`manager+0x2F0 & 2`) before it fills the normal
// output record. Outside the network that gate is clear, so the native
// producer leaves the local record empty and ED6200 never reaches ED6750.
//
// The producer remains native. When the functional network feature is enabled,
// the wrapper opens only its native manager admission bit for the call and
// restores it afterward; it never fabricates local output or lifecycle data.
typedef void (__fastcall* CoffinE3RecordProducerFn)(void*, UINT32, void*);
static CoffinE3RecordProducerFn g_nativeCoffinE3RecordProducer = 0;

static void v155_log_producer_entry(void* action, UINT32 request) {
    if (request != COFFIN_NETWORK_E3_REQUEST || !action) return;
    BYTE* a = (BYTE*)action;
    void* manager = readable_range(a + 0x60u, sizeof(void*)) ? *(void**)(a + 0x60u) : 0;
    void* selected = 0; UINT32 count = 0, selectedIndex = 0, capacity = 0, flags2F0 = 0, d4 = 0;
    UINT64 selectedVtable = 0, q08 = 0;
    if (manager) {
        if (readable_range((BYTE*)manager + 0x230u, sizeof(void*))) selected = *(void**)((BYTE*)manager + 0x230u);
        if (readable_range((BYTE*)manager + 0x2D0u, sizeof(UINT32))) count = *(UINT32*)((BYTE*)manager + 0x2D0u);
        if (readable_range((BYTE*)manager + 0x2D4u, sizeof(UINT32))) selectedIndex = *(UINT32*)((BYTE*)manager + 0x2D4u);
        if (readable_range((BYTE*)manager + 0x2D8u, sizeof(UINT32))) capacity = *(UINT32*)((BYTE*)manager + 0x2D8u);
        if (readable_range((BYTE*)manager + 0x2F0u, sizeof(UINT32))) flags2F0 = *(UINT32*)((BYTE*)manager + 0x2F0u);
    }
    if (selected && readable_range(selected, 0xD8u)) {
        selectedVtable = (UINT64)*(void**)selected;
        q08 = *(UINT64*)((BYTE*)selected + 8u);
        d4 = *(UINT32*)((BYTE*)selected + 0xD4u);
    }
    UINT64 sig = 0x1552000ull; sig=v155_mix(sig,(UINT64)manager); sig=v155_mix(sig,(UINT64)selected);
    sig=v155_mix(sig,count); sig=v155_mix(sig,selectedIndex); sig=v155_mix(sig,flags2F0); sig=v155_mix(sig,d4);
    UINT64 old=__atomic_exchange_n(&g_v155ProducerLastSignature,sig,__ATOMIC_ACQ_REL);
    if (old == sig) return;
    LONG idx=__atomic_fetch_add(&g_v155ProducerEntryLogs,1,__ATOMIC_ACQ_REL); if(idx>=96) return;
    UINT64 expectedVtable=(UINT64)((BYTE*)g_executable + RVA_COFFIN_BAGGAGE_INFO_VTABLE);
    LogBuffer m; log_init(&m); log_prefix(&m);
    log_text(&m,"V155 PRODUCER ENTRY #"); log_uint(&m,(UINT64)idx);
    log_text(&m,": manager="); log_uint(&m,(UINT64)manager);
    log_text(&m,", selected="); log_uint(&m,(UINT64)selected);
    log_text(&m,", candidateCount="); log_uint(&m,count);
    log_text(&m,", selectedIndex="); log_uint(&m,selectedIndex);
    log_text(&m,", capacity="); log_uint(&m,capacity);
    log_text(&m,", manager2F0="); log_uint(&m,flags2F0);
    log_text(&m,", selectedD4="); log_uint(&m,d4);
    log_text(&m,", selectedBit20="); log_uint(&m,(d4 & COFFIN_BAGGAGE_D4_MOUNT_BIT)?1u:0u);
    log_text(&m,", vtableMatch="); log_uint(&m,selectedVtable==expectedVtable?1u:0u);
    log_text(&m,", q08Match="); log_uint(&m,q08==COFFIN_BAGGAGE_Q08_SIGNATURE?1u:0u);
    log_text(&m,". Entry observed before any temporary 1000BD0 gate opening.\r\n"); append_log(&m);
}

static void __fastcall coffin_e3_record_producer_scope(void* action, UINT32 request, void* output) {
    v155_log_producer_entry(action, request);
    if (!g_nativeCoffinE3RecordProducer || request != COFFIN_NETWORK_E3_REQUEST ||
        !action || !readable_range((BYTE*)action + OFF_COFFIN_ACTION_MANAGER, sizeof(void*))) {
        if (g_nativeCoffinE3RecordProducer) g_nativeCoffinE3RecordProducer(action, request, output);
        return;
    }

    void* manager = *(void**)((BYTE*)action + OFF_COFFIN_ACTION_MANAGER);
    void* selected = 0;
    UINT32 savedFlags = 0;
    UINT32 savedCandidateD4 = 0;
    bool opened = false;

    // v1.45.1 runtime trace + v11 static proof:
    // ED7920 has already selected a genuine Coffin candidate into manager+0x230.
    // It derives manager+0x2F0 bit 1 from selectedCandidate+D4 bit 20.
    // Outside QPID both are clear, so 1000BD0 exits before building the E3 record.
    //
    // v1.50 never restores a cached/synthetic candidate. It opens these two
    // native gates only for the currently selected RTTI/signature-confirmed
    // DSBaggageInfo Coffin candidate, then restores both words immediately.
    if (g_enableOutsideNetworkMount && manager &&
        readable_range((BYTE*)manager + 0x230u, sizeof(void*)) &&
        readable_range((BYTE*)manager + 0x2F0u, sizeof(UINT32))) {
        selected = *(void**)((BYTE*)manager + 0x230u);
        if (selected &&
            readable_range(selected, sizeof(void*)) &&
            readable_range((BYTE*)selected + 0x08u, sizeof(UINT64)) &&
            readable_range((BYTE*)selected + 0xD4u, sizeof(UINT32))) {
            void* selectedVtable = *(void**)selected;
            UINT64 handle = *(UINT64*)((BYTE*)selected + 0x08u);
            bool genuineCoffin =
                selectedVtable == (void*)((BYTE*)g_executable + RVA_COFFIN_BAGGAGE_INFO_VTABLE) &&
                handle == COFFIN_BAGGAGE_Q08_SIGNATURE;
            if (genuineCoffin) {
                savedFlags = *(UINT32*)((BYTE*)manager + 0x2F0u);
                savedCandidateD4 = *(UINT32*)((BYTE*)selected + 0xD4u);
                *(UINT32*)((BYTE*)selected + 0xD4u) = savedCandidateD4 | COFFIN_BAGGAGE_D4_MOUNT_BIT;
                *(UINT32*)((BYTE*)manager + 0x2F0u) = savedFlags | 0x00000002u;
                opened = true;
            }
        }
    }

    g_nativeCoffinE3RecordProducer(action, request, output);

    UINT32 resultCode = 0;
    BYTE resultFlag = 0;
    if (output && readable_range(output, 0xEAu)) {
        resultCode = *(UINT32*)output;
        resultFlag = *((BYTE*)output + 9u);
    }

    if (opened && resultCode == 0x0Bu) {
        // v1.72: descriptor 0xB is observed, but D4 persistence is not synthesized.
        // The native 1221CB0 owner is authoritative; this scope only preserves the
        // already-proven ability to let the native producer see the selected Coffin.
        __atomic_store_n(&g_v151ValidatedCoffinAction, (UINT64)action, __ATOMIC_RELEASE);
        __atomic_store_n(&g_v151ValidatedCoffinManager, (UINT64)manager, __ATOMIC_RELEASE);
        __atomic_store_n(&g_v151ValidatedCoffinCandidate, (UINT64)selected, __ATOMIC_RELEASE);
        __atomic_store_n(&g_v151ValidatedNativeD4, savedCandidateD4, __ATOMIC_RELEASE);
        __atomic_store_n(&g_v151ValidatedNativeManagerFlags, savedFlags, __ATOMIC_RELEASE);
    } else if (opened) {
        UINT64 cachedAction = __atomic_load_n(&g_v151ValidatedCoffinAction, __ATOMIC_ACQUIRE);
        if (cachedAction == (UINT64)action) {
            __atomic_store_n(&g_v151ValidatedCoffinAction, 0ull, __ATOMIC_RELEASE);
            __atomic_store_n(&g_v151ValidatedCoffinManager, 0ull, __ATOMIC_RELEASE);
            __atomic_store_n(&g_v151ValidatedCoffinCandidate, 0ull, __ATOMIC_RELEASE);
            __atomic_store_n(&g_v151ValidatedNativeD4, 0u, __ATOMIC_RELEASE);
            __atomic_store_n(&g_v151ValidatedNativeManagerFlags, 0u, __ATOMIC_RELEASE);
        }
    }

    if (opened) {
        // v1.72 restores both temporary producer gates immediately. Persistent
        // eligibility must come only from the native 1221CB0 bit20 owner.
        if (selected && readable_range((BYTE*)selected + 0xD4u, sizeof(UINT32))) {
            *(UINT32*)((BYTE*)selected + 0xD4u) = savedCandidateD4;
        }
        if (manager && readable_range((BYTE*)manager + 0x2F0u, sizeof(UINT32))) {
            *(UINT32*)((BYTE*)manager + 0x2F0u) = savedFlags;
        }

        LONG index = __atomic_fetch_add(&g_coffinE3RecordProducerCalls, 1, __ATOMIC_ACQ_REL);
        if (index < 32) {
            LogBuffer m; log_init(&m); log_prefix(&m);
            log_text(&m, "OUTSIDE MOUNT ROOT #"); log_uint(&m, (UINT64)index);
            log_text(&m, ": genuineSelected="); log_uint(&m, (UINT64)selected);
            log_text(&m, ", D4(before)="); log_uint(&m, (UINT64)savedCandidateD4);
            log_text(&m, ", manager2F0(before)="); log_uint(&m, (UINT64)savedFlags);
            log_text(&m, ", nativeOutputCode="); log_uint(&m, (UINT64)resultCode);
            log_text(&m, ", outputFlag9="); log_uint(&m, (UINT64)resultFlag);
            log_text(&m, ". temporary D4/manager producer gates restored immediately; persistent bit20 is native-owned.\r\n");
            append_log(&m);
        }
    }
}

static bool install_coffin_e3_record_producer_scope(HMODULE executable) {
    if (!g_enableOutsideNetworkMount && !g_enableNativeE3Producer && !g_enableCoffinCandidateCursorTrace) return true;
    BYTE* base = (BYTE*)executable;
    void** target = (void**)(base + RVA_COFFIN_RIDE_FLOATER_ACTION_E3_RECORD_SLOT);
    void** availability = (void**)(base + RVA_COFFIN_RIDE_FLOATER_ACTION_E3_VTABLE_SLOT);
    void** vtable = (void**)(base + RVA_COFFIN_RIDE_FLOATER_ACTION_VTABLE);
    BYTE* nativeAvailability = base + RVA_COFFIN_RIDE_FLOATER_ACTION_E3_PREDICATE;
    BYTE* nativeProducer = base + 0x01000BD0u;
    if (!readable_range(vtable, 35u * sizeof(void*)) ||
        !readable_range(target, sizeof(void*)) ||
        !readable_range(availability, sizeof(void*)) ||
        vtable[33] != (void*)nativeAvailability || *availability != (void*)nativeAvailability ||
        *target != (void*)nativeProducer) return false;
    g_nativeCoffinE3RecordProducer = (CoffinE3RecordProducerFn)*target;
    if (!freeze_for_patch((const BYTE*)target, sizeof(void*))) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(target, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtection)) {
        unfreeze_after_patch(); return false;
    }
    *target = (void*)&coffin_e3_record_producer_scope;
    FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(void*));
    bool installed = *target == (void*)&coffin_e3_record_producer_scope;
    if (!installed) *target = (void*)g_nativeCoffinE3RecordProducer;
    DWORD ignored = 0; VirtualProtect(target, sizeof(void*), oldProtection, &ignored);
    unfreeze_after_patch();
    if (installed) g_coffinE3RecordProducerScopeInstalled = 1;
    return installed;
}

typedef UINT64 (__fastcall* CoffinMountEntryFn)(void*);
static CoffinMountEntryFn g_nativeCoffinMountEntry = 0;
typedef void (__fastcall* CoffinE3RegisterFn)(void*, void*, UINT64, UINT32, void*);
static CoffinE3RegisterFn g_nativeCoffinE3Register = 0;

typedef void (__fastcall* CoffinCandidateRegisterNativeFn)(void*, void*, UINT64, UINT64);
static CoffinCandidateRegisterNativeFn g_nativeCoffinCandidateRegister = 0;

// ED4EF0 is reached from a native pad-cache registration path.  The original
// function uses RDI as an additional live input before it overwrites it with
// the action id, so the detour saves every GPR, passes the original RDI to the
// observer in R9, restores all registers, and then enters an exact trampoline.
// No game state is changed by this probe.
static void __fastcall log_native_coffin_candidate_registration(
    void* context, void* registrationList, UINT64 actionId, UINT64 hiddenRdi) {
    LONG index = __atomic_fetch_add(&g_coffinCandidateRegistrationCalls, 1, __ATOMIC_ACQ_REL);
    if (index >= 64) return;
    BYTE* manager = 0;
    if (context && readable_range((BYTE*)context + 0x48u, sizeof(void*))) {
        manager = *(BYTE**)((BYTE*)context + 0x48u);
    }
    UINT32 listCount = 0;
    UINT32 listCapacity = 0;
    if (registrationList && readable_range((BYTE*)registrationList + 8u, sizeof(UINT32) * 2u)) {
        listCount = *(UINT32*)registrationList;
        listCapacity = *(UINT32*)((BYTE*)registrationList + 4u);
    }
    UINT32 managerCount = 0;
    void* selected = 0;
    UINT64 managerVtable = 0;
    if (manager && readable_range(manager, sizeof(void*)) &&
        readable_range(manager + 0x230u, sizeof(void*) * 2u)) {
        managerVtable = (UINT64)*(void**)manager;
        selected = *(void**)(manager + 0x230u);
        managerCount = *(UINT32*)(manager + 0x2D0u);
    }
    LogBuffer message; log_init(&message); log_prefix(&message);
    log_text(&message, "NATIVE CANDIDATE REGISTER #"); log_uint(&message, (UINT64)index);
    log_text(&message, ": context="); log_uint(&message, (UINT64)context);
    log_text(&message, ", list="); log_uint(&message, (UINT64)registrationList);
    log_text(&message, ", listCount="); log_uint(&message, (UINT64)listCount);
    log_text(&message, ", listCapacity="); log_uint(&message, (UINT64)listCapacity);
    log_text(&message, ", actionId="); log_uint(&message, actionId);
    log_text(&message, ", hiddenRdi="); log_uint(&message, hiddenRdi);
    log_text(&message, ", manager="); log_uint(&message, (UINT64)manager);
    log_text(&message, ", managerVtable="); log_uint(&message, managerVtable);
    log_text(&message, ", managerCount="); log_uint(&message, (UINT64)managerCount);
    log_text(&message, ", selected="); log_uint(&message, (UINT64)selected);
    log_text(&message, ". Native registration unchanged.\r\n"); append_log(&message);
}

static bool install_coffin_native_candidate_registration_trace(HMODULE executable) {
    if (!g_enableCoffinCandidateCursorTrace) return true;
    BYTE* target = (BYTE*)executable + RVA_COFFIN_NATIVE_CANDIDATE_REGISTER;
    // Copy through the first complete instruction after the stack setup.  No
    // RIP-relative instruction is included in this trampoline prefix.
    static const BYTE expected[17] = {
        0x48,0x8B,0xC4,0x53,0x41,0x54,0x48,0x81,0xEC,
        0xF8,0x00,0x00,0x00,0x48,0x89,0x70,0x08
    };
    if (!readable_range(target, sizeof(expected)) ||
        !bytes_equal(target, expected, sizeof(expected))) return false;

    BYTE* trampoline = (BYTE*)VirtualAlloc(0, 64u, MEM_COMMIT | MEM_RESERVE,
                                           PAGE_EXECUTE_READWRITE);
    BYTE* cave = (BYTE*)VirtualAlloc(0, 128u, MEM_COMMIT | MEM_RESERVE,
                                     PAGE_EXECUTE_READWRITE);
    if (!trampoline || !cave) return false;

    memcpy(trampoline, expected, sizeof(expected));
    SIZE_T trampolineJump = sizeof(expected);
    trampoline[trampolineJump++] = 0xFF; trampoline[trampolineJump++] = 0x25;
    trampoline[trampolineJump++] = 0x00; trampoline[trampolineJump++] = 0x00;
    trampoline[trampolineJump++] = 0x00; trampoline[trampolineJump++] = 0x00;
    UINT64 continuation = (UINT64)(target + sizeof(expected));
    memcpy(trampoline + trampolineJump, &continuation, sizeof(continuation));
    trampolineJump += sizeof(continuation);

    SIZE_T p = 0;
    // Save all integer registers so the native function sees exactly the
    // entry register state, including the otherwise hidden RDI input.
    const BYTE save[] = {
        0x50, 0x51, 0x52, 0x41,0x50, 0x41,0x51, 0x41,0x52, 0x41,0x53,
        0x57, 0x56, 0x53, 0x55, 0x41,0x54, 0x41,0x55, 0x41,0x56, 0x41,0x57
    };
    memcpy(cave + p, save, sizeof(save)); p += sizeof(save);
    // The saved original RDI is still in RDI.  Expose it to the logger as
    // the fourth ABI argument; the original R9 is restored from the stack.
    cave[p++] = 0x49; cave[p++] = 0x89; cave[p++] = 0xF9; // mov r9,rdi
    cave[p++] = 0x48; cave[p++] = 0x83; cave[p++] = 0xEC; cave[p++] = 0x20;
    cave[p++] = 0x48; cave[p++] = 0xB8;
    UINT64 logger = (UINT64)&log_native_coffin_candidate_registration;
    memcpy(cave + p, &logger, sizeof(logger)); p += sizeof(logger);
    cave[p++] = 0xFF; cave[p++] = 0xD0; // call rax
    cave[p++] = 0x48; cave[p++] = 0x83; cave[p++] = 0xC4; cave[p++] = 0x20;
    const BYTE restore[] = {
        0x41,0x5F, 0x41,0x5E, 0x41,0x5D, 0x41,0x5C, 0x5D, 0x5B, 0x5E,
        0x5F, 0x41,0x5B, 0x41,0x5A, 0x41,0x59, 0x41,0x58, 0x5A, 0x59, 0x58
    };
    memcpy(cave + p, restore, sizeof(restore)); p += sizeof(restore);
    cave[p++] = 0xFF; cave[p++] = 0x25;
    cave[p++] = 0x00; cave[p++] = 0x00; cave[p++] = 0x00; cave[p++] = 0x00;
    UINT64 nativeTrampoline = (UINT64)trampoline;
    memcpy(cave + p, &nativeTrampoline, sizeof(nativeTrampoline)); p += sizeof(nativeTrampoline);

    FlushInstructionCache((HANDLE)(INT64)-1, trampoline, trampolineJump);
    FlushInstructionCache((HANDLE)(INT64)-1, cave, p);
    g_nativeCoffinCandidateRegister = (CoffinCandidateRegisterNativeFn)trampoline;

    BYTE replacement[17] = {
        0xFF,0x25,0x00,0x00,0x00,0x00,0,0,0,0,0,0,0,0,
        0x90,0x90,0x90
    };
    UINT64 caveAddress = (UINT64)cave;
    memcpy(replacement + 6u, &caveAddress, sizeof(caveAddress));
    if (!freeze_for_patch(target, sizeof(replacement))) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(target, sizeof(replacement), PAGE_EXECUTE_READWRITE,
                        &oldProtection)) {
        unfreeze_after_patch(); return false;
    }
    memcpy(target, replacement, sizeof(replacement));
    FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(replacement));
    bool installed = bytes_equal(target, replacement, sizeof(replacement));
    if (!installed) {
        memcpy(target, expected, sizeof(expected));
        FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(expected));
    }
    DWORD ignored = 0; VirtualProtect(target, sizeof(replacement), oldProtection,
                                      &ignored);
    unfreeze_after_patch();
    if (installed) g_coffinCandidateRegistrationTraceInstalled = 1;
    return installed;
}

// 0x1618C0 is a generic entity-message dispatcher used by many systems.  A
// direct unfiltered observer would generate excessive traffic and add risk to
// save loading, so the logger returns before touching the log unless RCX is
// the exact Coffin manager candidate-container address captured above.
static void __fastcall log_coffin_candidate_message_dispatch(
    void* container, void* criticalSection, void* message, UINT64 mode) {
    UINT64 watched = __atomic_load_n(&g_coffinCandidateMessageContainer, __ATOMIC_ACQUIRE);
    if (!watched || (UINT64)container != watched) return;
    LONG index = __atomic_fetch_add(&g_coffinCandidateInsertTraceCalls, 1, __ATOMIC_ACQ_REL);
    if (index >= 64) return;
    UINT32 count = 0;
    UINT64 sortedEntries = 0;
    UINT64 dispatchEntries = 0;
    UINT64 messageVtable = 0;
    UINT32 messageId = 0;
    if (container && readable_range(container, 0x28u)) {
        count = *(UINT32*)container;
        sortedEntries = (UINT64)*(void**)((BYTE*)container + 8u);
        dispatchEntries = (UINT64)*(void**)((BYTE*)container + 0x18u);
    }
    if (message && readable_range(message, 0x18u)) {
        messageVtable = (UINT64)*(void**)message;
        messageId = *(UINT32*)((BYTE*)message + 0x10u);
    }
    LogBuffer log; log_init(&log); log_prefix(&log);
    log_text(&log, "COFFIN CANDIDATE MESSAGE #"); log_uint(&log, (UINT64)index);
    log_text(&log, ": container="); log_uint(&log, (UINT64)container);
    log_text(&log, ", count="); log_uint(&log, (UINT64)count);
    log_text(&log, ", sortedEntries="); log_uint(&log, sortedEntries);
    log_text(&log, ", dispatchEntries="); log_uint(&log, dispatchEntries);
    log_text(&log, ", criticalSection="); log_uint(&log, (UINT64)criticalSection);
    log_text(&log, ", message="); log_uint(&log, (UINT64)message);
    log_text(&log, ", messageVtable="); log_uint(&log, messageVtable);
    log_text(&log, ", messageId="); log_uint(&log, (UINT64)messageId);
    log_text(&log, ", mode="); log_uint(&log, mode);
    log_text(&log, ". Native message dispatch unchanged.\r\n"); append_log(&log);
}

static bool install_coffin_candidate_message_trace(HMODULE executable) {
    if (!g_enableCoffinCandidateInsertTrace) return true;
    BYTE* target = (BYTE*)executable + RVA_COFFIN_NATIVE_CANDIDATE_MESSAGE_DISPATCH;
    static const BYTE expected[26] = {
        0x48,0x89,0x54,0x24,0x10,0x57,0x48,0x83,0xEC,0x40,
        0x48,0x89,0x5C,0x24,0x50,0x48,0x8B,0xFA,0x48,0x89,
        0x74,0x24,0x68,0x48,0x8B,0xF1
    };
    if (!readable_range(target, sizeof(expected)) ||
        !bytes_equal(target, expected, sizeof(expected))) return false;

    BYTE* trampoline = (BYTE*)VirtualAlloc(0, 80u, MEM_COMMIT | MEM_RESERVE,
                                           PAGE_EXECUTE_READWRITE);
    BYTE* cave = (BYTE*)VirtualAlloc(0, 160u, MEM_COMMIT | MEM_RESERVE,
                                     PAGE_EXECUTE_READWRITE);
    if (!trampoline || !cave) return false;

    memcpy(trampoline, expected, sizeof(expected));
    SIZE_T trampolineSize = sizeof(expected);
    trampoline[trampolineSize++] = 0xFF; trampoline[trampolineSize++] = 0x25;
    trampoline[trampolineSize++] = 0x00; trampoline[trampolineSize++] = 0x00;
    trampoline[trampolineSize++] = 0x00; trampoline[trampolineSize++] = 0x00;
    UINT64 continuation = (UINT64)(target + sizeof(expected));
    memcpy(trampoline + trampolineSize, &continuation, sizeof(continuation));
    trampolineSize += sizeof(continuation);

    SIZE_T p = 0;
    const BYTE save[] = {
        0x50,0x51,0x52,0x41,0x50,0x41,0x51,0x41,0x52,0x41,0x53,
        0x57,0x56,0x53,0x55,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57
    };
    memcpy(cave + p, save, sizeof(save)); p += sizeof(save);
    cave[p++] = 0x48; cave[p++] = 0x83; cave[p++] = 0xEC; cave[p++] = 0x20;
    cave[p++] = 0x48; cave[p++] = 0xB8;
    UINT64 logger = (UINT64)&log_coffin_candidate_message_dispatch;
    memcpy(cave + p, &logger, sizeof(logger)); p += sizeof(logger);
    cave[p++] = 0xFF; cave[p++] = 0xD0;
    cave[p++] = 0x48; cave[p++] = 0x83; cave[p++] = 0xC4; cave[p++] = 0x20;
    const BYTE restore[] = {
        0x41,0x5F,0x41,0x5E,0x41,0x5D,0x41,0x5C,0x5D,0x5B,0x5E,
        0x5F,0x41,0x5B,0x41,0x5A,0x41,0x59,0x41,0x58,0x5A,0x59,0x58
    };
    memcpy(cave + p, restore, sizeof(restore)); p += sizeof(restore);
    cave[p++] = 0xFF; cave[p++] = 0x25;
    cave[p++] = 0x00; cave[p++] = 0x00; cave[p++] = 0x00; cave[p++] = 0x00;
    UINT64 nativeTrampoline = (UINT64)trampoline;
    memcpy(cave + p, &nativeTrampoline, sizeof(nativeTrampoline)); p += sizeof(nativeTrampoline);

    FlushInstructionCache((HANDLE)(INT64)-1, trampoline, trampolineSize);
    FlushInstructionCache((HANDLE)(INT64)-1, cave, p);
    BYTE replacement[26] = {
        0xFF,0x25,0x00,0x00,0x00,0x00,0,0,0,0,0,0,0,0,
        0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90
    };
    UINT64 caveAddress = (UINT64)cave;
    memcpy(replacement + 6u, &caveAddress, sizeof(caveAddress));
    if (!freeze_for_patch(target, sizeof(replacement))) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(target, sizeof(replacement), PAGE_EXECUTE_READWRITE,
                        &oldProtection)) {
        unfreeze_after_patch(); return false;
    }
    memcpy(target, replacement, sizeof(replacement));
    FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(replacement));
    bool installed = bytes_equal(target, replacement, sizeof(replacement));
    if (!installed) {
        memcpy(target, expected, sizeof(expected));
        FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(expected));
    }
    DWORD ignored = 0; VirtualProtect(target, sizeof(replacement), oldProtection, &ignored);
    unfreeze_after_patch();
    if (installed) g_coffinCandidateInsertTraceInstalled = 1;
    return installed;
}

// ED52C0 is the upstream event dispatcher that can reach ED4EF0.  Logging its
// event discriminator and action byte tells us whether the registration path
// is absent, filtered by event id, or stopped by its pad/object predicates.
// The detour is passive and restores every integer register before native
// execution.
[[maybe_unused]] static void __fastcall log_native_coffin_event_dispatch(
    void* context, void* eventRecord, UINT64 entryR8, UINT64 callerReturn) {
    UINT32 eventId = 0;
    if (eventRecord && readable_range((BYTE*)eventRecord + 0x10u, sizeof(UINT32))) {
        eventId = *(UINT32*)((BYTE*)eventRecord + 0x10u);
    }
    // Only the 0x4C87C18F branch can reach ED4EF0 in this DS2 build.  Keep
    // the probe cold for every other event so save/loading traffic is not
    // needlessly logged or decoded.
    if (eventId != 0x4C87C18Fu) return;

    LONG index = __atomic_fetch_add(&g_coffinEventDispatchCalls, 1, __ATOMIC_ACQ_REL);
    if (index >= 128) return;

    BYTE actionKey = 0;
    UINT32 recordFlag = 0;
    void* recordOwner = 0;
    void* binding = 0;
    void* bindingMeta = 0;
    BYTE bindingAction = 0;
    BYTE bindingType = 0xFFu;
    if (readable_range((BYTE*)eventRecord + 8u, sizeof(UINT32))) {
        recordFlag = *(UINT32*)((BYTE*)eventRecord + 8u);
    }
    if (readable_range((BYTE*)eventRecord + 0x98u, sizeof(BYTE))) {
        actionKey = *(BYTE*)((BYTE*)eventRecord + 0x98u);
    }
    if (readable_range((BYTE*)eventRecord + 0x90u, sizeof(void*))) {
        recordOwner = *(void**)((BYTE*)eventRecord + 0x90u);
    }
    if (recordOwner && readable_range((BYTE*)recordOwner + 0x98u, sizeof(void*))) {
        binding = *(void**)((BYTE*)recordOwner + 0x98u);
    }
    if (binding && readable_range(binding, 0x10u)) {
        bindingAction = *(BYTE*)binding;
        bindingMeta = *(void**)((BYTE*)binding + 8u);
    }
    if (bindingMeta && readable_range((BYTE*)bindingMeta + 0x10u, sizeof(BYTE))) {
        bindingType = *(BYTE*)((BYTE*)bindingMeta + 0x10u);
    }

    bool contextFlagOk = false;
    UINT64 contextFlags = 0;
    void* contextOwner = 0;
    if (context && readable_range((BYTE*)context + 0x48u, sizeof(void*))) {
        contextOwner = *(void**)((BYTE*)context + 0x48u);
    }
    if (contextOwner && readable_range((BYTE*)contextOwner + 0x98u, sizeof(UINT64))) {
        contextFlags = *(UINT64*)((BYTE*)contextOwner + 0x98u);
        contextFlagOk = ((contextFlags >> 1u) & 1u) != 0u;
    }

    bool actionKeyMatched = false;
    UINT32 actionRegistryCount = 0;
    void* matchedActionEntry = 0;
    BYTE* executableBase = (BYTE*)(g_executable ? g_executable : GetModuleHandleW(0));
    if (executableBase && readable_range(executableBase + RVA_NATIVE_EVENT_ACTION_REGISTRY_GLOBAL, sizeof(void*))) {
        void* globalRoot = *(void**)(executableBase + RVA_NATIVE_EVENT_ACTION_REGISTRY_GLOBAL);
        void* actionRegistry = 0;
        if (globalRoot && readable_range((BYTE*)globalRoot + 0x18u, sizeof(void*))) {
            actionRegistry = *(void**)((BYTE*)globalRoot + 0x18u);
        }
        if (actionRegistry && readable_range((BYTE*)actionRegistry + 0x38u, sizeof(void*)) &&
            readable_range((BYTE*)actionRegistry + 0x30u, sizeof(UINT32))) {
            actionRegistryCount = *(UINT32*)((BYTE*)actionRegistry + 0x30u);
            void** entries = *(void***)((BYTE*)actionRegistry + 0x38u);
            if (actionRegistryCount > 4096u) actionRegistryCount = 4096u;
            if (entries && actionRegistryCount && readable_range(entries, (SIZE_T)actionRegistryCount * sizeof(void*))) {
                for (UINT32 i = 0; i < actionRegistryCount; ++i) {
                    void* entry = entries[i];
                    if (entry && readable_range((BYTE*)entry + 0x20u, sizeof(BYTE)) &&
                        *(BYTE*)((BYTE*)entry + 0x20u) == actionKey) {
                        actionKeyMatched = true;
                        matchedActionEntry = entry;
                        break;
                    }
                }
            }
        }
    }

    UINT32 predictedStage = 6u;
    const char* stageText = "would-reach-ED4EF0";
    if (!contextFlagOk) { predictedStage = 1u; stageText = "context-flag"; }
    else if (!actionKeyMatched) { predictedStage = 2u; stageText = "action-key-table"; }
    else if (!recordOwner) { predictedStage = 3u; stageText = "record+0x90"; }
    else if (!binding) { predictedStage = 4u; stageText = "owner+0x98-binding"; }
    else if (bindingType != 0x01u && bindingType != 0x13u) {
        predictedStage = 5u; stageText = "binding-type";
    }

    UINT64 callerRva = callerReturn;
    if (executableBase && callerReturn >= (UINT64)executableBase) {
        callerRva = callerReturn - (UINT64)executableBase;
    }

    LogBuffer message; log_init(&message); log_prefix(&message);
    log_text(&message, "NATIVE 4C EVENT #"); log_uint(&message, (UINT64)index);
    log_text(&message, ": key="); log_uint(&message, (UINT64)actionKey);
    log_text(&message, ", stage="); log_uint(&message, (UINT64)predictedStage);
    log_text(&message, "("); log_text(&message, stageText); log_text(&message, ")");
    log_text(&message, ", callerRva="); log_uint(&message, callerRva);
    log_text(&message, ", context="); log_uint(&message, (UINT64)context);
    log_text(&message, ", contextOwner="); log_uint(&message, (UINT64)contextOwner);
    log_text(&message, ", contextFlags="); log_uint(&message, contextFlags);
    log_text(&message, ", contextFlagOk="); log_uint(&message, contextFlagOk ? 1u : 0u);
    log_text(&message, ", actionKeyMatched="); log_uint(&message, actionKeyMatched ? 1u : 0u);
    log_text(&message, ", registryCount="); log_uint(&message, (UINT64)actionRegistryCount);
    log_text(&message, ", matchedEntry="); log_uint(&message, (UINT64)matchedActionEntry);
    log_text(&message, ", recordFlag="); log_uint(&message, (UINT64)recordFlag);
    log_text(&message, ", recordOwner="); log_uint(&message, (UINT64)recordOwner);
    log_text(&message, ", binding="); log_uint(&message, (UINT64)binding);
    log_text(&message, ", bindingAction="); log_uint(&message, (UINT64)bindingAction);
    log_text(&message, ", bindingMeta="); log_uint(&message, (UINT64)bindingMeta);
    log_text(&message, ", bindingType="); log_uint(&message, (UINT64)bindingType);
    log_text(&message, ", entryR8="); log_uint(&message, entryR8);
    log_text(&message, ". Read-only prediction; native dispatch unchanged.\r\n");
    append_log(&message);
}

[[maybe_unused]] static bool install_coffin_native_event_dispatch_trace(HMODULE executable) {
    if (!g_enableCoffinCandidateCursorTrace) return true;
    BYTE* target = (BYTE*)executable + RVA_COFFIN_NATIVE_EVENT_DISPATCH;
    // Full 18-byte prologue.  The former 17-byte probe split the final
    // `lea rbp,[r11-0x48]` instruction before its B8 displacement byte and
    // therefore corrupted execution during save loading.
    static const BYTE expected[18] = {
        0x4C,0x8B,0xDC,0x49,0x89,0x53,0x10,0x49,0x89,
        0x4B,0x08,0x55,0x41,0x55,0x49,0x8D,0x6B,0xB8
    };
    if (!readable_range(target, sizeof(expected)) ||
        !bytes_equal(target, expected, sizeof(expected))) return false;

    BYTE* trampoline = (BYTE*)VirtualAlloc(0, 64u, MEM_COMMIT | MEM_RESERVE,
                                           PAGE_EXECUTE_READWRITE);
    BYTE* cave = (BYTE*)VirtualAlloc(0, 160u, MEM_COMMIT | MEM_RESERVE,
                                     PAGE_EXECUTE_READWRITE);
    if (!trampoline || !cave) return false;

    memcpy(trampoline, expected, sizeof(expected));
    SIZE_T trampolineJump = sizeof(expected);
    trampoline[trampolineJump++] = 0xFF; trampoline[trampolineJump++] = 0x25;
    trampoline[trampolineJump++] = 0x00; trampoline[trampolineJump++] = 0x00;
    trampoline[trampolineJump++] = 0x00; trampoline[trampolineJump++] = 0x00;
    UINT64 continuation = (UINT64)(target + sizeof(expected));
    memcpy(trampoline + trampolineJump, &continuation, sizeof(continuation));
    trampolineJump += sizeof(continuation);

    SIZE_T p = 0;
    const BYTE save[] = {
        0x50,0x51,0x52,0x41,0x50,0x41,0x51,0x41,0x52,0x41,0x53,
        0x57,0x56,0x53,0x55,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57
    };
    memcpy(cave + p, save, sizeof(save)); p += sizeof(save);
    // Fifteen pushes = 0x78 bytes.  The original caller return address is
    // therefore at [rsp+0x78]; expose it as the fourth logger argument.
    cave[p++] = 0x4C; cave[p++] = 0x8B; cave[p++] = 0x4C; cave[p++] = 0x24; cave[p++] = 0x78;
    cave[p++] = 0x48; cave[p++] = 0x83; cave[p++] = 0xEC; cave[p++] = 0x20;
    cave[p++] = 0x48; cave[p++] = 0xB8;
    UINT64 logger = (UINT64)&log_native_coffin_event_dispatch;
    memcpy(cave + p, &logger, sizeof(logger)); p += sizeof(logger);
    cave[p++] = 0xFF; cave[p++] = 0xD0;
    cave[p++] = 0x48; cave[p++] = 0x83; cave[p++] = 0xC4; cave[p++] = 0x20;
    const BYTE restore[] = {
        0x41,0x5F,0x41,0x5E,0x41,0x5D,0x41,0x5C,0x5D,0x5B,0x5E,
        0x5F,0x41,0x5B,0x41,0x5A,0x41,0x59,0x41,0x58,0x5A,0x59,0x58
    };
    memcpy(cave + p, restore, sizeof(restore)); p += sizeof(restore);
    cave[p++] = 0xFF; cave[p++] = 0x25;
    cave[p++] = 0x00; cave[p++] = 0x00; cave[p++] = 0x00; cave[p++] = 0x00;
    UINT64 nativeTrampoline = (UINT64)trampoline;
    memcpy(cave + p, &nativeTrampoline, sizeof(nativeTrampoline)); p += sizeof(nativeTrampoline);

    FlushInstructionCache((HANDLE)(INT64)-1, trampoline, trampolineJump);
    FlushInstructionCache((HANDLE)(INT64)-1, cave, p);
    BYTE replacement[18] = {
        0xFF,0x25,0x00,0x00,0x00,0x00,0,0,0,0,0,0,0,0,
        0x90,0x90,0x90,0x90
    };
    UINT64 caveAddress = (UINT64)cave;
    memcpy(replacement + 6u, &caveAddress, sizeof(caveAddress));
    if (!freeze_for_patch(target, sizeof(replacement))) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(target, sizeof(replacement), PAGE_EXECUTE_READWRITE,
                        &oldProtection)) {
        unfreeze_after_patch(); return false;
    }
    memcpy(target, replacement, sizeof(replacement));
    FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(replacement));
    bool installed = bytes_equal(target, replacement, sizeof(replacement));
    if (!installed) {
        memcpy(target, expected, sizeof(expected));
        FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(expected));
    }
    DWORD ignored = 0; VirtualProtect(target, sizeof(replacement), oldProtection, &ignored);
    unfreeze_after_patch();
    if (installed) g_coffinEventDispatchTraceInstalled = 1;
    return installed;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wframe-address"
// Passive one-shot lifecycle probe.  It preserves the native mount call and
// records only the actual caller and action vtable.  This is deliberately
// separate from the rejected E3/state-scope wrapper below: save/restore and
// voluntary mounting must first be distinguished without changing state.
static UINT64 __fastcall coffin_mount_caller_probe(void* action) {
    if (!g_nativeCoffinMountEntry) return 0;
    UINT64 caller = (UINT64)__builtin_return_address(0);
    UINT64 parentCaller = (UINT64)__builtin_return_address(1);
    UINT64 vtable = 0, playerContext = 0;
    UINT64 actionTargetBefore = ~0ull, actionTargetAfter = ~0ull;
    UINT64 interactionTargetBefore = ~0ull, interactionTargetAfter = ~0ull;
    UINT32 interactionStateBefore = 0xffffffffu, interactionStateAfter = 0xffffffffu;
    UINT32 flag11ABefore = 0xffffffffu, flag11AAfter = 0xffffffffu;
    UINT32 flagC20Before = 0xffffffffu, flagC20After = 0xffffffffu;

    if (action && readable_range(action, sizeof(void*))) vtable = (UINT64)*(void**)action;
    if (action && readable_range((BYTE*)action + 0x38u, sizeof(void*)))
        playerContext = (UINT64)*(void**)((BYTE*)action + 0x38u);
    if (action && readable_range((BYTE*)action + 0x150u, sizeof(UINT64)))
        actionTargetBefore = *(UINT64*)((BYTE*)action + 0x150u);
    if (action && readable_range((BYTE*)action + 0x11Au, sizeof(WORD)))
        flag11ABefore = *(WORD*)((BYTE*)action + 0x11Au);
    if (action && readable_range((BYTE*)action + 0xC20u, sizeof(BYTE)))
        flagC20Before = *(BYTE*)((BYTE*)action + 0xC20u);
    if (playerContext && readable_range((BYTE*)playerContext + 0x3970u, sizeof(BYTE)))
        interactionStateBefore = *(BYTE*)((BYTE*)playerContext + 0x3970u);
    if (playerContext && readable_range((BYTE*)playerContext + 0x39E0u, sizeof(UINT64)))
        interactionTargetBefore = *(UINT64*)((BYTE*)playerContext + 0x39E0u);

    UINT64 result = g_nativeCoffinMountEntry(action);

    if (action && readable_range((BYTE*)action + 0x150u, sizeof(UINT64)))
        actionTargetAfter = *(UINT64*)((BYTE*)action + 0x150u);
    if (action && readable_range((BYTE*)action + 0x11Au, sizeof(WORD)))
        flag11AAfter = *(WORD*)((BYTE*)action + 0x11Au);
    if (action && readable_range((BYTE*)action + 0xC20u, sizeof(BYTE)))
        flagC20After = *(BYTE*)((BYTE*)action + 0xC20u);
    if (playerContext && readable_range((BYTE*)playerContext + 0x3970u, sizeof(BYTE)))
        interactionStateAfter = *(BYTE*)((BYTE*)playerContext + 0x3970u);
    if (playerContext && readable_range((BYTE*)playerContext + 0x39E0u, sizeof(UINT64)))
        interactionTargetAfter = *(UINT64*)((BYTE*)playerContext + 0x39E0u);

    LONG index = __atomic_fetch_add(&g_coffinMountEntryCalls, 1, __ATOMIC_ACQ_REL);
    if (index < 64) {
        UINT64 executableBase = (UINT64)(g_executable ? g_executable : GetModuleHandleW(0));
        LogBuffer m; log_init(&m); log_prefix(&m);
        log_text(&m, "MOUNT NATIVE PROBE #"); log_uint(&m, (UINT64)index);
        log_text(&m, ": callerRva="); log_uint(&m, caller >= executableBase ? caller - executableBase : caller);
        log_text(&m, ", parentCallerRva="); log_uint(&m, parentCaller >= executableBase ? parentCaller - executableBase : parentCaller);
        log_text(&m, ", action="); log_uint(&m, (UINT64)action);
        log_text(&m, ", vtable="); log_uint(&m, vtable);
        log_text(&m, ", playerContext="); log_uint(&m, playerContext);
        log_text(&m, ", actionTarget="); log_uint(&m, actionTargetBefore); log_text(&m, "->"); log_uint(&m, actionTargetAfter);
        log_text(&m, ", pc3970="); log_uint(&m, interactionStateBefore); log_text(&m, "->"); log_uint(&m, interactionStateAfter);
        log_text(&m, ", pc39E0="); log_uint(&m, interactionTargetBefore); log_text(&m, "->"); log_uint(&m, interactionTargetAfter);
        log_text(&m, ", a11A="); log_uint(&m, flag11ABefore); log_text(&m, "->"); log_uint(&m, flag11AAfter);
        log_text(&m, ", aC20="); log_uint(&m, flagC20Before); log_text(&m, "->"); log_uint(&m, flagC20After);
        log_text(&m, ", nativeResult="); log_uint(&m, result);
        log_text(&m, ". READ-ONLY wrapper.\r\n"); append_log(&m);
    }
    return result;
}
#pragma clang diagnostic pop

typedef UINT64 (__fastcall* CoffinActionDispatchFn)(void*, void*, UINT32, void*);
static CoffinActionDispatchFn g_nativeCoffinActionDispatch = 0;

[[maybe_unused]] static bool coffin_voluntary_dispatch_scope() {
    UINT64 executableBase = (UINT64)(g_executable ? g_executable : GetModuleHandleW(0));
    return g_actionDispatchCountScope == 2u &&
        g_actionDispatchCallerScope == executableBase + 0x00EF4A7Fu;
}

// Exact passive probe for the shared action dispatcher.  The 14-byte native
// prologue is replayed in a trampoline; only the immediate caller is logged.
static UINT64 __fastcall coffin_action_dispatch_probe(void* actions, void* context,
                                                       UINT32 count, void* scratch) {
    if (!g_nativeCoffinActionDispatch) return 0;
    UINT64 caller = (UINT64)__builtin_return_address(0);
    UINT32 previousCount = g_actionDispatchCountScope;
    UINT64 previousCaller = g_actionDispatchCallerScope;
    g_actionDispatchCountScope = count;
    g_actionDispatchCallerScope = caller;
    UINT64 result = g_nativeCoffinActionDispatch(actions, context, count, scratch);
    g_actionDispatchCountScope = previousCount;
    g_actionDispatchCallerScope = previousCaller;
    LONG index = __atomic_fetch_add(&g_coffinActionDispatchCalls, 1, __ATOMIC_ACQ_REL);
    if (index < 64) {
        UINT64 executableBase = (UINT64)(g_executable ? g_executable : GetModuleHandleW(0));
        LogBuffer message; log_init(&message); log_prefix(&message);
        log_text(&message, "ACTION DISPATCH PROBE #"); log_uint(&message, (UINT64)index);
        log_text(&message, ": callerRva=");
        log_uint(&message, caller >= executableBase ? caller - executableBase : caller);
        log_text(&message, ", actions="); log_uint(&message, (UINT64)actions);
        log_text(&message, ", count="); log_uint(&message, count);
        log_text(&message, ", nativeResult="); log_uint(&message, result);
        log_text(&message, ".\r\n"); append_log(&message);
    }
    return result;
}

[[maybe_unused]] static bool install_coffin_action_dispatch_probe(HMODULE executable) {
    if (!g_enableMountCallerTrace && !g_enableNetworkTraversal && !g_enableOutsideNetworkMount) return true;
    BYTE* target = (BYTE*)executable + RVA_COFFIN_ACTION_DISPATCH;
    static const BYTE expected[14] = {
        0x41,0x83,0xF8,0x01,0x0F,0x86,0x19,0x03,0x00,0x00,
        0x53,0x55,0x41,0x54
    };
    if (!readable_range(target, sizeof(expected)) || !bytes_equal(target, expected, sizeof(expected))) return false;
    BYTE* trampoline = (BYTE*)VirtualAlloc(0, 28u, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!trampoline) return false;
    memcpy(trampoline, expected, sizeof(expected));
    static const BYTE absoluteJump[6] = {0xFF,0x25,0x00,0x00,0x00,0x00};
    memcpy(trampoline + 14u, absoluteJump, sizeof(absoluteJump));
    UINT64 continuation = (UINT64)(target + sizeof(expected));
    memcpy(trampoline + 20u, &continuation, sizeof(continuation));
    FlushInstructionCache((HANDLE)(INT64)-1, trampoline, 28u);
    g_nativeCoffinActionDispatch = (CoffinActionDispatchFn)trampoline;

    BYTE replacement[14] = {0xFF,0x25,0x00,0x00,0x00,0x00,0,0,0,0,0,0,0,0};
    UINT64 probe = (UINT64)&coffin_action_dispatch_probe;
    memcpy(replacement + 6u, &probe, sizeof(probe));
    if (!freeze_for_patch(target, sizeof(replacement))) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(target, sizeof(replacement), PAGE_EXECUTE_READWRITE, &oldProtection)) {
        unfreeze_after_patch(); return false;
    }
    memcpy(target, replacement, sizeof(replacement));
    FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(replacement));
    bool installed = bytes_equal(target, replacement, sizeof(replacement));
    if (!installed) {
        memcpy(target, expected, sizeof(expected));
        FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(expected));
    }
    DWORD ignored = 0; VirtualProtect(target, sizeof(replacement), oldProtection, &ignored);
    unfreeze_after_patch();
    if (installed) g_coffinActionDispatchProbeInstalled = 1;
    return installed;
}

// Passive wrapper around the Coffin's own mount entry.  It records the two
// immediate return addresses and preserves the native call and result exactly.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wframe-address"
static UINT64 __fastcall coffin_mount_entry_trace(void* action) {
    if (!g_nativeCoffinMountEntry) return 0;
    UINT64 caller = (UINT64)__builtin_return_address(0);
    UINT64 parentCaller = (UINT64)__builtin_return_address(1);
    UINT64 playerContext = 0;
    UINT64 actionTargetBefore = ~0ull, actionTargetAfter = ~0ull;
    UINT64 interactionTargetBefore = ~0ull, interactionTargetAfter = ~0ull;
    UINT32 interactionStateBefore = 0xffffffffu, interactionStateAfter = 0xffffffffu;

    if (action && readable_range((BYTE*)action + 0x38u, sizeof(void*)))
        playerContext = (UINT64)*(void**)((BYTE*)action + 0x38u);
    if (action && readable_range((BYTE*)action + 0x150u, sizeof(UINT64)))
        actionTargetBefore = *(UINT64*)((BYTE*)action + 0x150u);
    if (playerContext && readable_range((BYTE*)playerContext + 0x3970u, sizeof(BYTE)))
        interactionStateBefore = *(BYTE*)((BYTE*)playerContext + 0x3970u);
    if (playerContext && readable_range((BYTE*)playerContext + 0x39E0u, sizeof(UINT64)))
        interactionTargetBefore = *(UINT64*)((BYTE*)playerContext + 0x39E0u);

    UINT64 result = g_nativeCoffinMountEntry(action);

    if (action && readable_range((BYTE*)action + 0x150u, sizeof(UINT64)))
        actionTargetAfter = *(UINT64*)((BYTE*)action + 0x150u);
    if (playerContext && readable_range((BYTE*)playerContext + 0x3970u, sizeof(BYTE)))
        interactionStateAfter = *(BYTE*)((BYTE*)playerContext + 0x3970u);
    if (playerContext && readable_range((BYTE*)playerContext + 0x39E0u, sizeof(UINT64)))
        interactionTargetAfter = *(UINT64*)((BYTE*)playerContext + 0x39E0u);

    bool resolvedCoffin = false;
    UINT64 resolvedVehicle = 0;
    if ((result & 0xFFu) != 0u && action && actionTargetAfter != 0ull && actionTargetAfter != ~0ull && g_executable) {
        typedef void* (__fastcall* ResolveFn)(void*, UINT64*);
        ResolveFn resolve = (ResolveFn)((BYTE*)g_executable + RVA_V161_ACTION_VEHICLE_RESOLVE);
        UINT64 localHandle = actionTargetAfter;
        void* vehicle = resolve(action, &localHandle);
        resolvedVehicle = (UINT64)vehicle;
        resolvedCoffin = vehicle && readable_range(vehicle, sizeof(void*)) &&
            *(void**)vehicle == (void*)((BYTE*)g_executable + RVA_V161_COFFIN_VEHICLE_VTABLE);
        if (resolvedCoffin && playerContext) {
            __atomic_store_n(&g_nativeMountTarget, actionTargetAfter, __ATOMIC_RELEASE);
            __atomic_store_n(&g_nativeMountPlayerContext, playerContext, __ATOMIC_RELEASE);
            __atomic_fetch_add(&g_nativeMountTargetCaptures, 1, __ATOMIC_ACQ_REL);
        }
    }

    LONG mountIndex = __atomic_fetch_add(&g_coffinMountEntryCalls, 1, __ATOMIC_ACQ_REL);
    if (mountIndex < 64 || resolvedCoffin) {
        UINT64 executableBase = (UINT64)(g_executable ? g_executable : GetModuleHandleW(0));
        LogBuffer message; log_init(&message); log_prefix(&message);
        log_text(&message, "V163 MOUNT ENTRY #"); log_uint(&message, (UINT64)mountIndex);
        log_text(&message, ": callerRva=");
        log_uint(&message, caller >= executableBase ? caller - executableBase : caller);
        log_text(&message, ", parentCallerRva=");
        log_uint(&message, parentCaller >= executableBase ? parentCaller - executableBase : parentCaller);
        log_text(&message, ", action="); log_uint(&message, (UINT64)action);
        log_text(&message, ", nativeResult="); log_uint(&message, result & 0xFFu);
        log_text(&message, ", actionTarget="); log_uint(&message, actionTargetBefore); log_text(&message, "->"); log_uint(&message, actionTargetAfter);
        log_text(&message, ", pc3970="); log_uint(&message, interactionStateBefore); log_text(&message, "->"); log_uint(&message, interactionStateAfter);
        log_text(&message, ", pc39E0="); log_uint(&message, interactionTargetBefore); log_text(&message, "->"); log_uint(&message, interactionTargetAfter);
        log_text(&message, ", resolvedVehicle="); log_uint(&message, resolvedVehicle);
        log_text(&message, ", resolvedCoffin="); log_uint(&message, resolvedCoffin ? 1u : 0u);
        log_text(&message, ". Native result preserved; capture only.\r\n"); append_log(&message);
    }
    return result;
}
#pragma clang diagnostic pop

static BYTE __fastcall v163_interaction_gate_scope(void* manager, void* action) {
    BYTE nativeResult = g_nativeV163InteractionGate ? g_nativeV163InteractionGate(manager) : 1u;
    BYTE* packet = 0;
    BYTE* playerContext = 0;
    UINT64 packetTarget = ~0ull;
    UINT64 cachedTarget = __atomic_load_n(&g_nativeMountTarget, __ATOMIC_ACQUIRE);
    UINT64 cachedContext = __atomic_load_n(&g_nativeMountPlayerContext, __ATOMIC_ACQUIRE);
    UINT64 resolvedVehicle = 0;
    bool sameNativeTarget = false;
    bool resolvedCoffin = false;
    bool bypass = false;

    if (action && readable_range((BYTE*)action + 0xC28u, sizeof(void*)))
        packet = *(BYTE**)((BYTE*)action + 0xC28u);
    if (action && readable_range((BYTE*)action + 0x38u, sizeof(void*)))
        playerContext = *(BYTE**)((BYTE*)action + 0x38u);
    if (packet && readable_range(packet + 0x620u, sizeof(UINT64)))
        packetTarget = *(UINT64*)(packet + 0x620u);

    sameNativeTarget = packetTarget != 0ull && packetTarget != ~0ull &&
        cachedTarget != 0ull && cachedTarget != ~0ull &&
        packetTarget == cachedTarget && playerContext && (UINT64)playerContext == cachedContext;

    if (nativeResult != 0u && g_enableOutsideNetworkMount &&
        __atomic_load_n(&g_v151QpidHits, __ATOMIC_ACQUIRE) > 0 && sameNativeTarget && g_executable) {
        typedef void* (__fastcall* ResolveFn)(void*, UINT64*);
        ResolveFn resolve = (ResolveFn)((BYTE*)g_executable + RVA_V161_ACTION_VEHICLE_RESOLVE);
        UINT64 localHandle = packetTarget;
        void* vehicle = resolve(action, &localHandle);
        resolvedVehicle = (UINT64)vehicle;
        resolvedCoffin = vehicle && readable_range(vehicle, sizeof(void*)) &&
            *(void**)vehicle == (void*)((BYTE*)g_executable + RVA_V161_COFFIN_VEHICLE_VTABLE);
        if (resolvedCoffin) {
            bypass = true;
            __atomic_fetch_add(&g_v163InteractionGateBypasses, 1, __ATOMIC_ACQ_REL);
        }
    }

    LONG idx = __atomic_fetch_add(&g_v163InteractionGateCalls, 1, __ATOMIC_ACQ_REL);
    if (idx < 64 || bypass) {
        LogBuffer m; log_init(&m); log_prefix(&m);
        log_text(&m, "V163 DIRECT INTERACTION GATE #"); log_uint(&m, (UINT64)idx);
        log_text(&m, ": nativeGate="); log_uint(&m, nativeResult);
        log_text(&m, ", qpidHits="); log_uint(&m, (UINT64)__atomic_load_n(&g_v151QpidHits, __ATOMIC_ACQUIRE));
        log_text(&m, ", action="); log_uint(&m, (UINT64)action);
        log_text(&m, ", packet="); log_uint(&m, (UINT64)packet);
        log_text(&m, ", packetTarget="); log_uint(&m, packetTarget);
        log_text(&m, ", cachedTarget="); log_uint(&m, cachedTarget);
        log_text(&m, ", sameNativeTarget="); log_uint(&m, sameNativeTarget ? 1u : 0u);
        log_text(&m, ", resolvedVehicle="); log_uint(&m, resolvedVehicle);
        log_text(&m, ", resolvedCoffin="); log_uint(&m, resolvedCoffin ? 1u : 0u);
        log_text(&m, ", bypass="); log_uint(&m, bypass ? 1u : 0u);
        log_text(&m, bypass ? ". Native FFFBC0 branch allowed; E5B3F0 remains game-owned.\r\n" : ". Native gate preserved.\r\n");
        append_log(&m);
    }
    return bypass ? 0u : nativeResult;
}

[[maybe_unused]] static bool install_coffin_mount_entry_trace(HMODULE executable) {
    if (!g_enableNetworkTraversal && !g_enableOutsideNetworkMount && !g_enableMountCallerTrace) return true;
    BYTE* base = (BYTE*)executable;
    void** vtable = (void**)(base + RVA_COFFIN_RIDE_FLOATER_ACTION_VTABLE);
    void** target = (void**)(base + RVA_COFFIN_RIDE_FLOATER_ACTION_VTABLE + 8u);
    BYTE* entry = base + RVA_COFFIN_RIDE_FLOATER_MOUNT_ENTRY;
    static const BYTE expectedEntry[20] = {
        0x40,0x53,0x48,0x81,0xEC,0x90,0x00,0x00,0x00,
        0x48,0x8B,0x05,0xC8,0xEC,0x23,0x05,0x48,0x8B,0xD9,0x80
    };
    if (!readable_range(vtable, 2u * sizeof(void*)) ||
        !readable_range(target, sizeof(void*)) ||
        vtable[1] != (void*)entry ||
        !readable_range(entry, sizeof(expectedEntry)) ||
        !bytes_equal(entry, expectedEntry, sizeof(expectedEntry)) ||
        !readable_range(base + RVA_COFFIN_NETWORK_REGISTER_E3, 32u)) return false;
    g_nativeCoffinMountEntry = (CoffinMountEntryFn)*target;
    g_nativeCoffinE3Register = (CoffinE3RegisterFn)(base + RVA_COFFIN_NETWORK_REGISTER_E3);
    if (g_enableMountCallerTrace && !g_enableNetworkTraversal && !g_enableOutsideNetworkMount) {
        if (!freeze_for_patch((const BYTE*)target, sizeof(void*))) return false;
        DWORD probeProtection = 0;
        if (!VirtualProtect(target, sizeof(void*), PAGE_EXECUTE_READWRITE, &probeProtection)) {
            unfreeze_after_patch(); return false;
        }
        *target = (void*)&coffin_mount_caller_probe;
        FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(void*));
        bool probeInstalled = *target == (void*)&coffin_mount_caller_probe;
        if (!probeInstalled) *target = (void*)g_nativeCoffinMountEntry;
        DWORD probeIgnored = 0; VirtualProtect(target, sizeof(void*), probeProtection, &probeIgnored);
        unfreeze_after_patch();
        if (probeInstalled) g_coffinMountEntryTraceInstalled = 1;
        return probeInstalled;
    }
    if (!freeze_for_patch((const BYTE*)target, sizeof(void*))) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(target, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtection)) {
        unfreeze_after_patch(); return false;
    }
    *target = (void*)&coffin_mount_entry_trace;
    FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(void*));
    bool installed = *target == (void*)&coffin_mount_entry_trace;
    if (!installed) *target = (void*)g_nativeCoffinMountEntry;
    DWORD ignored = 0; VirtualProtect(target, sizeof(void*), oldProtection, &ignored);
    unfreeze_after_patch();
    if (installed) g_coffinMountEntryTraceInstalled = 1;
    return installed;
}

// v1.1.7 FIX: replay the original RideVehicle resolver block *before* any
// Coffin-only inspection. The native direct call did not clobber RAX on entry;
// older carrier shims used "mov rax,resolver / call rax", which changed the
// resolver's incoming register state for every vehicle. That is the key
// behavioural difference removed here.
//
// For non-Coffin vehicles the observable state at RVA 0xF9A81C is now native:
//   - mov rcx,[rdi+0xA8] is replayed first
//   - resolver is called exactly once with no GPR used as an address scratch
//   - RAX is the real resolver return value
//   - R10/R11/RSP are restored after the Coffin discriminator
//   - flags are restored to the native "test rax,rax" fall-through state
//
// Runtime qwords: resolver +19, native-skip +38, state +81,
// coffin-skip +143, native-continuation +164.
// v1.76: preserve a linked object only when it is a real DSBaggageCarrierComponent.
// This prevents an arbitrary strand/tether target (for example Sam) from being
// promoted to the "carrier" compatibility path merely because a Coffin is active.
static const BYTE CARRIER_LINK_PRESERVE_TEMPLATE[249] = {
    0xF3,0x0F,0x1E,0xFA,0x48,0x8B,0x8F,0xA8,0x00,0x00,0x00,0xFF,0x15,0x02,0x00,0x00,0x00,0xEB,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x48,0x85,0xC0,0x0F,0x84,0xC7,0x00,0x00,0x00,0x4C,0x8B,0x97,0x90,0x01,0x00,0x00,0x4D,0x85,0xD2,0x0F,0x84,0xA6,0x00,0x00,0x00,0x41,0x83,0xBA,0xA0,0x02,0x00,0x00,0x03,0x0F,0x85,0x98,0x00,0x00,0x00,0x48,0x83,0xEC,0x30,0x48,0x89,0x44,0x24,0x20,0x48,0x8D,0x88,0xA0,0x00,0x00,0x00,0x48,0xBA,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x15,0x02,0x00,0x00,0x00,0xEB,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x48,0x85,0xC0,0x0F,0x84,0x5C,0x00,0x00,0x00,0x48,0x8B,0x44,0x24,0x20,0x48,0x83,0xC4,0x30,0x41,0x52,0x41,0x53,0x48,0x8B,0x8F,0xA8,0x00,0x00,0x00,0x49,0xBA,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x49,0xC7,0x02,0x00,0x00,0x00,0x00,0x49,0xC7,0x42,0x18,0x00,0x00,0x00,0x00,0x49,0x89,0x4A,0x08,0x4C,0x8B,0x9E,0x20,0x03,0x00,0x00,0x4D,0x89,0x5A,0x10,0x4C,0x8B,0x98,0x20,0x03,0x00,0x00,0x4D,0x89,0x5A,0x18,0x49,0x89,0x02,0x41,0x5B,0x41,0x5A,0xFF,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x48,0x8B,0x44,0x24,0x20,0x48,0x83,0xC4,0x30,0x48,0x85,0xC0,0xFF,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
static bool install_floating_carrier_link_preserve(HMODULE executable) {
    if (!g_allowFloatingCarrier) return true;
    BYTE* base = (BYTE*)executable;
    BYTE* target = base + RVA_COFFIN_PRESERVE_CARRIER_HOOK;
    static const BYTE expected[21] = {
        0x48,0x8B,0x8F,0xA8,0x00,0x00,0x00,
        0xE8,0xDD,0xBA,0xEA,0xFF,
        0x48,0x85,0xC0,
        0x0F,0x84,0x4F,0x04,0x00,0x00
    };
    if (!readable_range(target, sizeof(expected)) ||
        !bytes_equal(target, expected, sizeof(expected))) return false;
    BYTE* cave = (BYTE*)VirtualAlloc(0, sizeof(CARRIER_LINK_PRESERVE_TEMPLATE),
        MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!cave) return false;
    memcpy(cave, CARRIER_LINK_PRESERVE_TEMPLATE, sizeof(CARRIER_LINK_PRESERVE_TEMPLATE));
    UINT64 resolver=(UINT64)(base+RVA_RESOLVE_LINKED_CARRIER);
    UINT64 typeDesc=(UINT64)(base+RVA_BAGGAGE_CARRIER_COMPONENT_TYPE);
    UINT64 lookup=(UINT64)(base+RVA_COMPONENT_LOOKUP);
    UINT64 stateAddress=(UINT64)&g_carrierRideLinkState;
    UINT64 coffinSkip=(UINT64)(base+RVA_COFFIN_PRESERVE_CARRIER_SKIP);
    UINT64 continuation=(UINT64)(base+RVA_COFFIN_PRESERVE_CARRIER_CONTINUE);
    UINT64 nativeSkip=(UINT64)(base+RVA_COFFIN_PRESERVE_CARRIER_SKIP);
    memcpy(cave+19u,&resolver,8); memcpy(cave+84u,&typeDesc,8); memcpy(cave+100u,&lookup,8);
    memcpy(cave+139u,&stateAddress,8); memcpy(cave+201u,&coffinSkip,8);
    memcpy(cave+227u,&continuation,8); memcpy(cave+241u,&nativeSkip,8);
    FlushInstructionCache((HANDLE)(INT64)-1,cave,sizeof(CARRIER_LINK_PRESERVE_TEMPLATE));
    BYTE replacement[21]={0xFF,0x25,0,0,0,0,0,0,0,0,0,0,0,0,0x90,0x90,0x90,0x90,0x90,0x90,0x90};
    UINT64 caveAddress=(UINT64)cave; memcpy(replacement+6u,&caveAddress,8);
    if(!freeze_for_patch(target,sizeof(replacement))) return false; DWORD oldProtection=0;
    if(!VirtualProtect(target,sizeof(replacement),PAGE_EXECUTE_READWRITE,&oldProtection)){unfreeze_after_patch();return false;}
    memcpy(target,replacement,sizeof(replacement)); FlushInstructionCache((HANDLE)(INT64)-1,target,sizeof(replacement));
    bool installed=bytes_equal(target,replacement,sizeof(replacement));
    if(!installed){memcpy(target,expected,sizeof(expected));FlushInstructionCache((HANDLE)(INT64)-1,target,sizeof(expected));}
    DWORD ignored=0;VirtualProtect(target,sizeof(replacement),oldProtection,&ignored);unfreeze_after_patch();
    if(!installed)return false; g_carrierLinkPreserveInstalled=1; return true;
}

static const BYTE CARRIER_DETACH_GUARD_TEMPLATE[124] = {
    0xF3,0x0F,0x1E,0xFA, 0x8B,0x42,0x10, 0x49,0x89,0xD5, 0x4C,0x8B,0x61,0x48,
    0x48,0x89,0xCE, 0x3D,0xE0,0x4F,0x75,0x13, 0x75,0x46,
    0x49,0xBA, 0,0,0,0,0,0,0,0, 0x49,0x39,0x72,0x08, 0x75,0x30,
    0x4D,0x8B,0x5A,0x18, 0x4D,0x85,0xDB, 0x74,0x27, 0x49,0x83,0xFB,0xFF, 0x74,0x21,
    0x4C,0x39,0x9E,0x60,0x0C,0x00,0x00, 0x75,0x18, 0x49,0x8B,0x02, 0x48,0x85,0xC0,
    0x74,0x10, 0x48,0x83,0xBE,0x68,0x0C,0x00,0x00,0xFF, 0x74,0x06,
    0xFF,0x25,0x0C,0x00,0x00,0x00, 0xFF,0x25,0x0E,0x00,0x00,0x00, 0xFF,0x25,0x10,0x00,0x00,0x00,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0
};
static bool install_floating_carrier_detach_guard(HMODULE executable) {
    if (!g_allowFloatingCarrier) return true;
    BYTE* base = (BYTE*)executable; BYTE* target = base + RVA_CARRIER_DETACH_EVENT_GATE;
    static const BYTE expected[24] = {0x8B,0x42,0x10,0x4C,0x8B,0xEA,0x4C,0x8B,0x61,0x48,0x48,0x8B,0xF1,0x3D,0xE0,0x4F,0x75,0x13,0x0F,0x84,0x11,0x06,0x00,0x00};
    if (!readable_range(target, sizeof(expected)) || !bytes_equal(target, expected, sizeof(expected))) return false;
    BYTE* cave = (BYTE*)VirtualAlloc(0, sizeof(CARRIER_DETACH_GUARD_TEMPLATE), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!cave) return false;
    memcpy(cave, CARRIER_DETACH_GUARD_TEMPLATE, sizeof(CARRIER_DETACH_GUARD_TEMPLATE));
    UINT64 stateAddress=(UINT64)&g_carrierRideLinkState, suppressed=(UINT64)(base+RVA_CARRIER_DETACH_SUPPRESSED), nativeDetach=(UINT64)(base+RVA_CARRIER_DETACH_NATIVE), otherEvent=(UINT64)(base+RVA_CARRIER_DETACH_OTHER_EVENT);
    memcpy(cave+26u,&stateAddress,8); memcpy(cave+100u,&suppressed,8); memcpy(cave+108u,&nativeDetach,8); memcpy(cave+116u,&otherEvent,8);
    FlushInstructionCache((HANDLE)(INT64)-1,cave,sizeof(CARRIER_DETACH_GUARD_TEMPLATE));
    BYTE replacement[24]={0xFF,0x25,0,0,0,0,0,0,0,0,0,0,0,0,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90};
    UINT64 caveAddress=(UINT64)cave; memcpy(replacement+6u,&caveAddress,8);
    if (!freeze_for_patch(target,sizeof(replacement))) return false;
    DWORD oldProtection=0; if(!VirtualProtect(target,sizeof(replacement),PAGE_EXECUTE_READWRITE,&oldProtection)){unfreeze_after_patch();return false;}
    memcpy(target,replacement,sizeof(replacement)); FlushInstructionCache((HANDLE)(INT64)-1,target,sizeof(replacement));
    bool installed=bytes_equal(target,replacement,sizeof(replacement)); if(!installed){memcpy(target,expected,sizeof(expected));FlushInstructionCache((HANDLE)(INT64)-1,target,sizeof(expected));}
    DWORD ignored=0; VirtualProtect(target,sizeof(replacement),oldProtection,&ignored); unfreeze_after_patch();
    if(!installed)return false; g_carrierDetachGuardInstalled=1; return true;
}

static const BYTE CARRIER_WARNING_GUARD_TEMPLATE[170] = {
    0xF3,0x0F,0x1E,0xFA,0x50,0x41,0x52,0x41,0x53,0xF0,0xFF,0x05,0x92,0x00,0x00,0x00,
    0x49,0xBA,0,0,0,0,0,0,0,0,0x49,0x8B,0x02,0x48,0x85,0xC0,0x74,0x54,
    0x49,0x8B,0x42,0x08,0x48,0x85,0xC0,0x74,0x4B,0x4D,0x8B,0x5A,0x18,0x4D,0x85,0xDB,0x74,0x42,
    0x49,0x83,0xFB,0xFF,0x74,0x3C,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,
    0x4C,0x39,0x98,0x60,0x0C,0x00,0x00,0x75,0x2A,0x4D,0x8B,0x5A,0x10,0x4D,0x85,0xDB,0x74,0x21,
    0x49,0x83,0xFB,0xFF,0x74,0x1B,0x4C,0x39,0x98,0x68,0x0C,0x00,0x00,0x75,0x12,
    0xF0,0xFF,0x05,0x3B,0x00,0x00,0x00,0x41,0x5B,0x41,0x5A,0x58,0xFF,0x25,0x24,0x00,0x00,0x00,
    0x41,0x5B,0x41,0x5A,0x58,0xC7,0x44,0x24,0x50,0xB0,0x9B,0x66,0x16,0x89,0x44,0x24,0x58,
    0x49,0xBB,0,0,0,0,0,0,0,0,0x41,0xFF,0xD3,0xFF,0x25,0x00,0x00,0x00,0x00,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};
static bool install_floating_carrier_warning_guard(HMODULE executable) {
    if (!g_allowFloatingCarrier) return true;
    BYTE* base=(BYTE*)executable; BYTE* target=base+RVA_CARRIER_WARNING_NOTIFICATION_GATE;
    static const BYTE expected[17]={0xC7,0x44,0x24,0x50,0xB0,0x9B,0x66,0x16,0x89,0x44,0x24,0x58,0xE8,0x16,0x34,0xF1,0xFE};
    if(!readable_range(target,sizeof(expected))||!bytes_equal(target,expected,sizeof(expected)))return false;
    BYTE* cave=(BYTE*)VirtualAlloc(0,sizeof(CARRIER_WARNING_GUARD_TEMPLATE),MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE); if(!cave)return false;
    memcpy(cave,CARRIER_WARNING_GUARD_TEMPLATE,sizeof(CARRIER_WARNING_GUARD_TEMPLATE));
    UINT64 stateAddress=(UINT64)&g_carrierRideLinkState, dispatcher=(UINT64)(base+RVA_ENTITY_MESSAGE_DISPATCH), continuation=(UINT64)(base+RVA_CARRIER_WARNING_NOTIFICATION_CONTINUE);
    memcpy(cave+18u,&stateAddress,8); memcpy(cave+137u,&dispatcher,8); memcpy(cave+154u,&continuation,8);
    FlushInstructionCache((HANDLE)(INT64)-1,cave,sizeof(CARRIER_WARNING_GUARD_TEMPLATE));
    BYTE replacement[17]={0xFF,0x25,0,0,0,0,0,0,0,0,0,0,0,0,0x90,0x90,0x90}; UINT64 caveAddress=(UINT64)cave; memcpy(replacement+6u,&caveAddress,8);
    if(!freeze_for_patch(target,sizeof(replacement)))return false; DWORD oldProtection=0;
    if(!VirtualProtect(target,sizeof(replacement),PAGE_EXECUTE_READWRITE,&oldProtection)){unfreeze_after_patch();return false;}
    memcpy(target,replacement,sizeof(replacement));FlushInstructionCache((HANDLE)(INT64)-1,target,sizeof(replacement)); bool installed=bytes_equal(target,replacement,sizeof(replacement));
    if(!installed){memcpy(target,expected,sizeof(expected));FlushInstructionCache((HANDLE)(INT64)-1,target,sizeof(expected));}
    DWORD ignored=0;VirtualProtect(target,sizeof(replacement),oldProtection,&ignored);unfreeze_after_patch(); if(!installed)return false;
    g_carrierWarningProducerHits=(volatile LONG*)(cave+162u);g_carrierWarningSuppressedHits=(volatile LONG*)(cave+166u);g_carrierWarningGuardInstalled=1;return true;
}
static const BYTE CARRIER_STRICT_GATE_TEMPLATE[187] = { 0xF3,0x0F,0x1E,0xFA,0x41,0x8D,0x45,0xFF,0x83,0xF8,0x01,0x0F,0x86,0x8E,0x00,0x00,0x00,0x41,0x83,0xFD,0x03,0x0F,0x85,0x62,0x00,0x00,0x00,0x49,0x8B,0x4E,0x38,0xFF,0x15,0x02,0x00,0x00,0x00,0xEB,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x48,0x85,0xC0,0x0F,0x84,0x67,0x00,0x00,0x00,0x48,0x83,0xEC,0x30,0x48,0x89,0x44,0x24,0x20,0x48,0x8D,0x88,0xA0,0x00,0x00,0x00,0x48,0xBA,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x15,0x02,0x00,0x00,0x00,0xEB,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x48,0x85,0xC0,0x0F,0x84,0x09,0x00,0x00,0x00,0x48,0x83,0xC4,0x30,0xE9,0x2B,0x00,0x00,0x00,0x48,0x83,0xC4,0x30,0xE9,0x30,0x00,0x00,0x00,0x49,0x8B,0x4E,0x38,0xFF,0x15,0x02,0x00,0x00,0x00,0xEB,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x48,0x85,0xC0,0x0F,0x84,0x05,0x00,0x00,0x00,0xE9,0x0E,0x00,0x00,0x00,0xFF,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
static bool install_floating_carrier_gate(HMODULE executable) {
    if(!g_allowFloatingCarrier)return true;
    BYTE* base=(BYTE*)executable; BYTE* target=base+(RVA_COFFIN_LINKED_OBJECT_TYPE_LIMIT-6u);
    static const BYTE expected[18]={0x41,0x8D,0x45,0xFF,0x83,0xF8,0x01,0x76,0x49,0x49,0x8B,0x4E,0x38,0xE8,0x50,0xE5,0xE3,0xFF};
    if(!readable_range(target,sizeof(expected))||!bytes_equal(target,expected,sizeof(expected)))return false;
    BYTE* cave=(BYTE*)VirtualAlloc(0,sizeof(CARRIER_STRICT_GATE_TEMPLATE),MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE); if(!cave)return false;
    memcpy(cave,CARRIER_STRICT_GATE_TEMPLATE,sizeof(CARRIER_STRICT_GATE_TEMPLATE));
    UINT64 resolver=(UINT64)(base+RVA_RESOLVE_LINKED_CARRIER);
    UINT64 typeDesc=(UINT64)(base+RVA_BAGGAGE_CARRIER_COMPONENT_TYPE);
    UINT64 lookup=(UINT64)(base+RVA_COMPONENT_LOOKUP);
    UINT64 allowAddr=(UINT64)(base+0x01007DE0u);
    UINT64 rejectAddr=(UINT64)(base+0x01007DA5u);
    memcpy(cave+39u,&resolver,8); memcpy(cave+74u,&typeDesc,8); memcpy(cave+90u,&lookup,8);
    memcpy(cave+137u,&resolver,8); memcpy(cave+165u,&allowAddr,8); memcpy(cave+179u,&rejectAddr,8);
    FlushInstructionCache((HANDLE)(INT64)-1,cave,sizeof(CARRIER_STRICT_GATE_TEMPLATE));
    BYTE replacement[18]={0xFF,0x25,0,0,0,0,0,0,0,0,0,0,0,0,0x90,0x90,0x90,0x90}; UINT64 caveAddress=(UINT64)cave; memcpy(replacement+6u,&caveAddress,8);
    if(!freeze_for_patch(target,sizeof(replacement)))return false; DWORD oldProtection=0;
    if(!VirtualProtect(target,sizeof(replacement),PAGE_EXECUTE_READWRITE,&oldProtection)){unfreeze_after_patch();return false;}
    memcpy(target,replacement,sizeof(replacement));FlushInstructionCache((HANDLE)(INT64)-1,target,sizeof(replacement));
    bool installed=bytes_equal(target,replacement,sizeof(replacement)); if(!installed){memcpy(target,expected,sizeof(expected));FlushInstructionCache((HANDLE)(INT64)-1,target,sizeof(expected));}
    DWORD ignored=0;VirtualProtect(target,sizeof(replacement),oldProtection,&ignored);unfreeze_after_patch();
    if(!installed)return false;g_carrierGateInstalled=1;return true;
}

// Normal Coffin mount root. DSPlayerRideVehicleActionPlugin::10047D0
// exits at its first branch when StreamingManager+0x2264BB is nonzero. That
// prevents the native A8 builder from producing action 0x158 and therefore
// prevents 1008970 from entering the normal Player RideVehicle state 3.
//
// Do not clear the global network byte. Replace only this local JNZ with a
// Coffin-scoped decision. 2264BB is a transient inhibit signal, reset by
// E0FD40 every frame. Neither it nor QPID history identifies the current
// geographical network side. Only inspect it at the original native gate.
static bool v181_mount_inhibit_signal(void) {
    if (!g_executable ||
        !readable_range((BYTE*)g_executable + RVA_CHIRAL_WORLD_MANAGER_GLOBAL, sizeof(void*))) return false;
    BYTE* manager = *(BYTE**)((BYTE*)g_executable + RVA_CHIRAL_WORLD_MANAGER_GLOBAL);
    return manager && readable_range(manager + 0x2264BBu, sizeof(BYTE)) &&
           manager[0x2264BBu] != 0u;
}

static bool v181_live_coffin_for_ridevehicle(void* plugin, void** outVehicle) {
    if (outVehicle) *outVehicle = 0;
    if (!g_enableOutsideNetworkMount || !g_executable || !plugin ||
        !readable_range(plugin, sizeof(void*)) ||
        *(void**)plugin != (BYTE*)g_executable + RVA_RIDEVEHICLE_PLUGIN_VTABLE ||
        !v181_mount_inhibit_signal()) return false;
    BYTE* vehicle = (BYTE*)__atomic_load_n(&g_v167CoffinVehicle, __ATOMIC_ACQUIRE);
    if (!vehicle || !readable_range(vehicle, sizeof(void*)) ||
        !readable_range(vehicle + 0x490u, sizeof(UINT32)) ||
        *(void**)vehicle != (BYTE*)g_executable + RVA_V161_COFFIN_VEHICLE_VTABLE ||
        *(UINT32*)(vehicle + 0x490u) != 3u) return false;
    if (outVehicle) *outVehicle = vehicle;
    return true;
}

static BYTE __fastcall v181_frontend_scope(void* plugin) {
    return v181_live_coffin_for_ridevehicle(plugin, 0) ? 1u : 0u;
}

static bool install_v176_ridevehicle_root(HMODULE executable) {
    if (!g_enableOutsideNetworkMount) return true;
    BYTE* base = (BYTE*)executable;
    BYTE* branch = base + RVA_COFFIN_RIDEVEHICLE_NETWORK_BRANCH;
    static const BYTE expected[6] = {0x0F,0x85,0x4E,0x01,0x00,0x00};
    if (!readable_range(branch, sizeof(expected)) || !bytes_equal(branch, expected, sizeof(expected))) return false;

    const UINT64 granularity = 0x10000ull;
    const UINT64 maxDistance = 0x70000000ull;
    UINT64 origin = ((UINT64)branch) & ~(granularity - 1ull);
    BYTE* cave = 0;
    for (UINT64 distance = 0x01000000ull; distance <= maxDistance && !cave; distance += 0x01000000ull) {
        UINT64 hints[2] = { origin + distance, origin > distance ? origin - distance : 0ull };
        for (UINT32 i = 0; i < 2u; ++i) {
            if (!hints[i]) continue;
            BYTE* candidate = (BYTE*)VirtualAlloc((void*)hints[i], 0x1000u, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
            if (!candidate) continue;
            INT64 disp = (INT64)candidate - (INT64)(branch + 5u);
            if (disp < -0x80000000ll || disp > 0x7fffffffll) continue;
            cave = candidate;
            break;
        }
    }
    if (!cave) return false;

    // The entry flags are from CMP [network+2264BB],0 at 10047E3.
    // Native inside flow jumps straight to the continuation. Outside calls
    // the guarded helper with Win64 shadow space; RBX holds this plugin.
    BYTE code[64] = {};
    SIZE_T q = 0;
    code[q++]=0x0F; code[q++]=0x84; int jzAllow=27; memcpy(code+q,&jzAllow,4); q+=4;
    code[q++]=0x48; code[q++]=0x8B; code[q++]=0xCB;                 // mov rcx,rbx
    code[q++]=0x48; code[q++]=0x83; code[q++]=0xEC; code[q++]=0x20; // sub rsp,20h
    code[q++]=0x48; code[q++]=0xB8; UINT64 helper=(UINT64)&v181_frontend_scope; memcpy(code+q,&helper,8); q+=8;
    code[q++]=0xFF; code[q++]=0xD0;                                 // call rax
    code[q++]=0x48; code[q++]=0x83; code[q++]=0xC4; code[q++]=0x20; // add rsp,20h
    code[q++]=0x84; code[q++]=0xC0;                                 // test al,al
    code[q++]=0x74; code[q++]=0x11;                                 // jz reject
    code[q++]=0x48; code[q++]=0x8B; code[q++]=0xCB;                 // mov rcx,rbx
    static const BYTE absJmp[6]={0xFF,0x25,0,0,0,0};
    memcpy(code+q,absJmp,6); q+=6; UINT64 allow=(UINT64)(base+RVA_COFFIN_RIDEVEHICLE_NETWORK_CONTINUE); memcpy(code+q,&allow,8); q+=8;
    memcpy(code+q,absJmp,6); q+=6; UINT64 reject=(UINT64)(base+RVA_COFFIN_RIDEVEHICLE_NETWORK_REJECT); memcpy(code+q,&reject,8); q+=8;
    if (q != sizeof(code)) return false;
    memcpy(cave,code,sizeof(code));
    FlushInstructionCache((HANDLE)(INT64)-1,cave,sizeof(code));

    INT64 rel = (INT64)cave - (INT64)(branch + 5u);
    int rel32=(int)rel;
    BYTE replacement[6]={0xE9,0,0,0,0,0x90}; memcpy(replacement+1,&rel32,4);
    if (!freeze_for_patch(branch,sizeof(replacement))) return false;
    DWORD oldProtection=0;
    if (!VirtualProtect(branch,sizeof(replacement),PAGE_EXECUTE_READWRITE,&oldProtection)) { unfreeze_after_patch(); return false; }
    memcpy(branch,replacement,sizeof(replacement)); FlushInstructionCache((HANDLE)(INT64)-1,branch,sizeof(replacement));
    bool installed=bytes_equal(branch,replacement,sizeof(replacement));
    if (!installed) { memcpy(branch,expected,sizeof(expected)); FlushInstructionCache((HANDLE)(INT64)-1,branch,sizeof(expected)); }
    DWORD ignored=0; VirtualProtect(branch,sizeof(replacement),oldProtection,&ignored); unfreeze_after_patch();
    if (!installed) return false;
    g_v176RideVehicleRootInstalled=1;
    return true;
}

// v1.77: the normal RideVehicle plugin has a second, independent direct
// Chiral-network gate in FUN_141006010.  After the native per-frame update,
// 10060B8 compares StreamingManager+0x2264BB and 10060C4 calls F4D290 when
// outside.  F4D290 does exactly one thing: plugin+0x0A=1, i.e. request the
// action plugin to be disabled.  The v1.76 10047EA eligibility bypass can
// therefore succeed while this later lifecycle callback immediately tears the
// same normal RideVehicle action back down.
//
// Replace only that single CALL.  The wrapper resolves the plugin's *current*
// interaction handle (playerContext+0xC68) through the same native 1783C0
// helper used by FUN_141006010 itself.  Suppression is permitted only when the
// live resolved entity owns an exact DSVehicleCoffin component (vtable + type
// 3) and the proven QPID boundary path has already fired.  All other vehicles
// and all other F4D290 callers retain native behaviour.
// v1.78: use the RideVehicle collector that DS2 itself consumes in 10079D0.
// 1006010 calls F4D290 before playerContext+0xC68 is guaranteed to contain a
// live interaction handle, so the v1.77 scope could never identify the Coffin
// at the moment the plugin was being disabled.  The collector already owns the
// current spatially selected entity at collector+0x58; 10079D0 then reads
// entity+0xA0 as the selected vehicle and type at vehicle+0x490.
static bool v178_live_ridevehicle_collector_is_coffin(void* plugin, void** outCollector, void** outEntity, void** outVehicle) {
    if (outCollector) *outCollector = 0;
    if (outEntity) *outEntity = 0;
    if (outVehicle) *outVehicle = 0;
    if (!plugin || !g_executable || !readable_range((BYTE*)plugin + 0x60u, sizeof(void*))) return false;
    BYTE* collector = *(BYTE**)((BYTE*)plugin + 0x60u);
    if (outCollector) *outCollector = collector;
    if (!collector || !readable_range(collector + 0x58u, sizeof(void*))) return false;
    BYTE* entity = *(BYTE**)(collector + 0x58u);
    if (outEntity) *outEntity = entity;
    if (!entity || !readable_range(entity + 0xA0u, sizeof(void*))) return false;
    BYTE* vehicle = *(BYTE**)(entity + 0xA0u);
    if (outVehicle) *outVehicle = vehicle;
    if (!vehicle || !readable_range(vehicle, sizeof(void*)) ||
        !readable_range(vehicle + 0x490u, sizeof(UINT32))) return false;
    if (*(void**)vehicle != (void*)((BYTE*)g_executable + RVA_V161_COFFIN_VEHICLE_VTABLE)) return false;
    return *(UINT32*)(vehicle + 0x490u) == 3u;
}

// 1006010 runs before a live collector/interaction target is guaranteed.
// The direct +0x2264BB branch disables the *whole* RideVehicle plugin through
// F4D290(plugin+0x0A=1), so target-scoping at that call site is causally too
// late. Scope this inhibit exception to a captured live DSVehicleCoffin type-3
// instance. The inhibit signal is frame-local, not a network-side indicator.
// Target specificity is restored downstream by the native 10079D0 builder;
// the observer records that builder but does not alter its descriptor.
static void __fastcall v177_ridevehicle_network_cancel_scope(void* plugin) {
    void* capturedVehicle = 0;
    bool bypass = v181_live_coffin_for_ridevehicle(plugin, &capturedVehicle);
    LONG qpidHits = __atomic_load_n(&g_v151QpidHits, __ATOMIC_ACQUIRE);
    LONG callIndex = __atomic_fetch_add(&g_v178RideVehicleCancelCalls, 1, __ATOMIC_ACQ_REL);

    if (bypass) {
        LONG idx = __atomic_fetch_add(&g_v177RideVehicleCancelBypasses, 1, __ATOMIC_ACQ_REL);
        if (idx < 4) {
            LogBuffer m; log_init(&m); log_prefix(&m);
            log_text(&m, "V181 RIDEVEHICLE PLUGIN KEEPALIVE #"); log_uint(&m, (UINT64)idx);
            log_text(&m, ": plugin="); log_uint(&m, (UINT64)plugin);
            log_text(&m, ", capturedCoffin="); log_uint(&m, (UINT64)capturedVehicle);
            log_text(&m, ", qpidHits="); log_uint(&m, (UINT64)qpidHits);
            log_text(&m, ". F4D290 suppressed at native inhibit call; live exact Coffin verified; native action builder remains authoritative.\r\n");
            append_log(&m);
        }
        return;
    }

    if (v181_mount_inhibit_signal() && callIndex < 4) {
        LogBuffer m; log_init(&m); log_prefix(&m);
        log_text(&m, "V181 RIDEVEHICLE PLUGIN KEEPALIVE MISS #"); log_uint(&m, (UINT64)callIndex);
        log_text(&m, ": plugin="); log_uint(&m, (UINT64)plugin);
        log_text(&m, ", capturedCoffin="); log_uint(&m, (UINT64)capturedVehicle);
        log_text(&m, ", qpidHits="); log_uint(&m, (UINT64)qpidHits);
        log_text(&m, ". Native F4D290 preserved.\r\n");
        append_log(&m);
    }

    if (g_nativeV177PluginDisable) g_nativeV177PluginDisable(plugin);
    else if (plugin && readable_range((BYTE*)plugin + 0x0Au, sizeof(BYTE))) *(BYTE*)((BYTE*)plugin + 0x0Au) = 1u;
}

static bool install_v177_ridevehicle_cancel_scope(HMODULE executable) {
    if (!g_enableOutsideNetworkMount) return true;
    BYTE* base = (BYTE*)executable;
    BYTE* callSite = base + RVA_COFFIN_RIDEVEHICLE_NETWORK_CANCEL_CALL;
    // 10060C4: E8 C7 71 F4 FF   call 140F4D290
    static const BYTE expected[5] = {0xE8,0xC7,0x71,0xF4,0xFF};
    if (!readable_range(callSite, sizeof(expected)) || !bytes_equal(callSite, expected, sizeof(expected))) return false;
    g_nativeV177PluginDisable = (V177PluginDisableFn)(base + RVA_ACTION_PLUGIN_DISABLE);

    const UINT64 granularity = 0x10000ull;
    const UINT64 maxDistance = 0x70000000ull;
    UINT64 origin = ((UINT64)callSite) & ~(granularity - 1ull);
    BYTE* cave = 0;
    for (UINT64 distance = 0x01000000ull; distance <= maxDistance && !cave; distance += 0x01000000ull) {
        UINT64 hints[2] = { origin + distance, origin > distance ? origin - distance : 0ull };
        for (UINT32 i = 0; i < 2u; ++i) {
            if (!hints[i]) continue;
            BYTE* candidate = (BYTE*)VirtualAlloc((void*)hints[i], 0x1000u, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
            if (!candidate) continue;
            INT64 disp = (INT64)candidate - (INT64)(callSite + 5u);
            if (disp < -0x80000000ll || disp > 0x7fffffffll) { continue; }
            cave = candidate; break;
        }
    }
    if (!cave) return false;

    // CALL pushes the native return address.  The cave tail-jumps to the C++
    // wrapper, so its RET returns directly to 10060C9 with ABI stack state
    // identical to the original F4D290 call.
    static const BYTE absJmp[6] = {0xFF,0x25,0,0,0,0};
    memcpy(cave, absJmp, sizeof(absJmp));
    UINT64 wrapper = (UINT64)&v177_ridevehicle_network_cancel_scope;
    memcpy(cave + 6u, &wrapper, sizeof(wrapper));
    FlushInstructionCache((HANDLE)(INT64)-1, cave, 14u);

    INT64 rel = (INT64)cave - (INT64)(callSite + 5u);
    int rel32 = (int)rel;
    BYTE replacement[5] = {0xE8,0,0,0,0}; memcpy(replacement + 1u, &rel32, 4u);
    if (!freeze_for_patch(callSite, sizeof(replacement))) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(callSite, sizeof(replacement), PAGE_EXECUTE_READWRITE, &oldProtection)) { unfreeze_after_patch(); return false; }
    memcpy(callSite, replacement, sizeof(replacement));
    FlushInstructionCache((HANDLE)(INT64)-1, callSite, sizeof(replacement));
    bool installed = bytes_equal(callSite, replacement, sizeof(replacement));
    if (!installed) { memcpy(callSite, expected, sizeof(expected)); FlushInstructionCache((HANDLE)(INT64)-1, callSite, sizeof(expected)); }
    DWORD ignored = 0; VirtualProtect(callSite, sizeof(replacement), oldProtection, &ignored); unfreeze_after_patch();
    if (!installed) return false;
    g_v177RideVehicleCancelScopeInstalled = 1;
    return true;
}


// v1.79 passive post-builder proof. This hooks only the exact RideVehicle
// plugin vtable slot +0x110 (10079D0), calls native first, and logs only a few
// post-QPID A8 descriptors. It never changes action id, disable/enable flags,
// target, vehicle flags or return semantics.
static void __fastcall v179_ridevehicle_action_builder_observer(void* plugin, int request, UINT32* descriptor) {
    if (g_nativeV179RideVehicleBuilder) g_nativeV179RideVehicleBuilder(plugin, request, descriptor);
#ifdef COFFIN_AB_DIAGNOSTIC
    if (descriptor && request == 0xA8) {
        void* diagnosticCollector=0; void* diagnosticEntity=0; void* diagnosticVehicle=0;
        if (v178_live_ridevehicle_collector_is_coffin(plugin,&diagnosticCollector,&diagnosticEntity,&diagnosticVehicle)) {
            __atomic_store_n(&g_diagCandidateVehicle,(UINT64)diagnosticVehicle,__ATOMIC_RELEASE);
            bool outside = __atomic_load_n(&g_v151QpidHits,__ATOMIC_ACQUIRE) > 0;
            LONG idx = __atomic_fetch_add(outside ? &g_diagBuilderOutside : &g_diagBuilderInside,1,__ATOMIC_ACQ_REL);
            if (idx < 12 && readable_range(descriptor,0xF0u)) {
                BYTE* d=(BYTE*)descriptor;
                LogBuffer m; log_init(&m); log_prefix(&m);
                log_text(&m,"AB BUILDER side="); log_text(&m,outside?"outside":"inside");
                log_text(&m," #"); log_uint(&m,(UINT64)idx);
                log_text(&m," plugin="); log_uint(&m,(UINT64)plugin);
                log_text(&m," action="); log_uint(&m,*(UINT32*)d);
                log_text(&m," enabled="); log_uint(&m,d[0x0B]);
                log_text(&m," target="); log_uint(&m,*(UINT64*)(d+0xE8u));
                log_text(&m," entity="); log_uint(&m,(UINT64)diagnosticEntity);
                log_text(&m," vehicle="); log_uint(&m,(UINT64)diagnosticVehicle);
                log_text(&m,"\r\n"); append_log(&m);
            }
        }
    }
#endif
    if (!descriptor || request != 0xA8) return;

    BYTE* d = (BYTE*)descriptor;
    UINT32 actionId = readable_range(d, sizeof(UINT32)) ? *(UINT32*)d : 0xffffffffu;
    UINT32 disabled = readable_range(d + 9u, 1u) ? d[9] : 0xffu;
    UINT32 enabled = readable_range(d + 0x0Bu, 1u) ? d[0x0B] : 0xffu;
    UINT64 target = readable_range(d + 0xE8u, sizeof(UINT64)) ? *(UINT64*)(d + 0xE8u) : ~0ull;
    void* collector=0; void* entity=0; void* vehicle=0;
    bool coffin = v178_live_ridevehicle_collector_is_coffin(plugin,&collector,&entity,&vehicle);
    bool actionReady = actionId == 0x158u && enabled == 1u && target != ~0ull;
    UINT64 signature = target ^ ((UINT64)actionId << 32) ^ ((UINT64)disabled << 8) ^ enabled;
    signature ^= (UINT64)vehicle * 0x100000001B3ull;
    signature ^= coffin ? 0x10000ull : 0ull;
    signature ^= __atomic_load_n(&g_v151QpidHits, __ATOMIC_ACQUIRE) > 0 ? 0x20000ull : 0ull;
    LONG call = __atomic_fetch_add(&g_v182BuilderCalls, 1, __ATOMIC_ACQ_REL);
    bool changed = __atomic_exchange_n(&g_v182BuilderSignature, signature, __ATOMIC_ACQ_REL) != signature;
    if (!changed && call % 600 != 0) return;
    LONG idx = __atomic_fetch_add(&g_v182BuilderLogs, 1, __ATOMIC_ACQ_REL);
    if (idx >= 256) return;
    UINT32 vehicleType = 0xffffffffu, flags4E8 = 0xffffffffu;
    if (vehicle && readable_range((BYTE*)vehicle + 0x490u, sizeof(UINT32))) vehicleType = *(UINT32*)((BYTE*)vehicle + 0x490u);
    if (vehicle && readable_range((BYTE*)vehicle + 0x4E8u, sizeof(UINT32))) flags4E8 = *(UINT32*)((BYTE*)vehicle + 0x4E8u);

    LogBuffer m; log_init(&m); log_prefix(&m);
    log_text(&m, actionReady ? "V183 RIDEVEHICLE A8 READY #" : "V183 RIDEVEHICLE A8 IDLE #");
    log_uint(&m,(UINT64)idx);
    log_text(&m, ": action="); log_uint(&m,(UINT64)actionId);
    log_text(&m, ", disabled="); log_uint(&m,(UINT64)disabled);
    log_text(&m, ", enabled="); log_uint(&m,(UINT64)enabled);
    log_text(&m, ", reason54="); log_uint(&m,readable_range(d+0x54u,4u)?*(UINT32*)(d+0x54u):0xffffffffu);
    log_text(&m, ", target="); log_uint(&m,target);
    if (plugin && readable_range(plugin, 0x0Bu)) {
        log_text(&m, ", pluginActive8="); log_uint(&m,((BYTE*)plugin)[8]);
        log_text(&m, ", pluginDisableA="); log_uint(&m,((BYTE*)plugin)[0x0A]);
    }
    log_text(&m, ", collector="); log_uint(&m,(UINT64)collector);
    log_text(&m, ", entity="); log_uint(&m,(UINT64)entity);
    log_text(&m, ", vehicle="); log_uint(&m,(UINT64)vehicle);
    log_text(&m, ", type="); log_uint(&m,(UINT64)vehicleType);
    log_text(&m, ", flags4E8="); log_uint(&m,(UINT64)flags4E8);
    if (vehicle && readable_range(vehicle, 0x500u)) {
        log_text(&m, ", bit9="); log_uint(&m,(*(UINT64*)((BYTE*)vehicle+0x98u)&0x200ull)?1u:0u);
        log_text(&m, ", state4E4="); log_uint(&m,*(UINT32*)((BYTE*)vehicle+0x4E4u));
        log_text(&m, ", reserved4FC="); log_uint(&m,*(UINT32*)((BYTE*)vehicle+0x4FCu));
    }
    if (coffin && readable_range(vehicle, 0x126Fu)) {
        log_text(&m, ", coverage126E="); log_uint(&m,((BYTE*)vehicle)[0x126E]);
        log_text(&m, ", scalarB78="); log_float2(&m,*(float*)((BYTE*)vehicle+0xB78u));
        log_text(&m, ", flagsB94="); log_uint(&m,((BYTE*)vehicle)[0xB94]);
    }
    log_text(&m, ", coffin="); log_uint(&m,(UINT64)(coffin?1:0));
    log_text(&m, ", inhibitBB="); log_uint(&m,v181_mount_inhibit_signal()?1u:0u);
    log_text(&m, ", qpidHits="); log_uint(&m,(UINT64)__atomic_load_n(&g_v151QpidHits, __ATOMIC_ACQUIRE));
    log_text(&m, ". Read-only post-builder proof.\r\n");
    append_log(&m);
}

static bool install_v179_ridevehicle_action_builder_observer(HMODULE executable) {
    BYTE* base=(BYTE*)executable;
    BYTE** slot=(BYTE**)(base + RVA_RIDEVEHICLE_PLUGIN_VTABLE + 0x110u);
    if (!readable_range(slot,sizeof(void*))) return false;
    BYTE* expected=base + RVA_RIDEVEHICLE_ACTION_BUILDER;
    if (*slot != expected) return false;
    g_nativeV179RideVehicleBuilder=(V179RideVehicleBuilderFn)(*slot);
    DWORD oldProtection=0;
    if (!VirtualProtect(slot,sizeof(void*),PAGE_EXECUTE_READWRITE,&oldProtection)) return false;
    *slot=(BYTE*)&v179_ridevehicle_action_builder_observer;
    FlushInstructionCache((HANDLE)(INT64)-1,slot,sizeof(void*));
    bool installed=*slot==(BYTE*)&v179_ridevehicle_action_builder_observer;
    if (!installed) *slot=expected;
    DWORD ignored=0; VirtualProtect(slot,sizeof(void*),oldProtection,&ignored);
    if (!installed) return false;
    g_v179RideVehicleBuilderObserverInstalled=1;
    return true;
}

// v1.78 low-frequency observer: exact DSPlayerRideVehicleActionPlugin start
// vtable slot (+0x70).  It never changes arguments, result, target or player
// state; it only proves whether the normal 1008970 state-3 builder is reached.
static UINT64 __fastcall v178_ridevehicle_start_observer(void* plugin) {
    UINT64 actionData = 0, target = ~0ull;
    UINT32 stateBefore = 0xffffffffu, stateAfter = 0xffffffffu;
    if (plugin && readable_range((BYTE*)plugin + 0x24490u, sizeof(void*))) {
        BYTE* data = *(BYTE**)((BYTE*)plugin + 0x24490u);
        actionData = (UINT64)data;
        if (data && readable_range(data + 0x620u, sizeof(UINT64))) target = *(UINT64*)(data + 0x620u);
        if (readable_range((BYTE*)plugin + 0x38u, sizeof(void*))) {
            BYTE* player = *(BYTE**)((BYTE*)plugin + 0x38u);
            if (player && readable_range(player + 0x3970u, sizeof(BYTE))) stateBefore = *(BYTE*)(player + 0x3970u);
        }
    }
    UINT64 result = g_nativeV178RideVehicleStart ? g_nativeV178RideVehicleStart(plugin) : 0;
    if (plugin && readable_range((BYTE*)plugin + 0x38u, sizeof(void*))) {
        BYTE* player = *(BYTE**)((BYTE*)plugin + 0x38u);
        if (player && readable_range(player + 0x3970u, sizeof(BYTE))) stateAfter = *(BYTE*)(player + 0x3970u);
    }
    LONG idx = __atomic_fetch_add(&g_v178RideVehicleStartCalls, 1, __ATOMIC_ACQ_REL);
    if (idx < 8) {
        LogBuffer m; log_init(&m); log_prefix(&m);
        log_text(&m, "V178 RIDEVEHICLE START #"); log_uint(&m, (UINT64)idx);
        log_text(&m, ": actionData="); log_uint(&m, actionData);
        log_text(&m, ", target="); log_uint(&m, target);
        log_text(&m, ", state="); log_uint(&m, stateBefore); log_text(&m, "->"); log_uint(&m, stateAfter);
        log_text(&m, ", nativeResult="); log_uint(&m, result);
        log_text(&m, ". Read-only observer.\r\n");
        append_log(&m);
    }
    return result;
}

static bool install_v178_ridevehicle_start_observer(HMODULE executable) {
    BYTE* base = (BYTE*)executable;
    BYTE** slot = (BYTE**)(base + RVA_RIDEVEHICLE_PLUGIN_VTABLE + 0x70u);
    if (!readable_range(slot, sizeof(void*))) return false;
    BYTE* expected = base + RVA_RIDEVEHICLE_START_THUNK;
    if (*slot != expected) return false;
    g_nativeV178RideVehicleStart = (V178RideVehicleStartFn)(*slot);
    DWORD oldProtection = 0;
    if (!VirtualProtect(slot, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtection)) return false;
    *slot = (BYTE*)&v178_ridevehicle_start_observer;
    FlushInstructionCache((HANDLE)(INT64)-1, slot, sizeof(void*));
    bool installed = *slot == (BYTE*)&v178_ridevehicle_start_observer;
    if (!installed) *slot = expected;
    DWORD ignored = 0; VirtualProtect(slot, sizeof(void*), oldProtection, &ignored);
    if (!installed) return false;
    g_v178RideVehicleStartObserverInstalled = 1;
    return true;
}

// The native resolver has already verified the request owner and generation
// before this point. It may still reject E3 or mark it inactive. We only
// bypass either of those two availability predicates when the registered
// request is still proven to be the Coffin mount vfunc.
static const BYTE COFFIN_NETWORK_RESOLVER_TEMPLATE[] = {
    0xF3,0x0F,0x1E,0xFA,                         // endbr64
    0x80,0xB9,0x56,0x51,0x02,0x00,0x00,          // cmp byte [rcx+25156h],0
    0x74,0x0B,                                    // je active predicate
    0x41,0x81,0xFA,0xE3,0x00,0x00,0x00,          // cmp r10d,0E3h
    0x75,0x42,                                    // jne false
    0xEB,0x12,                                    // jmp Coffin proof
    0x80,0xB9,0xE8,0x51,0x02,0x00,0x00,          // cmp byte [rcx+251E8h],0
    0x75,0x29,                                    // jne continue
    0x41,0x81,0xFA,0xE3,0x00,0x00,0x00,          // cmp r10d,0E3h
    0x75,0x2E,                                    // jne false
    0x48,0x85,0xD2,                               // test rdx,rdx
    0x74,0x29,                                    // je false
    0x48,0x8B,0x02,                               // mov rax,[rdx]
    0x48,0x85,0xC0,                               // test rax,rax
    0x74,0x21,                                    // je false
    0x49,0xBB,0,0,0,0,0,0,0,0,                   // mov r11,Coffin mount vfunc (+57)
    0x4C,0x39,0x98,0xC8,0x00,0x00,0x00,          // cmp [rax+C8h],r11
    0x75,0x0E,                                    // jne false
    0xFF,0x25,0x00,0x00,0x00,0x00,               // jmp [rip+0]
    0,0,0,0,0,0,0,0,                             // resolver continue (+80)
    0xFF,0x25,0x00,0x00,0x00,0x00,               // jmp [rip+0]
    0,0,0,0,0,0,0,0                              // resolver false (+94)
};
[[maybe_unused]] static bool install_coffin_network_resolver_fallback(HMODULE executable) {
    if(!g_enableNetworkTraversal)return true;
    BYTE* base=(BYTE*)executable;
    BYTE* target=base+RVA_COFFIN_NETWORK_RESOLVER_REJECT_GATE;
    static const BYTE expected[16]={0x80,0xB9,0x56,0x51,0x02,0x00,0x00,0x75,0x79,0x80,0xB9,0xE8,0x51,0x02,0x00,0x00};
    if(!readable_range(target,sizeof(expected))||!bytes_equal(target,expected,sizeof(expected)))return false;
    BYTE* cave=(BYTE*)VirtualAlloc(0,sizeof(COFFIN_NETWORK_RESOLVER_TEMPLATE),MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);if(!cave)return false;
    memcpy(cave,COFFIN_NETWORK_RESOLVER_TEMPLATE,sizeof(COFFIN_NETWORK_RESOLVER_TEMPLATE));
    UINT64 mountVfunc=(UINT64)(base+RVA_COFFIN_MOUNT_VFUNC),continuation=(UINT64)(base+RVA_COFFIN_NETWORK_RESOLVER_CONTINUE),nativeFalse=(UINT64)(base+RVA_COFFIN_NETWORK_RESOLVER_FALSE);
    memcpy(cave+57u,&mountVfunc,8);memcpy(cave+80u,&continuation,8);memcpy(cave+94u,&nativeFalse,8);FlushInstructionCache((HANDLE)(INT64)-1,cave,sizeof(COFFIN_NETWORK_RESOLVER_TEMPLATE));
    BYTE replacement[16]={0xFF,0x25,0,0,0,0,0,0,0,0,0,0,0,0,0x90,0x90};UINT64 caveAddress=(UINT64)cave;memcpy(replacement+6u,&caveAddress,8);
    if(!freeze_for_patch(target,sizeof(replacement)))return false;DWORD oldProtection=0;if(!VirtualProtect(target,sizeof(replacement),PAGE_EXECUTE_READWRITE,&oldProtection)){unfreeze_after_patch();return false;}
    memcpy(target,replacement,sizeof(replacement));FlushInstructionCache((HANDLE)(INT64)-1,target,sizeof(replacement));bool installed=bytes_equal(target,replacement,sizeof(replacement));
    if(!installed){memcpy(target,expected,sizeof(expected));FlushInstructionCache((HANDLE)(INT64)-1,target,sizeof(expected));}DWORD ignored=0;VirtualProtect(target,sizeof(replacement),oldProtection,&ignored);unfreeze_after_patch();if(!installed)return false;
    g_networkResolverFallbackInstalled=1;return true;
}

// E3 fails outside the network because the request is registered to a
// different owner and its required-active byte is zero (passive trace flags
// 0x05).  The shared resolver cannot be patched: it has unrelated E3 callers.
// This wrapper is installed only in DSPlayerRideFloaterActionPlugin's own C8
// slot, calls native code first, and overrides only a false E3 result.  It
// resumes at the native success-copy block so the request's output payload is
// populated exactly as in a native successful return.
static const BYTE COFFIN_NETWORK_C8_FALLBACK_TEMPLATE[94] = {
    0xF3,0x0F,0x1E,0xFA,0x53,0x57,0x56,0x48,0x83,0xEC,0x20,
    0x48,0x8B,0xD9,0x8B,0xFA,0x49,0x8B,0xF0,0x4D,0x8B,0xC8,0x44,0x8B,0xC2,0x48,0x8B,0xD1,0x48,0x8B,0x49,0x60,
    0xFF,0x15,0x02,0x00,0x00,0x00,0xEB,0x08,0,0,0,0,0,0,0,0,
    0x84,0xC0,0x75,0x22,
    0x81,0xFF,0xE3,0x00,0x00,0x00,0x75,0x18,
    0x4C,0x8B,0xCE,0x48,0x83,0xC4,0x20,0x5E,0x5F,0x5B,
    0xFF,0x25,0x00,0x00,0x00,0x00,
    0,0,0,0,0,0,0,0,
    0x32,0xC0,0x48,0x83,0xC4,0x20,0x5E,0x5F,0x5B,0xC3
};
[[maybe_unused]] static bool install_coffin_network_c8_fallback(HMODULE executable) {
    if(!g_enableOutsideNetworkMount)return true;
    BYTE* base=(BYTE*)executable;void** vtable=(void**)(base+RVA_COFFIN_RIDE_FLOATER_ACTION_VTABLE);void** target=(void**)(base+RVA_COFFIN_RIDE_FLOATER_ACTION_C8_SLOT);
    static const BYTE expectedC8[18]={0x4D,0x8B,0xC8,0x44,0x8B,0xC2,0x48,0x8B,0xD1,0x48,0x8B,0x49,0x60,0xE9,0x2E,0xA1,0xDB,0xFF};
    static const BYTE expectedSuccessCopy[17]={0x41,0x8B,0x80,0x24,0x89,0x04,0x00,0x41,0x89,0x01,0x41,0x8B,0x80,0x28,0x89,0x04,0x00};
    if(!readable_range(vtable,26u*sizeof(void*))||!readable_range(target,sizeof(void*))||vtable[1]!=(void*)(base+RVA_COFFIN_RIDE_FLOATER_MOUNT_ENTRY)||vtable[24]!=(void*)(base+RVA_COFFIN_MOUNT_VFUNC-0x50u)||*target!=(void*)(base+RVA_COFFIN_MOUNT_VFUNC)||!readable_range(base+RVA_COFFIN_MOUNT_VFUNC,sizeof(expectedC8))||!bytes_equal(base+RVA_COFFIN_MOUNT_VFUNC,expectedC8,sizeof(expectedC8))||!readable_range(base+RVA_COFFIN_NETWORK_RESOLVER_CONTINUE,sizeof(expectedSuccessCopy))||!bytes_equal(base+RVA_COFFIN_NETWORK_RESOLVER_CONTINUE,expectedSuccessCopy,sizeof(expectedSuccessCopy)))return false;
    BYTE* cave=(BYTE*)VirtualAlloc(0,sizeof(COFFIN_NETWORK_C8_FALLBACK_TEMPLATE),MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);if(!cave)return false;
    memcpy(cave,COFFIN_NETWORK_C8_FALLBACK_TEMPLATE,sizeof(COFFIN_NETWORK_C8_FALLBACK_TEMPLATE));
    UINT64 resolver=(UINT64)(base+0x00ED9B30u),successCopy=(UINT64)(base+RVA_COFFIN_NETWORK_RESOLVER_CONTINUE);
    memcpy(cave+40u,&resolver,8);memcpy(cave+76u,&successCopy,8);FlushInstructionCache((HANDLE)(INT64)-1,cave,sizeof(COFFIN_NETWORK_C8_FALLBACK_TEMPLATE));
    if(!freeze_for_patch((const BYTE*)target,sizeof(void*)))return false;DWORD oldProtection=0;if(!VirtualProtect(target,sizeof(void*),PAGE_EXECUTE_READWRITE,&oldProtection)){unfreeze_after_patch();return false;}
    *target=(void*)cave;FlushInstructionCache((HANDLE)(INT64)-1,target,sizeof(void*));bool installed=*target==(void*)cave;
    if(!installed){*target=(void*)(base+RVA_COFFIN_MOUNT_VFUNC);FlushInstructionCache((HANDLE)(INT64)-1,target,sizeof(void*));}DWORD ignored=0;VirtualProtect(target,sizeof(void*),oldProtection,&ignored);unfreeze_after_patch();if(!installed)return false;g_networkC8FallbackInstalled=1;return true;
}

// The native action-refresh routine normally creates this record only when
// its network request bitset contains E3.  Outside the network that bit is
// absent, so C8 sees a null owner/inactive record.  Re-enter the same native
// record builder from the real Coffin mount entry instead of writing the
// owner/state fields by hand.  The fifth argument is the same candidate-list
// cursor passed by FUN_140ed9c80 (the native routine advances it first).
[[maybe_unused]] static void refresh_coffin_e3_registration(void* action) {
    if (!g_nativeCoffinE3Register || !action ||
        !readable_range((BYTE*)action + OFF_COFFIN_ACTION_MANAGER, sizeof(void*))) return;
    BYTE* manager = *(BYTE**)((BYTE*)action + OFF_COFFIN_ACTION_MANAGER);
    if (!manager || !readable_range(manager + OFF_COFFIN_MANAGER_TABLES, sizeof(void*))) return;
    BYTE* tables = *(BYTE**)(manager + OFF_COFFIN_MANAGER_TABLES);
    if (!tables || !readable_range(tables + OFF_COFFIN_TABLES_CANDIDATE_CURSOR, sizeof(void*))) return;
    void* candidateList = *(void**)(tables + OFF_COFFIN_TABLES_CANDIDATE_CURSOR);
    if (!candidateList) return;
    BYTE* record = manager + OFF_COFFIN_MANAGER_RECORDS +
        (SIZE_T)COFFIN_NETWORK_E3_REQUEST * COFFIN_REQUEST_RECORD_STRIDE;
    if (!readable_range(record, COFFIN_REQUEST_RECORD_STRIDE)) return;
    // ED9C80 passes its manager as the second argument to ED6200.  The
    // Coffin action is only the source of that manager at +0x60; passing the
    // action here would give ED6750 the wrong lifecycle context.
    g_nativeCoffinE3Register(record, manager, 0, COFFIN_NETWORK_E3_REQUEST, candidateList);
    LONG registrationIndex = __atomic_fetch_add(&g_coffinE3RegistrationLogCalls, 1, __ATOMIC_ACQ_REL);
    if (registrationIndex < 8) {
        BYTE* state = manager + (SIZE_T)COFFIN_NETWORK_E3_REQUEST * 0xD8u;
        // The resolver addresses the request owner as manager + request*4D0
        // + 0x48918; `record` already includes the +0x48870 record base.
        void* owner = readable_range(record + 0xA8u, sizeof(void*))
            ? *(void**)(record + 0xA8u) : 0;
        BYTE active = readable_range(state + 0x251E8u, sizeof(BYTE))
            ? *(BYTE*)(state + 0x251E8u) : 0;
        LogBuffer message; log_init(&message); log_prefix(&message);
        log_text(&message, "Native E3 registration #"); log_uint(&message, (UINT64)registrationIndex);
        log_text(&message, ": owner="); log_uint(&message, (UINT64)owner);
        log_text(&message, ", action="); log_uint(&message, (UINT64)action);
        log_text(&message, ", ownerMatchesAction="); log_uint(&message, (UINT64)(owner == action));
        log_text(&message, ", requiredActive="); log_uint(&message, (UINT64)active);
        log_text(&message, ", C8 remains native.\r\n"); append_log(&message);
    }
}

// Diagnostic state for the narrow Coffin mount-entry wrapper.  The wrapper
// above performs the native E3 registration; this trace is result-only.
[[maybe_unused]] static const BYTE COFFIN_NETWORK_C8_TRACE_TEMPLATE[89] = {
    0xF3,0x0F,0x1E,0xFA,0x53,0x57,0x56,0x48,0x83,0xEC,0x30,
    0x48,0x8B,0xD9,0x8B,0xFA,0x49,0x8B,0xF0,
    0x4D,0x8B,0xC8,0x44,0x8B,0xC2,0x48,0x8B,0xD1,0x48,0x8B,0x49,0x60,
    0xFF,0x15,0x02,0x00,0x00,0x00,0xEB,0x08,0,0,0,0,0,0,0,0,
    0x88,0x44,0x24,0x28,0x48,0x8B,0xCB,0x8B,0xD7,0x44,0x0F,0xB6,0xC0,
    0xFF,0x15,0x02,0x00,0x00,0x00,0xEB,0x08,0,0,0,0,0,0,0,0,
    0x8A,0x44,0x24,0x28,0x48,0x83,0xC4,0x30,0x5E,0x5F,0x5B,0xC3
};
static DWORD __stdcall network_trace_log_worker(LPVOID) {
    LogBuffer m;log_init(&m);log_prefix(&m);log_text(&m,"NETWORK TRACE (passive): E3 calls=");log_uint(&m,(UINT64)__atomic_load_n(&g_networkTraceE3Calls,__ATOMIC_ACQUIRE));log_text(&m," native-success=");log_uint(&m,(UINT64)__atomic_load_n(&g_networkTraceE3NativeSuccess,__ATOMIC_ACQUIRE));log_text(&m," native-false=");log_uint(&m,(UINT64)__atomic_load_n(&g_networkTraceE3NativeFalse,__ATOMIC_ACQUIRE));log_text(&m," false-flags=");log_uint(&m,(UINT64)__atomic_load_n(&g_networkTraceFalseFlags,__ATOMIC_ACQUIRE));log_text(&m,". No result was changed.\r\n");append_log(&m);return 0;
}
static void start_network_trace_log_worker() {HANDLE thread=CreateThread(0,0,network_trace_log_worker,0,0,0);if(thread)CloseHandle(thread);}
static void __fastcall record_coffin_network_trace(void* plugin,UINT32 request,UINT32 nativeResult) {
    if(request!=COFFIN_NETWORK_E3_REQUEST)return;
    LONG traceIndex = __atomic_fetch_add(&g_networkTraceFirstLogs,1,__ATOMIC_ACQ_REL);
    __atomic_fetch_add(&g_networkTraceE3Calls,1,__ATOMIC_ACQ_REL);
    LONG flags=0;
    BYTE* manager=0;
    void* recordOwner=0;
    UINT32 sourceGeneration=0xFFFFFFFFu;
    UINT32 generation=0xFFFFFFFFu;
    UINT32 reject=0xFFFFFFFFu;
    UINT32 active=0xFFFFFFFFu;
    if(plugin&&readable_range((BYTE*)plugin+0x60u,sizeof(void*))){manager=*(BYTE**)((BYTE*)plugin+0x60u);}
    const SIZE_T requestStride=0x4D0u, generationStride=0xD8u;
    if(manager){
        BYTE* requestEntry=manager+(SIZE_T)request*requestStride;
        BYTE* generationEntry=manager+(SIZE_T)request*generationStride;
        if(readable_range(requestEntry+0x48948u,8u)&&readable_range(generationEntry+0x251E8u,1u)){
            flags|=1;
            recordOwner=*(void**)(requestEntry+0x48918u);
            sourceGeneration=*(UINT32*)(requestEntry+0x488BCu);
            generation=*(UINT32*)(generationEntry+0x2518Cu);
            reject=*(BYTE*)(generationEntry+0x25156u);
            active=*(BYTE*)(generationEntry+0x251E8u);
            if(recordOwner==plugin)flags|=2;
            if(sourceGeneration==generation)flags|=4;
            if(reject!=0)flags|=8;
            if(active!=0)flags|=16;
        }
    }
    if(nativeResult) __atomic_fetch_add(&g_networkTraceE3NativeSuccess,1,__ATOMIC_ACQ_REL);
    else {
        __atomic_fetch_or(&g_networkTraceFalseFlags,flags,__ATOMIC_ACQ_REL);
        if(__atomic_fetch_add(&g_networkTraceE3NativeFalse,1,__ATOMIC_ACQ_REL)==0)start_network_trace_log_worker();
    }
    if(traceIndex < 16) {
        LogBuffer message; log_init(&message); log_prefix(&message);
        log_text(&message, "NETWORK TRACE E3 #"); log_uint(&message, (UINT64)traceIndex);
        log_text(&message, ": nativeResult="); log_uint(&message, nativeResult);
        log_text(&message, ", flags=0x"); log_uint(&message, (UINT64)(UINT32)flags);
        log_text(&message, ", manager="); log_uint(&message, (UINT64)manager);
        log_text(&message, ", recordOwner="); log_uint(&message, (UINT64)recordOwner);
        log_text(&message, ", plugin="); log_uint(&message, (UINT64)plugin);
        log_text(&message, ", sourceGeneration="); log_uint(&message, sourceGeneration);
        log_text(&message, ", generation="); log_uint(&message, generation);
        log_text(&message, ", reject="); log_uint(&message, reject);
        log_text(&message, ", active="); log_uint(&message, active);
        log_text(&message, ". Native result unchanged.\r\n"); append_log(&message);
    }
}
static void copy_coffin_c8_payload(BYTE* destination, const BYTE* source) {
    if (!destination || !source) return;
    for (UINT32 i = 0; i < 0x64u; ++i) destination[i] = source[i];
}

static volatile LONG g_v175FreshC8Calls = 0;
static volatile LONG g_v175FreshC8Promotions = 0;
static volatile LONG g_v175FreshC8Success = 0;

// v1.75: fresh-record C8 activation.  The old dev12 route proved that a
// buttonless outside mount is possible once the RideFloater E3 state is marked
// active, but its persistent post-producer write and cached 0x64-byte C8 replay
// could carry an incoherent lifecycle forward.  Here the native resolver is
// retried only while its CURRENT E3 record is already complete and coherent.
// We temporarily expose state.active for the duration of one native C8 call,
// then restore the exact byte.  ED9B30 therefore copies a fresh payload from
// the current record; no payload, target, transform, state4 or input is forged.
static BYTE __fastcall coffin_network_c8_bridge(void* plugin, UINT32 request, void* output) {
    if (!g_nativeCoffinC8) return 0;
    BYTE nativeResult = g_nativeCoffinC8(plugin, request, output);
    if (request != COFFIN_NETWORK_E3_REQUEST) return nativeResult;

    LONG callIndex = __atomic_fetch_add(&g_v175FreshC8Calls, 1, __ATOMIC_ACQ_REL);
    if (nativeResult || !g_enableOutsideNetworkMount || !plugin || !output ||
        !readable_range((BYTE*)plugin + 0x60u, sizeof(void*))) {
        if (callIndex < 24) {
            LogBuffer m; log_init(&m); log_prefix(&m);
            log_text(&m, "V175 FRESH C8 #"); log_uint(&m, (UINT64)callIndex);
            log_text(&m, ": native="); log_uint(&m, nativeResult);
            log_text(&m, ", promoted=0 (native/non-E3-context).\r\n"); append_log(&m);
        }
        return nativeResult;
    }

    BYTE* manager = *(BYTE**)((BYTE*)plugin + 0x60u);
    if (!manager) return nativeResult;
    BYTE* record = manager + 0x48870u + (SIZE_T)request * 0x4D0u;
    BYTE* state  = manager + 0x25140u + (SIZE_T)request * 0xD8u;

    void* owner = 0;
    UINT32 recordGen = 0xffffffffu, stateGen = 0xffffffffu, descriptor = 0xffffffffu;
    BYTE reject = 0xffu, active = 0xffu, bb = 0;
    void* selected = 0;
    UINT32 selectedD4 = 0;
    UINT64 selectedHandle = ~0ull, recordTarget = ~0ull;
    bool selectedCoffin = false;

    bool recordReadable =
        readable_range(record + 0x08u, sizeof(UINT32)) &&
        readable_range(record + 0x4Cu, sizeof(UINT32)) &&
        readable_range(record + 0xA8u, sizeof(void*)) &&
        readable_range(record + 0xC8u, sizeof(UINT64)) &&
        readable_range(state + 0x16u, sizeof(BYTE)) &&
        readable_range(state + 0x4Cu, sizeof(UINT32)) &&
        readable_range(state + 0xA8u, sizeof(BYTE)) &&
        readable_range(manager + 0x230u, sizeof(void*));
    if (recordReadable) {
        descriptor = *(UINT32*)(record + 0x08u);
        recordGen = *(UINT32*)(record + 0x4Cu);
        owner = *(void**)(record + 0xA8u);
        recordTarget = *(UINT64*)(record + 0xC8u);
        reject = *(BYTE*)(state + 0x16u);
        stateGen = *(UINT32*)(state + 0x4Cu);
        active = *(BYTE*)(state + 0xA8u);
        selected = *(void**)(manager + 0x230u);
    }
    if (selected && readable_range(selected, sizeof(void*)) &&
        readable_range((BYTE*)selected + 0x08u, sizeof(UINT64)) &&
        readable_range((BYTE*)selected + 0xD4u, sizeof(UINT32))) {
        selectedCoffin = *(void**)selected == (void*)((BYTE*)g_executable + RVA_COFFIN_BAGGAGE_INFO_VTABLE);
        selectedHandle = *(UINT64*)((BYTE*)selected + 0x08u);
        selectedD4 = *(UINT32*)((BYTE*)selected + 0xD4u);
    }

    // BB is the game's own outside-network RideFloater rejection signal.  The
    // existing Coffin-only FFFD03 patch bypasses the rejection branch but does
    // not alter this signal, so it remains a precise scope discriminator.
    BYTE* base = (BYTE*)g_executable;
    if (base && readable_range(base + RVA_CHIRAL_WORLD_MANAGER_GLOBAL, sizeof(void*))) {
        BYTE* world = *(BYTE**)(base + RVA_CHIRAL_WORLD_MANAGER_GLOBAL);
        if (world && readable_range(world + 0x2264BBu, sizeof(BYTE))) bb = *(BYTE*)(world + 0x2264BBu);
    }

    bool coherent = recordReadable && bb != 0 && owner == plugin &&
        descriptor == 0x0Bu && recordGen == stateGen && reject == 0 && active == 0 &&
        selectedCoffin && (selectedD4 & COFFIN_BAGGAGE_D4_MOUNT_BIT) != 0u &&
        selectedHandle != ~0ull && selectedHandle == recordTarget;

    BYTE retried = 0;
    if (coherent) {
        BYTE* activePtr = state + 0xA8u;
        BYTE original = *activePtr;
        *activePtr = 1u;
        retried = g_nativeCoffinC8(plugin, request, output);
        *activePtr = original;
        __atomic_fetch_add(&g_v175FreshC8Promotions, 1, __ATOMIC_ACQ_REL);
        if (retried) __atomic_fetch_add(&g_v175FreshC8Success, 1, __ATOMIC_ACQ_REL);
    }

    if (callIndex < 80 || retried) {
        UINT64 payloadTarget = ~0ull;
        UINT32 payloadSelector = 0xffffffffu;
        if (retried && readable_range((BYTE*)output + 0x18u, sizeof(UINT64))) {
            payloadSelector = *(UINT32*)((BYTE*)output + 0x04u);
            payloadTarget = *(UINT64*)((BYTE*)output + 0x10u);
        }
        LogBuffer m; log_init(&m); log_prefix(&m);
        log_text(&m, "V175 FRESH C8 #"); log_uint(&m, (UINT64)callIndex);
        log_text(&m, ": native="); log_uint(&m, nativeResult);
        log_text(&m, ", coherent="); log_uint(&m, coherent ? 1u : 0u);
        log_text(&m, ", bb="); log_uint(&m, bb);
        log_text(&m, ", owner="); log_uint(&m, owner == plugin ? 1u : 0u);
        log_text(&m, ", desc="); log_uint(&m, descriptor);
        log_text(&m, ", gen="); log_uint(&m, recordGen); log_text(&m, "/"); log_uint(&m, stateGen);
        log_text(&m, ", reject="); log_uint(&m, reject);
        log_text(&m, ", active="); log_uint(&m, active);
        log_text(&m, ", selected="); log_uint(&m, (UINT64)selected);
        log_text(&m, ", bit20="); log_uint(&m, (selectedD4 & COFFIN_BAGGAGE_D4_MOUNT_BIT) ? 1u : 0u);
        log_text(&m, ", targetMatch="); log_uint(&m, selectedHandle == recordTarget ? 1u : 0u);
        log_text(&m, ", retry="); log_uint(&m, retried);
        log_text(&m, ", selector="); log_uint(&m, payloadSelector);
        log_text(&m, ", payloadTarget="); log_uint(&m, payloadTarget);
        log_text(&m, ". Active byte restored immediately; payload is native/fresh.\r\n"); append_log(&m);
    }
    return retried ? retried : nativeResult;
}

static bool install_coffin_network_c8_trace(HMODULE executable) {
    if (!g_enableNetworkTrace && !g_enableOutsideNetworkMount) return true;
    BYTE* base = (BYTE*)executable;
    void** vtable = (void**)(base + RVA_COFFIN_RIDE_FLOATER_ACTION_VTABLE);
    void** target = (void**)(base + RVA_COFFIN_RIDE_FLOATER_ACTION_C8_SLOT);
    static const BYTE expectedC8[18] = {0x4D,0x8B,0xC8,0x44,0x8B,0xC2,0x48,0x8B,0xD1,0x48,0x8B,0x49,0x60,0xE9,0x2E,0xA1,0xDB,0xFF};
    if (!readable_range(vtable, 26u * sizeof(void*)) || !readable_range(target, sizeof(void*)) ||
        vtable[1] != (void*)(base + RVA_COFFIN_RIDE_FLOATER_MOUNT_ENTRY) ||
        vtable[24] != (void*)(base + RVA_COFFIN_MOUNT_VFUNC - 0x50u) ||
        *target != (void*)(base + RVA_COFFIN_MOUNT_VFUNC) ||
        !readable_range(base + RVA_COFFIN_MOUNT_VFUNC, sizeof(expectedC8)) ||
        !bytes_equal(base + RVA_COFFIN_MOUNT_VFUNC, expectedC8, sizeof(expectedC8))) return false;
    g_nativeCoffinC8 = (CoffinC8Fn)*target;
    if (!freeze_for_patch((const BYTE*)target, sizeof(void*))) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(target, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtection)) {
        unfreeze_after_patch(); return false;
    }
    *target = (void*)&coffin_network_c8_bridge;
    FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(void*));
    bool installed = *target == (void*)&coffin_network_c8_bridge;
    if (!installed) *target = (void*)g_nativeCoffinC8;
    DWORD ignored = 0; VirtualProtect(target, sizeof(void*), oldProtection, &ignored);
    unfreeze_after_patch();
    if (installed) g_networkTraceInstalled = 1;
    return installed;
}

static bool replace_exact_branch_with_nops(BYTE* context,const BYTE* expected,SIZE_T contextSize,SIZE_T branchOffset);
static bool install_coffin_network_mount_gate(HMODULE executable) {
    if(!g_enableOutsideNetworkMount)return true;BYTE* base=(BYTE*)executable;BYTE* gate=base+RVA_COFFIN_RIDE_FLOATER_MOUNT_NETWORK_GATE;
    static const BYTE expected[13]={0x80,0xB8,0xBB,0x64,0x22,0x00,0x00,0x0F,0x85,0x9C,0x00,0x00,0x00};
    if(!replace_exact_branch_with_nops(gate,expected,sizeof(expected),7u))return false;g_networkMountGateInstalled=1;return true;
}

// The Coffin Drive handler reaches state 6 through these two local branches:
// the network manager signal and the forced-END state bit.  They are not
// global writes; both branches exist only in the Coffin Drive state handler.
static bool replace_exact_branch_with_nops(BYTE* context,const BYTE* expected,SIZE_T contextSize,SIZE_T branchOffset) {
    if(!context||!expected||branchOffset+6u>contextSize||!readable_range(context,contextSize)||!bytes_equal(context,expected,contextSize))return false;
    BYTE* branch=context+branchOffset;static const BYTE nops[6]={0x66,0x0F,0x1F,0x44,0x00,0x00};
    if(!freeze_for_patch(branch,sizeof(nops)))return false;DWORD oldProtection=0;if(!VirtualProtect(branch,sizeof(nops),PAGE_EXECUTE_READWRITE,&oldProtection)){unfreeze_after_patch();return false;}
    memcpy(branch,nops,sizeof(nops));FlushInstructionCache((HANDLE)(INT64)-1,branch,sizeof(nops));bool installed=bytes_equal(branch,nops,sizeof(nops));
    if(!installed){memcpy(branch,expected+branchOffset,sizeof(nops));FlushInstructionCache((HANDLE)(INT64)-1,branch,sizeof(nops));}DWORD ignored=0;VirtualProtect(branch,sizeof(nops),oldProtection,&ignored);unfreeze_after_patch();return installed;
}
static bool replace_exact_short_branch_with_nops(BYTE* context,const BYTE* expected,SIZE_T contextSize,SIZE_T branchOffset) {
    if(!context||!expected||branchOffset+2u>contextSize||!readable_range(context,contextSize)||!bytes_equal(context,expected,contextSize))return false;
    BYTE* branch=context+branchOffset;static const BYTE nops[2]={0x90,0x90};
    if(!freeze_for_patch(branch,sizeof(nops)))return false;DWORD oldProtection=0;if(!VirtualProtect(branch,sizeof(nops),PAGE_EXECUTE_READWRITE,&oldProtection)){unfreeze_after_patch();return false;}
    memcpy(branch,nops,sizeof(nops));FlushInstructionCache((HANDLE)(INT64)-1,branch,sizeof(nops));bool installed=bytes_equal(branch,nops,sizeof(nops));
    if(!installed){memcpy(branch,expected+branchOffset,sizeof(nops));FlushInstructionCache((HANDLE)(INT64)-1,branch,sizeof(nops));}DWORD ignored=0;VirtualProtect(branch,sizeof(nops),oldProtection,&ignored);unfreeze_after_patch();return installed;
}
static bool install_coffin_network_traversal_branches(HMODULE executable) {
    if (!g_enableNetworkTraversal) return true;
    BYTE* base = (BYTE*)executable;
    BYTE* driveManager = base + RVA_COFFIN_DRIVE_NETWORK_GATE;
    BYTE* driveGlobal = base + 0x00F8A7C4u;
    BYTE* driveState = base + RVA_COFFIN_DRIVE_FORCED_END_GATE;
    BYTE* kickManager = base + RVA_COFFIN_KICK_NETWORK_GATE;
    BYTE* kickGlobal = base + 0x00F8DC7Fu;
    BYTE* kickState = base + 0x00F8DC81u;
    static const BYTE manager[13] = {0x80,0xB8,0xBC,0x64,0x22,0x00,0x00,0x0F,0x85,0x63,0x01,0x00,0x00};
    static const BYTE driveGlobalExpected[6] = {0x0F,0x84,0x43,0x01,0x00,0x00};
    static const BYTE driveStateExpected[13] = {0xF6,0x87,0x61,0x01,0x00,0x00,0x02,0x0F,0x85,0x36,0x01,0x00,0x00};
    static const BYTE kickManagerExpected[13] = {0x80,0xB8,0xBC,0x64,0x22,0x00,0x00,0x0F,0x85,0x9B,0x00,0x00,0x00};
    static const BYTE kickGlobalExpected[2] = {0x74,0x78};
    static const BYTE kickStateExpected[5] = {0xF6,0xC1,0x02,0x75,0x73};
    if (!readable_range(driveManager,sizeof(manager)) || !bytes_equal(driveManager,manager,sizeof(manager)) ||
        !readable_range(driveGlobal,sizeof(driveGlobalExpected)) || !bytes_equal(driveGlobal,driveGlobalExpected,sizeof(driveGlobalExpected)) ||
        !readable_range(driveState,sizeof(driveStateExpected)) || !bytes_equal(driveState,driveStateExpected,sizeof(driveStateExpected)) ||
        !readable_range(kickManager,sizeof(kickManagerExpected)) || !bytes_equal(kickManager,kickManagerExpected,sizeof(kickManagerExpected)) ||
        !readable_range(kickGlobal,sizeof(kickGlobalExpected)) || !bytes_equal(kickGlobal,kickGlobalExpected,sizeof(kickGlobalExpected)) ||
        !readable_range(kickState,sizeof(kickStateExpected)) || !bytes_equal(kickState,kickStateExpected,sizeof(kickStateExpected))) return false;
    if (!replace_exact_branch_with_nops(driveManager,manager,sizeof(manager),7u)) return false;
    if (!replace_exact_branch_with_nops(driveGlobal,driveGlobalExpected,sizeof(driveGlobalExpected),0u)) return false;
    if (!replace_exact_branch_with_nops(driveState,driveStateExpected,sizeof(driveStateExpected),7u)) return false;
    if (!replace_exact_branch_with_nops(kickManager,kickManagerExpected,sizeof(kickManagerExpected),7u)) return false;
    if (!replace_exact_short_branch_with_nops(kickGlobal,kickGlobalExpected,sizeof(kickGlobalExpected),0u)) return false;
    if (!replace_exact_short_branch_with_nops(kickState,kickStateExpected,sizeof(kickStateExpected),3u)) return false;
    g_networkDriveEndGuardsInstalled = 1;
    return true;
}

// ED9C80 normally skips the native ED6200 action-record builder when its
// active-action bitset omits E3.  Keep every non-E3 bitset decision byte-for-
// byte native; for E3 only, make the existing test report active and let the
// original call to ED6200 enumerate and validate the native action candidates.
static bool install_coffin_network_action_bitset_refresh(HMODULE executable) {
    if (!g_enableNativeE3Producer && !g_enableCoffinCandidateCursorTrace) return true;
    BYTE* target = (BYTE*)executable + RVA_COFFIN_NETWORK_ACTION_BITSET_TEST;
    static const BYTE expected[14] = {
        0x8B,0xC3,0x48,0xC1,0xE8,0x05,0x45,0x85,0xB4,0x85,0x94,0x00,0x00,0x00
    };
    if (!readable_range(target, sizeof(expected)) || !bytes_equal(target, expected, sizeof(expected))) return false;
    BYTE* cave = (BYTE*)VirtualAlloc(0, 128u, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!cave) return false;
    SIZE_T p = 0;
    static const BYTE requestCompare[6] = {0x81,0xFB,0xE3,0x00,0x00,0x00};
    memcpy(cave + p, requestCompare, sizeof(requestCompare)); p += sizeof(requestCompare);
    SIZE_T forceBranch = p; cave[p++] = 0x74; cave[p++] = 0;
    memcpy(cave + p, expected, sizeof(expected)); p += sizeof(expected);
    static const BYTE absoluteJump[14] = {
        0xFF,0x25,0x00,0x00,0x00,0x00,0,0,0,0,0,0,0,0
    };
    memcpy(cave + p, absoluteJump, sizeof(absoluteJump));
    SIZE_T nativeContinuationPointer = p + 6u;
    p += sizeof(absoluteJump);
    SIZE_T forceLabel = p;
    static const BYTE forceTest[3] = {0x45,0x85,0xF6};
    memcpy(cave + p, forceTest, sizeof(forceTest)); p += sizeof(forceTest);
    memcpy(cave + p, absoluteJump, sizeof(absoluteJump));
    SIZE_T forcedContinuationPointer = p + 6u;
    p += sizeof(absoluteJump);
    cave[forceBranch + 1u] = (BYTE)(forceLabel - (forceBranch + 2u));
    UINT64 continuation = (UINT64)(target + sizeof(expected));
    memcpy(cave + nativeContinuationPointer, &continuation, sizeof(continuation));
    memcpy(cave + forcedContinuationPointer, &continuation, sizeof(continuation));
    FlushInstructionCache((HANDLE)(INT64)-1, cave, p);
    BYTE replacement[14] = {
        0xFF,0x25,0x00,0x00,0x00,0x00,0,0,0,0,0,0,0,0
    };
    UINT64 caveAddress = (UINT64)cave;
    memcpy(replacement + 6u, &caveAddress, sizeof(caveAddress));
    if (!freeze_for_patch(target, sizeof(replacement))) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(target, sizeof(replacement), PAGE_EXECUTE_READWRITE, &oldProtection)) {
        unfreeze_after_patch(); return false;
    }
    memcpy(target, replacement, sizeof(replacement));
    FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(replacement));
    bool installed = bytes_equal(target, replacement, sizeof(replacement));
    if (!installed) {
        memcpy(target, expected, sizeof(expected));
        FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(expected));
    }
    DWORD ignored = 0; VirtualProtect(target, sizeof(replacement), oldProtection, &ignored);
    unfreeze_after_patch();
    if (installed) g_coffinNetworkActionBitsetRefreshInstalled = 1;
    return installed;
}

// The native refresh normally skips EDFE10 when the request bit is absent.
// E3 is the Coffin's border request, so replay only that native call when its
// bit is missing, then continue through the post-producer hook.  All other
// requests retain the original bitset decision and the original loop.
static bool install_coffin_network_state_bitset_refresh(HMODULE executable) {
    if (!g_enableNetworkTraversal && !g_enableOutsideNetworkMount && !g_enableNativeE3Producer) return true;
    BYTE* base = (BYTE*)executable;
    BYTE* target = base + RVA_COFFIN_NETWORK_STATE_BITSET_TEST;
    static const BYTE expected[14] = {
        0x8B,0xC6,0x48,0xC1,0xE8,0x05,0x41,0x85,0xBC,0x86,0x94,0x00,0x00,0x00
    };
    if (!readable_range(target, sizeof(expected)) || !bytes_equal(target, expected, sizeof(expected))) return false;
    BYTE* cave = (BYTE*)VirtualAlloc(0, 192u, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!cave) return false;
    SIZE_T p = 0;
    memcpy(cave + p, expected, sizeof(expected)); p += sizeof(expected);
    SIZE_T bitSet = p; cave[p++] = 0x75; cave[p++] = 0;
    static const BYTE requestCmp[6] = {0x81,0xFE,0xE3,0x00,0x00,0x00};
    memcpy(cave + p, requestCmp, sizeof(requestCmp)); p += sizeof(requestCmp);
    SIZE_T skipE3 = p; cave[p++] = 0x75; cave[p++] = 0;
    static const BYTE absoluteJump[] = {0xFF,0x25,0x00,0x00,0x00,0x00,0,0,0,0,0,0,0,0};
    SIZE_T nativeLabel = p;
    memcpy(cave + p, absoluteJump, sizeof(absoluteJump)); p += sizeof(absoluteJump);
    SIZE_T nativePointer = nativeLabel + 6u;
    SIZE_T skipLabel = p;
    memcpy(cave + p, absoluteJump, sizeof(absoluteJump)); p += sizeof(absoluteJump);
    SIZE_T skipPointer = skipLabel + 6u;
    cave[bitSet + 1u] = (BYTE)(nativeLabel - (bitSet + 2u));
    cave[skipE3 + 1u] = (BYTE)(skipLabel - (skipE3 + 2u));
    UINT64 postProducer = (UINT64)(base + RVA_COFFIN_NETWORK_STATE_POST_PRODUCER);
    UINT64 nativeCallBlock = (UINT64)(base + 0x00ED9DC0u);
    memcpy(cave + skipPointer, &postProducer, sizeof(postProducer));
    memcpy(cave + nativePointer, &nativeCallBlock, sizeof(nativeCallBlock));
    FlushInstructionCache((HANDLE)(INT64)-1, cave, p);
    BYTE replacement[14] = {0xFF,0x25,0x00,0x00,0x00,0x00,0,0,0,0,0,0,0,0};
    UINT64 caveAddress = (UINT64)cave;
    memcpy(replacement + 6u, &caveAddress, sizeof(caveAddress));
    if (!freeze_for_patch(target, sizeof(replacement))) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(target, sizeof(replacement), PAGE_EXECUTE_READWRITE, &oldProtection)) {
        unfreeze_after_patch(); return false;
    }
    memcpy(target, replacement, sizeof(replacement));
    FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(replacement));
    bool installed = bytes_equal(target, replacement, sizeof(replacement));
    if (!installed) {
        memcpy(target, expected, sizeof(expected));
        FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(expected));
    }
    DWORD ignored = 0; VirtualProtect(target, sizeof(replacement), oldProtection, &ignored);
    unfreeze_after_patch();
    if (installed) g_coffinNetworkStateBitsetRefreshInstalled = 1;
    return installed;
}

// EDFE10 is the native per-request state producer.  Its normal success paths
// write stateEntry[0] through the r9+0xA8 alias, but the network-border E3
// request can return false after the Coffin owner record has been built.  At
// ED9DEB the refresh loop has completed that native call and RBX is the exact
// state entry.  Promote only the E3 entry whose resolver owner is the Coffin
// action, then replay the original loop bookkeeping.  This is deliberately
// a refresh-time hook: it never borrows state during mount dispatch and never
// changes the shared C8 resolver.
static bool install_coffin_network_state_post_producer(HMODULE executable) {
    if (!g_enableNetworkTraversal && !g_enableOutsideNetworkMount && !g_enableNativeE3Producer) return true;
    BYTE* base = (BYTE*)executable;
    BYTE* target = base + RVA_COFFIN_NETWORK_STATE_POST_PRODUCER;
    static const BYTE expected[19] = {
        0xFF,0xC6,0xD1,0xC7,0x48,0x81,0xC3,0xD8,0x00,0x00,0x00,
        0x81,0xFE,0x6B,0x01,0x00,0x00,0x7C,0xB2
    };
    if (!readable_range(target, sizeof(expected)) || !bytes_equal(target, expected, sizeof(expected))) return false;
    BYTE* cave = (BYTE*)VirtualAlloc(0, 160u, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!cave) return false;
    SIZE_T p = 0;
    static const BYTE requestCmp[6] = {0x81,0xFE,0xE3,0x00,0x00,0x00};
    memcpy(cave + p, requestCmp, sizeof(requestCmp)); p += sizeof(requestCmp);
    SIZE_T skipRequest = p; cave[p++] = 0x75; cave[p++] = 0;
    static const BYTE makeRecordAddress[3] = {0x48,0x8B,0xC6};
    memcpy(cave + p, makeRecordAddress, sizeof(makeRecordAddress)); p += sizeof(makeRecordAddress);
    static const BYTE scaleRequest[7] = {0x48,0x69,0xC0,0xD0,0x04,0x00,0x00};
    memcpy(cave + p, scaleRequest, sizeof(scaleRequest)); p += sizeof(scaleRequest);
    static const BYTE addRecordField[8] = {0x48,0x8D,0x84,0x05,0x18,0x89,0x04,0x00};
    memcpy(cave + p, addRecordField, sizeof(addRecordField)); p += sizeof(addRecordField);
    static const BYTE loadOwner[3] = {0x48,0x8B,0x08};
    memcpy(cave + p, loadOwner, sizeof(loadOwner)); p += sizeof(loadOwner);
    static const BYTE testOwner[3] = {0x48,0x85,0xC9};
    memcpy(cave + p, testOwner, sizeof(testOwner)); p += sizeof(testOwner);
    SIZE_T skipOwner = p; cave[p++] = 0x74; cave[p++] = 0;
    static const BYTE loadOwnerVtable[3] = {0x48,0x8B,0x01};
    memcpy(cave + p, loadOwnerVtable, sizeof(loadOwnerVtable)); p += sizeof(loadOwnerVtable);
    SIZE_T vtableImmediate = p;
    static const BYTE loadCoffinVtable[10] = {0x49,0xBB,0,0,0,0,0,0,0,0};
    memcpy(cave + p, loadCoffinVtable, sizeof(loadCoffinVtable)); p += sizeof(loadCoffinVtable);
    static const BYTE compareVtable[3] = {0x4C,0x39,0xD8};
    memcpy(cave + p, compareVtable, sizeof(compareVtable)); p += sizeof(compareVtable);
    SIZE_T skipVtable = p; cave[p++] = 0x75; cave[p++] = 0;
    static const BYTE activate[3] = {0xC6,0x03,0x01};
    memcpy(cave + p, activate, sizeof(activate)); p += sizeof(activate);
    SIZE_T noWrite = p;
    cave[skipRequest + 1u] = (BYTE)(noWrite - (skipRequest + 2u));
    cave[skipOwner + 1u] = (BYTE)(noWrite - (skipOwner + 2u));
    cave[skipVtable + 1u] = (BYTE)(noWrite - (skipVtable + 2u));
    static const BYTE originalLoopPrefix[17] = {
        0xFF,0xC6,0xD1,0xC7,0x48,0x81,0xC3,0xD8,0x00,0x00,0x00,
        0x81,0xFE,0x6B,0x01,0x00,0x00
    };
    memcpy(cave + p, originalLoopPrefix, sizeof(originalLoopPrefix)); p += sizeof(originalLoopPrefix);
    SIZE_T loopCondition = p;
    static const BYTE loopIfLess[6] = {0x0F,0x8C,0,0,0,0};
    memcpy(cave + p, loopIfLess, sizeof(loopIfLess)); p += sizeof(loopIfLess);
    static const BYTE absoluteJump[6] = {0xFF,0x25,0x00,0x00,0x00,0x00};
    static const BYTE continuationJump[14] = {0xFF,0x25,0x00,0x00,0x00,0x00,0,0,0,0,0,0,0,0};
    memcpy(cave + p, continuationJump, sizeof(continuationJump));
    SIZE_T continuationPointer = p + 6u;
    p += sizeof(continuationJump);
    SIZE_T loopJump = p;
    memcpy(cave + p, absoluteJump, sizeof(absoluteJump));
    SIZE_T loopPointer = p + 6u;
    p += sizeof(absoluteJump) + sizeof(UINT64);
    INT64 loopDisplacement = (INT64)loopJump - (INT64)(loopCondition + 6u);
    memcpy(cave + loopCondition + 2u, &loopDisplacement, 4u);
    UINT64 coffinVtable = (UINT64)(base + RVA_COFFIN_RIDE_FLOATER_ACTION_VTABLE);
    UINT64 continuation = (UINT64)(target + sizeof(expected));
    UINT64 loop = (UINT64)(base + RVA_COFFIN_NETWORK_STATE_POST_PRODUCER - 0x3Bu);
    memcpy(cave + vtableImmediate + 2u, &coffinVtable, sizeof(coffinVtable));
    memcpy(cave + continuationPointer, &continuation, sizeof(continuation));
    memcpy(cave + loopPointer, &loop, sizeof(loop));
    FlushInstructionCache((HANDLE)(INT64)-1, cave, p);
    BYTE replacement[19] = {
        0xFF,0x25,0x00,0x00,0x00,0x00,0,0,0,0,0,0,0,0,
        0x90,0x90,0x90,0x90,0x90
    };
    UINT64 caveAddress = (UINT64)cave;
    memcpy(replacement + 6u, &caveAddress, sizeof(caveAddress));
    if (!freeze_for_patch(target, sizeof(replacement))) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(target, sizeof(replacement), PAGE_EXECUTE_READWRITE, &oldProtection)) {
        unfreeze_after_patch(); return false;
    }
    memcpy(target, replacement, sizeof(replacement));
    FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(replacement));
    bool installed = bytes_equal(target, replacement, sizeof(replacement));
    if (!installed) {
        memcpy(target, expected, sizeof(expected));
        FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(expected));
    }
    DWORD ignored = 0; VirtualProtect(target, sizeof(replacement), oldProtection, &ignored);
    unfreeze_after_patch();
    if (installed) g_coffinNetworkStatePostProducerInstalled = 1;
    return installed;
}

// v1.61: capture the exact vehicle that native 1001BD0 is about to use.
typedef void* (__fastcall* V161ActionVehicleResolveFn)(void*, UINT64*);
static bool v161_is_live_coffin_vehicle(void* vehicle) {
    if (!vehicle || !g_executable || !readable_range(vehicle, sizeof(void*))) return false;
    return *(void**)vehicle == (void*)((BYTE*)g_executable + RVA_V161_COFFIN_VEHICLE_VTABLE);
}
static void v161_capture_ridden_coffin(void* action) {
    if (!g_enableOutsideNetworkMount || !action || !g_executable ||
        !readable_range((BYTE*)action + 0x38u, sizeof(void*))) return;
    BYTE* state = *(BYTE**)((BYTE*)action + 0x38u);
    if (!state || !readable_range(state + 0xC68u, sizeof(UINT64))) return;
    UINT64 handle = *(UINT64*)(state + 0xC68u);
    if (handle == 0xFFFFFFFFFFFFFFFFull) return;
    UINT64 rememberedHandle = __atomic_load_n(&g_v161RideCoffinHandle, __ATOMIC_ACQUIRE);
    void* rememberedVehicle = (void*)__atomic_load_n(&g_v161RideCoffinVehicle, __ATOMIC_ACQUIRE);
    if (rememberedHandle == handle && v161_is_live_coffin_vehicle(rememberedVehicle)) return;
    V161ActionVehicleResolveFn resolve = (V161ActionVehicleResolveFn)((BYTE*)g_executable + RVA_V161_ACTION_VEHICLE_RESOLVE);
    UINT64 localHandle = handle;
    void* vehicle = resolve(action, &localHandle);
    bool isCoffin = v161_is_live_coffin_vehicle(vehicle);
    LONG pidx = __atomic_fetch_add(&g_v162RideProbeLogs, 1, __ATOMIC_ACQ_REL);
    if (pidx < 16) {
        UINT64 vt = (vehicle && readable_range(vehicle, sizeof(void*))) ? (UINT64)*(void**)vehicle : 0u;
        UINT64 vtRva = (g_executable && vt >= (UINT64)g_executable) ? vt - (UINT64)g_executable : vt;
        LogBuffer m; log_init(&m); log_prefix(&m);
        log_text(&m, "V162 RIDE VEHICLE PROBE #"); log_uint(&m, (UINT64)pidx);
        log_text(&m, ": action="); log_uint(&m, (UINT64)action);
        log_text(&m, ", state="); log_uint(&m, (UINT64)state);
        log_text(&m, ", handle="); log_uint(&m, handle);
        log_text(&m, ", resolvedHandle="); log_uint(&m, localHandle);
        log_text(&m, ", vehicle="); log_uint(&m, (UINT64)vehicle);
        log_text(&m, ", vtableRva="); log_uint(&m, vtRva);
        log_text(&m, ", isCoffin="); log_uint(&m, isCoffin ? 1u : 0u);
        log_text(&m, ". Passive 1001BD0 pre-call probe.\r\n"); append_log(&m);
    }
    if (!isCoffin) return;
    UINT64 previous = __atomic_exchange_n(&g_v161RideCoffinVehicle, (UINT64)vehicle, __ATOMIC_ACQ_REL);
    __atomic_store_n(&g_v161RideCoffinHandle, handle, __ATOMIC_RELEASE);
    if (previous != (UINT64)vehicle || rememberedHandle != handle) {
        LONG idx = __atomic_fetch_add(&g_v161RideCaptureLogs, 1, __ATOMIC_ACQ_REL);
        if (idx < 16) {
            UINT64 physics = readable_range((BYTE*)vehicle + 0x1A00u, sizeof(UINT64))
                ? *(UINT64*)((BYTE*)vehicle + 0x1A00u) : 0u;
            LogBuffer m; log_init(&m); log_prefix(&m);
            log_text(&m, "V162 RIDE COFFIN CAPTURE #"); log_uint(&m, (UINT64)idx);
            log_text(&m, ": vehicle="); log_uint(&m, (UINT64)vehicle);
            log_text(&m, ", handle="); log_uint(&m, handle);
            log_text(&m, ", physics="); log_uint(&m, physics);
            log_text(&m, ". Native 1001BD0 vehicle resolver; read-only capture.\r\n"); append_log(&m);
        }
    }
}

// v1.36: Hook the four direct callsites that actually invoke 1001BD0.
static void __fastcall coffin_ride_update_scope(void* action, float deltaSeconds, char mode) {
    float before198 = -999.0f, before3710 = -999.0f;
    float after198 = -999.0f, after3710 = -999.0f;
    void* rideControl = 0;
    bool touched198 = false, touched3710 = false;

    v161_capture_ridden_coffin(action);

    if (g_testHoldRideScalar && action) {
        if (readable_range((BYTE*)action + 0x198u, sizeof(float))) {
            before198 = *(float*)((BYTE*)action + 0x198u);
            *(float*)((BYTE*)action + 0x198u) = 1.0f;
            touched198 = true;
        }
        if (readable_range((BYTE*)action + 0x30u, sizeof(void*))) {
            rideControl = *(void**)((BYTE*)action + 0x30u);
        }
        if (rideControl && readable_range((BYTE*)rideControl + 0x3710u, sizeof(float)) &&
            readable_range((BYTE*)rideControl + 0x3700u, sizeof(BYTE))) {
            before3710 = *(float*)((BYTE*)rideControl + 0x3710u);
            *(BYTE*)((BYTE*)rideControl + 0x3700u) |= 4u;
            *(float*)((BYTE*)rideControl + 0x3710u) = 8.0f;
            touched3710 = true;
        }
    }

    if (g_nativeCoffinRideUpdate) g_nativeCoffinRideUpdate(action, deltaSeconds, mode);

    if (touched198 && readable_range((BYTE*)action + 0x198u, sizeof(float)))
        after198 = *(float*)((BYTE*)action + 0x198u);
    if (touched3710 && rideControl && readable_range((BYTE*)rideControl + 0x3710u, sizeof(float)))
        after3710 = *(float*)((BYTE*)rideControl + 0x3710u);

    LONG index = __atomic_fetch_add(&g_rideUpdateScopeEvents, 1, __ATOMIC_ACQ_REL);
    if (g_testHoldRideScalar && index < 96 &&
        (index < 12 || before198 < 0.99f || before3710 < 7.90f ||
         after198 < 0.99f || after3710 < 7.90f)) {
        LogBuffer m; log_init(&m); log_prefix(&m);
        log_text(&m, "RIDE UPDATE 1001BD0 #"); log_uint(&m, (UINT64)index);
        log_text(&m, ": action="); log_uint(&m, (UINT64)action);
        log_text(&m, ", mode="); log_uint(&m, (UINT64)(BYTE)mode);
        log_text(&m, ", smooth198="); log_float2(&m, before198); log_text(&m, "->"); log_float2(&m, after198);
        log_text(&m, ", player3710="); log_float2(&m, before3710); log_text(&m, "->"); log_float2(&m, after3710);
        log_text(&m, ", forcedBeforeNative="); log_uint(&m, (UINT64)(g_testHoldRideScalar ? 1u : 0u));
        log_text(&m, ".\r\n"); append_log(&m);
    }
}

static void* allocate_call_relay_near(BYTE* nearAddress, void* destination) {
    const UINT64 granularity = 0x10000ull;
    const UINT64 maxDistance = 0x70000000ull;
    UINT64 origin = ((UINT64)nearAddress) & ~(granularity - 1ull);
    for (UINT64 distance = 0x01000000ull; distance <= maxDistance; distance += 0x01000000ull) {
        UINT64 hints[2] = { origin + distance, origin > distance ? origin - distance : 0ull };
        for (UINT32 i = 0; i < 2u; ++i) {
            if (!hints[i]) continue;
            BYTE* relay = (BYTE*)VirtualAlloc((void*)hints[i], 0x1000u, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
            if (!relay) continue;
            INT64 disp = (INT64)relay - (INT64)(nearAddress + 5u);
            if (disp < -0x80000000ll || disp > 0x7fffffffll) continue;
            static const BYTE jmp[6] = {0xFF,0x25,0x00,0x00,0x00,0x00};
            memcpy(relay, jmp, sizeof(jmp));
            UINT64 target = (UINT64)destination; memcpy(relay + 6u, &target, sizeof(target));
            FlushInstructionCache((HANDLE)(INT64)-1, relay, 14u);
            return relay;
        }
    }
    return 0;
}


static BYTE* allocate_v163_interaction_gate_relay_near(BYTE* nearAddress) {
    const UINT64 granularity = 0x10000ull;
    const UINT64 maxDistance = 0x70000000ull;
    UINT64 origin = ((UINT64)nearAddress) & ~(granularity - 1ull);
    for (UINT64 distance = 0x01000000ull; distance <= maxDistance; distance += 0x01000000ull) {
        UINT64 hints[2] = { origin + distance, origin > distance ? origin - distance : 0ull };
        for (UINT32 i = 0; i < 2u; ++i) {
            if (!hints[i]) continue;
            BYTE* relay = (BYTE*)VirtualAlloc((void*)hints[i], 0x1000u, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
            if (!relay) continue;
            INT64 disp = (INT64)relay - (INT64)(nearAddress + 5u);
            if (disp < -0x80000000ll || disp > 0x7fffffffll) { continue; }
            // Original FFFBC0 keeps its action pointer in nonvolatile RDI at the 11CA860 call.
            // Tail-jump into a normal __fastcall helper after copying RDI -> RDX:
            // RCX = native interaction manager, RDX = RideFloater action.
            const BYTE prefix[9] = {0x48,0x89,0xFA, 0xFF,0x25,0x00,0x00,0x00,0x00};
            memcpy(relay, prefix, sizeof(prefix));
            UINT64 helper = (UINT64)&v163_interaction_gate_scope;
            memcpy(relay + sizeof(prefix), &helper, sizeof(helper));
            FlushInstructionCache((HANDLE)(INT64)-1, relay, sizeof(prefix) + sizeof(helper));
            return relay;
        }
    }
    return 0;
}

static bool install_v163_direct_interaction_gate(HMODULE executable) {
    if (!g_enableOutsideNetworkMount) return true;
    BYTE* base = (BYTE*)executable;
    BYTE* call = base + RVA_V163_INTERACTION_GATE_CALL;
    BYTE* nativeTarget = base + RVA_V163_INTERACTION_GATE_NATIVE;
    if (!readable_range(call, 5u) || call[0] != 0xE8u) return false;
    int oldDisp = 0; memcpy(&oldDisp, call + 1u, sizeof(oldDisp));
    if (call + 5u + oldDisp != nativeTarget) return false;
    g_nativeV163InteractionGate = (V163InteractionGateFn)nativeTarget;
    BYTE* relay = allocate_v163_interaction_gate_relay_near(call);
    if (!relay) return false;
    INT64 newDisp64 = (INT64)relay - (INT64)(call + 5u);
    if (newDisp64 < -0x80000000ll || newDisp64 > 0x7fffffffll) return false;
    int newDisp = (int)newDisp64;
    if (!freeze_for_patch(call, 5u)) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(call, 5u, PAGE_EXECUTE_READWRITE, &oldProtection)) { unfreeze_after_patch(); return false; }
    memcpy(call + 1u, &newDisp, sizeof(newDisp));
    FlushInstructionCache((HANDLE)(INT64)-1, call, 5u);
    int verifyDisp = 0; memcpy(&verifyDisp, call + 1u, sizeof(verifyDisp));
    bool installed = call[0] == 0xE8u && call + 5u + verifyDisp == relay;
    if (!installed) {
        memcpy(call + 1u, &oldDisp, sizeof(oldDisp));
        FlushInstructionCache((HANDLE)(INT64)-1, call, 5u);
    }
    DWORD ignored = 0; VirtualProtect(call, 5u, oldProtection, &ignored); unfreeze_after_patch();
    if (installed) g_v163InteractionGateInstalled = 1;
    return installed;
}


// v1.43/v10: DSPhysicsCoffinInstance inherits the shared 1F457A0 attenuation
// virtual, but its own vtable slot can be redirected without touching Truck/Bike.
// The wrapper clears only the four fields proven by v10 to arm/hold the wheel
// attenuation (+5DC/+5DD/+60C/+61B) and restores +540 to 1.0 before native code.
static void __fastcall coffin_physics_attenuation_scope(void* physics, float deltaSeconds) {
    if (physics && g_enableNetworkTraversal) {
        BYTE* p = (BYTE*)physics;
        if (readable_range(p + 0x61Bu, 1u) && readable_range(p + 0x60Cu, sizeof(float)) &&
            readable_range(p + 0x5DCu, 2u) && readable_range(p + 0x540u, sizeof(float))) {
            p[0x61B] = 0u;
            p[0x5DC] = 0u;
            p[0x5DD] = 0u;
            *(float*)(p + 0x60C) = 0.0f;
            *(float*)(p + 0x540) = 1.0f;
        }
    }
    if (g_nativeCoffinPhysicsAttenuation) g_nativeCoffinPhysicsAttenuation(physics, deltaSeconds);
}

static bool install_coffin_physics_attenuation_hook(HMODULE executable) {
    if (!g_enableNetworkTraversal) return true;
    BYTE* base = (BYTE*)executable;
    void** vtable = (void**)(base + RVA_COFFIN_PHYSICS_VTABLE);
    void** slot = (void**)((BYTE*)vtable + RVA_COFFIN_PHYSICS_ATTENUATION_SLOT);
    void* expected = (void*)(base + RVA_COFFIN_PHYSICS_ATTENUATION_VFUNC);
    if (!readable_range(slot, sizeof(void*)) || *slot != expected) return false;
    g_nativeCoffinPhysicsAttenuation = (CoffinPhysicsAttenuationFn)expected;
    if (!freeze_for_patch((const BYTE*)slot, sizeof(void*))) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(slot, sizeof(void*), PAGE_READWRITE, &oldProtection)) { unfreeze_after_patch(); return false; }
    *slot = (void*)&coffin_physics_attenuation_scope;
    FlushInstructionCache((HANDLE)(INT64)-1, slot, sizeof(void*));
    bool installed = *slot == (void*)&coffin_physics_attenuation_scope;
    if (!installed) *slot = expected;
    DWORD ignored = 0; VirtualProtect(slot, sizeof(void*), oldProtection, &ignored); unfreeze_after_patch();
    if (!installed) return false;
    g_coffinPhysicsAttenuationHookInstalled = 1;
    return true;
}

// At EDA26D RCX is already the resolved vehicle Entity.  Native code calls its
// virtual +0x50 method and rejects the candidate if AL != 0.  v10 resolves this
// exact dynamic-dispatch gap.  This helper preserves the native virtual result
// for every vehicle except type 3 (Coffin Board), and only this callsite uses it.
static BYTE __fastcall coffin_candidate_filter_scope(void* filterObject, void* candidateRecord, void* root) {
    if (!filterObject || !readable_range(filterObject, sizeof(void*))) return 1u;
    void** vtable = *(void***)filterObject;
    if (!vtable || !readable_range(vtable + 10u, sizeof(void*))) return 1u;
    typedef BYTE (__fastcall* RejectFn)(void*);
    RejectFn nativeReject = (RejectFn)vtable[10u]; // slot +0x50 of the resolved filter object
    if (!nativeReject || !readable_range((void*)nativeReject, 1u)) return 1u;
    BYTE nativeResult = nativeReject(filterObject);

    // v10/EDCFE0 proves candidateRecord+0xA0 is the VehicleEntity pointer and
    // VehicleEntity+0x490 == 3 is the Coffin Board type.  v1.43 incorrectly
    // applied +0x490 to filterObject itself, which is reached through
    // VehicleEntity+0x68 -> handle -> +0x20 and is not the VehicleEntity.
    UINT32 vehicleType = 0xffffffffu;
    void* vehicleEntity = 0;
    if (candidateRecord && readable_range((BYTE*)candidateRecord + 0xA0u, sizeof(void*))) {
        vehicleEntity = *(void**)((BYTE*)candidateRecord + 0xA0u);
        if (vehicleEntity && readable_range((BYTE*)vehicleEntity + OFF_VEHICLE_TYPE, sizeof(UINT32)))
            vehicleType = *(UINT32*)((BYTE*)vehicleEntity + OFF_VEHICLE_TYPE);
    }

    __atomic_fetch_add(&g_coffinCandidateFilterHitCount, 1, __ATOMIC_ACQ_REL);
    if (vehicleType == VEHICLE_TYPE_COFFIN_BOARD) {
        LONG seen = __atomic_fetch_add(&g_coffinCandidateSeenCount, 1, __ATOMIC_ACQ_REL);
        if (seen < 24) {
            UINT32 rootCandidateCount=0, rootSpatialCount=0; void* rootSelected=0; UINT32 rootFlags218=0; UINT64 rootHandle220=0;
            if(root && readable_range((BYTE*)root+0x1E4u,sizeof(UINT32))) rootCandidateCount=*(UINT32*)((BYTE*)root+0x1E0u);
            if(root && readable_range((BYTE*)root+0x12D98u,sizeof(UINT32))) rootSpatialCount=*(UINT32*)((BYTE*)root+0x12D98u);
            if(root && readable_range((BYTE*)root+0x58u,sizeof(void*))) rootSelected=*(void**)((BYTE*)root+0x58u);
            if(root && readable_range((BYTE*)root+0x218u,sizeof(BYTE))) rootFlags218=*(BYTE*)((BYTE*)root+0x218u);
            if(root && readable_range((BYTE*)root+0x220u,sizeof(UINT64))) rootHandle220=*(UINT64*)((BYTE*)root+0x220u);
            LogBuffer t; log_init(&t); log_prefix(&t); log_text(&t,"TRACE EDA270 COFFIN #"); log_uint(&t,(UINT64)seen);
            log_text(&t,": native="); log_text(&t,nativeResult?"REJECT":"ACCEPT"); log_text(&t,", root="); log_uint(&t,(UINT64)root);
            log_text(&t,", list1E0="); log_uint(&t,rootCandidateCount); log_text(&t,", spatial12D98="); log_uint(&t,rootSpatialCount);
            log_text(&t,", selected58="); log_uint(&t,(UINT64)rootSelected); log_text(&t,", flags218="); log_uint(&t,rootFlags218);
            log_text(&t,", handle220="); log_uint(&t,rootHandle220); log_text(&t,". Native result unchanged.\r\n"); append_log(&t);
        }
    } else {
        LONG other = __atomic_fetch_add(&g_nonCoffinCandidateLogCount, 1, __ATOMIC_ACQ_REL);
        if (other < 4) log_line("OUTSIDE MOUNT: EDA270 candidate filter hit non-Coffin/unknown candidate.");
    }

    if (!g_enableOutsideNetworkMount || vehicleType != VEHICLE_TYPE_COFFIN_BOARD || nativeResult == 0u)
        return nativeResult;

    LONG index = __atomic_fetch_add(&g_coffinCandidateFilterOverrideCount, 1, __ATOMIC_ACQ_REL);
    if (index < 4) log_line("OUTSIDE MOUNT: native +0x50 rejection overridden for candidate+0xA0 Coffin Board at ED9EB0 admission.");
    return 0u;
}

static bool install_coffin_candidate_filter_hook(HMODULE executable) {
    if (!g_enableOutsideNetworkMount && !g_enableNetworkTrace) return true;
    BYTE* base = (BYTE*)executable;
    BYTE* target = base + RVA_COFFIN_CANDIDATE_FILTER_BLOCK;
    // 48 8B 01          mov rax,[rcx]
    // FF 50 50          call qword ptr [rax+50]
    // 84 C0             test al,al
    // 75 15             jnz EDA28C
    // 48 8B 47 50       mov rax,[rdi+50]
    static const BYTE expected[14] = {0x48,0x8B,0x01,0xFF,0x50,0x50,0x84,0xC0,0x75,0x15,0x48,0x8B,0x47,0x50};
    if (!readable_range(target, sizeof(expected)) || !bytes_equal(target, expected, sizeof(expected))) return false;
    BYTE* cave = (BYTE*)VirtualAlloc(0, 0x1000u, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!cave) return false;
    SIZE_T q = 0;
    // RCX already holds the resolved entity. Reserve Win64 shadow space while preserving alignment.
    // RCX is the resolved filter object.  RSI is the candidate record; pass it
    // as arg2 so the helper can scope on candidate+0xA0 -> VehicleEntity.
    const BYTE movRdxRsi[3] = {0x48,0x89,0xF2}; memcpy(cave+q,movRdxRsi,3); q+=3;
    const BYTE movR8Rdi[3] = {0x49,0x89,0xF8}; memcpy(cave+q,movR8Rdi,3); q+=3;
    const BYTE subRsp[4] = {0x48,0x83,0xEC,0x20}; memcpy(cave+q, subRsp, sizeof(subRsp)); q += sizeof(subRsp);
    // mov rax, imm64(helper); call rax
    const BYTE movRax[2] = {0x48,0xB8}; memcpy(cave+q,movRax,2); q+=2; UINT64 helper=(UINT64)&coffin_candidate_filter_scope; memcpy(cave+q,&helper,8); q+=8;
    const BYTE callRax[2] = {0xFF,0xD0}; memcpy(cave+q,callRax,2); q+=2;
    const BYTE addRsp[4] = {0x48,0x83,0xC4,0x20}; memcpy(cave+q,addRsp,4); q+=4;
    // test al,al; jz accepted (+14 bytes: mov rax,[rdi+50] + absolute jmp)
    const BYTE testAl[2] = {0x84,0xC0}; memcpy(cave+q,testAl,2); q+=2;
    const BYTE jzAccepted[2] = {0x74,0x0E}; memcpy(cave+q,jzAccepted,2); q+=2;
    // rejected: absolute jmp EDA28C
    const BYTE absJmp[6] = {0xFF,0x25,0,0,0,0}; memcpy(cave+q,absJmp,6); q+=6; UINT64 reject=(UINT64)(base+RVA_COFFIN_CANDIDATE_REJECT); memcpy(cave+q,&reject,8); q+=8;
    // accepted: replay mov rax,[rdi+50], then absolute jmp EDA27B
    const BYTE replay[4] = {0x48,0x8B,0x47,0x50}; memcpy(cave+q,replay,4); q+=4;
    memcpy(cave+q,absJmp,6); q+=6; UINT64 accept=(UINT64)(base+RVA_COFFIN_CANDIDATE_ACCEPT); memcpy(cave+q,&accept,8); q+=8;
    FlushInstructionCache((HANDLE)(INT64)-1,cave,q);

    BYTE replacement[14] = {0xFF,0x25,0,0,0,0,0,0,0,0,0,0,0,0};
    UINT64 caveAddress=(UINT64)cave; memcpy(replacement+6,&caveAddress,8);
    if (!freeze_for_patch(target, sizeof(replacement))) return false;
    DWORD oldProtection=0;
    if(!VirtualProtect(target,sizeof(replacement),PAGE_EXECUTE_READWRITE,&oldProtection)){unfreeze_after_patch();return false;}
    memcpy(target,replacement,sizeof(replacement)); FlushInstructionCache((HANDLE)(INT64)-1,target,sizeof(replacement));
    bool installed=bytes_equal(target,replacement,sizeof(replacement));
    if(!installed){memcpy(target,expected,sizeof(expected));FlushInstructionCache((HANDLE)(INT64)-1,target,sizeof(expected));}
    DWORD ignored=0;VirtualProtect(target,sizeof(replacement),oldProtection,&ignored);unfreeze_after_patch();
    if(!installed)return false;
    g_coffinCandidateFilterHookInstalled=1;
    return true;
}


static bool install_abs_cave_patch(BYTE* target, const BYTE* expected, SIZE_T patchLen, const BYTE* caveCode, SIZE_T caveLen) {
    if (!target || !expected || patchLen < 14u || !readable_range(target, patchLen) || !bytes_equal(target, expected, patchLen)) return false;
    BYTE* cave = (BYTE*)VirtualAlloc(0, 0x1000u, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!cave) return false;
    memcpy(cave, caveCode, caveLen);
    FlushInstructionCache((HANDLE)(INT64)-1, cave, caveLen);
    BYTE replacement[32];
    if (patchLen > sizeof(replacement)) return false;
    for (SIZE_T i=0;i<patchLen;++i) replacement[i]=0x90;
    replacement[0]=0xFF; replacement[1]=0x25; replacement[2]=0; replacement[3]=0; replacement[4]=0; replacement[5]=0;
    UINT64 caveAddress=(UINT64)cave; memcpy(replacement+6,&caveAddress,8);
    if (!freeze_for_patch(target,patchLen)) return false;
    DWORD oldProtection=0;
    if(!VirtualProtect(target,patchLen,PAGE_EXECUTE_READWRITE,&oldProtection)){unfreeze_after_patch();return false;}
    memcpy(target,replacement,patchLen); FlushInstructionCache((HANDLE)(INT64)-1,target,patchLen);
    bool installed=bytes_equal(target,replacement,patchLen);
    if(!installed){memcpy(target,expected,patchLen);FlushInstructionCache((HANDLE)(INT64)-1,target,patchLen);}
    DWORD ignored=0;VirtualProtect(target,patchLen,oldProtection,&ignored);unfreeze_after_patch();
    return installed;
}

// v1.44: patch the actual DSPlayerVehicleDriving writers instead of assuming
// the shared attenuation virtual is the active Coffin dispatch.  Each cave uses
// the VehicleEntity pointer already live in the original function and suppresses
// only type 3 (Coffin Board).  Other vehicles execute the original write.
static bool install_coffin_direct_stopstate_patches(HMODULE executable) {
    if (!g_enableNetworkTraversal) return true;
    BYTE* base=(BYTE*)executable;

    // 1F4FA8C: mov rax,[rax+340]; mov byte [rax+5DC],1
    static const BYTE expA[14]={0x48,0x8B,0x80,0x40,0x03,0x00,0x00,0xC6,0x80,0xDC,0x05,0x00,0x00,0x01};
    BYTE codeA[96]; SIZE_T a=0;
    // cmp dword ptr [rax+490],3
    // Build explicitly to make branch displacement obvious.
    const BYTE cmpA[7]={0x83,0xB8,0x90,0x04,0x00,0x00,0x03}; memcpy(codeA+a,cmpA,7);a+=7;
    const BYTE jeA[2]={0x74,0x0E}; memcpy(codeA+a,jeA,2);a+=2;
    const BYTE origA1[7]={0x48,0x8B,0x80,0x40,0x03,0x00,0x00}; memcpy(codeA+a,origA1,7);a+=7;
    const BYTE origA2[7]={0xC6,0x80,0xDC,0x05,0x00,0x00,0x01}; memcpy(codeA+a,origA2,7);a+=7;
    // skip target: replay mov rax,[rbx+30]
    const BYTE reloadA[4]={0x48,0x8B,0x43,0x30}; memcpy(codeA+a,reloadA,4);a+=4;
    const BYTE absJmp[6]={0xFF,0x25,0,0,0,0}; memcpy(codeA+a,absJmp,6);a+=6; UINT64 retA=(UINT64)(base+0x01F4FA9Eu); memcpy(codeA+a,&retA,8);a+=8;
    if(!install_abs_cave_patch(base+RVA_COFFIN_STOPSTATE_5DC_A,expA,sizeof(expA),codeA,a)) return false;

    // 1F4FAB5: mov rax,[r13+340]; shr ecx,0C; and cl,1; mov [rax+61B],cl
    static const BYTE expB[19]={0x49,0x8B,0x85,0x40,0x03,0x00,0x00,0xC1,0xE9,0x0C,0x80,0xE1,0x01,0x88,0x88,0x1B,0x06,0x00,0x00};
    BYTE codeB[96]; SIZE_T b=0;
    const BYTE b1[7]={0x49,0x8B,0x85,0x40,0x03,0x00,0x00}; memcpy(codeB+b,b1,7);b+=7;
    const BYTE b2[3]={0xC1,0xE9,0x0C}; memcpy(codeB+b,b2,3);b+=3;
    const BYTE b3[3]={0x80,0xE1,0x01}; memcpy(codeB+b,b3,3);b+=3;
    const BYTE cmpB[8]={0x41,0x83,0xBD,0x90,0x04,0x00,0x00,0x03}; memcpy(codeB+b,cmpB,8);b+=8;
    const BYTE jneB[2]={0x75,0x02}; memcpy(codeB+b,jneB,2);b+=2;
    const BYTE zeroCl[2]={0xB1,0x00}; memcpy(codeB+b,zeroCl,2);b+=2;
    const BYTE storeB[6]={0x88,0x88,0x1B,0x06,0x00,0x00}; memcpy(codeB+b,storeB,6);b+=6;
    memcpy(codeB+b,absJmp,6);b+=6; UINT64 retB=(UINT64)(base+0x01F4FAC8u); memcpy(codeB+b,&retB,8);b+=8;
    if(!install_abs_cave_patch(base+RVA_COFFIN_STOPSTATE_61B,expB,sizeof(expB),codeB,b)) return false;

    // 1F4FC1B: mov rax,[rbx+30]; mov rcx,[rax+340]; mov byte [rcx+5DC],1
    static const BYTE expC[18]={0x48,0x8B,0x43,0x30,0x48,0x8B,0x88,0x40,0x03,0x00,0x00,0xC6,0x81,0xDC,0x05,0x00,0x00,0x01};
    BYTE codeC[96]; SIZE_T c=0;
    const BYTE c1[4]={0x48,0x8B,0x43,0x30}; memcpy(codeC+c,c1,4);c+=4;
    const BYTE cmpC[7]={0x83,0xB8,0x90,0x04,0x00,0x00,0x03}; memcpy(codeC+c,cmpC,7);c+=7;
    const BYTE jeC[2]={0x74,0x0E}; memcpy(codeC+c,jeC,2);c+=2;
    const BYTE c2[7]={0x48,0x8B,0x88,0x40,0x03,0x00,0x00}; memcpy(codeC+c,c2,7);c+=7;
    const BYTE c3[7]={0xC6,0x81,0xDC,0x05,0x00,0x00,0x01}; memcpy(codeC+c,c3,7);c+=7;
    memcpy(codeC+c,absJmp,6);c+=6; UINT64 retC=(UINT64)(base+0x01F4FC2Du); memcpy(codeC+c,&retC,8);c+=8;
    if(!install_abs_cave_patch(base+RVA_COFFIN_STOPSTATE_5DC_B,expC,sizeof(expC),codeC,c)) return false;

    g_coffinDirectStopStatePatchInstalled=1;
    return true;
}

// v1.42 root-cause: preserve the native mode-6 predicate globally, but restore
// only this ED9EB0 callsite when it suppresses the spatial candidate rebuild.
// The wrapper never creates a target and never touches E3/state4/manager fields;
// all downstream native candidate/action filters still decide what is usable.
static BYTE __fastcall coffin_spatial_gate_scope(void* context, UINT32 mode, UINT32 flags) {
    BYTE nativeResult = g_nativeCoffinSpatialGate ? g_nativeCoffinSpatialGate(context, mode, flags) : 0u;
    if (nativeResult != 0u || !g_enableOutsideNetworkMount || mode != 6u) return nativeResult;
    LONG index = __atomic_fetch_add(&g_spatialGateOverrideCount, 1, __ATOMIC_ACQ_REL);
    if (index < 4) {
        log_line("OUTSIDE MOUNT: ED9EB0 mode-6 spatial-candidate gate restored; native downstream filters remain active.");
    }
    return 1u;
}

static bool install_coffin_progressive_decay_bypass(HMODULE executable) {
    if (!g_enableNetworkTraversal) return true;
    BYTE* branch = (BYTE*)executable + RVA_COFFIN_PROGRESSIVE_DECAY_ARM_BRANCH;
    static const BYTE expected[2] = {0x76, 0x09}; // jbe -> skip +5DD arm when threshold not exceeded
    static const BYTE replacement[2] = {0xEB, 0x09}; // always skip the +5DD arm write
    if (!readable_range(branch, sizeof(expected)) || !bytes_equal(branch, expected, sizeof(expected))) return false;
    if (!freeze_for_patch(branch, sizeof(replacement))) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(branch, sizeof(replacement), PAGE_EXECUTE_READWRITE, &oldProtection)) {
        unfreeze_after_patch(); return false;
    }
    memcpy(branch, replacement, sizeof(replacement));
    FlushInstructionCache((HANDLE)(INT64)-1, branch, sizeof(replacement));
    bool installed = bytes_equal(branch, replacement, sizeof(replacement));
    if (!installed) {
        memcpy(branch, expected, sizeof(expected));
        FlushInstructionCache((HANDLE)(INT64)-1, branch, sizeof(expected));
    }
    DWORD ignored = 0;
    VirtualProtect(branch, sizeof(replacement), oldProtection, &ignored);
    unfreeze_after_patch();
    if (!installed) return false;
    g_progressiveDecayPatchInstalled = 1;
    return true;
}

static bool install_coffin_spatial_gate_restore(HMODULE executable) {
    if (!g_enableOutsideNetworkMount) return true;
    BYTE* base = (BYTE*)executable;
    BYTE* call = base + RVA_COFFIN_SPATIAL_GATE_CALL;
    BYTE* nativeTarget = base + RVA_COFFIN_SPATIAL_GATE_NATIVE;
    if (!readable_range(call, 5u) || call[0] != 0xE8u) return false;
    int oldDisp = 0; memcpy(&oldDisp, call + 1u, sizeof(oldDisp));
    BYTE* resolved = call + 5u + oldDisp;
    if (resolved != nativeTarget) return false;
    g_nativeCoffinSpatialGate = (CoffinSpatialGateFn)nativeTarget;
    BYTE* relay = (BYTE*)allocate_call_relay_near(call, (void*)&coffin_spatial_gate_scope);
    if (!relay) return false;
    INT64 newDisp64 = (INT64)relay - (INT64)(call + 5u);
    if (newDisp64 < -0x80000000ll || newDisp64 > 0x7fffffffll) return false;
    int newDisp = (int)newDisp64;
    if (!freeze_for_patch(call, 5u)) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(call, 5u, PAGE_EXECUTE_READWRITE, &oldProtection)) {
        unfreeze_after_patch(); return false;
    }
    memcpy(call + 1u, &newDisp, sizeof(newDisp)); // keep E8 opcode intact
    FlushInstructionCache((HANDLE)(INT64)-1, call, 5u);
    int verifyDisp = 0; memcpy(&verifyDisp, call + 1u, sizeof(verifyDisp));
    bool installed = call[0] == 0xE8u && call + 5u + verifyDisp == relay;
    if (!installed) {
        memcpy(call + 1u, &oldDisp, sizeof(oldDisp));
        FlushInstructionCache((HANDLE)(INT64)-1, call, 5u);
    }
    DWORD ignored = 0;
    VirtualProtect(call, 5u, oldProtection, &ignored);
    unfreeze_after_patch();
    if (!installed) return false;
    g_spatialGatePatchInstalled = 1;
    return true;
}

static bool install_coffin_ride_update_callscope(HMODULE executable) {
    // v1.62: this callscope is also the passive ridden-Coffin capture path.
    // It must be installed for Outside-Mount even when the old scalar experiment is disabled.
    if (!g_enableOutsideNetworkMount && !g_testHoldRideScalar) return true;
    BYTE* base = (BYTE*)executable;
    BYTE* nativeTarget = base + RVA_COFFIN_RIDE_UPDATE;
    BYTE* firstCall = base + RVA_COFFIN_RIDE_UPDATE_CALLS[0];
    BYTE* relay = (BYTE*)allocate_call_relay_near(firstCall, (void*)&coffin_ride_update_scope);
    if (!relay) return false;

    for (UINT32 i = 0; i < 4u; ++i) {
        BYTE* call = base + RVA_COFFIN_RIDE_UPDATE_CALLS[i];
        if (!readable_range(call, 5u) || call[0] != 0xE8u) return false;
        int oldDisp = *(int*)(call + 1u);
        BYTE* resolved = call + 5u + oldDisp;
        if (resolved != nativeTarget) return false;
        INT64 newDisp64 = (INT64)relay - (INT64)(call + 5u);
        if (newDisp64 < -0x80000000ll || newDisp64 > 0x7fffffffll) return false;
    }

    g_nativeCoffinRideUpdate = (CoffinRideUpdateFn)nativeTarget;
    for (UINT32 i = 0; i < 4u; ++i) {
        BYTE* call = base + RVA_COFFIN_RIDE_UPDATE_CALLS[i];
        INT64 newDisp64 = (INT64)relay - (INT64)(call + 5u);
        int newDisp = (int)newDisp64;
        if (!freeze_for_patch(call, 5u)) return false;
        DWORD oldProtection = 0;
        if (!VirtualProtect(call, 5u, PAGE_EXECUTE_READWRITE, &oldProtection)) { unfreeze_after_patch(); return false; }
        *(int*)(call + 1u) = newDisp;
        FlushInstructionCache((HANDLE)(INT64)-1, call, 5u);
        DWORD ignored = 0; VirtualProtect(call, 5u, oldProtection, &ignored); unfreeze_after_patch();
        int verifyDisp = *(int*)(call + 1u);
        if (call[0] != 0xE8u || call + 5u + verifyDisp != relay) return false;
    }
    g_rideUpdateCallScopeInstalled = 1;
    return true;
}

static void __fastcall log_coffin_direct_state(void* state, UINT32 kind) {
    // DriveEntry is the first unambiguous proof that DS2 has completed the
    // Coffin mount and handed control to the real RideFloater state machine.
    // Restore only the D4 bit we kept alive; all ride/control state stays native.
    if (kind == 0u && __atomic_load_n(&g_v171MountTxnActive, __ATOMIC_ACQUIRE) != 0)
        v171_restore_mount_transaction("RideFloater-DriveEntry");
    if (!state || kind >= 7u) return;
    LONG index = __atomic_fetch_add(&g_coffinDirectStateCalls[kind], 1, __ATOMIC_ACQ_REL);

    UINT32 bb = 0xFFFFFFFFu, bc = 0xFFFFFFFFu, type = 0xFFFFFFFFu, id = 0xFFFFFFFFu;
    BYTE* base = (BYTE*)g_executable;
    void* manager = 0;
    if (base && readable_range(base + RVA_CHIRAL_WORLD_MANAGER_GLOBAL, sizeof(void*))) {
        manager = *(void**)(base + RVA_CHIRAL_WORLD_MANAGER_GLOBAL);
        if (manager && readable_range((BYTE*)manager + 0x2264C4u, sizeof(UINT32))) {
            bb = *(BYTE*)((BYTE*)manager + 0x2264BBu);
            bc = *(BYTE*)((BYTE*)manager + 0x2264BCu);
            type = *(UINT32*)((BYTE*)manager + 0x2264C0u);
            id = *(UINT32*)((BYTE*)manager + 0x2264C4u);
        }
    }

    if (kind == 5u) {
        UINT32 actionMode = 0xFFFFFFFFu;
        void* player = 0;
        float axisX = 0.0f, axisY = 0.0f;
        UINT32 movementFlag = 0xFFFFFFFFu;
        if (readable_range((BYTE*)state + 0x2A0u, sizeof(UINT32))) actionMode = *(UINT32*)((BYTE*)state + 0x2A0u);
        if (readable_range((BYTE*)state + 0x28u, sizeof(void*))) player = *(void**)((BYTE*)state + 0x28u);
        if (player && readable_range((BYTE*)player + 0x7358u, 1u)) {
            movementFlag = *(BYTE*)((BYTE*)player + 0x7358u) & 1u;
            if (readable_range((BYTE*)player + 0x3188u, sizeof(float))) {
                axisX = *(float*)((BYTE*)player + 0x3184u);
                axisY = *(float*)((BYTE*)player + 0x3188u);
            }
        }
        if (index < 96) {
            LogBuffer m; log_init(&m); log_prefix(&m);
            log_text(&m, "COFFIN BORDER MODE #"); log_uint(&m, (UINT64)index);
            log_text(&m, ": action="); log_uint(&m, (UINT64)state);
            log_text(&m, ", actionMode="); log_uint(&m, actionMode);
            log_text(&m, ", player="); log_uint(&m, (UINT64)player);
            log_text(&m, ", moveX="); log_float2(&m, axisX);
            log_text(&m, ", moveY="); log_float2(&m, axisY);
            log_text(&m, ", movementFlag="); log_uint(&m, movementFlag);
            log_text(&m, ", BB="); log_uint(&m, bb);
            log_text(&m, ", BC="); log_uint(&m, bc);
            log_text(&m, ", borderType="); log_uint(&m, type);
            log_text(&m, ", borderId="); log_uint(&m, id);
            log_text(&m, ". Native border-mode controller still executes.\r\n"); append_log(&m);
        }
        return;
    }

    if (kind == 6u) {
        UINT32 actionMode = 0xFFFFFFFFu, candidateCount = 0xFFFFFFFFu;
        void* root = 0;
        void* activity = 0;
        UINT32 activity5D8 = 0, activity5E8 = 0, activity5E4 = 0;
        UINT32 activity5D0 = 0;
        UINT32 playerStateA = 0xFFFFFFFFu, playerStateB = 0xFFFFFFFFu;
        UINT32 actionFlags = 0;
        if (readable_range((BYTE*)state + 0x2A0u, sizeof(UINT32))) actionMode = *(UINT32*)((BYTE*)state + 0x2A0u);
        if (readable_range((BYTE*)state + 0x60u, sizeof(void*))) root = *(void**)((BYTE*)state + 0x60u);
        if (root && readable_range((BYTE*)root + 0xD98u + 0x12000u, sizeof(UINT32)))
            candidateCount = *(UINT32*)((BYTE*)root + 0xD98u + 0x12000u);
        if (readable_range((BYTE*)state + 0x50u, sizeof(void*))) {
            void* p = *(void**)((BYTE*)state + 0x50u);
            if (p && readable_range((BYTE*)p + 0x2A8u, sizeof(void*))) activity = *(void**)((BYTE*)p + 0x2A8u);
        }
        if (activity && readable_range((BYTE*)activity + 0x5E8u, sizeof(UINT32))) {
            activity5D8 = *(UINT32*)((BYTE*)activity + 0x5D8u);
            activity5E8 = *(UINT32*)((BYTE*)activity + 0x5E8u);
            activity5E4 = *(UINT32*)((BYTE*)activity + 0x5E4u);
            activity5D0 = *(BYTE*)((BYTE*)activity + 0x5D0u);
            __atomic_store_n(&g_lastCoffinActivity, (UINT64)activity, __ATOMIC_RELEASE);
            if (g_testClearActivity5D0Bit2 && (activity5D0 & 0x02u) != 0u &&
                writable_range((BYTE*)activity + 0x5D0u, sizeof(BYTE))) {
                BYTE masked = (BYTE)(activity5D0 & (BYTE)~0x02u);
                *(BYTE*)((BYTE*)activity + 0x5D0u) = masked;
                LONG maskIndex = __atomic_fetch_add(&g_activity5D0MaskEvents, 1, __ATOMIC_ACQ_REL);
                if (maskIndex < 64) {
                    LogBuffer t; log_init(&t); log_prefix(&t);
                    log_text(&t, "ACTIVITY5D0 MASK #"); log_uint(&t, (UINT64)maskIndex);
                    log_text(&t, ": activity="); log_uint(&t, (UINT64)activity);
                    log_text(&t, ", before="); log_uint(&t, activity5D0);
                    log_text(&t, ", after="); log_uint(&t, masked);
                    log_text(&t, ". Diagnostic bit-0x02 clear only; all other activity bits preserved.\r\n"); append_log(&t);
                }
            }
        }
        if (readable_range((BYTE*)state + 0x28u, sizeof(void*))) {
            void* p = *(void**)((BYTE*)state + 0x28u);
            if (p && readable_range((BYTE*)p + 0x7550u, sizeof(UINT32))) {
                playerStateA = *(UINT32*)((BYTE*)p + 0x754Cu);
                playerStateB = *(UINT32*)((BYTE*)p + 0x7550u);
            }
        }
        if (readable_range((BYTE*)state + 0x38u, sizeof(void*))) {
            void* p = *(void**)((BYTE*)state + 0x38u);
            if (p && readable_range((BYTE*)p + 0x5F4u, sizeof(UINT32))) actionFlags = *(UINT32*)((BYTE*)p + 0x5F4u);
        }
        if (index < 48) {
            LogBuffer m; log_init(&m); log_prefix(&m);
            log_text(&m, "COFFIN 4C PRODUCER ENTRY #"); log_uint(&m, (UINT64)index);
            log_text(&m, ": action="); log_uint(&m, (UINT64)state);
            log_text(&m, ", root="); log_uint(&m, (UINT64)root);
            UINT64 stateTarget150 = readable_range((BYTE*)state + 0x150u, sizeof(UINT64)) ? *(UINT64*)((BYTE*)state + 0x150u) : 0ull;
            UINT32 stateFlag11A = readable_range((BYTE*)state + 0x11Au, sizeof(WORD)) ? *(WORD*)((BYTE*)state + 0x11Au) : 0xFFFFFFFFu;
            log_text(&m, ", actionMode="); log_uint(&m, actionMode);
            log_text(&m, ", target150="); log_uint(&m, stateTarget150);
            log_text(&m, ", flag11A="); log_uint(&m, stateFlag11A);
            log_text(&m, ", candidateCount="); log_uint(&m, candidateCount);
            log_text(&m, ", activity="); log_uint(&m, (UINT64)activity);
            log_text(&m, ", f5D8="); log_uint(&m, activity5D8);
            log_text(&m, ", f5E8="); log_uint(&m, activity5E8);
            log_text(&m, ", f5E4="); log_uint(&m, activity5E4);
            log_text(&m, ", b5D0="); log_uint(&m, activity5D0);
            log_text(&m, ", player754C="); log_uint(&m, playerStateA);
            log_text(&m, ", player7550="); log_uint(&m, playerStateB);
            log_text(&m, ", action5F4="); log_uint(&m, actionFlags);
            log_text(&m, ", BB="); log_uint(&m, bb);
            log_text(&m, ", BC="); log_uint(&m, bc);
            log_text(&m, ", borderType="); log_uint(&m, type);
            log_text(&m, ". Native producer still executes.\r\n"); append_log(&m);
        }
        return;
    }

    void* vtable = readable_range(state, sizeof(void*)) ? *(void**)state : 0;
    void* stateData = readable_range((BYTE*)state + 0x88u, sizeof(void*)) ? *(void**)((BYTE*)state + 0x88u) : 0;
    void* actionData = readable_range((BYTE*)state + 0x90u, sizeof(void*)) ? *(void**)((BYTE*)state + 0x90u) : 0;
    void* player = readable_range((BYTE*)state + 0x98u, sizeof(void*)) ? *(void**)((BYTE*)state + 0x98u) : 0;
    void* action = readable_range((BYTE*)state + 0x190u, sizeof(void*)) ? *(void**)((BYTE*)state + 0x190u) : 0;
    // Static audit of 1001BD0: param_1+0x198 is smoothed each RideFloater
    // update and then multiplied by 8 into player/control+0x3710. For one
    // causal test, reset only those two native scalars before DriveTick. The
    // native 1001BD0 update still executes immediately afterwards, so this
    // prevents cumulative decay without skipping any state/physics function.
    if (kind == 3u && g_testHoldRideScalar && action &&
        readable_range((BYTE*)action + 0x198u, sizeof(float)) &&
        readable_range((BYTE*)action + 0x30u, sizeof(void*))) {
        float beforeSmooth = *(float*)((BYTE*)action + 0x198u);
        void* rideControl = *(void**)((BYTE*)action + 0x30u);
        float before3710 = -999.0f;
        if (rideControl && readable_range((BYTE*)rideControl + 0x3710u, sizeof(float))) {
            before3710 = *(float*)((BYTE*)rideControl + 0x3710u);
        }
        *(float*)((BYTE*)action + 0x198u) = 1.0f;
        if (rideControl && readable_range((BYTE*)rideControl + 0x3710u, sizeof(float)) &&
            readable_range((BYTE*)rideControl + 0x3700u, sizeof(BYTE))) {
            *(BYTE*)((BYTE*)rideControl + 0x3700u) = *(BYTE*)((BYTE*)rideControl + 0x3700u) | 4u;
            *(float*)((BYTE*)rideControl + 0x3710u) = 8.0f;
        }
        LONG scalarEvent = __atomic_fetch_add(&g_rideScalarHoldEvents, 1, __ATOMIC_ACQ_REL);
        if (scalarEvent < 96 && (scalarEvent < 8 || beforeSmooth < 0.99f || before3710 < 7.90f)) {
            LogBuffer t; log_init(&t); log_prefix(&t);
            log_text(&t, "RIDE SCALAR HOLD #"); log_uint(&t, (UINT64)scalarEvent);
            log_text(&t, ": action="); log_uint(&t, (UINT64)action);
            log_text(&t, ", smooth198Before="); log_float2(&t, beforeSmooth);
            log_text(&t, ", player3710Before="); log_float2(&t, before3710);
            log_text(&t, ", forced198=1.00, forced3710=8.00. Native 1001BD0 still executes.\r\n"); append_log(&t);
        }
    }
    UINT32 stateValue = 0xFFFFFFFFu;
    if (stateData && readable_range((BYTE*)stateData + 0x11Au, sizeof(WORD))) stateValue = *(WORD*)((BYTE*)stateData + 0x11Au);

    bool changed = bb != g_coffinDirectStateLastBB[kind] || bc != g_coffinDirectStateLastBC[kind] ||
                   type != g_coffinDirectStateLastType[kind] || id != g_coffinDirectStateLastId[kind] ||
                   stateValue != g_coffinDirectStateLastState[kind];
    if (index < 4 || changed || (kind == 3u && index < 64) || (kind == 4u && index < 64)) {
        g_coffinDirectStateLastBB[kind] = bb; g_coffinDirectStateLastBC[kind] = bc;
        g_coffinDirectStateLastType[kind] = type; g_coffinDirectStateLastId[kind] = id;
        g_coffinDirectStateLastState[kind] = stateValue;
        const char* label = kind == 0u ? "DriveEntry" :
                            (kind == 1u ? "Falling" :
                            (kind == 2u ? "Kick" :
                            (kind == 3u ? "DriveTick" : "EndMotion")));
        LogBuffer message; log_init(&message); log_prefix(&message);
        log_text(&message, "COFFIN DIRECT STATE "); log_text(&message, label);
        log_text(&message, " #"); log_uint(&message, (UINT64)index);
        log_text(&message, ": state="); log_uint(&message, (UINT64)state);
        log_text(&message, ", vtable="); log_uint(&message, (UINT64)vtable);
        log_text(&message, ", stateData="); log_uint(&message, (UINT64)stateData);
        log_text(&message, ", actionData="); log_uint(&message, (UINT64)actionData);
        log_text(&message, ", player="); log_uint(&message, (UINT64)player);
        log_text(&message, ", action="); log_uint(&message, (UINT64)action);
        log_text(&message, ", stateValue="); log_uint(&message, stateValue);
        if (kind == 4u && player && readable_range((BYTE*)player + 0x3890u, sizeof(float))) {
            float driveScalar = *(float*)((BYTE*)player + 0x3890u);
            float endMetric = -1.0f;
            void* motion = readable_range((BYTE*)state + 0xA8u, sizeof(void*)) ? *(void**)((BYTE*)state + 0xA8u) : 0;
            if (motion && readable_range((BYTE*)motion + 0xB6Cu, sizeof(float))) endMetric = *(float*)((BYTE*)motion + 0xB6Cu);
            log_text(&message, ", drive3890="); log_float2(&message, driveScalar);
            log_text(&message, ", endB6C="); log_float2(&message, endMetric);
        }
        log_text(&message, ", BB="); log_uint(&message, bb);
        log_text(&message, ", BC="); log_uint(&message, bc);
        log_text(&message, ", borderType="); log_uint(&message, type);
        log_text(&message, ", borderId="); log_uint(&message, id);
        log_text(&message, ". Native state method still executes.\r\n"); append_log(&message);
    }
}

static bool install_direct_state_entry_trace(HMODULE executable, UINT32 targetRva,
                                             const BYTE* expected, SIZE_T displacedSize,
                                             UINT32 kind) {
    BYTE* base = (BYTE*)executable;
    BYTE* target = base + targetRva;
    if (!expected || displacedSize < 14u || !readable_range(target, displacedSize) ||
        !bytes_equal(target, expected, displacedSize)) return false;
    BYTE* trampoline = (BYTE*)VirtualAlloc(0, displacedSize + 14u, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    BYTE* cave = (BYTE*)VirtualAlloc(0, 192u, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!trampoline || !cave) return false;
    memcpy(trampoline, expected, displacedSize);
    static const BYTE absoluteJump[6] = {0xFF,0x25,0x00,0x00,0x00,0x00};
    memcpy(trampoline + displacedSize, absoluteJump, sizeof(absoluteJump));
    UINT64 continuation = (UINT64)(target + displacedSize);
    memcpy(trampoline + displacedSize + sizeof(absoluteJump), &continuation, sizeof(continuation));
    FlushInstructionCache((HANDLE)(INT64)-1, trampoline, displacedSize + 14u);

    SIZE_T q = 0;
    static const BYTE sub[] = {0x48,0x81,0xEC,0x88,0x00,0x00,0x00}; memcpy(cave+q,sub,sizeof(sub)); q+=sizeof(sub);
    static const BYTE saveRc[] = {0x48,0x89,0x4C,0x24,0x20}; memcpy(cave+q,saveRc,sizeof(saveRc)); q+=sizeof(saveRc);
    static const BYTE saveRd[] = {0x48,0x89,0x54,0x24,0x28}; memcpy(cave+q,saveRd,sizeof(saveRd)); q+=sizeof(saveRd);
    static const BYTE saveR8[] = {0x4C,0x89,0x44,0x24,0x30}; memcpy(cave+q,saveR8,sizeof(saveR8)); q+=sizeof(saveR8);
    static const BYTE saveR9[] = {0x4C,0x89,0x4C,0x24,0x38}; memcpy(cave+q,saveR9,sizeof(saveR9)); q+=sizeof(saveR9);
    static const BYTE saveX0[] = {0x0F,0x29,0x44,0x24,0x40}; memcpy(cave+q,saveX0,sizeof(saveX0)); q+=sizeof(saveX0);
    static const BYTE saveX1[] = {0x0F,0x29,0x4C,0x24,0x50}; memcpy(cave+q,saveX1,sizeof(saveX1)); q+=sizeof(saveX1);
    static const BYTE saveX2[] = {0x0F,0x29,0x54,0x24,0x60}; memcpy(cave+q,saveX2,sizeof(saveX2)); q+=sizeof(saveX2);
    static const BYTE saveX3[] = {0x0F,0x29,0x5C,0x24,0x70}; memcpy(cave+q,saveX3,sizeof(saveX3)); q+=sizeof(saveX3);
    static const BYTE loadRc[] = {0x48,0x8B,0x4C,0x24,0x20}; memcpy(cave+q,loadRc,sizeof(loadRc)); q+=sizeof(loadRc);
    static const BYTE kindMove[] = {0xBA,0,0,0,0}; memcpy(cave+q,kindMove,sizeof(kindMove)); memcpy(cave+q+1u,&kind,sizeof(kind)); q+=sizeof(kindMove);
    static const BYTE callHelper[] = {0xFF,0x15,0x02,0x00,0x00,0x00,0xEB,0x08}; memcpy(cave+q,callHelper,sizeof(callHelper)); q+=sizeof(callHelper);
    UINT64 helper = (UINT64)&log_coffin_direct_state; memcpy(cave+q,&helper,sizeof(helper)); q+=sizeof(helper);
    static const BYTE loadX0[] = {0x0F,0x28,0x44,0x24,0x40}; memcpy(cave+q,loadX0,sizeof(loadX0)); q+=sizeof(loadX0);
    static const BYTE loadX1[] = {0x0F,0x28,0x4C,0x24,0x50}; memcpy(cave+q,loadX1,sizeof(loadX1)); q+=sizeof(loadX1);
    static const BYTE loadX2[] = {0x0F,0x28,0x54,0x24,0x60}; memcpy(cave+q,loadX2,sizeof(loadX2)); q+=sizeof(loadX2);
    static const BYTE loadX3[] = {0x0F,0x28,0x5C,0x24,0x70}; memcpy(cave+q,loadX3,sizeof(loadX3)); q+=sizeof(loadX3);
    static const BYTE loadRc2[] = {0x48,0x8B,0x4C,0x24,0x20}; memcpy(cave+q,loadRc2,sizeof(loadRc2)); q+=sizeof(loadRc2);
    static const BYTE loadRd[] = {0x48,0x8B,0x54,0x24,0x28}; memcpy(cave+q,loadRd,sizeof(loadRd)); q+=sizeof(loadRd);
    static const BYTE loadR8[] = {0x4C,0x8B,0x44,0x24,0x30}; memcpy(cave+q,loadR8,sizeof(loadR8)); q+=sizeof(loadR8);
    static const BYTE loadR9[] = {0x4C,0x8B,0x4C,0x24,0x38}; memcpy(cave+q,loadR9,sizeof(loadR9)); q+=sizeof(loadR9);
    static const BYTE add[] = {0x48,0x81,0xC4,0x88,0x00,0x00,0x00}; memcpy(cave+q,add,sizeof(add)); q+=sizeof(add);
    memcpy(cave+q,absoluteJump,sizeof(absoluteJump)); q+=sizeof(absoluteJump);
    UINT64 trampolineAddress=(UINT64)trampoline; memcpy(cave+q,&trampolineAddress,sizeof(trampolineAddress)); q+=sizeof(trampolineAddress);
    FlushInstructionCache((HANDLE)(INT64)-1,cave,q);

    BYTE replacement[32]; for (SIZE_T i=0;i<sizeof(replacement);++i) replacement[i]=0x90;
    memcpy(replacement, absoluteJump, sizeof(absoluteJump)); UINT64 caveAddress=(UINT64)cave;
    memcpy(replacement+6u,&caveAddress,sizeof(caveAddress));
    if (!freeze_for_patch(target, displacedSize)) return false;
    DWORD oldProtection=0; if(!VirtualProtect(target,displacedSize,PAGE_EXECUTE_READWRITE,&oldProtection)){unfreeze_after_patch();return false;}
    memcpy(target,replacement,displacedSize); FlushInstructionCache((HANDLE)(INT64)-1,target,displacedSize);
    bool installed=bytes_equal(target,replacement,displacedSize);
    if(!installed){memcpy(target,expected,displacedSize);FlushInstructionCache((HANDLE)(INT64)-1,target,displacedSize);}
    DWORD ignored=0;VirtualProtect(target,displacedSize,oldProtection,&ignored);unfreeze_after_patch();
    return installed;
}

static bool install_coffin_state_trace(HMODULE executable) {
    if (!g_enableNetworkStateTrace) return true;
    static const BYTE drive[20] = {0x48,0x89,0x5C,0x24,0x20,0x57,0x48,0x83,0xEC,0x30,0x48,0x8B,0xD9,0x48,0x8B,0x89,0x98,0x00,0x00,0x00};
    static const BYTE falling[25] = {0x40,0x53,0x48,0x83,0xEC,0x30,0xC5,0xF2,0x58,0x81,0x80,0x01,0x00,0x00,0xC5,0xFA,0x11,0x81,0x80,0x01,0x00,0x00,0x48,0x8B,0xD9};
    static const BYTE kick[20] = {0x48,0x89,0x5C,0x24,0x10,0x48,0x89,0x6C,0x24,0x18,0x48,0x89,0x74,0x24,0x20,0x57,0x48,0x83,0xEC,0x20};
    static const BYTE driveTick[19] = {0x40,0x53,0x48,0x83,0xEC,0x30,0x48,0x8B,0xD9,0x45,0x33,0xC0,0x48,0x8B,0x89,0x90,0x01,0x00,0x00};
    static const BYTE endMotion[22] = {0x48,0x89,0x6C,0x24,0x18,0x48,0x89,0x74,0x24,0x20,0x57,0x48,0x83,0xEC,0x40,0x48,0x8B,0x81,0xA8,0x00,0x00,0x00};
    static const BYTE borderMode[17] = {0x4C,0x8B,0xDC,0x55,0x41,0x56,0x49,0x8D,0x6B,0xA1,0x48,0x81,0xEC,0x88,0x00,0x00,0x00};
    static const BYTE producer[19] = {0x40,0x53,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57,0x48,0x81,0xEC,0xD0,0x01,0x00,0x00};
    if (!install_direct_state_entry_trace(executable,RVA_COFFIN_DRIVE_STATE_ENTRY,drive,sizeof(drive),0u)) return false;
    if (!install_direct_state_entry_trace(executable,RVA_COFFIN_FALLING_STATE_ENTRY,falling,sizeof(falling),1u)) return false;
    if (!install_direct_state_entry_trace(executable,RVA_COFFIN_KICK_STATE_ENTRY,kick,sizeof(kick),2u)) return false;
    if (!install_direct_state_entry_trace(executable,RVA_COFFIN_DRIVE_STATE_TICK,driveTick,sizeof(driveTick),3u)) return false;
    if (!install_direct_state_entry_trace(executable,RVA_COFFIN_ENDSTATE_MOTION,endMotion,sizeof(endMotion),4u)) return false;
    if (!install_direct_state_entry_trace(executable,RVA_COFFIN_BORDER_MODE_CONTROLLER,borderMode,sizeof(borderMode),5u)) return false;
    if (!install_direct_state_entry_trace(executable,RVA_COFFIN_4C_PRODUCER,producer,sizeof(producer),6u)) return false;
    g_coffinDirectStateTraceInstalled=1;
    return true;
}

static void* __fastcall coffin_4c_candidate_check_trace(void* candidate, void* context, void* aux, UINT64 probeMode) {
    UINT64 caller = (UINT64)__builtin_return_address(0);
    BYTE* base = (BYTE*)g_executable;
    UINT64 callerRva = base && caller >= (UINT64)base ? caller - (UINT64)base : caller;
    void* result = g_nativeCoffin4CCandidateCheck ? g_nativeCoffin4CCandidateCheck(candidate, context, aux, probeMode) : 0;
    if (callerRva == RVA_COFFIN_4C_CHECK_RETURN_FALSE_PROBE || callerRva == RVA_COFFIN_4C_CHECK_RETURN_TRUE_PROBE) {
        LONG index = __atomic_fetch_add(&g_coffin4CCandidateCheckCalls, 1, __ATOMIC_ACQ_REL);
        if (index < 128) {
            UINT64 handle18 = 0, object20 = 0;
            void* meta = 0;
            UINT32 typeHash = 0xFFFFFFFFu, metaCode = 0xFFFFFFFFu, state83 = 0xFFFFFFFFu;
            if (candidate && readable_range((BYTE*)candidate + 0x84u, 1u)) {
                handle18 = *(UINT64*)((BYTE*)candidate + 0x18u);
                object20 = *(UINT64*)((BYTE*)candidate + 0x20u);
                meta = *(void**)((BYTE*)candidate + 0x38u);
                state83 = *(BYTE*)((BYTE*)candidate + 0x83u);
            }
            if (meta && readable_range((BYTE*)meta + 0x4Au, 1u)) {
                typeHash = *(UINT32*)((BYTE*)meta + 0x44u);
                metaCode = *(BYTE*)((BYTE*)meta + 0x49u);
            }
            LogBuffer m; log_init(&m); log_prefix(&m);
            log_text(&m, "COFFIN 4C CANDIDATE CHECK #"); log_uint(&m, (UINT64)index);
            log_text(&m, ": callerRva="); log_uint(&m, callerRva);
            log_text(&m, ", probeMode="); log_uint(&m, probeMode);
            log_text(&m, ", candidate="); log_uint(&m, (UINT64)candidate);
            log_text(&m, ", context="); log_uint(&m, (UINT64)context);
            log_text(&m, ", handle18="); log_uint(&m, handle18);
            log_text(&m, ", object20="); log_uint(&m, object20);
            log_text(&m, ", state83="); log_uint(&m, state83);
            log_text(&m, ", meta="); log_uint(&m, (UINT64)meta);
            log_text(&m, ", typeHash="); log_uint(&m, typeHash);
            log_text(&m, ", metaCode="); log_uint(&m, metaCode);
            log_text(&m, ", nativeResult="); log_uint(&m, (UINT64)result);
            log_text(&m, ". Native candidate check unchanged.\r\n"); append_log(&m);
        }
    }
    return result;
}

static bool install_coffin_4c_candidate_check_trace(HMODULE executable) {
    if (!g_enableNetworkStateTrace) return true;
    BYTE* target = (BYTE*)executable + RVA_COFFIN_4C_CANDIDATE_CHECK;
    static const BYTE expected[16] = {
        0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x48,0x89,0x7C,0x24,0x18,0x55
    };
    if (!readable_range(target, sizeof(expected)) || !bytes_equal(target, expected, sizeof(expected))) return false;
    BYTE* trampoline = (BYTE*)VirtualAlloc(0, sizeof(expected) + 14u, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!trampoline) return false;
    memcpy(trampoline, expected, sizeof(expected));
    static const BYTE jump[6] = {0xFF,0x25,0x00,0x00,0x00,0x00};
    memcpy(trampoline + sizeof(expected), jump, sizeof(jump));
    UINT64 continuation = (UINT64)(target + sizeof(expected));
    memcpy(trampoline + sizeof(expected) + sizeof(jump), &continuation, sizeof(continuation));
    FlushInstructionCache((HANDLE)(INT64)-1, trampoline, sizeof(expected) + 14u);
    g_nativeCoffin4CCandidateCheck = (Coffin4CCandidateCheckFn)trampoline;

    BYTE replacement[16];
    for (SIZE_T i=0;i<sizeof(replacement);++i) replacement[i]=0x90;
    memcpy(replacement, jump, sizeof(jump));
    UINT64 hook = (UINT64)&coffin_4c_candidate_check_trace;
    memcpy(replacement + sizeof(jump), &hook, sizeof(hook));
    if (!freeze_for_patch(target, sizeof(replacement))) return false;
    DWORD oldProtection=0;
    if(!VirtualProtect(target,sizeof(replacement),PAGE_EXECUTE_READWRITE,&oldProtection)){unfreeze_after_patch();return false;}
    memcpy(target,replacement,sizeof(replacement));
    FlushInstructionCache((HANDLE)(INT64)-1,target,sizeof(replacement));
    bool installed=bytes_equal(target,replacement,sizeof(replacement));
    if(!installed){memcpy(target,expected,sizeof(expected));FlushInstructionCache((HANDLE)(INT64)-1,target,sizeof(expected));g_nativeCoffin4CCandidateCheck=0;}
    DWORD ignored=0;VirtualProtect(target,sizeof(replacement),oldProtection,&ignored);unfreeze_after_patch();
    if(installed)g_coffin4CCandidateCheckInstalled=1;
    return installed;
}

static void __fastcall trace_coffin_border_signal_setter(UINT32 type, UINT32 id) {
    UINT64 caller=(UINT64)__builtin_return_address(0);
    BYTE* base=(BYTE*)g_executable;
    UINT64 callerRva=base&&caller>=(UINT64)base?caller-(UINT64)base:caller;
    void* manager=0;
    if(base&&readable_range(base+RVA_CHIRAL_WORLD_MANAGER_GLOBAL,sizeof(void*)))manager=*(void**)(base+RVA_CHIRAL_WORLD_MANAGER_GLOBAL);
    UINT32 normalized=type<=2u?type:0xFFFFFFFFu;
    if(manager&&readable_range((BYTE*)manager+0x2264C4u,sizeof(UINT32))){
        *(BYTE*)((BYTE*)manager+0x2264BCu)=1u; *(UINT32*)((BYTE*)manager+0x2264C0u)=normalized; *(UINT32*)((BYTE*)manager+0x2264C4u)=id;
    }
    LONG index=__atomic_fetch_add(&g_coffinBorderSignalCalls,1,__ATOMIC_ACQ_REL);
    if(index<64){LogBuffer m;log_init(&m);log_prefix(&m);log_text(&m,"BORDER SIGNAL BC #");log_uint(&m,(UINT64)index);log_text(&m,": type=");log_uint(&m,type);log_text(&m,", normalized=");log_uint(&m,normalized);log_text(&m,", id=");log_uint(&m,id);log_text(&m,", callerRva=");log_uint(&m,callerRva);log_text(&m,", manager=");log_uint(&m,(UINT64)manager);log_text(&m,". Native setter semantics preserved.\r\n");append_log(&m);}
}

static void __fastcall trace_coffin_mount_signal_setter() {
    UINT64 caller=(UINT64)__builtin_return_address(0);
    BYTE* base=(BYTE*)g_executable;
    UINT64 callerRva=base&&caller>=(UINT64)base?caller-(UINT64)base:caller;
    void* manager=0;
    if(base&&readable_range(base+RVA_CHIRAL_WORLD_MANAGER_GLOBAL,sizeof(void*)))manager=*(void**)(base+RVA_CHIRAL_WORLD_MANAGER_GLOBAL);
    if(manager&&readable_range((BYTE*)manager+0x2264BBu,sizeof(BYTE))) *(BYTE*)((BYTE*)manager+0x2264BBu)=1u;
    LONG index=__atomic_fetch_add(&g_coffinMountSignalCalls,1,__ATOMIC_ACQ_REL);
    if(index<64){LogBuffer m;log_init(&m);log_prefix(&m);log_text(&m,"MOUNT SIGNAL BB #");log_uint(&m,(UINT64)index);log_text(&m,": callerRva=");log_uint(&m,callerRva);log_text(&m,", manager=");log_uint(&m,(UINT64)manager);log_text(&m,". Native setter semantics preserved.\r\n");append_log(&m);}
}

static bool install_simple_setter_trace(BYTE* target,const BYTE* expected,SIZE_T size,void* hook) {
    if(!target||!expected||!hook||size<14u||size>32u||!readable_range(target,size)||!bytes_equal(target,expected,size))return false;
    BYTE replacement[32];for(SIZE_T i=0;i<sizeof(replacement);++i)replacement[i]=0x90;
    static const BYTE jump[6]={0xFF,0x25,0x00,0x00,0x00,0x00};memcpy(replacement,jump,sizeof(jump));UINT64 hookAddress=(UINT64)hook;memcpy(replacement+6u,&hookAddress,sizeof(hookAddress));
    if(!freeze_for_patch(target,size))return false;DWORD oldProtection=0;if(!VirtualProtect(target,size,PAGE_EXECUTE_READWRITE,&oldProtection)){unfreeze_after_patch();return false;}
    memcpy(target,replacement,size);FlushInstructionCache((HANDLE)(INT64)-1,target,size);bool installed=bytes_equal(target,replacement,size);
    if(!installed){memcpy(target,expected,size);FlushInstructionCache((HANDLE)(INT64)-1,target,size);}DWORD ignored=0;VirtualProtect(target,size,oldProtection,&ignored);unfreeze_after_patch();return installed;
}

static bool install_coffin_border_signal_traces(HMODULE executable) {
    if(!g_enableNetworkStateTrace)return true;BYTE* base=(BYTE*)executable;
    static const BYTE border[20]={0x48,0x8B,0x05,0x31,0x32,0x4A,0x05,0x83,0xF9,0x02,0x41,0xB8,0xFF,0xFF,0xFF,0xFF,0x41,0x0F,0x47,0xC8};
    static const BYTE mount[14]={0x48,0x8B,0x05,0x01,0x32,0x4A,0x05,0xC6,0x80,0xBB,0x64,0x22,0x00,0x01};
    if(!install_simple_setter_trace(base+RVA_COFFIN_BORDER_SIGNAL_SETTER,border,sizeof(border),(void*)&trace_coffin_border_signal_setter))return false;
    g_coffinBorderSignalTraceInstalled=1;
    if(!install_simple_setter_trace(base+RVA_COFFIN_MOUNT_SIGNAL_SETTER,mount,sizeof(mount),(void*)&trace_coffin_mount_signal_setter))return false;
    g_coffinMountSignalTraceInstalled=1;return true;
}

static bool install_coffin_endstate_border_correction_bypass(HMODULE executable) {
    if (!g_enableNetworkTraversal) return true;
    BYTE* target = (BYTE*)executable + RVA_COFFIN_ENDSTATE_BORDER_CORRECTION_GATE;
    static const BYTE expected[6] = {0x0F,0x86,0x8D,0x00,0x00,0x00};
    static const BYTE replacement[6] = {0xE9,0x8E,0x00,0x00,0x00,0x90};
    if (!readable_range(target, sizeof(expected)) || !bytes_equal(target, expected, sizeof(expected))) return false;
    if (!freeze_for_patch(target, sizeof(replacement))) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(target, sizeof(replacement), PAGE_EXECUTE_READWRITE, &oldProtection)) {
        unfreeze_after_patch(); return false;
    }
    memcpy(target, replacement, sizeof(replacement));
    FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(replacement));
    bool installed = bytes_equal(target, replacement, sizeof(replacement));
    if (!installed) {
        memcpy(target, expected, sizeof(expected));
        FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(expected));
    }
    DWORD ignored = 0; VirtualProtect(target, sizeof(replacement), oldProtection, &ignored);
    unfreeze_after_patch();
    if (installed) g_coffinEndStateBorderCorrectionBypassInstalled = 1;
    return installed;
}

[[maybe_unused]] static bool install_coffin_network_drive_end_guards(HMODULE executable) {
    if(!g_enableNetworkTraversal)return true;BYTE* base=(BYTE*)executable;
    BYTE* networkGate=base+RVA_COFFIN_DRIVE_NETWORK_GATE;BYTE* forcedEndGate=base+RVA_COFFIN_DRIVE_FORCED_END_GATE;
    static const BYTE expectedNetworkGate[13]={0x80,0xB8,0xBC,0x64,0x22,0x00,0x00,0x0F,0x85,0x63,0x01,0x00,0x00};
    static const BYTE expectedForcedEndGate[13]={0xF6,0x87,0x61,0x01,0x00,0x00,0x02,0x0F,0x85,0x36,0x01,0x00,0x00};
    if(!replace_exact_branch_with_nops(forcedEndGate,expectedForcedEndGate,sizeof(expectedForcedEndGate),7u))return false;
    if(!replace_exact_branch_with_nops(networkGate,expectedNetworkGate,sizeof(expectedNetworkGate),7u))return false;
    g_networkDriveEndGuardsInstalled=1;return true;
}
// The mount, Drive, and Kick handlers each test a manager signal before
// their normal Coffin-only state validation.  This candidate changes only the
// conditional jumps from those three checks; it retains the F3, owner,
// movement, and physics predicates and never writes either manager flag.
[[maybe_unused]] static bool install_coffin_network_manager_branch_bypasses(HMODULE executable) {
    if (!g_enableNetworkTraversal && !g_enableOutsideNetworkMount) return true;
    BYTE* base = (BYTE*)executable;
    BYTE* mount = base + RVA_COFFIN_RIDE_FLOATER_MOUNT_NETWORK_GATE;
    BYTE* drive = base + RVA_COFFIN_DRIVE_NETWORK_GATE;
    BYTE* kick = base + RVA_COFFIN_KICK_NETWORK_GATE;
    static const BYTE expectedMount[13] = {0x80,0xB8,0xBB,0x64,0x22,0x00,0x00,0x0F,0x85,0x9C,0x00,0x00,0x00};
    static const BYTE expectedDrive[13] = {0x80,0xB8,0xBC,0x64,0x22,0x00,0x00,0x0F,0x85,0x63,0x01,0x00,0x00};
    static const BYTE expectedKick[13] = {0x80,0xB8,0xBC,0x64,0x22,0x00,0x00,0x0F,0x85,0x9B,0x00,0x00,0x00};
    if (g_enableOutsideNetworkMount &&
        (!readable_range(mount, sizeof(expectedMount)) || !bytes_equal(mount, expectedMount, sizeof(expectedMount)))) return false;
    if (g_enableNetworkTraversal &&
        ((!readable_range(drive, sizeof(expectedDrive)) || !bytes_equal(drive, expectedDrive, sizeof(expectedDrive))) ||
         (!readable_range(kick, sizeof(expectedKick)) || !bytes_equal(kick, expectedKick, sizeof(expectedKick))))) return false;
    if (g_enableOutsideNetworkMount &&
        !replace_exact_branch_with_nops(mount, expectedMount, sizeof(expectedMount), 7u)) return false;
    if (g_enableNetworkTraversal &&
        (!replace_exact_branch_with_nops(drive, expectedDrive, sizeof(expectedDrive), 7u) ||
         !replace_exact_branch_with_nops(kick, expectedKick, sizeof(expectedKick), 7u))) return false;
    g_networkMountGateInstalled = g_enableOutsideNetworkMount != 0;
    g_networkDriveEndGuardsInstalled = g_enableNetworkTraversal != 0;
    return true;
}
[[maybe_unused]] static bool install_coffin_f3_descriptor_guard(HMODULE executable) {
    if (!g_enableNetworkTraversal && !g_enableOutsideNetworkMount) return true;
    BYTE* call = (BYTE*)executable + RVA_COFFIN_F3_DESCRIPTOR_REMOVE_CALL;
    static const BYTE expected[5] = {0xE8,0xFB,0x48,0xE8,0xFF};
    static const BYTE nops[5] = {0x0F,0x1F,0x44,0x00,0x00};
    if (!readable_range(call, sizeof(expected)) || !bytes_equal(call, expected, sizeof(expected))) return false;
    if (!freeze_for_patch(call, sizeof(nops))) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(call, sizeof(nops), PAGE_EXECUTE_READWRITE, &oldProtection)) { unfreeze_after_patch(); return false; }
    memcpy(call, nops, sizeof(nops));
    FlushInstructionCache((HANDLE)(INT64)-1, call, sizeof(nops));
    bool installed = bytes_equal(call, nops, sizeof(nops));
    if (!installed) { memcpy(call, expected, sizeof(expected)); FlushInstructionCache((HANDLE)(INT64)-1, call, sizeof(expected)); }
    DWORD ignored = 0; VirtualProtect(call, sizeof(nops), oldProtection, &ignored);
    unfreeze_after_patch();
    return installed;
}


// v1.30 passive observer for the exact fixed candidate array consumed by
// 100A300.  No list element, count, generation or manager state is changed.
static void __fastcall log_coffin_spatial_candidate_list(void* root) {
    LONG index = __atomic_fetch_add(&g_coffinSpatialListTraceCalls, 1, __ATOMIC_ACQ_REL);
    if (!root || !readable_range((BYTE*)root + 0x12D98u, sizeof(UINT32))) return;
    UINT32 count = *(UINT32*)((BYTE*)root + 0x12D98u);
    LONG lastCount = __atomic_load_n(&g_coffinSpatialLastCount, __ATOMIC_ACQUIRE);
    UINT64 lastRoot = __atomic_load_n(&g_coffinSpatialLastRoot, __ATOMIC_ACQUIRE);
    bool changed = lastCount != (LONG)count || lastRoot != (UINT64)root;
    if (!changed && index >= 8 && (index % 120) != 0) return;
    __atomic_store_n(&g_coffinSpatialLastCount, (LONG)count, __ATOMIC_RELEASE);
    __atomic_store_n(&g_coffinSpatialLastRoot, (UINT64)root, __ATOMIC_RELEASE);

    LogBuffer m; log_init(&m); log_prefix(&m);
    log_text(&m, "COFFIN SPATIAL LIST #"); log_uint(&m, (UINT64)index);
    log_text(&m, ": root="); log_uint(&m, (UINT64)root);
    log_text(&m, ", count="); log_uint(&m, count);
    UINT32 list1E0=0, flags218=0, managerCount2D0=0, selectedIndex2D4=0xFFFFFFFFu; void* selected58=0; void* managerSelected230=0; UINT64 handle220=0;
    if(readable_range((BYTE*)root+0x1E4u,sizeof(UINT32))) list1E0=*(UINT32*)((BYTE*)root+0x1E0u);
    if(readable_range((BYTE*)root+0x58u,sizeof(void*))) selected58=*(void**)((BYTE*)root+0x58u);
    if(readable_range((BYTE*)root+0x218u,sizeof(BYTE))) flags218=*(BYTE*)((BYTE*)root+0x218u);
    if(readable_range((BYTE*)root+0x220u,sizeof(UINT64))) handle220=*(UINT64*)((BYTE*)root+0x220u);
    if(readable_range((BYTE*)root+0x2D8u,sizeof(UINT32))) { managerCount2D0=*(UINT32*)((BYTE*)root+0x2D0u); selectedIndex2D4=*(UINT32*)((BYTE*)root+0x2D4u); }
    if(readable_range((BYTE*)root+0x230u,sizeof(void*))) managerSelected230=*(void**)((BYTE*)root+0x230u);
    log_text(&m, ", list1E0="); log_uint(&m,list1E0); log_text(&m, ", selected58="); log_uint(&m,(UINT64)selected58);
    log_text(&m, ", flags218="); log_uint(&m,flags218); log_text(&m, ", handle220="); log_uint(&m,handle220);
    log_text(&m, ", managerCount2D0="); log_uint(&m,managerCount2D0); log_text(&m, ", selectedIndex2D4="); log_uint(&m,selectedIndex2D4);
    log_text(&m, ", managerSelected230="); log_uint(&m,(UINT64)managerSelected230);
    UINT32 limit = count < 4u ? count : 4u;
    for (UINT32 i = 0; i < limit; ++i) {
        void* candidate = 0; void* meta = 0;
        UINT32 typeHash = 0xFFFFFFFFu, metaCode = 0xFFFFFFFFu, state83 = 0xFFFFFFFFu;
        BYTE* slot = (BYTE*)root + 0xD98u + (SIZE_T)i * sizeof(void*);
        if (readable_range(slot, sizeof(void*))) candidate = *(void**)slot;
        if (candidate && readable_range((BYTE*)candidate + 0x84u, 1u)) {
            meta = *(void**)((BYTE*)candidate + 0x38u);
            state83 = *(BYTE*)((BYTE*)candidate + 0x83u);
        }
        if (meta && readable_range((BYTE*)meta + 0x4Au, 1u)) {
            typeHash = *(UINT32*)((BYTE*)meta + 0x44u);
            metaCode = *(BYTE*)((BYTE*)meta + 0x49u);
        }
        log_text(&m, ", c"); log_uint(&m, i); log_text(&m, "="); log_uint(&m, (UINT64)candidate);
        log_text(&m, "/t"); log_uint(&m, typeHash);
        log_text(&m, "/m"); log_uint(&m, metaCode);
        log_text(&m, "/s"); log_uint(&m, state83);
    }
    log_text(&m, ". Native spatial list unchanged.\r\n"); append_log(&m);
}

static bool install_coffin_spatial_list_trace(HMODULE executable) {
    if (!g_enableOutsideNetworkMount && !g_enableNetworkTrace) return true;
    BYTE* target = (BYTE*)executable + RVA_COFFIN_SPATIAL_LIST_POST;
    static const BYTE expected[14] = {
        0x44,0x8D,0x79,0xFF,0x24,0x01,0x48,0x89,0x44,0x24,0x60,0x45,0x85,0xFF
    };
    if (!readable_range(target, sizeof(expected)) || !bytes_equal(target, expected, sizeof(expected))) return false;
    BYTE* trampoline = (BYTE*)VirtualAlloc(0, sizeof(expected) + 14u, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    BYTE* cave = (BYTE*)VirtualAlloc(0, 192u, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!trampoline || !cave) return false;
    memcpy(trampoline, expected, sizeof(expected));
    static const BYTE absJump[6] = {0xFF,0x25,0,0,0,0};
    memcpy(trampoline + sizeof(expected), absJump, sizeof(absJump));
    UINT64 continuation = (UINT64)(target + sizeof(expected));
    memcpy(trampoline + sizeof(expected) + sizeof(absJump), &continuation, sizeof(continuation));
    FlushInstructionCache((HANDLE)(INT64)-1, trampoline, sizeof(expected)+14u);

    SIZE_T q=0;
    static const BYTE sub[] = {0x48,0x81,0xEC,0x60,0x00,0x00,0x00}; memcpy(cave+q,sub,sizeof(sub)); q+=sizeof(sub);
    static const BYTE saveAx[] = {0x48,0x89,0x44,0x24,0x20}; memcpy(cave+q,saveAx,sizeof(saveAx)); q+=sizeof(saveAx);
    static const BYTE saveCx[] = {0x48,0x89,0x4C,0x24,0x28}; memcpy(cave+q,saveCx,sizeof(saveCx)); q+=sizeof(saveCx);
    static const BYTE saveDx[] = {0x48,0x89,0x54,0x24,0x30}; memcpy(cave+q,saveDx,sizeof(saveDx)); q+=sizeof(saveDx);
    static const BYTE saveR8[] = {0x4C,0x89,0x44,0x24,0x38}; memcpy(cave+q,saveR8,sizeof(saveR8)); q+=sizeof(saveR8);
    static const BYTE saveR9[] = {0x4C,0x89,0x4C,0x24,0x40}; memcpy(cave+q,saveR9,sizeof(saveR9)); q+=sizeof(saveR9);
    static const BYTE saveR10[] = {0x4C,0x89,0x54,0x24,0x48}; memcpy(cave+q,saveR10,sizeof(saveR10)); q+=sizeof(saveR10);
    static const BYTE saveR11[] = {0x4C,0x89,0x5C,0x24,0x50}; memcpy(cave+q,saveR11,sizeof(saveR11)); q+=sizeof(saveR11);
    static const BYTE movRcRdi[] = {0x48,0x89,0xF9}; memcpy(cave+q,movRcRdi,sizeof(movRcRdi)); q+=sizeof(movRcRdi);
    cave[q++]=0x48; cave[q++]=0xB8; UINT64 helper=(UINT64)&log_coffin_spatial_candidate_list; memcpy(cave+q,&helper,8); q+=8;
    cave[q++]=0xFF; cave[q++]=0xD0;
    static const BYTE loadAx[] = {0x48,0x8B,0x44,0x24,0x20}; memcpy(cave+q,loadAx,sizeof(loadAx)); q+=sizeof(loadAx);
    static const BYTE loadCx[] = {0x48,0x8B,0x4C,0x24,0x28}; memcpy(cave+q,loadCx,sizeof(loadCx)); q+=sizeof(loadCx);
    static const BYTE loadDx[] = {0x48,0x8B,0x54,0x24,0x30}; memcpy(cave+q,loadDx,sizeof(loadDx)); q+=sizeof(loadDx);
    static const BYTE loadR8[] = {0x4C,0x8B,0x44,0x24,0x38}; memcpy(cave+q,loadR8,sizeof(loadR8)); q+=sizeof(loadR8);
    static const BYTE loadR9[] = {0x4C,0x8B,0x4C,0x24,0x40}; memcpy(cave+q,loadR9,sizeof(loadR9)); q+=sizeof(loadR9);
    static const BYTE loadR10[] = {0x4C,0x8B,0x54,0x24,0x48}; memcpy(cave+q,loadR10,sizeof(loadR10)); q+=sizeof(loadR10);
    static const BYTE loadR11[] = {0x4C,0x8B,0x5C,0x24,0x50}; memcpy(cave+q,loadR11,sizeof(loadR11)); q+=sizeof(loadR11);
    static const BYTE add[] = {0x48,0x81,0xC4,0x60,0x00,0x00,0x00}; memcpy(cave+q,add,sizeof(add)); q+=sizeof(add);
    memcpy(cave+q,absJump,sizeof(absJump)); q+=sizeof(absJump); UINT64 tramp=(UINT64)trampoline; memcpy(cave+q,&tramp,8); q+=8;
    FlushInstructionCache((HANDLE)(INT64)-1,cave,q);

    BYTE replacement[14]={0xFF,0x25,0,0,0,0,0,0,0,0,0,0,0,0}; UINT64 caveAddr=(UINT64)cave; memcpy(replacement+6,&caveAddr,8);
    if(!freeze_for_patch(target,sizeof(replacement))) return false; DWORD old=0;
    if(!VirtualProtect(target,sizeof(replacement),PAGE_EXECUTE_READWRITE,&old)){unfreeze_after_patch();return false;}
    memcpy(target,replacement,sizeof(replacement)); FlushInstructionCache((HANDLE)(INT64)-1,target,sizeof(replacement));
    bool ok=bytes_equal(target,replacement,sizeof(replacement)); if(!ok){memcpy(target,expected,sizeof(expected));FlushInstructionCache((HANDLE)(INT64)-1,target,sizeof(expected));}
    DWORD ignored=0; VirtualProtect(target,sizeof(replacement),old,&ignored); unfreeze_after_patch();
    if(ok) g_coffinSpatialListTraceInstalled=1; return ok;
}

// v1.32 live Chiral-world cluster telemetry.  This deliberately samples from
// the same manager global hard-coded by the game's FFFCF0 / D9B790 / D9B7C0
// code paths. It logs only byte changes in +0x2264B0..+0x2264FF.
static void log_chiral_world_cluster(float x6, float x12, float torque) {
    BYTE* base=(BYTE*)(g_executable ? g_executable : GetModuleHandleW(0));
    if(!base || !readable_range(base+RVA_CHIRAL_WORLD_MANAGER_GLOBAL,sizeof(void*))) return;
    BYTE* manager=*(BYTE**)(base+RVA_CHIRAL_WORLD_MANAGER_GLOBAL);
    if(!manager || !readable_range(manager+0x2264B0u,0x50u)) return;
    LONG expected=0;
    if(!__atomic_compare_exchange_n(&g_chiralClusterLock,&expected,1,false,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE)) return;
    BYTE current[0x50];
    for(UINT32 i=0;i<0x50u;++i) current[i]=manager[0x2264B0u+i];
    LONG initialized=__atomic_load_n(&g_chiralClusterInitialized,__ATOMIC_ACQUIRE);
    if(!initialized){
        for(UINT32 i=0;i<0x50u;++i) g_chiralClusterLast[i]=current[i];
        __atomic_store_n(&g_chiralClusterInitialized,1,__ATOMIC_RELEASE);
        LogBuffer m;log_init(&m);log_prefix(&m);
        log_text(&m,"CHIRAL CLUSTER BASELINE: manager=");log_uint(&m,(UINT64)manager);
        log_text(&m,", B7=");log_uint(&m,current[0x07]);
        log_text(&m,", B8=");log_uint(&m,current[0x08]);
        log_text(&m,", BB=");log_uint(&m,current[0x0B]);
        log_text(&m,", BC=");log_uint(&m,current[0x0C]);
        log_text(&m,", C0=");log_uint(&m,*(UINT32*)(current+0x10));
        log_text(&m,", C4=");log_uint(&m,*(UINT32*)(current+0x14));
        log_text(&m,", D8=");log_uint(&m,current[0x28]);
        log_text(&m,", F8=");log_uint(&m,*(UINT32*)(current+0x48));
        log_text(&m,". Correct game world-manager global.\r\n");append_log(&m);
    }else{
        UINT32 changed=0;
        for(UINT32 i=0;i<0x50u;++i) if(current[i]!=g_chiralClusterLast[i]) ++changed;
        if(changed){
            LONG event=__atomic_fetch_add(&g_chiralClusterEvents,1,__ATOMIC_ACQ_REL);
            if(event<192){
                LogBuffer m;log_init(&m);log_prefix(&m);
                log_text(&m,"CHIRAL CLUSTER CHANGE #");log_uint(&m,(UINT64)event);
                log_text(&m,": x6=");log_float2(&m,x6);log_text(&m,", x12=");log_float2(&m,x12);log_text(&m,", torque=");log_float2(&m,torque);
                log_text(&m,", changes=");
                UINT32 emitted=0;
                for(UINT32 i=0;i<0x50u && emitted<20u;++i){
                    if(current[i]==g_chiralClusterLast[i]) continue;
                    if(emitted) log_text(&m,"|");
                    log_text(&m,"+");log_uint(&m,(UINT64)(0xB0u+i));log_text(&m,":");
                    log_uint(&m,(UINT64)g_chiralClusterLast[i]);log_text(&m,">");log_uint(&m,(UINT64)current[i]);
                    ++emitted;
                }
                if(changed>emitted){log_text(&m,"|more=");log_uint(&m,(UINT64)(changed-emitted));}
                log_text(&m,".\r\n");append_log(&m);
            }
            for(UINT32 i=0;i<0x50u;++i) g_chiralClusterLast[i]=current[i];
        }
    }
    __atomic_store_n(&g_chiralClusterLock,0,__ATOMIC_RELEASE);
}

static float raw_u32_to_float(UINT32 value) { union { UINT32 u; float f; } bits; bits.u=value; return bits.f; }
static float absf_local(float value) { return value < 0.0f ? -value : value; }

static void trace_coffin_physics_gear_state(void* physics, float x12) {
    if (!physics || !readable_range(physics, sizeof(g_coffinPhysicsSnapshot))) {
        g_coffinPhysicsSnapshotValid = 0;
        g_coffinPhysicsSnapshotObject = 0;
        g_coffinPhysicsSnapshotLastX12 = x12;
        return;
    }
    UINT64 object=(UINT64)physics;
    if (!g_coffinPhysicsSnapshotValid || g_coffinPhysicsSnapshotObject != object) {
        memcpy(g_coffinPhysicsSnapshot, physics, sizeof(g_coffinPhysicsSnapshot));
        g_coffinPhysicsSnapshotObject = object;
        g_coffinPhysicsSnapshotValid = 1;
        g_coffinPhysicsSnapshotLastX12 = x12;
        return;
    }
    float oldX12=g_coffinPhysicsSnapshotLastX12;
    bool positiveDrop = oldX12 > 0.10f && x12 > 0.10f && x12 < oldX12 - 0.10f;
    if (positiveDrop) {
        LONG event=__atomic_fetch_add(&g_coffinPhysicsGearDropEvents,1,__ATOMIC_ACQ_REL);
        if(event<24){
            BYTE* current=(BYTE*)physics;
            LogBuffer m; log_init(&m); log_prefix(&m);
            log_text(&m,"COFFIN GEAR-STATE DROP #"); log_uint(&m,(UINT64)event);
            log_text(&m,": x12="); log_float2(&m,oldX12); log_text(&m,"->"); log_float2(&m,x12);
            log_text(&m,", smallFields=");
            UINT32 emittedSmall=0;
            for(UINT32 off=0; off<sizeof(g_coffinPhysicsSnapshot) && emittedSmall<40u; ++off){
                BYTE a=g_coffinPhysicsSnapshot[off], b=current[off];
                if(a==b || a>31u || b>31u) continue;
                if(emittedSmall) log_char(&m,'|');
                log_text(&m,"+"); log_uint(&m,(UINT64)off); log_text(&m,":"); log_uint(&m,a); log_text(&m,">"); log_uint(&m,b);
                ++emittedSmall;
            }
            if(!emittedSmall) log_text(&m,"none");
            log_text(&m,", dwords=");
            UINT32 emittedDword=0;
            for(UINT32 off=0; off+4u<=sizeof(g_coffinPhysicsSnapshot) && emittedDword<32u; off+=4u){
                UINT32 a=*(UINT32*)(g_coffinPhysicsSnapshot+off), b=*(UINT32*)(current+off);
                if(a==b) continue;
                float af=raw_u32_to_float(a), bf=raw_u32_to_float(b);
                bool small=(a<=64u && b<=64u);
                bool plausibleFloat=(af>-10000.0f && af<10000.0f && bf>-10000.0f && bf<10000.0f && absf_local(af-bf)>0.005f);
                if(!small && !plausibleFloat) continue;
                if(emittedDword) log_char(&m,'|');
                log_text(&m,"+"); log_uint(&m,(UINT64)off); log_text(&m,":0x"); log_uint(&m,(UINT64)a); log_text(&m,">0x"); log_uint(&m,(UINT64)b);
                if(plausibleFloat){ log_text(&m,"("); log_float2(&m,af); log_text(&m,">"); log_float2(&m,bf); log_text(&m,")"); }
                ++emittedDword;
            }
            if(!emittedDword) log_text(&m,"none");
            log_text(&m,". Passive 0x1000-byte DSPhysicsCoffinInstance diff.\\r\\n"); append_log(&m);
        }
    }
    memcpy(g_coffinPhysicsSnapshot, physics, sizeof(g_coffinPhysicsSnapshot));
    g_coffinPhysicsSnapshotLastX12 = x12;
}

// Passive torque telemetry.  The helper runs only for the RTTI-confirmed
// DSPhysicsCoffinInstance and samples the native XMM6/XMM12 factors plus the
// resulting torque slot after the existing drive-force multiplier.
static void __fastcall log_coffin_drive_torque(void* torqueSlot, float x6, float x12, void* physics) {
    BYTE* base=(BYTE*)(g_executable ? g_executable : GetModuleHandleW(0));
    if (!physics || !base || !readable_range(physics,sizeof(void*)) || *(void**)physics != (void*)(base+RVA_COFFIN_PHYSICS_VTABLE)) return;
    if (!torqueSlot || !readable_range((BYTE*)torqueSlot+0x18u,sizeof(float))) return;
    trace_coffin_physics_gear_state(physics,x12);
    float torque=*(float*)((BYTE*)torqueSlot+0x18u);
    float e8 = 0.0f;
    if (readable_range((BYTE*)physics + 0xE8u, sizeof(float))) e8 = *(float*)((BYTE*)physics + 0xE8u);

    // Static audit of FUN_14247A1C0: physics+0xC8 is the source count and
    // physics+0xD0 points to an array of source pointers.  The engine copies
    // source[min(abs(source+0x10))]+0x10 into physics+0xE8.  Compute the same
    // selection read-only so we can identify the exact source that collapses
    // when the Coffin approaches the Chiral boundary.
    int sourceCount = 0;
    void** sourceTable = 0;
    float sourceThreshold = 3.0e30f;
    if (readable_range((BYTE*)physics + 0xC8u, sizeof(int))) sourceCount = *(int*)((BYTE*)physics + 0xC8u);
    if (readable_range((BYTE*)physics + 0xD0u, sizeof(void*))) sourceTable = *(void***)((BYTE*)physics + 0xD0u);
    if (readable_range(base + 0x03462488u, sizeof(float))) sourceThreshold = *(float*)(base + 0x03462488u);
    if (!(sourceThreshold > 0.0f) || sourceThreshold != sourceThreshold) sourceThreshold = 3.0e30f;
    int scanCount = sourceCount; if (scanCount < 0) scanCount = 0; if (scanCount > 256) scanCount = 256;
    int minIndex = -1; float minAbs = sourceThreshold; float minValue = 0.0f; UINT64 minPtr = 0, minTag = 0;
    if (sourceTable && scanCount > 0 && readable_range(sourceTable, (SIZE_T)scanCount * sizeof(void*))) {
        for (int i=0; i<scanCount; ++i) {
            BYTE* src=(BYTE*)sourceTable[i];
            if (!src || !readable_range(src+0x10u,sizeof(float))) continue;
            float v=*(float*)(src+0x10u); if (v!=v) continue;
            float a=v<0.0f?-v:v;
            if (a < minAbs) { minAbs=a; minValue=v; minIndex=i; minPtr=(UINT64)src; if(readable_range(src,sizeof(UINT64)))minTag=*(UINT64*)src; else minTag=0; }
        }
    }

    float scale1 = 0.0f, scale2 = 0.0f;
    if (readable_range(base + 0x03461E24u, sizeof(float))) scale1 = *(float*)(base + 0x03461E24u);
    if (readable_range(base + 0x03460C68u, sizeof(float))) scale2 = *(float*)(base + 0x03460C68u);
    float curveInput = x12 * e8 * scale1; if (curveInput < 0.0f) curveInput = -curveInput; curveInput *= scale2;
    float driveFactor = 1.0f;
    if (g_driveFactorBits) driveFactor = raw_u32_to_float((UINT32)__atomic_load_n(g_driveFactorBits, __ATOMIC_ACQUIRE));
    float denominator = x6 * x12 * driveFactor;
    float curveOutput = (denominator > 0.0001f || denominator < -0.0001f) ? (torque / denominator) : 0.0f;
    log_chiral_world_cluster(x6,x12,torque);
    LONG index=__atomic_fetch_add(&g_coffinTorqueTraceCalls,1,__ATOMIC_ACQ_REL);
    float d6=x6-g_coffinTorqueLastX6; if(d6<0.0f)d6=-d6;
    float d12=x12-g_coffinTorqueLastX12; if(d12<0.0f)d12=-d12;
    float dt=torque-g_coffinTorqueLast; if(dt<0.0f)dt=-dt;
    float scale=g_coffinTorqueLast; if(scale<0.0f)scale=-scale; if(scale<1.0f)scale=1.0f;
    bool sourceChanged = minIndex != g_coffinTorqueLastSourceIndex;
    bool e8NearZero = (e8 > -5.0f && e8 < 5.0f);
    bool changed=d6>0.02f || d12>0.02f || dt>(scale*0.20f) || sourceChanged || e8NearZero;
    bool sampled=(index<16) || ((index<1200) && ((index%30)==0));
    if(!changed && !sampled) return;
    g_coffinTorqueLastX6=x6; g_coffinTorqueLastX12=x12; g_coffinTorqueLast=torque; g_coffinTorqueLastSourceIndex=minIndex;
    LogBuffer m; log_init(&m); log_prefix(&m);
    log_text(&m,"COFFIN TORQUE SOURCE #"); log_uint(&m,(UINT64)index);
    log_text(&m,": physics="); log_uint(&m,(UINT64)physics);
    log_text(&m,", x6="); log_float2(&m,x6);
    log_text(&m,", x12="); log_float2(&m,x12);
    log_text(&m,", torque="); log_float2(&m,torque);
    log_text(&m,", e8="); log_float2(&m,e8);
    log_text(&m,", sourceCount="); log_uint(&m,(UINT64)(UINT32)sourceCount);
    log_text(&m,", minIndex="); if(minIndex<0) log_text(&m,"-1"); else log_uint(&m,(UINT64)minIndex);
    log_text(&m,", minPtr="); log_uint(&m,minPtr);
    log_text(&m,", minTag="); log_uint(&m,minTag);
    log_text(&m,", minValue="); log_float2(&m,minValue);
    log_text(&m,", threshold="); log_float2(&m,sourceThreshold);
    log_text(&m,", curveInput="); log_float2(&m,curveInput);
    log_text(&m,", curveOutput="); log_float2(&m,curveOutput);
    log_text(&m,", driveFactor="); log_float2(&m,driveFactor);
    BYTE p5dc=0,p5dd=0,p61b=0; float p540=0.0f,p60c=0.0f,p528=0.0f,p534=0.0f,p120=0.0f; WORD p132=0;
    if(physics){BYTE* pp=(BYTE*)physics; if(readable_range(pp+0x61Bu,1))p61b=pp[0x61B]; if(readable_range(pp+0x5DCu,2)){p5dc=pp[0x5DC];p5dd=pp[0x5DD];}
      if(readable_range(pp+0x540u,4))p540=*(float*)(pp+0x540u); if(readable_range(pp+0x60Cu,4))p60c=*(float*)(pp+0x60Cu);
      if(readable_range(pp+0x528u,4))p528=*(float*)(pp+0x528u); if(readable_range(pp+0x534u,4))p534=*(float*)(pp+0x534u);
      if(readable_range(pp+0x120u,4))p120=*(float*)(pp+0x120u); if(readable_range(pp+0x132u,2))p132=*(WORD*)(pp+0x132u);}
    BYTE bb=0,bc=0; UINT32 bt=0,bid=0; BYTE* exe=(BYTE*)g_executable; void* wm=0;
    if(exe&&readable_range(exe+RVA_CHIRAL_WORLD_MANAGER_GLOBAL,sizeof(void*)))wm=*(void**)(exe+RVA_CHIRAL_WORLD_MANAGER_GLOBAL);
    if(wm&&readable_range((BYTE*)wm+0x2264C8u,1)){bb=*((BYTE*)wm+0x2264BBu);bc=*((BYTE*)wm+0x2264BCu);bt=*(UINT32*)((BYTE*)wm+0x2264C0u);bid=*(UINT32*)((BYTE*)wm+0x2264C4u);}
    log_text(&m,", p5DC=");log_uint(&m,p5dc);log_text(&m,", p5DD=");log_uint(&m,p5dd);log_text(&m,", p61B=");log_uint(&m,p61b);
    log_text(&m,", p540=");log_float2(&m,p540);log_text(&m,", p60C=");log_float2(&m,p60c);log_text(&m,", p528=");log_float2(&m,p528);
    log_text(&m,", p534=");log_float2(&m,p534);log_text(&m,", p120=");log_float2(&m,p120);log_text(&m,", p132=");log_uint(&m,p132);
    log_text(&m,", BB=");log_uint(&m,bb);log_text(&m,", BC=");log_uint(&m,bc);log_text(&m,", borderType=");log_uint(&m,bt);log_text(&m,", borderId=");log_uint(&m,bid);
    log_text(&m,", sources=");
    int emit = scanCount < 12 ? scanCount : 12;
    if (sourceTable && emit>0 && readable_range(sourceTable,(SIZE_T)emit*sizeof(void*))) {
        for(int i=0;i<emit;++i){
            if(i)log_char(&m,'|'); log_uint(&m,(UINT64)i); log_char(&m,':');
            BYTE* src=(BYTE*)sourceTable[i]; log_uint(&m,(UINT64)src); log_char(&m,':');
            if(src && readable_range(src+0x10u,sizeof(float))) log_float2(&m,*(float*)(src+0x10u)); else log_text(&m,"NA");
        }
    } else log_text(&m,"NA");
    log_text(&m,". READ-ONLY source selection telemetry.\r\n"); append_log(&m);
}

static bool install_coffin_drive_torque_trace(HMODULE executable) {
    BYTE* target=(BYTE*)executable+RVA_COFFIN_DRIVE_TORQUE_POST;
    static const BYTE expected[17]={0xC5,0xE2,0x5C,0xC6,0xC5,0xD8,0x57,0xE4,0xC5,0xFA,0x5F,0xC4,0xC4,0xC1,0x78,0x28,0xCC};
    if(!readable_range(target,sizeof(expected))||!bytes_equal(target,expected,sizeof(expected)))return false;
    BYTE* trampoline=(BYTE*)VirtualAlloc(0,sizeof(expected)+14u,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);
    BYTE* cave=(BYTE*)VirtualAlloc(0,224u,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);
    if(!trampoline||!cave)return false;
    memcpy(trampoline,expected,sizeof(expected)); static const BYTE absJump[6]={0xFF,0x25,0,0,0,0};
    memcpy(trampoline+sizeof(expected),absJump,sizeof(absJump)); UINT64 continuation=(UINT64)(target+sizeof(expected));
    memcpy(trampoline+sizeof(expected)+sizeof(absJump),&continuation,8); FlushInstructionCache((HANDLE)(INT64)-1,trampoline,sizeof(expected)+14u);
    SIZE_T q=0;
    static const BYTE sub[]={0x48,0x81,0xEC,0x80,0x00,0x00,0x00};memcpy(cave+q,sub,sizeof(sub));q+=sizeof(sub);
    static const BYTE saveAx[]={0x48,0x89,0x44,0x24,0x20};memcpy(cave+q,saveAx,sizeof(saveAx));q+=sizeof(saveAx);
    static const BYTE saveCx[]={0x48,0x89,0x4C,0x24,0x28};memcpy(cave+q,saveCx,sizeof(saveCx));q+=sizeof(saveCx);
    static const BYTE saveDx[]={0x48,0x89,0x54,0x24,0x30};memcpy(cave+q,saveDx,sizeof(saveDx));q+=sizeof(saveDx);
    static const BYTE saveR8[]={0x4C,0x89,0x44,0x24,0x38};memcpy(cave+q,saveR8,sizeof(saveR8));q+=sizeof(saveR8);
    static const BYTE saveR9[]={0x4C,0x89,0x4C,0x24,0x40};memcpy(cave+q,saveR9,sizeof(saveR9));q+=sizeof(saveR9);
    static const BYTE saveR10[]={0x4C,0x89,0x54,0x24,0x48};memcpy(cave+q,saveR10,sizeof(saveR10));q+=sizeof(saveR10);
    static const BYTE saveR11[]={0x4C,0x89,0x5C,0x24,0x50};memcpy(cave+q,saveR11,sizeof(saveR11));q+=sizeof(saveR11);
    static const BYTE saveX3[]={0x0F,0x29,0x5C,0x24,0x60};memcpy(cave+q,saveX3,sizeof(saveX3));q+=sizeof(saveX3);
    static const BYTE saveX4[]={0x0F,0x29,0x64,0x24,0x70};memcpy(cave+q,saveX4,sizeof(saveX4));q+=sizeof(saveX4);
    static const BYTE movRcRdi[]={0x48,0x89,0xF9};memcpy(cave+q,movRcRdi,sizeof(movRcRdi));q+=sizeof(movRcRdi);
    static const BYTE movX1X6[]={0x0F,0x28,0xCE};memcpy(cave+q,movX1X6,sizeof(movX1X6));q+=sizeof(movX1X6);
    static const BYTE movX2X12[]={0x41,0x0F,0x28,0xD4};memcpy(cave+q,movX2X12,sizeof(movX2X12));q+=sizeof(movX2X12);
    static const BYTE movR9Rbx[]={0x49,0x89,0xD9};memcpy(cave+q,movR9Rbx,sizeof(movR9Rbx));q+=sizeof(movR9Rbx);
    cave[q++]=0x48;cave[q++]=0xB8;UINT64 helper=(UINT64)&log_coffin_drive_torque;memcpy(cave+q,&helper,8);q+=8;cave[q++]=0xFF;cave[q++]=0xD0;
    static const BYTE loadX3[]={0x0F,0x28,0x5C,0x24,0x60};memcpy(cave+q,loadX3,sizeof(loadX3));q+=sizeof(loadX3);
    static const BYTE loadX4[]={0x0F,0x28,0x64,0x24,0x70};memcpy(cave+q,loadX4,sizeof(loadX4));q+=sizeof(loadX4);
    static const BYTE loadAx[]={0x48,0x8B,0x44,0x24,0x20};memcpy(cave+q,loadAx,sizeof(loadAx));q+=sizeof(loadAx);
    static const BYTE loadCx[]={0x48,0x8B,0x4C,0x24,0x28};memcpy(cave+q,loadCx,sizeof(loadCx));q+=sizeof(loadCx);
    static const BYTE loadDx[]={0x48,0x8B,0x54,0x24,0x30};memcpy(cave+q,loadDx,sizeof(loadDx));q+=sizeof(loadDx);
    static const BYTE loadR8[]={0x4C,0x8B,0x44,0x24,0x38};memcpy(cave+q,loadR8,sizeof(loadR8));q+=sizeof(loadR8);
    static const BYTE loadR9[]={0x4C,0x8B,0x4C,0x24,0x40};memcpy(cave+q,loadR9,sizeof(loadR9));q+=sizeof(loadR9);
    static const BYTE loadR10[]={0x4C,0x8B,0x54,0x24,0x48};memcpy(cave+q,loadR10,sizeof(loadR10));q+=sizeof(loadR10);
    static const BYTE loadR11[]={0x4C,0x8B,0x5C,0x24,0x50};memcpy(cave+q,loadR11,sizeof(loadR11));q+=sizeof(loadR11);
    static const BYTE add[]={0x48,0x81,0xC4,0x80,0x00,0x00,0x00};memcpy(cave+q,add,sizeof(add));q+=sizeof(add);
    memcpy(cave+q,absJump,sizeof(absJump));q+=sizeof(absJump);UINT64 tramp=(UINT64)trampoline;memcpy(cave+q,&tramp,8);q+=8;FlushInstructionCache((HANDLE)(INT64)-1,cave,q);
    BYTE replacement[17];for(SIZE_T i=0;i<sizeof(replacement);++i)replacement[i]=0x90;memcpy(replacement,absJump,sizeof(absJump));UINT64 caveAddr=(UINT64)cave;memcpy(replacement+6,&caveAddr,8);
    if(!freeze_for_patch(target,sizeof(replacement)))return false;DWORD old=0;if(!VirtualProtect(target,sizeof(replacement),PAGE_EXECUTE_READWRITE,&old)){unfreeze_after_patch();return false;}
    memcpy(target,replacement,sizeof(replacement));FlushInstructionCache((HANDLE)(INT64)-1,target,sizeof(replacement));bool ok=bytes_equal(target,replacement,sizeof(replacement));
    if(!ok){memcpy(target,expected,sizeof(expected));FlushInstructionCache((HANDLE)(INT64)-1,target,sizeof(expected));}DWORD ignored=0;VirtualProtect(target,sizeof(replacement),old,&ignored);unfreeze_after_patch();
    if(ok)g_coffinTorqueTraceInstalled=1;return ok;
}

static const BYTE DRIVE_HOOK_NO_TELEMETRY_TEMPLATE[72] = {
    0xF3,0x0F,0x1E,0xFA,0xC4,0xC1,0x4A,0x59,0xCC,0xC5,0xFA,0x59,0xC1,0x50,
    0x48,0xB8,0,0,0,0,0,0,0,0,0x48,0x39,0x03,0x75,0x08,
    0xC5,0xFA,0x59,0x05,0x1F,0,0,0,0x58,0xC5,0xFA,0x11,0x47,0x18,0xFF,0x25,0x03,0,0,0,
    0xCC,0xCC,0xCC,0,0,0,0,0,0,0,0,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0,0,0,0
};
static bool install_drive_force_hook(HMODULE executable) {
    BYTE* base=(BYTE*)executable; BYTE* target=base+RVA_DRIVE_TORQUE_HOOK; void** v=(void**)(base+RVA_COFFIN_PHYSICS_VTABLE);
    static const BYTE expected[18]={0xC4,0xC1,0x4A,0x59,0xCC,0xC5,0xFA,0x59,0xC1,0xC5,0xFA,0x11,0x47,0x18,0xC5,0xE2,0x5C,0xC6};
    if(!readable_range(v,4u*sizeof(void*))||v[0]!=(void*)(base+RVA_COFFIN_VFUNC_0)||v[1]!=(void*)(base+RVA_COFFIN_VFUNC_1)||v[2]!=(void*)(base+RVA_COFFIN_VFUNC_2)||v[3]!=(void*)(base+RVA_COFFIN_VFUNC_3)||!readable_range(target,sizeof(expected))||!bytes_equal(target,expected,sizeof(expected)))return false;
    BYTE* cave=(BYTE*)VirtualAlloc(0,sizeof(DRIVE_HOOK_NO_TELEMETRY_TEMPLATE),MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);if(!cave)return false;
    memcpy(cave,DRIVE_HOOK_NO_TELEMETRY_TEMPLATE,sizeof(DRIVE_HOOK_NO_TELEMETRY_TEMPLATE)); UINT64 coffinVtable=(UINT64)(base+RVA_COFFIN_PHYSICS_VTABLE),returnAddress=(UINT64)(target+14u);float factor=1.0f;
    memcpy(cave+16u,&coffinVtable,8);memcpy(cave+52u,&returnAddress,8);memcpy(cave+68u,&factor,4);FlushInstructionCache((HANDLE)(INT64)-1,cave,sizeof(DRIVE_HOOK_NO_TELEMETRY_TEMPLATE));
    BYTE replacement[14]={0xFF,0x25,0,0,0,0,0,0,0,0,0,0,0,0};UINT64 caveAddress=(UINT64)cave;memcpy(replacement+6u,&caveAddress,8);
    if(!freeze_for_patch(target,sizeof(replacement)))return false;DWORD oldProtection=0;if(!VirtualProtect(target,sizeof(replacement),PAGE_EXECUTE_READWRITE,&oldProtection)){unfreeze_after_patch();return false;}
    memcpy(target,replacement,sizeof(replacement));FlushInstructionCache((HANDLE)(INT64)-1,target,sizeof(replacement));bool installed=bytes_equal(target,replacement,sizeof(replacement));
    if(!installed){memcpy(target,expected,14u);FlushInstructionCache((HANDLE)(INT64)-1,target,14u);}DWORD ignored=0;VirtualProtect(target,sizeof(replacement),oldProtection,&ignored);unfreeze_after_patch();if(!installed)return false;
    g_driveFactorBits=(volatile LONG*)(cave+68u);g_driveHookInstalled=1;return true;
}
static bool publish_drive_factor_percent(int percent) {
    if(!g_driveHookInstalled||!g_driveFactorBits)return false;float factor=(float)percent/100.0f;LONG bits=0;memcpy(&bits,&factor,4);__atomic_store_n(g_driveFactorBits,bits,__ATOMIC_RELEASE);return __atomic_load_n(g_driveFactorBits,__ATOMIC_ACQUIRE)==bits;
}
// Focused DSPlayerRideFloaterEndState observer.  This vtable slot is RTTI-
// confirmed as Coffin RideFloater EndState.  It never changes state selection;
// it only snapshots the live Coffin config and then calls the original method.
static void __fastcall coffin_endstate_trace(void* state, float deltaSeconds) {
    LONG index = __atomic_fetch_add(&g_coffinEndStateTraceCalls, 1, __ATOMIC_ACQ_REL);
    if (index < 24 && state) {
        void* actionData = 0;
        void* config = 0;
        float timer = -1.0f;
        float escapeSpeed = -1.0f;
        UINT32 stateValue = 0xFFFFFFFFu;
        UINT32 managerBorderFlag = 0xFFFFFFFFu;
        if (readable_range((BYTE*)state + 0x198u, 1u)) {
            if (readable_range((BYTE*)state + 0x90u, sizeof(void*))) {
                actionData = *(void**)((BYTE*)state + 0x90u);
            }
            if (readable_range((BYTE*)state + 0x180u, sizeof(float))) {
                timer = *(float*)((BYTE*)state + 0x180u);
            }
            if (readable_range((BYTE*)state + 0x88u, sizeof(void*))) {
                void* stateData = *(void**)((BYTE*)state + 0x88u);
                if (stateData && readable_range((BYTE*)stateData + 0x11Au, sizeof(WORD))) {
                    stateValue = *(WORD*)((BYTE*)stateData + 0x11Au);
                }
            }
        }
        if (actionData && readable_range((BYTE*)actionData + 0xAE8u, sizeof(void*))) {
            config = *(void**)((BYTE*)actionData + 0xAE8u);
            if (config && readable_range((BYTE*)config + OFF_COFFIN_QPID_BORDER_ESCAPE_SPEED, sizeof(float))) {
                escapeSpeed = *(float*)((BYTE*)config + OFF_COFFIN_QPID_BORDER_ESCAPE_SPEED);
            }
        }
        BYTE* executableBase = (BYTE*)(g_executable ? g_executable : GetModuleHandleW(0));
        if (executableBase && readable_range(executableBase + RVA_CHIRAL_WORLD_MANAGER_GLOBAL, sizeof(void*))) {
            void* manager = *(void**)(executableBase + RVA_CHIRAL_WORLD_MANAGER_GLOBAL);
            if (manager && readable_range((BYTE*)manager + 0x2264BCu, sizeof(BYTE))) {
                managerBorderFlag = *(BYTE*)((BYTE*)manager + 0x2264BCu);
            }
        }
        LogBuffer message; log_init(&message); log_prefix(&message);
        log_text(&message, "COFFIN ENDSTATE #"); log_uint(&message, (UINT64)index);
        log_text(&message, ": dt="); log_float2(&message, deltaSeconds);
        log_text(&message, ", timer="); log_float2(&message, timer);
        log_text(&message, ", state="); log_uint(&message, stateValue);
        log_text(&message, ", manager2264BC="); log_uint(&message, managerBorderFlag);
        log_text(&message, ", actionData="); log_uint(&message, (UINT64)actionData);
        log_text(&message, ", config="); log_uint(&message, (UINT64)config);
        log_text(&message, ", qpidEscape="); log_float2(&message, escapeSpeed);
        log_text(&message, ". Native EndState still executes.\r\n"); append_log(&message);
    }
    if (g_nativeCoffinEndStateTick) g_nativeCoffinEndStateTick(state, deltaSeconds);
}

static bool install_coffin_endstate_trace(HMODULE executable) {
    if (!g_enableNetworkTraversal && !g_enableNetworkStateTrace && !g_enableQpidBorderEscapeClamp) return true;
    BYTE* base = (BYTE*)executable;
    void** slot = (void**)(base + RVA_COFFIN_QPID_ESCAPE_STATE_A_VTABLE_SLOT);
    if (!readable_range(slot, sizeof(void*)) || *slot != (void*)(base + RVA_COFFIN_QPID_ESCAPE_STATE_A_ENTRY)) return false;
    g_nativeCoffinEndStateTick = (CoffinEndStateTickFn)*slot;
    if (!freeze_for_patch((const BYTE*)slot, sizeof(void*))) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(slot, sizeof(void*), PAGE_READWRITE, &oldProtection)) { unfreeze_after_patch(); return false; }
    *slot = (void*)&coffin_endstate_trace;
    FlushInstructionCache((HANDLE)(INT64)-1, slot, sizeof(void*));
    bool installed = *slot == (void*)&coffin_endstate_trace;
    if (!installed) *slot = (void*)g_nativeCoffinEndStateTick;
    DWORD ignored = 0; VirtualProtect(slot, sizeof(void*), oldProtection, &ignored); unfreeze_after_patch();
    if (installed) g_coffinEndStateTraceInstalled = 1;
    return installed;
}

// Target A is RTTI-confirmed DSPlayerRideFloaterEndState and is the only target
// used by the Coffin experiment below. Target B was reclassified as
// DSPlayerVehicleRideOffState and MUST NOT be installed for Coffin traversal.
// The target-A detour preserves the native config-pointer load, sets only
// DSPlayerRideCoffinConfig::QpidBorderEscapeSpeed to the engine's own 0.1
// lower bound, reloads the scalar, and resumes after the displaced clamp/store.
static bool install_coffin_qpid_escape_state_hook(HMODULE executable, UINT32 targetRva, UINT32 entryRva, UINT32 vtableRva, bool useRdx) {
    BYTE* base = (BYTE*)executable;
    BYTE* target = base + targetRva;
    void** vtableSlot = (void**)(base + vtableRva);
    static const BYTE expectedA[20] = {0x48,0x8B,0x90,0xE8,0x0A,0x00,0x00,0xC5,0xFA,0x10,0x42,0x5C,0xC5,0xFA,0x5F,0x05,0x32,0x61,0x4D,0x02};
    static const BYTE expectedB[20] = {0x48,0x8B,0x88,0xE8,0x0A,0x00,0x00,0xC5,0xFA,0x10,0x41,0x5C,0xC5,0xFA,0x5F,0x05,0xB5,0x92,0x4C,0x02};
    const BYTE* expected = useRdx ? expectedA : expectedB;
    if (!readable_range(vtableSlot, sizeof(void*)) || *vtableSlot != (void*)(base + entryRva) ||
        !readable_range(target, sizeof(expectedA)) || !bytes_equal(target, expected, sizeof(expectedA))) return false;
    BYTE* cave = (BYTE*)VirtualAlloc(0, 48u, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!cave) return false;
    SIZE_T offset = 0;
    memcpy(cave + offset, expected, 7u); offset += 7u;
    if (useRdx) {
        static const BYTE writeAndLoad[] = {0xC7,0x42,0x5C,0xCD,0xCC,0xCC,0x3D,0xC5,0xFA,0x10,0x42,0x5C};
        memcpy(cave + offset, writeAndLoad, sizeof(writeAndLoad)); offset += sizeof(writeAndLoad);
    } else {
        static const BYTE writeAndLoad[] = {0xC7,0x41,0x5C,0xCD,0xCC,0xCC,0x3D,0xC5,0xFA,0x10,0x41,0x5C};
        memcpy(cave + offset, writeAndLoad, sizeof(writeAndLoad)); offset += sizeof(writeAndLoad);
    }
    static const BYTE jump[] = {0xFF,0x25,0x00,0x00,0x00,0x00};
    memcpy(cave + offset, jump, sizeof(jump)); offset += sizeof(jump);
    UINT64 resume = (UINT64)(target + sizeof(expectedA));
    memcpy(cave + offset, &resume, sizeof(resume)); offset += sizeof(resume);
    FlushInstructionCache((HANDLE)(INT64)-1, cave, offset);
    BYTE replacement[20] = {0xFF,0x25,0x00,0x00,0x00,0x00,0,0,0,0,0,0,0,0,0,0,0x0F,0x1F,0x40,0x00};
    UINT64 caveAddress = (UINT64)cave; memcpy(replacement + 6u, &caveAddress, sizeof(caveAddress));
    if (!freeze_for_patch(target, sizeof(replacement))) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(target, sizeof(replacement), PAGE_EXECUTE_READWRITE, &oldProtection)) { unfreeze_after_patch(); return false; }
    memcpy(target, replacement, sizeof(replacement));
    FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(replacement));
    bool installed = bytes_equal(target, replacement, sizeof(replacement));
    if (!installed) { memcpy(target, expected, sizeof(expectedA)); FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(expectedA)); }
    DWORD ignored = 0; VirtualProtect(target, sizeof(replacement), oldProtection, &ignored); unfreeze_after_patch();
    return installed;
}
// Legacy research helper only. DO NOT call: target B is VehicleRideOffState.
[[maybe_unused]] static bool install_coffin_qpid_escape_state_hooks(HMODULE executable) {
    if (!install_coffin_qpid_escape_state_hook(executable, RVA_COFFIN_QPID_ESCAPE_STATE_A, RVA_COFFIN_QPID_ESCAPE_STATE_A_ENTRY, RVA_COFFIN_QPID_ESCAPE_STATE_A_VTABLE_SLOT, true)) return false;
    if (!install_coffin_qpid_escape_state_hook(executable, RVA_COFFIN_QPID_ESCAPE_STATE_B, RVA_COFFIN_QPID_ESCAPE_STATE_B_ENTRY, RVA_COFFIN_QPID_ESCAPE_STATE_B_VTABLE_SLOT, false)) return false;
    g_qpidEscapeStateHooksInstalled = 1;
    return true;
}
static int string_compare(const char* a,const char* b){if(!a||!b)return a==b?0:1;while(*a&&*b&&*a==*b){++a;++b;}return(unsigned char)*a-(unsigned char)*b;}
static const char* rtti_type_name(void* object,void** vtable){if(!object||!vtable||!readable_range(vtable,sizeof(void*)))return 0;typedef void*(__fastcall* GetRttiFn)(void*);GetRttiFn getRtti=(GetRttiFn)vtable[0];if(!getRtti||!readable_range((void*)getRtti,1))return 0;void* rtti=getRtti(object);if(!readable_range(rtti,0x48))return 0;if(*((BYTE*)rtti+4)!=4)return 0;const char* typeName=*(const char**)((BYTE*)rtti+0x40);return readable_range(typeName,1)?typeName:0;}
static bool object_has_uuid(void* object,UINT64 low,UINT64 high){if(!object||!readable_range((BYTE*)object+0x10u,16u))return false;const UINT64* uuid=(const UINT64*)((BYTE*)object+0x10u);return uuid[0]==low&&uuid[1]==high;}
static bool object_has_exact_type(void* object,const char* expectedType){if(!object||!readable_range(object,sizeof(void*)))return false;void** vtable=*(void***)object;if(!vtable)return false;++g_rttiLookups;const char* typeName=rtti_type_name(object,vtable);return typeName&&string_compare(typeName,expectedType)==0;}
static bool patch_coffin_physics(void* resource) {
    if(!resource||!readable_range((BYTE*)resource+OFF_SLIP_SPEED,sizeof(float))||!publish_drive_factor_percent(100))return false;
    float* land=(float*)((BYTE*)resource+OFF_LAND_TOP_SPEED);float* gear=(float*)((BYTE*)resource+OFF_FINAL_GEAR_RATIO);float* boost=(float*)((BYTE*)resource+OFF_LAND_BOOST_TOP_SPEED);float* water=(float*)((BYTE*)resource+OFF_WATER_TOP_SPEED);float* waterBoost=(float*)((BYTE*)resource+OFF_WATER_BOOST_TOP_SPEED);float* slip=(float*)((BYTE*)resource+OFF_SLIP_SPEED);
    float oldLand=*land,oldGear=*gear,oldBoost=*boost,oldWater=*water,oldWaterBoost=*waterBoost,oldSlip=*slip;
    bool ownWater=g_waterBaseline>0.0f&&g_waterBoostBaseline>0.0f&&f_near(oldWater,g_lastAppliedWater,0.05f)&&f_near(oldWaterBoost,g_lastAppliedWaterBoost,0.05f);float baseWater=ownWater?g_waterBaseline:oldWater,baseWaterBoost=ownWater?g_waterBoostBaseline:oldWaterBoost;
    bool ownGear=g_gearRatioBaseline>0.0f&&f_near(oldGear,g_lastAppliedGearRatio,0.05f);float baseGear=ownGear?g_gearRatioBaseline:oldGear;
    if(baseWater<10.0f||baseWater>250.0f||baseWaterBoost<10.0f||baseWaterBoost>300.0f||!f_near(baseGear,15.0f,0.05f)){log_line("WARNING: Coffin physics layout/value guard failed; resource skipped.");return false;}
    float finalLand=baseWater*(float)g_speedPercent/100.0f,finalBoost=baseWaterBoost*(float)g_speedPercent/100.0f,finalWater=f_max(oldWater,finalLand),finalWaterBoost=f_max(oldWaterBoost,finalBoost),finalGear=baseGear*(float)g_gearRatioPercent/100.0f,finalSlip=f_max(oldSlip,finalLand);
    if(finalLand>650.0f||finalBoost>850.0f||finalWater>650.0f||finalWaterBoost>850.0f||finalGear<1.5f||finalGear>30.0f||!(f_near(oldSlip,39.0f,0.05f)||oldSlip>=finalLand)||!value_is_native_or_compatible(oldLand,40.0f,finalLand)||!value_is_native_or_compatible(oldBoost,60.0f,finalBoost)){log_line("WARNING: Coffin physics safety/conflict guard failed; resource skipped.");return false;}
    int a=write_float_checked(land,finalLand);int b=a?write_float_checked(boost,finalBoost):0;int c=b?write_float_checked(water,finalWater):0;int d=c?write_float_checked(waterBoost,finalWaterBoost):0;int e=d?write_float_checked(gear,finalGear):0;int f=e?write_float_checked(slip,finalSlip):0;bool drive=f&&publish_drive_factor_percent(g_driveForcePercent);
    if(!drive){publish_drive_factor_percent(100);if(f==2)write_float_checked(slip,oldSlip);if(e==2)write_float_checked(gear,oldGear);if(d==2)write_float_checked(waterBoost,oldWaterBoost);if(c==2)write_float_checked(water,oldWater);if(b==2)write_float_checked(boost,oldBoost);if(a==2)write_float_checked(land,oldLand);log_line("ERROR: Coffin physics transaction failed and was rolled back.");return false;}
    g_waterBaseline=baseWater;g_waterBoostBaseline=baseWaterBoost;g_lastAppliedWater=finalWater;g_lastAppliedWaterBoost=finalWaterBoost;g_gearRatioBaseline=baseGear;g_lastAppliedGearRatio=finalGear;g_seenPhysicsResource=resource;g_physicsReady=1;
    LogBuffer m;log_init(&m);log_prefix(&m);log_text(&m,"Coffin physics land ");log_float2(&m,oldLand);log_text(&m," -> ");log_float2(&m,finalLand);log_text(&m,", boost ");log_float2(&m,oldBoost);log_text(&m," -> ");log_float2(&m,finalBoost);log_text(&m,", water ");log_float2(&m,oldWater);log_text(&m," -> ");log_float2(&m,finalWater);log_text(&m,", water boost ");log_float2(&m,oldWaterBoost);log_text(&m," -> ");log_float2(&m,finalWaterBoost);log_text(&m,", slip ");log_float2(&m,oldSlip);log_text(&m," -> ");log_float2(&m,finalSlip);log_text(&m,", gear ratio ");log_float2(&m,oldGear);log_text(&m," -> ");log_float2(&m,finalGear);log_text(&m,", drive factor ");log_uint(&m,(UINT64)g_driveForcePercent);log_text(&m,"%.\r\n");append_log(&m);return true;
}
static bool patch_coffin_qpid_border_config(void* resource) {
    // v1.45 trace: read-only. Do NOT change the QPID border escape resource.
    if (!resource || !readable_range((BYTE*)resource + OFF_COFFIN_QPID_BORDER_ESCAPE_SPEED, sizeof(float))) return false;
    float oldValue = *(float*)((BYTE*)resource + OFF_COFFIN_QPID_BORDER_ESCAPE_SPEED);
    g_seenQpidBorderConfig = resource;
    g_qpidBorderConfigReady = 1;
    LogBuffer m; log_init(&m); log_prefix(&m);
    log_text(&m, "TRACE: Coffin QPID border escape speed native/current="); log_float2(&m, oldValue);
    log_text(&m, ". READ-ONLY; value unchanged.\r\n"); append_log(&m);
    return true;
}

struct RawArray{UINT32 count;UINT32 capacity;void* entries;};struct StreamingEvents{void** vtable;};
static bool validate_group_array(const RawArray* objects){if(!objects||!readable_range(objects,sizeof(RawArray)))return false;if(objects->count<1u||objects->count>1000000u||objects->capacity<objects->count||objects->capacity>1000000u||!objects->entries)return false;return readable_range(objects->entries,sizeof(void*));}
static bool group_object_at(const RawArray* objects,UINT32 index,void** object){if(!objects||!object||objects->count<=index)return false;void** slot=(void**)((BYTE*)objects->entries+(SIZE_T)index*sizeof(void*));if(!readable_range(slot,sizeof(void*)))return false;*object=*slot;return true;}
static void inspect_target_positions(const RawArray* objects,bool countTelemetry){if(!validate_group_array(objects))return;AcquireSRWLockExclusive(&g_targetStateLock);if(countTelemetry){++g_callbackGroups;g_callbackObjects+=objects->count;}void* candidate=0;if(!g_physicsReady&&group_object_at(objects,COFFIN_PHYSICS_OBJECT_INDEX,&candidate)){++g_targetChecks;if(object_has_uuid(candidate,COFFIN_PHYSICS_UUID_LOW,COFFIN_PHYSICS_UUID_HIGH)&&object_has_exact_type(candidate,"DSPhysicsCoffinResource"))patch_coffin_physics(candidate);}if(!g_qpidBorderConfigReady&&group_object_at(objects,COFFIN_RIDE_CONFIG_OBJECT_INDEX,&candidate)){++g_targetChecks;if(object_has_uuid(candidate,COFFIN_RIDE_CONFIG_UUID_LOW,COFFIN_RIDE_CONFIG_UUID_HIGH)&&object_has_exact_type(candidate,"DSPlayerRideCoffinConfig"))patch_coffin_qpid_border_config(candidate);}if(g_physicsReady&&g_qpidBorderConfigReady&&!patch_complete_acquire()){publish_patch_complete(true);LogBuffer m;log_init(&m);log_prefix(&m);log_text(&m,"Patch complete. groups=");log_uint(&m,g_callbackGroups);log_text(&m," objects=");log_uint(&m,g_callbackObjects);log_text(&m," rttiLookups=");log_uint(&m,g_rttiLookups);log_text(&m," targetChecks=");log_uint(&m,g_targetChecks);log_text(&m,".\r\n");append_log(&m);}ReleaseSRWLockExclusive(&g_targetStateLock);}
static void inspect_target_unload_positions(const RawArray* objects){if(!validate_group_array(objects)||!g_seenPhysicsResource)return;AcquireSRWLockExclusive(&g_targetStateLock);void* candidate=0;if(group_object_at(objects,COFFIN_PHYSICS_OBJECT_INDEX,&candidate)&&candidate==g_seenPhysicsResource){publish_drive_factor_percent(100);g_seenPhysicsResource=0;g_physicsReady=0;publish_patch_complete(false);log_line("STATE: Coffin physics resource unloaded; drive factor neutralized.");}ReleaseSRWLockExclusive(&g_targetStateLock);}
static void __fastcall on_finish_load(StreamingEvents*,const RawArray* objects){if(!g_enabled||patch_complete_acquire())return;inspect_target_positions(objects,true);}static void __fastcall on_before_unload(StreamingEvents*,const RawArray* objects){inspect_target_unload_positions(objects);}static void __fastcall on_load_asset(StreamingEvents*,const RawArray*){}
static void* g_listenerVtable[3]={(void*)&on_finish_load,(void*)&on_before_unload,(void*)&on_load_asset};static StreamingEvents g_listener={g_listenerVtable};
static UINT64 resolve_rip(UINT64 instruction,UINT32 displacementOffset){int displacement=*(int*)(instruction+displacementOffset);return instruction+displacementOffset+4u+(INT64)displacement;}
static UINT64 find_streaming_manager_global(HMODULE executable){BYTE* base=(BYTE*)executable;BYTE* candidate=base+RVA_STREAMING_SIGNATURE;if(!readable_range(candidate,28u))return 0;static const BYTE signature[28]={0x48,0x89,0x05,0,0,0,0,0xE8,0,0,0,0,0x33,0xD2,0x41,0xB8,0xF8,0x0A,0x00,0x00,0x48,0x8B,0xC8,0x48,0x8B,0xD8,0xE8,0};static const BYTE mask[28]={1,1,1,0,0,0,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0};for(UINT32 i=0;i<28u;++i)if(mask[i]&&candidate[i]!=signature[i])return 0;UINT64 resolved=resolve_rip((UINT64)candidate,3u);UINT64 expected=(UINT64)(base+RVA_STREAMING_MANAGER_GLOBAL);return resolved==expected?resolved:0;}
typedef void(__fastcall* AddListenerFn)(void*,void*);typedef void(__fastcall* RemoveListenerFn)(void*,void*);
static bool register_streaming_listener(void* manager){if(!manager||!readable_range((BYTE*)manager+0x578,sizeof(void*)))return false;void* system=*(void**)((BYTE*)manager+0x578);BYTE* exe=(BYTE*)GetModuleHandleW(0);if(!system||!exe||!readable_range(system,sizeof(void*)))return false;void** vtable=*(void***)system;if(vtable!=(void**)(exe+RVA_STREAMING_SYSTEM_VTABLE)||!readable_range(vtable,5u*sizeof(void*)))return false;AddListenerFn add=(AddListenerFn)vtable[3];RemoveListenerFn remove=(RemoveListenerFn)vtable[4];if(add!=(AddListenerFn)(exe+RVA_STREAMING_ADD_LISTENER)||remove!=(RemoveListenerFn)(exe+RVA_STREAMING_REMOVE_LISTENER)||listener_state_acquire()!=0)return false;add(system,&g_listener);g_streamingSystem=system;g_removeStreamingListener=remove;__atomic_store_n(&g_listenerRegistered,1,__ATOMIC_RELEASE);return true;}
static bool unregister_streaming_listener(){LONG state=listener_state_acquire();if(state==0)return true;if(state!=1||!g_streamingSystem||!g_removeStreamingListener)return false;LONG expected=1;if(!__atomic_compare_exchange_n(&g_listenerRegistered,&expected,2,false,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE))return expected==0;g_removeStreamingListener(g_streamingSystem,&g_listener);g_removeStreamingListener=0;g_streamingSystem=0;__atomic_store_n(&g_listenerRegistered,0,__ATOMIC_RELEASE);return true;}
static void inspect_already_loaded_group(void* streamingSystem,UINT32 groupId){if(!streamingSystem||!readable_range((BYTE*)streamingSystem+OFF_STREAMING_GROUP_LOCK,sizeof(void*)))return;void* lock=(BYTE*)streamingSystem+OFF_STREAMING_GROUP_LOCK;AcquireSRWLockShared(lock);UINT32* arrayHeader=(UINT32*)((BYTE*)streamingSystem+OFF_STREAMING_GROUP_COUNT);void*** tableField=(void***)((BYTE*)streamingSystem+OFF_STREAMING_GROUP_TABLE);if(readable_range(arrayHeader,2u*sizeof(UINT32))&&readable_range(tableField,sizeof(void*))&&arrayHeader[0]>groupId&&arrayHeader[0]<=arrayHeader[1]&&arrayHeader[1]<1000000u&&*tableField){BYTE* entry=(BYTE*)(*tableField)+(SIZE_T)groupId*STREAMING_GROUP_ENTRY_STRIDE;if(readable_range(entry,16u)){void* descriptor=*(void**)(entry+0u);void* loadedGroup=*(void**)(entry+8u);if(descriptor&&loadedGroup&&readable_range(descriptor,sizeof(UINT32))&&*(UINT32*)descriptor==groupId&&readable_range((BYTE*)loadedGroup+OFF_LOADED_GROUP_OBJECTS,sizeof(RawArray)))inspect_target_positions((const RawArray*)((BYTE*)loadedGroup+OFF_LOADED_GROUP_OBJECTS),false);}}ReleaseSRWLockShared(lock);}
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#define worker legacy_worker_never_called
static DWORD __stdcall worker(LPVOID){read_configuration();log_line("DS2 Coffin Board All-Terrain Speed v1.7.1-stability-lock loaded.");if(g_networkTraversalRequested)log_line("NETWORK: EnableNetworkTraversal=1 ignored; traversal remains disabled pending Coffin-state evidence.");if(g_outsideNetworkMountRequested)log_line("NETWORK: EnableOutsideNetworkMount=1 ignored; all Coffin C8 resolver hooks are disabled after save-load crash evidence.");if(!g_enabled){log_line("Disabled in INI; no game memory was changed.");return 0;}HMODULE executable=GetModuleHandleW(0);if(!executable||!validate_target_build(executable)){log_line("ERROR: Unsupported DS2.exe. Expected Steam build 1.10.89.0; no patch applied.");return 0;}UINT64 globalAddress=find_streaming_manager_global(executable);if(!globalAddress||!readable_range((void*)globalAddress,sizeof(void*))) {log_line("ERROR: Exact StreamingManager anchor did not match; no patch applied.");return 0;}void** managerGlobal=(void**)globalAddress;void* manager=0;for(int attempt=0;attempt<1200;++attempt){if(readable_range(managerGlobal,sizeof(void*)))manager=*managerGlobal;if(manager)break;Sleep(50);}if(!manager){log_line("ERROR: StreamingManager did not initialize within 60 seconds.");return 0;}
    if(!install_coffin_network_c8_trace(executable)){log_line("ERROR: Exact passive Coffin C8 trace anchor did not match.");return 0;}if(g_networkTraceInstalled)log_line("NETWORK TRACE: passive Coffin C8 observer installed; native mount result remains unchanged.");
    if(!install_coffin_network_mount_gate(executable)){log_line("ERROR: Exact Coffin RideFloater mount network-gate anchor did not match.");return 0;}
    if(!install_coffin_network_c8_fallback(executable)){log_line("ERROR: Exact Coffin C8 E3 entry anchor did not match.");return 0;}if(g_networkC8FallbackInstalled)log_line("OUTSIDE MOUNT: Coffin-C8 E3 override installed; native output copy is preserved and shared actions are untouched.");
    if(!install_coffin_network_drive_end_guards(executable)){log_line("ERROR: Exact Coffin Drive network/forced-END anchors did not match.");return 0;}
    if(!install_floating_carrier_link_preserve(executable)){log_line("ERROR: Exact Coffin/Floating Carrier transition anchor did not match.");return 0;}
    if(!install_floating_carrier_detach_guard(executable)){log_line("ERROR: Exact Floating Carrier detach-event anchor did not match.");return 0;}
    if(!install_floating_carrier_warning_guard(executable)){log_line("ERROR: Exact Floating Carrier notification anchor did not match.");return 0;}
    if(!install_floating_carrier_gate(executable)){log_line("ERROR: Exact Coffin/Floating Carrier gate anchor did not match.");return 0;}
    if(g_carrierGateInstalled)log_line("Coffin-only Floating Carrier mount gate installed; the active loaded carrier remains linked during the ride.");
    if(g_carrierLinkPreserveInstalled)log_line("Coffin-only RideVehicle carrier-disconnect transaction bypass installed (v1.1.7 native resolver replay + post-call Coffin guard active).");
    if(g_carrierDetachGuardInstalled)log_line("Coffin-only Floating Carrier jump-detach event guard installed.");
    if(g_carrierWarningGuardInstalled)log_line("Coffin-only Floating Carrier overextension HUD/voice notification guard installed.");
    if(g_networkMountGateInstalled&&g_networkC8FallbackInstalled)log_line("Coffin RideFloater mount gate bypassed; the C8 resolver fallback is scoped to that plugin's vtable slot.");
    if(g_networkDriveEndGuardsInstalled)log_line("Coffin Drive-only network and forced-END branches neutralized; all later state transitions remain native.");
    if(g_networkTraceInstalled)log_line("NETWORK TRACE: passive Coffin C8 observer installed; native results remain unchanged.");
    if(!install_drive_force_hook(executable)){log_line("ERROR: Exact Coffin drive-force hook anchor/vtable did not match; no speed patch applied.");return 0;}
    {LogBuffer m;log_init(&m);log_prefix(&m);log_text(&m,"Coffin-only drive-force hook installed neutral at 100%; requested ");log_uint(&m,(UINT64)g_driveForcePercent);log_text(&m,"% activates only after the resource patch.\r\n");append_log(&m);}
    if(!register_streaming_listener(manager)){log_line("ERROR: Could not register resource listener; no speed patch was applied.");return 0;}
    {LogBuffer m;log_init(&m);log_prefix(&m);log_text(&m,"Listener registered. speed=");log_uint(&m,(UINT64)g_speedPercent);log_text(&m,"%, acceleration=");log_uint(&m,(UINT64)g_accelerationPercent);log_text(&m,"%, drive=");log_uint(&m,(UINT64)g_driveForcePercent);log_text(&m,"%, gearRatioPercent=");log_uint(&m,(UINT64)g_gearRatioPercent);log_text(&m,"%, AllowFloatingCarrier=");log_uint(&m,(UINT64)(g_allowFloatingCarrier!=0));log_text(&m,", EnableOutsideNetworkMount=");log_uint(&m,(UINT64)(g_enableOutsideNetworkMount!=0));log_text(&m,", EnableNetworkTraversal=");log_uint(&m,(UINT64)(g_enableNetworkTraversal!=0));log_text(&m,".\r\n");append_log(&m);}
    for(UINT32 poll=0;poll<1200u&&!patch_complete_acquire();++poll){if((poll%10u)==0u)inspect_already_loaded_group(g_streamingSystem,COFFIN_PHYSICS_GROUP_ID);if(!patch_complete_acquire())Sleep(50);}bool completed=patch_complete_acquire();if(!unregister_streaming_listener()){log_line("LIFECYCLE ERROR: exact native listener removal failed; restart DS2 before changing this ASI.");return 0;}if(completed)log_line("LIFECYCLE: patch completion observed; one-shot listener removed outside callbacks.");else log_line("LIFECYCLE WARNING: 60-second resource window expired; listener removed without a completed speed patch.");return 0;}


// -----------------------------------------------------------------------------
// v1.47 post-success root
// -----------------------------------------------------------------------------
// v12 closes the next native mount stage: ED6200 accepts the RideFloater
// provider (1000A70 returns 4), 1000BD0 builds descriptor 0xB on the genuine
// selected Coffin, ED6750 copies it into the request record, and EDFE10 is the
// next per-request availability/state gate.  Hook only the single ED9C80 call
// to EDFE10, call native first, and promote only E3 records that are already
// owned by the RTTI-proven RideFloater provider and carry descriptor 0xB.
static const UINT32 RVA_V147_EDFE10_CALL = 0x00ED9DD1u;
static const UINT32 RVA_V147_EDFE10 = 0x00EDFE10u;
static const UINT32 RVA_V147_BOUNDARY_QUERY_CALL = 0x01F51630u;
static const UINT32 RVA_V147_BOUNDARY_QUERY = 0x02451100u;

typedef UINT64 (__fastcall* CoffinEdfe10Fn)(void*, float, UINT32, void*);
static CoffinEdfe10Fn g_nativeV147Edfe10 = 0;
static volatile LONG g_v147Edfe10Logs = 0;
static volatile LONG g_v147Edfe10Forced = 0;
static int g_v147Edfe10Installed = 0;

typedef UINT32 (__fastcall* CoffinBoundaryQueryFn)(UINT64, void*, void*, UINT32, void*, void*, BYTE);
static CoffinBoundaryQueryFn g_nativeV147BoundaryQuery = 0;
static volatile LONG g_v147BoundaryPositiveCalls = 0;
static volatile LONG g_v147BoundaryCaptureLogs = 0;
static UINT64 g_v147BoundaryTraceLock = 0;
static UINT64 g_v147BoundarySeen0[192] = {};
static UINT64 g_v147BoundarySeen9[192] = {};
static UINT64 g_v147BoundarySeenVt0[192] = {};
static UINT64 g_v147BoundarySeenVt9[192] = {};
static UINT32 g_v147BoundarySeenCount = 0;
static int g_v147BoundaryQueryInstalled = 0;

typedef BYTE (__fastcall* CoffinC8PassiveFn)(void*, UINT32, void*);
static CoffinC8PassiveFn g_nativeV147C8 = 0;
static volatile LONG g_v147C8Logs = 0;
static int g_v147C8Installed = 0;

static UINT64 v147_module_rva(UINT64 address) {
    UINT64 base = (UINT64)g_executable;
    if (!base || address < base || address >= base + (UINT64)EXPECTED_IMAGE_SIZE) return ~0ull;
    return address - base;
}

static UINT64 v147_object_vtable(UINT64 object) {
    if (!object || !readable_range((void*)object, sizeof(UINT64))) return 0;
    UINT64 vtable = *(UINT64*)object;
    if (!vtable || !readable_range((void*)vtable, sizeof(UINT64))) return 0;
    return vtable;
}

static UINT64 __fastcall coffin_v147_edfe10_scope(void* manager, float deltaSeconds,
                                                   UINT32 request, void* stateEntry) {
    UINT64 nativeResult = g_nativeV147Edfe10
        ? g_nativeV147Edfe10(manager, deltaSeconds, request, stateEntry) : 0ull;
    if (request != COFFIN_NETWORK_E3_REQUEST || !manager || !stateEntry) return nativeResult;

    BYTE* record = (BYTE*)manager + 0x48870u + (SIZE_T)request * 0x4D0u;
    void* provider = 0;
    UINT64 providerVtable = 0;
    UINT32 descriptor = 0xffffffffu;
    UINT32 recordGeneration = 0xffffffffu;
    UINT32 stateGeneration = 0xffffffffu;
    BYTE activeBefore = 0xffu;
    BYTE activeAfter = 0xffu;
    BYTE reject = 0xffu;

    if (readable_range(record + 0xA8u, sizeof(void*))) provider = *(void**)(record + 0xA8u);
    if (provider && readable_range(provider, sizeof(void*))) providerVtable = (UINT64)*(void**)provider;
    if (readable_range(record + 0x08u, sizeof(UINT32))) descriptor = *(UINT32*)(record + 0x08u);
    if (readable_range(record + 0x4Cu, sizeof(UINT32))) recordGeneration = *(UINT32*)(record + 0x4Cu);
    if (readable_range((BYTE*)stateEntry + 0x4Cu, sizeof(UINT32))) stateGeneration = *(UINT32*)((BYTE*)stateEntry + 0x4Cu);
    if (readable_range((BYTE*)stateEntry + 0x16u, sizeof(BYTE))) reject = *(BYTE*)((BYTE*)stateEntry + 0x16u);
    if (readable_range((BYTE*)stateEntry + 0xA8u, sizeof(BYTE))) activeBefore = *(BYTE*)((BYTE*)stateEntry + 0xA8u);

    const UINT64 rideFloaterVtable = (UINT64)((BYTE*)g_executable + RVA_COFFIN_RIDE_FLOATER_ACTION_VTABLE);
    const bool exactRideFloaterRecord = provider && providerVtable == rideFloaterVtable && descriptor == 0x0Bu;
    const bool coherentNativeRecord = exactRideFloaterRecord && reject == 0u &&
        recordGeneration != 0xffffffffu && recordGeneration == stateGeneration;
    bool forced = false;
    if (g_enableOutsideNetworkMount && (nativeResult & 0xFFu) == 0u && coherentNativeRecord &&
        readable_range((BYTE*)stateEntry + 0xA8u, sizeof(BYTE))) {
        // Mimic only the common native EDFE10 success contract: the request is
        // active and the call returns true.  Descriptor, provider, generation,
        // target and every later RideFloater lifecycle field remain native.
        *(BYTE*)((BYTE*)stateEntry + 0xA8u) = 1u;
        activeAfter = 1u;
        nativeResult = 1ull;
        forced = true;
        __atomic_fetch_add(&g_v147Edfe10Forced, 1, __ATOMIC_ACQ_REL);
    } else if (readable_range((BYTE*)stateEntry + 0xA8u, sizeof(BYTE))) {
        activeAfter = *(BYTE*)((BYTE*)stateEntry + 0xA8u);
    }

    LONG index = __atomic_fetch_add(&g_v147Edfe10Logs, 1, __ATOMIC_ACQ_REL);
    if (index < 40 || forced) {
        LogBuffer m; log_init(&m); log_prefix(&m);
        log_text(&m, "V147 EDFE10 E3 #"); log_uint(&m, (UINT64)index);
        log_text(&m, ": provider="); log_uint(&m, (UINT64)provider);
        log_text(&m, ", providerVtableRva="); log_uint(&m, v147_module_rva(providerVtable));
        log_text(&m, ", descriptor="); log_uint(&m, descriptor);
        log_text(&m, ", recordGen="); log_uint(&m, recordGeneration);
        log_text(&m, ", stateGen="); log_uint(&m, stateGeneration);
        log_text(&m, ", reject="); log_uint(&m, reject);
        log_text(&m, ", active="); log_uint(&m, activeBefore); log_text(&m, "->"); log_uint(&m, activeAfter);
        log_text(&m, ", resultAL="); log_uint(&m, nativeResult & 0xFFu);
        log_text(&m, ", forced="); log_uint(&m, forced ? 1u : 0u);
        log_text(&m, ".\r\n"); append_log(&m);
    }
    return nativeResult;
}

static bool install_v147_edfe10_scope(HMODULE executable) {
    if (!g_enableOutsideNetworkMount) return true;
    BYTE* base = (BYTE*)executable;
    BYTE* call = base + RVA_V147_EDFE10_CALL;
    BYTE* nativeTarget = base + RVA_V147_EDFE10;
    if (!readable_range(call, 5u) || call[0] != 0xE8u) return false;
    int oldDisp = 0; memcpy(&oldDisp, call + 1u, sizeof(oldDisp));
    if (call + 5u + oldDisp != nativeTarget) return false;
    g_nativeV147Edfe10 = (CoffinEdfe10Fn)nativeTarget;
    BYTE* relay = (BYTE*)allocate_call_relay_near(call, (void*)&coffin_v147_edfe10_scope);
    if (!relay) return false;
    INT64 disp64 = (INT64)relay - (INT64)(call + 5u);
    if (disp64 < -0x80000000ll || disp64 > 0x7fffffffll) return false;
    int newDisp = (int)disp64;
    if (!freeze_for_patch(call, 5u)) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(call, 5u, PAGE_EXECUTE_READWRITE, &oldProtection)) { unfreeze_after_patch(); return false; }
    memcpy(call + 1u, &newDisp, sizeof(newDisp));
    FlushInstructionCache((HANDLE)(INT64)-1, call, 5u);
    int verify = 0; memcpy(&verify, call + 1u, sizeof(verify));
    bool installed = call[0] == 0xE8u && call + 5u + verify == relay;
    if (!installed) { memcpy(call + 1u, &oldDisp, sizeof(oldDisp)); FlushInstructionCache((HANDLE)(INT64)-1, call, 5u); }
    DWORD ignored = 0; VirtualProtect(call, 5u, oldProtection, &ignored); unfreeze_after_patch();
    if (installed) g_v147Edfe10Installed = 1;
    return installed;
}

static bool v147_boundary_hit_new(UINT64 q0, UINT64 q9, UINT64 vt0, UINT64 vt9) {
    bool isNew = false;
    AcquireSRWLockExclusive(&g_v147BoundaryTraceLock);
    for (UINT32 i = 0; i < g_v147BoundarySeenCount; ++i) {
        if (g_v147BoundarySeen0[i] == q0 && g_v147BoundarySeen9[i] == q9 &&
            g_v147BoundarySeenVt0[i] == vt0 && g_v147BoundarySeenVt9[i] == vt9) {
            ReleaseSRWLockExclusive(&g_v147BoundaryTraceLock); return false;
        }
    }
    if (g_v147BoundarySeenCount < 192u) {
        UINT32 i = g_v147BoundarySeenCount++;
        g_v147BoundarySeen0[i] = q0; g_v147BoundarySeen9[i] = q9;
        g_v147BoundarySeenVt0[i] = vt0; g_v147BoundarySeenVt9[i] = vt9;
        isNew = true;
    }
    ReleaseSRWLockExclusive(&g_v147BoundaryTraceLock);
    return isNew;
}

static UINT32 __fastcall coffin_v147_boundary_query_scope(UINT64 p1, void* query, void* output,
                                                           UINT32 maxResults, void* filter,
                                                           void* p6, BYTE p7) {
    if (!g_nativeV147BoundaryQuery) return 0;
    UINT32 nativeResult = g_nativeV147BoundaryQuery(p1, query, output, maxResults, filter, p6, p7);
    if (!g_enableNetworkTraversal || nativeResult == 0u || output != 0 || maxResults != 1u) return nativeResult;

    LONG positive = __atomic_fetch_add(&g_v147BoundaryPositiveCalls, 1, __ATOMIC_ACQ_REL);
    // Preserve the original query/result exactly.  Sample its hit object with a
    // second read-only query: dense at startup, then every 16th positive hit.
    if (positive >= 32 && (positive & 15) != 0) return nativeResult;

    BYTE hit[0x50]; memset(hit, 0, sizeof(hit));
    UINT32 copied = g_nativeV147BoundaryQuery(p1, query, hit, 1u, filter, p6, p7);
    if (copied == 0u) return nativeResult;

    UINT64 q[10] = {};
    memcpy(q, hit, sizeof(q));
    UINT64 vt0 = v147_object_vtable(q[0]);
    UINT64 vt9 = v147_object_vtable(q[9]);
    if (!v147_boundary_hit_new(q[0], q[9], vt0, vt9)) return nativeResult;

    LONG index = __atomic_fetch_add(&g_v147BoundaryCaptureLogs, 1, __ATOMIC_ACQ_REL);
    if (index < 192) {
        LogBuffer m; log_init(&m); log_prefix(&m);
        log_text(&m, "V147 BOUNDARY HIT #"); log_uint(&m, (UINT64)index);
        log_text(&m, ": q0="); log_uint(&m, q[0]);
        log_text(&m, ", vt0Rva="); log_uint(&m, v147_module_rva(vt0));
        log_text(&m, ", q9="); log_uint(&m, q[9]);
        log_text(&m, ", vt9Rva="); log_uint(&m, v147_module_rva(vt9));
        log_text(&m, ", raw=");
        for (UINT32 i = 0; i < 10u; ++i) { if (i) log_char(&m, '|'); log_uint(&m, q[i]); }
        log_text(&m, ". Native query result unchanged.\r\n"); append_log(&m);
    }
    return nativeResult;
}

static bool install_v147_boundary_query_trace(HMODULE executable) {
    if (!g_enableNetworkTraversal) return true;
    BYTE* base = (BYTE*)executable;
    BYTE* call = base + RVA_V147_BOUNDARY_QUERY_CALL;
    BYTE* nativeTarget = base + RVA_V147_BOUNDARY_QUERY;
    if (!readable_range(call, 5u) || call[0] != 0xE8u) return false;
    int oldDisp = 0; memcpy(&oldDisp, call + 1u, sizeof(oldDisp));
    if (call + 5u + oldDisp != nativeTarget) return false;
    g_nativeV147BoundaryQuery = (CoffinBoundaryQueryFn)nativeTarget;
    BYTE* relay = (BYTE*)allocate_call_relay_near(call, (void*)&coffin_v147_boundary_query_scope);
    if (!relay) return false;
    INT64 disp64 = (INT64)relay - (INT64)(call + 5u);
    if (disp64 < -0x80000000ll || disp64 > 0x7fffffffll) return false;
    int newDisp = (int)disp64;
    if (!freeze_for_patch(call, 5u)) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(call, 5u, PAGE_EXECUTE_READWRITE, &oldProtection)) { unfreeze_after_patch(); return false; }
    memcpy(call + 1u, &newDisp, sizeof(newDisp));
    FlushInstructionCache((HANDLE)(INT64)-1, call, 5u);
    int verify = 0; memcpy(&verify, call + 1u, sizeof(verify));
    bool installed = call[0] == 0xE8u && call + 5u + verify == relay;
    if (!installed) { memcpy(call + 1u, &oldDisp, sizeof(oldDisp)); FlushInstructionCache((HANDLE)(INT64)-1, call, 5u); }
    DWORD ignored = 0; VirtualProtect(call, 5u, oldProtection, &ignored); unfreeze_after_patch();
    if (installed) g_v147BoundaryQueryInstalled = 1;
    return installed;
}

// Passive C8 observer for the same test.  It never replays or changes a result.
static BYTE __fastcall coffin_v147_c8_passive(void* plugin, UINT32 request, void* output) {
    BYTE result = g_nativeV147C8 ? g_nativeV147C8(plugin, request, output) : 0u;
    if (request != COFFIN_NETWORK_E3_REQUEST) return result;
    LONG index = __atomic_fetch_add(&g_v147C8Logs, 1, __ATOMIC_ACQ_REL);
    if (index < 40) {
        void* manager = 0; void* owner = 0; UINT32 recordGen = 0xffffffffu, stateGen = 0xffffffffu;
        BYTE reject = 0xffu, active = 0xffu; UINT64 target = ~0ull;
        if (plugin && readable_range((BYTE*)plugin + 0x60u, sizeof(void*))) manager = *(void**)((BYTE*)plugin + 0x60u);
        if (manager) {
            BYTE* record = (BYTE*)manager + 0x48870u + (SIZE_T)request * 0x4D0u;
            BYTE* state = (BYTE*)manager + (SIZE_T)request * 0xD8u + 0x25140u;
            if (readable_range(record + 0xA8u, sizeof(void*))) owner = *(void**)(record + 0xA8u);
            if (readable_range(record + 0x4Cu, sizeof(UINT32))) recordGen = *(UINT32*)(record + 0x4Cu);
            if (readable_range(state + 0x4Cu, sizeof(UINT32))) stateGen = *(UINT32*)(state + 0x4Cu);
            if (readable_range(state + 0x16u, sizeof(BYTE))) reject = *(BYTE*)(state + 0x16u);
            if (readable_range(state + 0xA8u, sizeof(BYTE))) active = *(BYTE*)(state + 0xA8u);
        }
        if (output && result && readable_range((BYTE*)output + 0x18u, sizeof(UINT64))) target = *(UINT64*)((BYTE*)output + 0x10u);
        LogBuffer m; log_init(&m); log_prefix(&m);
        log_text(&m, "V147 C8 E3 #"); log_uint(&m, (UINT64)index);
        log_text(&m, ": result="); log_uint(&m, result);
        log_text(&m, ", ownerMatchesPlugin="); log_uint(&m, owner == plugin ? 1u : 0u);
        log_text(&m, ", recordGen="); log_uint(&m, recordGen);
        log_text(&m, ", stateGen="); log_uint(&m, stateGen);
        log_text(&m, ", reject="); log_uint(&m, reject);
        log_text(&m, ", active="); log_uint(&m, active);
        log_text(&m, ", payloadTarget="); log_uint(&m, target);
        log_text(&m, ". Native result unchanged.\r\n"); append_log(&m);
    }
    return result;
}

static bool install_v147_c8_passive(HMODULE executable) {
    if (!g_enableOutsideNetworkMount) return true;
    BYTE* base = (BYTE*)executable;
    void** vtable = (void**)(base + RVA_COFFIN_RIDE_FLOATER_ACTION_VTABLE);
    void** slot = (void**)(base + RVA_COFFIN_RIDE_FLOATER_ACTION_C8_SLOT);
    void* expected = (void*)(base + RVA_COFFIN_MOUNT_VFUNC);
    if (!readable_range(vtable, 26u * sizeof(void*)) || !readable_range(slot, sizeof(void*)) ||
        vtable[1] != (void*)(base + RVA_COFFIN_RIDE_FLOATER_MOUNT_ENTRY) || *slot != expected) return false;
    g_nativeV147C8 = (CoffinC8PassiveFn)expected;
    if (!freeze_for_patch((const BYTE*)slot, sizeof(void*))) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(slot, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtection)) { unfreeze_after_patch(); return false; }
    *slot = (void*)&coffin_v147_c8_passive;
    FlushInstructionCache((HANDLE)(INT64)-1, slot, sizeof(void*));
    bool installed = *slot == (void*)&coffin_v147_c8_passive;
    if (!installed) *slot = expected;
    DWORD ignored = 0; VirtualProtect(slot, sizeof(void*), oldProtection, &ignored); unfreeze_after_patch();
    if (installed) g_v147C8Installed = 1;
    return installed;
}


// -----------------------------------------------------------------------------
// v1.51.0: runtime-validated correction of the two roots exposed by v1.50.
//
// Crossing:
//   v1.50 proved +0x514 is only the progressive drive clamp. F43700's same
//   boundary-hit branch also owns +0x624 (near-QPID-wall distance) and +0x62D
//   (wall-plane response enable). The only DSPhysicsCoffinInstance consumer of
//   +0x62D feeds the stored +0x5F0..+0x610 plane into the later boundary
//   response. Therefore the exact hit branch is neutralized as one state:
//   +514=-1, +624=-1, +62D=0. +62E and ordinary collision state stay native.
//
// Mount:
//   F67B10 can break its action-poll loop before E3, so a hook at its
//   213B980(E3,2) callsite is not sufficient. F67ED0 is the final native
//   availability-rule merge and only ORs rule outputs into +0x67E0..+0x686C.
//   Hook its sole F66B60 callsite, run native first, then expose E3 Base only
//   when all of the following are true:
//     * current manager+0x230 is the RTTI/signature-confirmed Coffin candidate,
//     * the native outside-network D4 bit20 and manager+0x2F0 bit1 are absent,
//     * the native NxAction E3 channel 2 (+0x200) is physically > 0,
//     * E3 is not already present and the native Block bit is clear.
//   F68750 and F689B0 then create Raw-B and Rising-A themselves. No lifecycle,
//   target, EDFE10 result, state4 or player ride-state field is synthesized.
static const UINT32 RVA_V151_QPID_LIMITER_CALL = 0x01F43AAAu;
static const UINT32 RVA_V151_QPID_LIMITER_NATIVE = 0x01F43700u;
static const UINT32 RVA_V151_ACTION_REGISTRY = 0x06246690u;
static const UINT32 RVA_V151_RULE_MERGE_CALL = 0x00F675ADu;
static const UINT32 RVA_V151_RULE_MERGE_NATIVE = 0x00F67ED0u;
static const UINT32 RVA_V151_EDFE10_CALL = 0x00ED9DD1u;
static const UINT32 RVA_V151_EDFE10_NATIVE = 0x00EDFE10u;

// v1.60 Outside-Mount discovery/lifecycle fix. EDB900 is the proven native producer of
// DSPlayerPadCacheInfoCart<DSBaggageInfo,2> (+0x228). Its physical query at
// EDBCD3 returns raw physics entries; the later resolver at EDBD6C maps an
// accepted type-8 result to DSBaggageInfo before native cache admission.
// Both callsites are observed read-only. Crossing code is not modified here.
static const UINT32 RVA_V157_BAGGAGE_QUERY_CALL = 0x00EDBCD3u;
static const UINT32 RVA_V157_BAGGAGE_QUERY_NATIVE = 0x011DB210u;
static const UINT32 RVA_V157_BAGGAGE_RESOLVE_CALL = 0x00EDBD6Cu;
static const UINT32 RVA_V157_BAGGAGE_RESOLVE_NATIVE = 0x011D6C10u;
static const UINT32 RVA_V157_ACTIVE_WORLD_GLOBAL = 0x06266968u;
static const UINT32 RVA_V160_PHYSICS_REGISTRY_GLOBAL = 0x0623EA48u;
static const UINT32 RVA_V160_QUERY_RADIUS_PAD = 0x03461164u;


static volatile LONG g_v151E3PressLogs = 0;
static volatile LONG g_v151E3BaseInjections = 0;
static volatile LONG g_v151EdfeChangeLogs = 0;
static volatile UINT64 g_v151EdfeLastSignature = ~0ull;
static int g_v151QpidInstalled = 0;
static int g_v151RuleMergeInstalled = 0;
static int g_v151EdfePassiveInstalled = 0;

typedef void (__fastcall* V151QpidFn)(void*, UINT32);
typedef void (__fastcall* V151RuleMergeFn)(void*);
typedef UINT64 (__fastcall* V151Edfe10Fn)(void*, float, UINT32, void*);
static V151QpidFn g_nativeV151Qpid = 0;
static V151RuleMergeFn g_nativeV151RuleMerge = 0;
static V151Edfe10Fn g_nativeV151Edfe10 = 0;

typedef UINT32 (__fastcall* V157BaggageQueryFn)(UINT64, void*, double*, float, float, UINT32);
typedef void* (__fastcall* V157BaggageResolveFn)(void*, UINT64*);
static V157BaggageQueryFn g_nativeV157BaggageQuery = 0;
static V157BaggageResolveFn g_nativeV157BaggageResolve = 0;
static volatile UINT64 g_v157CoffinQueryEntry = 0;
static volatile UINT64 g_v157CoffinCandidate = 0;
static volatile UINT64 g_v157LastSnapshotSig = ~0ull;
static volatile UINT64 g_v157LastResolverSig = ~0ull;
static volatile LONG g_v157SnapshotLogs = 0;
static volatile LONG g_v157ResolverLogs = 0;
static volatile LONG g_v157QueryCalls = 0;
static volatile DWORD g_v157LastQueryThread = 0;
static volatile UINT32 g_v157LastQueryCount = 0;
static volatile UINT32 g_v157LastStoredCount = 0;
static volatile UINT64 g_v157LastOutput = 0;
static UINT64 g_v157LastEntries[512] = {};
static UINT64 g_v157LastHandles[512] = {};
static double g_v157LastCenter[3] = {};
static float g_v157LastRadius = 0.0f;
static int g_v157QueryTraceInstalled = 0;
static int g_v157ResolverTraceInstalled = 0;

static bool v151_validate_call(BYTE* call, BYTE* nativeTarget, int* oldDisp) {
    if (!readable_range(call, 5u) || call[0] != 0xE8u) return false;
    int disp = 0; memcpy(&disp, call + 1u, sizeof(disp));
    if (call + 5u + disp != nativeTarget) return false;
    if (oldDisp) *oldDisp = disp;
    return true;
}

static bool v151_patch_call(BYTE* call, BYTE* relay, int oldDisp) {
    INT64 disp64 = (INT64)relay - (INT64)(call + 5u);
    if (disp64 < -0x80000000ll || disp64 > 0x7fffffffll) return false;
    int newDisp = (int)disp64;
    if (!freeze_for_patch(call, 5u)) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(call, 5u, PAGE_EXECUTE_READWRITE, &oldProtection)) {
        unfreeze_after_patch(); return false;
    }
    memcpy(call + 1u, &newDisp, sizeof(newDisp));
    FlushInstructionCache((HANDLE)(INT64)-1, call, 5u);
    int verify = 0; memcpy(&verify, call + 1u, sizeof(verify));
    bool installed = call[0] == 0xE8u && call + 5u + verify == relay;
    if (!installed) {
        memcpy(call + 1u, &oldDisp, sizeof(oldDisp));
        FlushInstructionCache((HANDLE)(INT64)-1, call, 5u);
    }
    DWORD ignored = 0; VirtualProtect(call, 5u, oldProtection, &ignored);
    unfreeze_after_patch();
    return installed;
}


static bool v157_is_genuine_coffin_candidate(void* candidate) {
    if (!candidate || !g_executable ||
        !readable_range(candidate, sizeof(void*)) ||
        !readable_range((BYTE*)candidate + 0x08u, sizeof(UINT64))) return false;
    return *(void**)candidate == (void*)((BYTE*)g_executable + RVA_COFFIN_BAGGAGE_INFO_VTABLE) &&
           *(UINT64*)((BYTE*)candidate + 0x08u) == COFFIN_BAGGAGE_Q08_SIGNATURE;
}

static UINT64 v157_hash(UINT64 h, UINT64 v) {
    h ^= v + 0x9E3779B97F4A7C15ull + (h << 6) + (h >> 2);
    return h;
}

static bool v157_entry_fields(BYTE* entry, short* worldId, UINT32* flags, float* score,
                              BYTE* type, UINT64* handle, UINT64* owner98,
                              double* x, double* y, double* z) {
    if (!entry || !readable_range(entry, 0xF0u)) return false;
    if (worldId) *worldId = *(short*)(entry + 0x80u);
    if (flags) *flags = *(UINT32*)(entry + 0xECu);
    if (score) *score = *(float*)(entry + 0x88u);
    if (handle) *handle = *(UINT64*)(entry + 0x20u);
    if (owner98) *owner98 = *(UINT64*)(entry + 0x98u);
    BYTE t = 0xFFu;
    void* typeOwner = *(void**)(entry + 0x38u);
    if (typeOwner && readable_range((BYTE*)typeOwner + 0x49u, sizeof(BYTE)))
        t = *(BYTE*)((BYTE*)typeOwner + 0x49u);
    if (type) *type = t;
    // 1411DB210 snapshots three doubles from +0x40/+0x48/+0x50 for its
    // native radius/cone tests. They are safe to observe under the same
    // readable-range guard used above; no physics state is modified.
    if (x) *x = *(double*)(entry + 0x40u);
    if (y) *y = *(double*)(entry + 0x48u);
    if (z) *z = *(double*)(entry + 0x50u);
    return true;
}

static void v157_snapshot_cached_entry(BYTE* output, UINT32 count, bool forceLog) {
    BYTE* entry = (BYTE*)__atomic_load_n(&g_v157CoffinQueryEntry, __ATOMIC_ACQUIRE);
    BYTE* candidate = (BYTE*)__atomic_load_n(&g_v157CoffinCandidate, __ATOMIC_ACQUIRE);
    if (!entry) return;

    bool present = false;
    UINT32 safeCount = count > 0x2400u ? 0x2400u : count;
    if (output && readable_range(output, (SIZE_T)safeCount * sizeof(void*))) {
        void** results = (void**)output;
        for (UINT32 i = 0; i < safeCount; ++i) {
            if ((BYTE*)results[i] == entry) { present = true; break; }
        }
    }

    short entryWorld = -1;
    UINT32 flags = 0;
    float score = 0.0f;
    BYTE type = 0xFFu;
    UINT64 handle = 0, owner98 = 0;
    double ex = 0.0, ey = 0.0, ez = 0.0;
    bool entryReadable = v157_entry_fields(entry, &entryWorld, &flags, &score, &type,
                                           &handle, &owner98, &ex, &ey, &ez);

    bool currentWorldKnown = false;
    short currentWorld = -1;
    if (output && safeCount > 0u && readable_range(output, (SIZE_T)safeCount * sizeof(void*))) {
        void** results = (void**)output;
        for (UINT32 i = 0; i < safeCount; ++i) {
            BYTE* e = (BYTE*)results[i];
            if (e && readable_range(e + 0x80u, sizeof(short))) {
                currentWorld = *(short*)(e + 0x80u);
                currentWorldKnown = true;
                break;
            }
        }
    }
    // Native 1411DB210 uses this direct world-id source when its +0xA8 flag
    // is set. It is a safe fallback when the query returns zero other entries.
    if (!currentWorldKnown && g_executable &&
        readable_range((BYTE*)g_executable + RVA_V157_ACTIVE_WORLD_GLOBAL, sizeof(void*))) {
        BYTE* activeWorld = *(BYTE**)((BYTE*)g_executable + RVA_V157_ACTIVE_WORLD_GLOBAL);
        if (activeWorld && readable_range(activeWorld + 0xA8u, sizeof(BYTE)) &&
            *(BYTE*)(activeWorld + 0xA8u) != 0u &&
            readable_range(activeWorld + 0x50u, sizeof(short))) {
            currentWorld = *(short*)(activeWorld + 0x50u);
            currentWorldKnown = true;
        }
    }

    UINT32 cacheCount = 0xFFFFFFFFu, selectedIndex = 0xFFFFFFFFu;
    void* cacheSelected = 0;
    BYTE* cache = (BYTE*)__atomic_load_n(&g_nativeBaggageCache, __ATOMIC_ACQUIRE);
    if (cache && readable_range(cache + 0xB0u, 4u)) {
        if (readable_range(cache + 0xA8u, sizeof(UINT32))) cacheCount = *(UINT32*)(cache + 0xA8u);
        if (readable_range(cache + 0xACu, sizeof(UINT32))) selectedIndex = *(UINT32*)(cache + 0xACu);
        if (readable_range(cache + 0x08u, sizeof(void*))) cacheSelected = *(void**)(cache + 0x08u);
    }

    bool worldPass = !currentWorldKnown || !entryReadable || currentWorld == entryWorld;
    bool flagsPass = entryReadable && ((flags & 0x00200002u) == 0u) && ((flags & 0x00100000u) == 0u);
    bool ownerPass = entryReadable && owner98 == 0u; // EDB900 uses param6=9, so 1411DB210 requires +0x98 == 0.
    bool scorePass = entryReadable && score > 0.0f;
    bool typePass = entryReadable && type == 8u;
    bool candidateStillValid = v157_is_genuine_coffin_candidate(candidate) &&
                               readable_range(candidate + 0x08u, sizeof(UINT64)) &&
                               *(UINT64*)(candidate + 0x08u) != ~0ull;

    float baseRadius = 0.0f;
    if (g_executable && readable_range((BYTE*)g_executable + 0x03461164u, sizeof(float)))
        baseRadius = *(float*)((BYTE*)g_executable + 0x03461164u);
    float threshold = baseRadius + g_v157LastRadius;
    double dx = ex - g_v157LastCenter[0];
    double dy = ey - g_v157LastCenter[1];
    double dz = ez - g_v157LastCenter[2];
    double dist2 = dx*dx + dy*dy + dz*dz;
    double threshold2 = (double)threshold * (double)threshold;
    bool distancePass = entryReadable && dist2 < threshold2;

    UINT64 sig = 0x1570000ull;
    sig = v157_hash(sig, present ? 1u : 0u);
    sig = v157_hash(sig, entryReadable ? 1u : 0u);
    sig = v157_hash(sig, currentWorldKnown ? (UINT32)(unsigned short)currentWorld : 0xFFFFFFFFu);
    sig = v157_hash(sig, (UINT32)(unsigned short)entryWorld);
    sig = v157_hash(sig, flags);
    sig = v157_hash(sig, type);
    sig = v157_hash(sig, worldPass ? 1u : 0u);
    sig = v157_hash(sig, flagsPass ? 1u : 0u);
    sig = v157_hash(sig, distancePass ? 1u : 0u);
    sig = v157_hash(sig, ownerPass ? 1u : 0u);
    sig = v157_hash(sig, scorePass ? 1u : 0u);
    sig = v157_hash(sig, typePass ? 1u : 0u);
    sig = v157_hash(sig, cacheCount);
    sig = v157_hash(sig, candidateStillValid ? 1u : 0u);
    sig = v157_hash(sig, __atomic_load_n(&g_v151QpidHits, __ATOMIC_ACQUIRE) > 0 ? 1u : 0u);

    UINT64 old = __atomic_exchange_n(&g_v157LastSnapshotSig, sig, __ATOMIC_ACQ_REL);
    if (!forceLog && old == sig) return;
    LONG idx = __atomic_fetch_add(&g_v157SnapshotLogs, 1, __ATOMIC_ACQ_REL);
    if (idx >= 96) return;

    LogBuffer m; log_init(&m); log_prefix(&m);
    log_text(&m, "V157 BAGGAGE ROOT #"); log_uint(&m, (UINT64)idx);
    log_text(&m, ": entry="); log_uint(&m, (UINT64)entry);
    log_text(&m, ", candidate="); log_uint(&m, (UINT64)candidate);
    log_text(&m, ", inQuery="); log_uint(&m, present ? 1u : 0u);
    log_text(&m, ", queryCount="); log_uint(&m, count);
    log_text(&m, ", qpidSeen="); log_uint(&m, __atomic_load_n(&g_v151QpidHits, __ATOMIC_ACQUIRE) > 0 ? 1u : 0u);
    log_text(&m, ", currentWorldKnown="); log_uint(&m, currentWorldKnown ? 1u : 0u);
    log_text(&m, ", currentWorld="); log_uint(&m, (UINT64)(UINT32)(unsigned short)currentWorld);
    log_text(&m, ", entryWorld="); log_uint(&m, (UINT64)(UINT32)(unsigned short)entryWorld);
    log_text(&m, ", worldPass="); log_uint(&m, worldPass ? 1u : 0u);
    log_text(&m, ", flagsEC="); log_uint(&m, flags);
    log_text(&m, ", flagsPass="); log_uint(&m, flagsPass ? 1u : 0u);
    log_text(&m, ", distancePass="); log_uint(&m, distancePass ? 1u : 0u);
    log_text(&m, ", dist2="); log_float2(&m, (float)dist2);
    log_text(&m, ", radius2="); log_float2(&m, (float)threshold2);
    log_text(&m, ", owner98="); log_uint(&m, owner98);
    log_text(&m, ", ownerPass="); log_uint(&m, ownerPass ? 1u : 0u);
    log_text(&m, ", score88="); log_float2(&m, score);
    log_text(&m, ", scorePass="); log_uint(&m, scorePass ? 1u : 0u);
    log_text(&m, ", type49="); log_uint(&m, type);
    log_text(&m, ", type8="); log_uint(&m, typePass ? 1u : 0u);
    log_text(&m, ", handle20="); log_uint(&m, handle);
    log_text(&m, ", cacheCount="); log_uint(&m, cacheCount);
    log_text(&m, ", selectedIndex="); log_uint(&m, selectedIndex);
    log_text(&m, ", cacheSelectedMatch="); log_uint(&m, cacheSelected == candidate ? 1u : 0u);
    log_text(&m, ", candidateValid="); log_uint(&m, candidateStillValid ? 1u : 0u);
    log_text(&m, ". READ-ONLY EDB900/1411DB210 root snapshot.\r\n");
    append_log(&m);
}

static UINT32 v159_query_result_count(void* output) {
    if (!output || !readable_range((BYTE*)output + 0x12000u, sizeof(UINT32))) return 0u;
    return *(UINT32*)((BYTE*)output + 0x12000u);
}

// Last-resort discovery only. This does NOT fabricate a DSBaggageInfo object.
// It scans the same live broadphase registry consumed by 1411DB210 and accepts
// an entry only when all of the following are true:
//   * same active world (when known), native physics exclusion bits clear,
//   * Type-8 metadata, positive score and within the native EDB900 radius,
//   * outside owner marker (+0x98 != 0), and
//   * the original 1411D6C10 registry resolver maps its handle to the exact
//     Coffin DSBaggageInfo vtable + q08 signature.
// EDB900 then performs its normal resolver/cache-admission path on this entry.
static BYTE* v160_find_genuine_outside_coffin_entry(double* center, float radius,
                                                     UINT64* resolvedCandidateOut,
                                                     double* bestDist2Out) {
    if (resolvedCandidateOut) *resolvedCandidateOut = 0u;
    if (bestDist2Out) *bestDist2Out = 0.0;
    if (!g_executable || !g_nativeV157BaggageResolve || !center ||
        !readable_range(center, 3u * sizeof(double))) return 0;

    BYTE* base = (BYTE*)g_executable;
    BYTE** registrySlot = (BYTE**)(base + RVA_V160_PHYSICS_REGISTRY_GLOBAL);
    if (!readable_range(registrySlot, sizeof(void*))) return 0;
    BYTE* registry = *registrySlot;
    if (!registry || !readable_range(registry + 0x24468u, sizeof(void*)) ||
        !readable_range(registry + 0x24460u, sizeof(UINT32))) return 0;

    UINT32 recordCount = *(UINT32*)(registry + 0x24460u);
    UINT64* records = *(UINT64**)(registry + 0x24468u);
    if (!records || recordCount == 0u || recordCount > 0x200000u) return 0;

    bool worldKnown = false;
    short currentWorld = -1;
    BYTE** activeWorldSlot = (BYTE**)(base + RVA_V157_ACTIVE_WORLD_GLOBAL);
    if (readable_range(activeWorldSlot, sizeof(void*))) {
        BYTE* activeWorld = *activeWorldSlot;
        if (activeWorld && readable_range(activeWorld + 0xA8u, sizeof(BYTE)) &&
            *(BYTE*)(activeWorld + 0xA8u) != 0u &&
            readable_range(activeWorld + 0x50u, sizeof(short))) {
            currentWorld = *(short*)(activeWorld + 0x50u);
            worldKnown = true;
        }
    }

    float radiusPad = 0.0f;
    if (readable_range(base + RVA_V160_QUERY_RADIUS_PAD, sizeof(float)))
        radiusPad = *(float*)(base + RVA_V160_QUERY_RADIUS_PAD);
    float threshold = radius + radiusPad;
    double threshold2 = (double)threshold * (double)threshold;

    BYTE* bestEntry = 0;
    void* bestCandidate = 0;
    double bestDist2 = 1.0e300;
    for (UINT32 i = 0; i < recordCount; ++i) {
        UINT64* record = records + (SIZE_T)i * 13u;
        if (!readable_range(record, sizeof(UINT64))) break;
        BYTE* entry = (BYTE*)(UINT64)*record;
        short world = -1; UINT32 entryFlags = 0; float score = 0.0f; BYTE type = 0xFFu;
        UINT64 handle = 0, owner98 = 0; double x = 0.0, y = 0.0, z = 0.0;
        if (!v157_entry_fields(entry, &world, &entryFlags, &score, &type,
                               &handle, &owner98, &x, &y, &z)) continue;
        if (worldKnown && world != currentWorld) continue;
        if ((entryFlags & 0x00300002u) != 0u) continue;
        if (type != 8u || !(score > 0.0f) || owner98 == 0u || handle == 0u) continue;

        double dx = x - center[0];
        double dy = y - center[1];
        double dz = z - center[2];
        double dist2 = dx*dx + dy*dy + dz*dz;
        if (!(dist2 < threshold2) || !(dist2 < bestDist2)) continue;

        UINT64 h = handle;
        void* candidate = g_nativeV157BaggageResolve(registry, &h);
        if (!v157_is_genuine_coffin_candidate(candidate)) continue;
        bestEntry = entry;
        bestCandidate = candidate;
        bestDist2 = dist2;
    }

    if (bestEntry) {
        if (resolvedCandidateOut) *resolvedCandidateOut = (UINT64)bestCandidate;
        if (bestDist2Out) *bestDist2Out = bestDist2;
    }
    return bestEntry;
}

static BYTE* v161_resolve_candidate_from_vehicle(BYTE* vehicle) {
    if (!v161_is_live_coffin_vehicle(vehicle) ||
        !readable_range(vehicle + 0xB0u, sizeof(UINT32)) ||
        !readable_range(vehicle + 0xA8u, sizeof(void*))) return 0;
    BYTE* collection = vehicle + 0xA0u;
    int count = *(int*)(collection + 0x10u);
    void** items = *(void***)(collection + 0x08u);
    if (count <= 0 || count > 512 || !items ||
        !readable_range(items, (SIZE_T)count * sizeof(void*))) return 0;
    BYTE* base = (BYTE*)g_executable;
    void* expected = (void*)(base + RVA_V161_BAGGAGE_COMPONENT_VTABLE);
    void* expectedAlt = (void*)(base + RVA_V161_BAGGAGE_COMPONENT_ALT_VTABLE);
    for (int i = 0; i < count; ++i) {
        BYTE* component = (BYTE*)items[i];
        if (!component || !readable_range(component, sizeof(void*))) continue;
        void* vt = *(void**)component;
        BYTE* candidate = 0;
        if (vt == expected) {
            candidate = component + 0x58u;
        } else if (vt == expectedAlt) {
            // This alternate vtable is the +0x20 DSBaggageComponent subobject.
            // Validate both possible pointer forms and accept only the exact
            // DSBaggageInfo vtable+q08 signature.
            BYTE* p1 = component + 0x58u;
            BYTE* p2 = component + 0x38u;
            if (v159_is_genuine_coffin_candidate_early(p1)) candidate = p1;
            else if (v159_is_genuine_coffin_candidate_early(p2)) candidate = p2;
        }
        if (candidate && v159_is_genuine_coffin_candidate_early(candidate)) return candidate;
    }
    return 0;
}

static void __fastcall v167_coffin_update_capture(void* vehicle) {
    if (vehicle && v161_is_live_coffin_vehicle((BYTE*)vehicle)) {
        UINT64 old = __atomic_exchange_n(&g_v167CoffinVehicle, (UINT64)vehicle, __ATOMIC_ACQ_REL);
        if (old != (UINT64)vehicle) {
            LONG idx = __atomic_fetch_add(&g_v167VehicleCaptureLogs, 1, __ATOMIC_ACQ_REL);
            if (idx < 16) {
                LogBuffer m; log_init(&m); log_prefix(&m);
                log_text(&m, "V169 COFFIN ENTITY CAPTURE #"); log_uint(&m, (UINT64)idx);
                log_text(&m, ": vehicle="); log_uint(&m, (UINT64)vehicle);
                log_text(&m, ", qpidHits="); log_uint(&m, (UINT64)__atomic_load_n(&g_v151QpidHits, __ATOMIC_ACQUIRE));
                log_text(&m, ". Passive DSVehicleCoffin::vftable+0x180 capture; native method still called.\r\n");
                append_log(&m);
            }
        }
    }
    if (g_nativeV167CoffinUpdate) g_nativeV167CoffinUpdate(vehicle);
}

static void __fastcall v167_edb900_scope(void* manager, double* referencePosition, UINT64 param3, char param4) {
    if (g_nativeV167Edb900) g_nativeV167Edb900(manager, referencePosition, param3, param4);

    if (!g_enableOutsideNetworkMount || !g_nativeCoffinCacheAdmit || !g_executable ||
        !g_nativeV169ComponentLookup || !manager || !referencePosition ||
        __atomic_load_n(&g_v151QpidHits, __ATOMIC_ACQUIRE) <= 0) return;

    BYTE* root = (BYTE*)manager;
    BYTE* cache = root + 0x228u;
    if (!readable_range(cache, 0xC8u) ||
        *(void**)cache != (void*)((BYTE*)g_executable + RVA_COFFIN_BAGGAGE_INFO_CACHE_VTABLE) ||
        *(UINT32*)(cache + 0xA8u) != 0u ||
        !readable_range(referencePosition, sizeof(double) * 3u)) return;

    BYTE* vehicle = (BYTE*)__atomic_load_n(&g_v167CoffinVehicle, __ATOMIC_ACQUIRE);
    UINT64 qpidHits = (UINT64)__atomic_load_n(&g_v151QpidHits, __ATOMIC_ACQUIRE);
    void* component = 0;
    BYTE* candidate = 0;
    UINT64 candidateVtable = 0;
    UINT64 candidateQ08 = 0;
    UINT32 stage = 0;

    if (!vehicle || !v161_is_live_coffin_vehicle(vehicle)) {
        stage = 1; // captured entity unavailable/invalid
    } else if (!readable_range(vehicle + 0xE8u, sizeof(double) * 8u) ||
               !readable_range(vehicle + 0xA0u, 0x20u)) {
        stage = 2; // live transform/component container unreadable
    } else {
        component = g_nativeV169ComponentLookup(vehicle + 0xA0u,
                    (const short*)((BYTE*)g_executable + RVA_V169_BAGGAGE_COMPONENT_TYPE));
        if (!component || !readable_range(component, 0x60u)) {
            stage = 3; // native DS2 component lookup returned nothing
        } else {
            candidate = (BYTE*)component + 0x58u;
            if (readable_range(candidate, sizeof(void*))) candidateVtable = (UINT64)*(void**)candidate;
            if (readable_range(candidate + 0x08u, sizeof(UINT64))) candidateQ08 = *(UINT64*)(candidate + 0x08u);
            if (!v159_is_genuine_coffin_candidate_early(candidate)) stage = 4;
        }
    }

    if (stage != 0) {
        LONG idx = __atomic_fetch_add(&g_v169ComponentLookupLogs, 1, __ATOMIC_ACQ_REL);
        if (idx < 32) {
            LogBuffer m; log_init(&m); log_prefix(&m);
            log_text(&m, "V169 DIRECT LOOKUP BLOCK #"); log_uint(&m, (UINT64)idx);
            log_text(&m, ": stage="); log_uint(&m, stage);
            log_text(&m, ", vehicle="); log_uint(&m, (UINT64)vehicle);
            log_text(&m, ", component="); log_uint(&m, (UINT64)component);
            log_text(&m, ", candidate="); log_uint(&m, (UINT64)candidate);
            log_text(&m, ", candidateVtableRva=");
            log_uint(&m, candidateVtable >= (UINT64)g_executable ? candidateVtable - (UINT64)g_executable : candidateVtable);
            log_text(&m, ", q08="); log_uint(&m, candidateQ08);
            log_text(&m, ", qpidHits="); log_uint(&m, qpidHits);
            log_text(&m, ". Native 11FFA0(vehicle+0xA0, DAT_14436C9F0) lookup; no manual collection parsing.\r\n");
            append_log(&m);
        }
        return;
    }

    // EDB900's native cache path copies the accepted physics-entry transform
    // (entry+0x40..+0x7F) and passes EDB900 param_2 as the reference position.
    // DSVehicleCoffin owns the same live 64-byte world transform at +0xE8.
    double transform[8] = {};
    memcpy(transform, vehicle + 0xE8u, sizeof(transform));

    UINT32 before = *(UINT32*)(cache + 0xA8u);
    BYTE result = g_nativeCoffinCacheAdmit(cache, candidate, transform, referencePosition);
    UINT32 after = readable_range(cache + 0xA8u, sizeof(UINT32)) ? *(UINT32*)(cache + 0xA8u) : 0u;
    if (result != 0u && after > 0u) {
        __atomic_fetch_add(&g_v167DirectAdmitSuccesses, 1, __ATOMIC_ACQ_REL);
        // Arm the existing frame-stability backup immediately with real vectors.
        v164_capture_native_coffin_candidate(cache, candidate, transform, referencePosition, result, after);
    }

    LONG idx = __atomic_fetch_add(&g_v167DirectAdmitLogs, 1, __ATOMIC_ACQ_REL);
    if (idx < 96 || (result != 0u && after > 0u)) {
        LogBuffer m; log_init(&m); log_prefix(&m);
        log_text(&m, "V169 DIRECT EDB900 ADMIT #"); log_uint(&m, (UINT64)idx);
        log_text(&m, ": vehicle="); log_uint(&m, (UINT64)vehicle);
        log_text(&m, ", component="); log_uint(&m, (UINT64)component);
        log_text(&m, ", candidate="); log_uint(&m, (UINT64)candidate);
        log_text(&m, ", cache="); log_uint(&m, (UINT64)cache);
        log_text(&m, ", count="); log_uint(&m, before); log_text(&m, "->"); log_uint(&m, after);
        log_text(&m, ", nativeResult="); log_uint(&m, result);
        log_text(&m, ", qpidHits="); log_uint(&m, qpidHits);
        log_text(&m, ". Native 11FFA0 component + real Coffin transform + live EDB900 reference.\r\n");
        append_log(&m);
    }
}

static void v161_restore_direct_d4_gate_if_safe() {
    BYTE* candidate = (BYTE*)__atomic_load_n(&g_v161DirectGateCandidate, __ATOMIC_ACQUIRE);
    if (!candidate) return;
    if (!v159_is_genuine_coffin_candidate_early(candidate) ||
        !readable_range(candidate + 0xD4u, sizeof(UINT32))) {
        __atomic_store_n(&g_v161DirectGateCandidate, 0ull, __ATOMIC_RELEASE);
        return;
    }
    UINT32 original = __atomic_load_n(&g_v161DirectGateOriginalD4, __ATOMIC_ACQUIRE);
    UINT32 current = *(UINT32*)(candidate + 0xD4u);
    UINT32 restored = (current & ~COFFIN_BAGGAGE_D4_MOUNT_BIT) |
                      (original & COFFIN_BAGGAGE_D4_MOUNT_BIT);
    *(UINT32*)(candidate + 0xD4u) = restored;
    LONG idx = __atomic_fetch_add(&g_v161DirectGateLogs, 1, __ATOMIC_ACQ_REL);
    if (idx < 32) {
        LogBuffer m; log_init(&m); log_prefix(&m);
        log_text(&m, "V161 DIRECT D4 RESTORE #"); log_uint(&m, (UINT64)idx);
        log_text(&m, ": candidate="); log_uint(&m, (UINT64)candidate);
        log_text(&m, ", d4="); log_uint(&m, current); log_text(&m, " -> "); log_uint(&m, restored);
        log_text(&m, ". Native candidate recovery observed.\r\n"); append_log(&m);
    }
    __atomic_store_n(&g_v161DirectGateCandidate, 0ull, __ATOMIC_RELEASE);
}

static bool v161_direct_cache_reinsert() {
    if (!g_enableOutsideNetworkMount || !g_nativeCoffinCacheAdmit || !g_executable ||
        __atomic_load_n(&g_v151QpidHits, __ATOMIC_ACQUIRE) == 0) return false;
    DWORD tid = GetCurrentThreadId();
    if (__atomic_load_n(&g_v161LatestBaggageCacheThread, __ATOMIC_ACQUIRE) != tid) return false;
    BYTE* cache = (BYTE*)__atomic_load_n(&g_v161LatestBaggageCache, __ATOMIC_ACQUIRE);
    BYTE* vehicle = (BYTE*)__atomic_load_n(&g_v161RideCoffinVehicle, __ATOMIC_ACQUIRE);
    if (!cache || !vehicle || !v161_is_live_coffin_vehicle(vehicle) ||
        !readable_range(cache, sizeof(void*)) ||
        *(void**)cache != (void*)((BYTE*)g_executable + RVA_COFFIN_BAGGAGE_INFO_CACHE_VTABLE) ||
        !readable_range(cache + 0xA8u, sizeof(UINT32)) || *(UINT32*)(cache + 0xA8u) != 0u) return false;

    BYTE* candidate = v161_resolve_candidate_from_vehicle(vehicle);
    if (!candidate || !readable_range(candidate + 0xD4u, sizeof(UINT32))) return false;

    LONG cidx = __atomic_fetch_add(&g_v161DirectCandidateLogs, 1, __ATOMIC_ACQ_REL);
    if (cidx < 16) {
        LogBuffer m; log_init(&m); log_prefix(&m);
        log_text(&m, "V161 DIRECT CANDIDATE #"); log_uint(&m, (UINT64)cidx);
        log_text(&m, ": vehicle="); log_uint(&m, (UINT64)vehicle);
        log_text(&m, ", candidate="); log_uint(&m, (UINT64)candidate);
        log_text(&m, ", d4="); log_uint(&m, *(UINT32*)(candidate + 0xD4u));
        log_text(&m, ". Engine-owned DSBaggageComponent+0x58; no fabricated object.\r\n"); append_log(&m);
    }

    BYTE* held = (BYTE*)__atomic_load_n(&g_v161DirectGateCandidate, __ATOMIC_ACQUIRE);
    if (held && held != candidate) v161_restore_direct_d4_gate_if_safe();
    UINT32 currentD4 = *(UINT32*)(candidate + 0xD4u);
    bool openedNow = false;
    if (__atomic_load_n(&g_v161DirectGateCandidate, __ATOMIC_ACQUIRE) != (UINT64)candidate) {
        __atomic_store_n(&g_v161DirectGateOriginalD4, currentD4, __ATOMIC_RELEASE);
        __atomic_store_n(&g_v161DirectGateCandidate, (UINT64)candidate, __ATOMIC_RELEASE);
    }
    if ((currentD4 & COFFIN_BAGGAGE_D4_MOUNT_BIT) == 0u) {
        *(UINT32*)(candidate + 0xD4u) = currentD4 | COFFIN_BAGGAGE_D4_MOUNT_BIT;
        openedNow = true;
        LONG gidx = __atomic_fetch_add(&g_v161DirectGateLogs, 1, __ATOMIC_ACQ_REL);
        if (gidx < 32) {
            LogBuffer m; log_init(&m); log_prefix(&m);
            log_text(&m, "V161 DIRECT D4 OPEN #"); log_uint(&m, (UINT64)gidx);
            log_text(&m, ": candidate="); log_uint(&m, (UINT64)candidate);
            log_text(&m, ", d4="); log_uint(&m, currentD4); log_text(&m, " -> ");
            log_uint(&m, *(UINT32*)(candidate + 0xD4u));
            log_text(&m, ". ED7920 may derive manager bit1 natively.\r\n"); append_log(&m);
        }
    }

    double dummyA[8] = {0,0,0,0,0,0,0,0};
    double dummyB[8] = {0,0,0,0,0,0,0,0};
    BYTE admitted = g_nativeCoffinCacheAdmit(cache, candidate, dummyA, dummyB);
    UINT32 after = readable_range(cache + 0xA8u, sizeof(UINT32)) ? *(UINT32*)(cache + 0xA8u) : 0u;
    LONG ridx = __atomic_fetch_add(&g_v161DirectReinsertLogs, 1, __ATOMIC_ACQ_REL);
    if (ridx < 64) {
        LogBuffer m; log_init(&m); log_prefix(&m);
        log_text(&m, "V161 DIRECT CACHE REINSERT #"); log_uint(&m, (UINT64)ridx);
        log_text(&m, ": cache="); log_uint(&m, (UINT64)cache);
        log_text(&m, ", vehicle="); log_uint(&m, (UINT64)vehicle);
        log_text(&m, ", candidate="); log_uint(&m, (UINT64)candidate);
        log_text(&m, ", nativeAdmit="); log_uint(&m, admitted);
        log_text(&m, ", countAfter="); log_uint(&m, after);
        log_text(&m, ". Direct object-path recovery after native/relaxed discovery exhausted.\r\n"); append_log(&m);
    }
    if (!admitted || after == 0u) {
        if (openedNow && v159_is_genuine_coffin_candidate_early(candidate))
            *(UINT32*)(candidate + 0xD4u) = currentD4;
        if (openedNow) __atomic_store_n(&g_v161DirectGateCandidate, 0ull, __ATOMIC_RELEASE);
        return false;
    }
    __atomic_store_n(&g_nativeBaggageCandidate, (UINT64)candidate, __ATOMIC_RELEASE);
    __atomic_store_n(&g_nativeBaggageCache, (UINT64)cache, __ATOMIC_RELEASE);
    return true;
}

static UINT32 __fastcall v157_baggage_query_scope(UINT64 p1, void* output, double* center,
                                                  float radius, float angle, UINT32 flags) {
    // v1.60 keeps EDB900 fully native first. If the proven boundary root has
    // fired and the native query is empty, retry the same engine function with
    // all three native bypasses needed by the outside Coffin state:
    //   0x2      = allow entries carrying +0x98 owner,
    //   angle pi = no directional-cone rejection,
    //   0x10000  = native bypass of the final FUN_142451100 collision/LOS test.
    // If even that engine-native retry is empty, recover only a real broadphase
    // entry whose handle resolves through original 1411D6C10 to the exact Coffin
    // DSBaggageInfo object; EDB900 then continues its normal pipeline itself.
    v159_restore_candidate_gate_if_inside_or_invalid();

    UINT32 result = g_nativeV157BaggageQuery ?
        g_nativeV157BaggageQuery(p1, output, center, radius, angle, flags) : 0u;
    UINT32 nativeCount = v159_query_result_count(output);
    UINT32 finalCount = nativeCount;

    if (g_nativeV157BaggageQuery && g_enableOutsideNetworkMount &&
        __atomic_load_n(&g_v151QpidHits, __ATOMIC_ACQUIRE) != 0 &&
        nativeCount == 0u) {
        const float fullAngle = 3.14159265358979323846f;
        UINT32 fallbackFlags = flags | 0x10002u;
        result = g_nativeV157BaggageQuery(p1, output, center, radius, fullAngle, fallbackFlags);
        finalCount = v159_query_result_count(output);

        LONG idx = __atomic_fetch_add(&g_v159QueryFallbackLogs, 1, __ATOMIC_ACQ_REL);
        if (idx < 48) {
            LogBuffer m; log_init(&m); log_prefix(&m);
            log_text(&m, "V160 BAGGAGE NATIVE RELAX #"); log_uint(&m, (UINT64)idx);
            log_text(&m, ": nativeCount="); log_uint(&m, nativeCount);
            log_text(&m, ", retryCount="); log_uint(&m, finalCount);
            log_text(&m, ", flags="); log_uint(&m, flags);
            log_text(&m, " -> "); log_uint(&m, fallbackFlags);
            log_text(&m, ", ownerBypass=1, angleFull=1, losBypass=1, radius="); log_float2(&m, radius);
            log_text(&m, ". Same native 1411DB210.\r\n");
            append_log(&m);
        }

        if (finalCount == 0u && output &&
            readable_range(output, sizeof(void*)) &&
            readable_range((BYTE*)output + 0x12000u, sizeof(UINT32))) {
            UINT64 recoveredCandidate = 0u;
            double recoveredDist2 = 0.0;
            BYTE* recoveredEntry = v160_find_genuine_outside_coffin_entry(
                center, radius, &recoveredCandidate, &recoveredDist2);
            if (recoveredEntry) {
                *(void**)output = recoveredEntry;
                *(UINT32*)((BYTE*)output + 0x12000u) = 1u;
                finalCount = 1u;
                result = 1u;

                LONG midx = __atomic_fetch_add(&g_v160ManualRecoveryLogs, 1, __ATOMIC_ACQ_REL);
                if (midx < 48) {
                    UINT64 owner98 = v159_entry_owner98(recoveredEntry);
                    UINT64 handle = readable_range(recoveredEntry + 0x20u, sizeof(UINT64))
                        ? *(UINT64*)(recoveredEntry + 0x20u) : 0u;
                    LogBuffer m; log_init(&m); log_prefix(&m);
                    log_text(&m, "V160 GENUINE COFFIN RECOVERY #"); log_uint(&m, (UINT64)midx);
                    log_text(&m, ": entry="); log_uint(&m, (UINT64)recoveredEntry);
                    log_text(&m, ", resolvedCandidate="); log_uint(&m, recoveredCandidate);
                    log_text(&m, ", handle20="); log_uint(&m, handle);
                    log_text(&m, ", owner98="); log_uint(&m, owner98);
                    log_text(&m, ", dist2="); log_float2(&m, (float)recoveredDist2);
                    log_text(&m, ". Real broadphase entry + original 1411D6C10 proof; EDB900 remains native downstream.\r\n");
                    append_log(&m);
                }
            }
        }
        // v1.61 final fallback: the outside network can remove the Coffin from
        // this physics broadphase entirely.  Do not fabricate a query entry.
        // Instead, reinsert the exact engine-owned DSBaggageInfo derived from
        // the currently ridden DSVehicleCoffin into the native two-slot cache.
        if (finalCount == 0u) v161_direct_cache_reinsert();
    }

    if (!output || !readable_range((BYTE*)output + 0x12000u, sizeof(UINT32))) return result;

    UINT32 count = *(UINT32*)((BYTE*)output + 0x12000u);
    UINT32 safeCount = count > 512u ? 512u : count;
    DWORD tid = GetCurrentThreadId();
    g_v157LastQueryThread = tid;
    g_v157LastQueryCount = count;
    g_v157LastStoredCount = 0u;
    g_v157LastOutput = (UINT64)output;
    if (center && readable_range(center, 3u * sizeof(double))) {
        g_v157LastCenter[0] = center[0]; g_v157LastCenter[1] = center[1]; g_v157LastCenter[2] = center[2];
    }
    g_v157LastRadius = radius;

    if (safeCount > 0u && readable_range(output, (SIZE_T)safeCount * sizeof(void*))) {
        void** entries = (void**)output;
        for (UINT32 i = 0; i < safeCount; ++i) {
            BYTE* entry = (BYTE*)entries[i];
            UINT64 handle = 0;
            if (entry && readable_range(entry + 0x20u, sizeof(UINT64))) handle = *(UINT64*)(entry + 0x20u);
            g_v157LastEntries[i] = (UINT64)entry;
            g_v157LastHandles[i] = handle;
        }
        g_v157LastStoredCount = safeCount;
    }
    __atomic_add_fetch(&g_v157QueryCalls, 1, __ATOMIC_RELAXED);
    if (__atomic_load_n(&g_v157CoffinQueryEntry, __ATOMIC_ACQUIRE) != 0u)
        v157_snapshot_cached_entry((BYTE*)output, count, false);
    return result;
}

static void* __fastcall v157_baggage_resolve_scope(void* registry, UINT64* handlePtr) {
    UINT64 handle = (handlePtr && readable_range(handlePtr, sizeof(UINT64))) ? *handlePtr : 0u;
    void* result = g_nativeV157BaggageResolve ? g_nativeV157BaggageResolve(registry, handlePtr) : 0;

    BYTE* cachedEntry = (BYTE*)__atomic_load_n(&g_v157CoffinQueryEntry, __ATOMIC_ACQUIRE);
    UINT64 cachedHandle = 0;
    if (cachedEntry && readable_range(cachedEntry + 0x20u, sizeof(UINT64)))
        cachedHandle = *(UINT64*)(cachedEntry + 0x20u);
    bool cachedHandleCall = cachedHandle != 0u && handle == cachedHandle;
    bool genuine = v157_is_genuine_coffin_candidate(result);
    BYTE* cachedCandidateBefore = (BYTE*)__atomic_load_n(&g_v157CoffinCandidate, __ATOMIC_ACQUIRE);

    BYTE* entry = 0;
    if (GetCurrentThreadId() == g_v157LastQueryThread) {
        UINT32 n = g_v157LastStoredCount;
        if (n > 512u) n = 512u;
        for (UINT32 i = 0; i < n; ++i) {
            if (g_v157LastHandles[i] == handle) { entry = (BYTE*)g_v157LastEntries[i]; break; }
        }
    }

    bool firstCapture = genuine && entry &&
                        ((BYTE*)__atomic_load_n(&g_v157CoffinQueryEntry, __ATOMIC_ACQUIRE) != entry ||
                         cachedCandidateBefore != (BYTE*)result);
    if (genuine && entry) {
        if (v159_entry_owner98(entry) == 0u) v161_restore_direct_d4_gate_if_safe();
        __atomic_store_n(&g_v157CoffinQueryEntry, (UINT64)entry, __ATOMIC_RELEASE);
        __atomic_store_n(&g_v157CoffinCandidate, (UINT64)result, __ATOMIC_RELEASE);
        __atomic_store_n(&g_v159ResolvedCoffinEntry, (UINT64)entry, __ATOMIC_RELEASE);
        __atomic_store_n(&g_v159ResolvedCoffinCandidate, (UINT64)result, __ATOMIC_RELEASE);
    }

    if (genuine || cachedHandleCall) {
        UINT64 resolverSig = 0x1571000ull;
        resolverSig = v157_hash(resolverSig, handle);
        resolverSig = v157_hash(resolverSig, (UINT64)result);
        resolverSig = v157_hash(resolverSig, (UINT64)(entry ? entry : cachedEntry));
        resolverSig = v157_hash(resolverSig, genuine ? 1u : 0u);
        UINT64 resolverOld = __atomic_exchange_n(&g_v157LastResolverSig, resolverSig, __ATOMIC_ACQ_REL);
        LONG idx = -1;
        if (resolverOld != resolverSig) idx = __atomic_fetch_add(&g_v157ResolverLogs, 1, __ATOMIC_ACQ_REL);
        if (idx >= 0 && idx < 48) {
            short world = -1; UINT32 ef = 0; float score = 0.0f; BYTE type = 0xFFu;
            UINT64 h = 0, o98 = 0; double x=0,y=0,z=0;
            BYTE* inspectEntry = entry ? entry : cachedEntry;
            v157_entry_fields(inspectEntry,&world,&ef,&score,&type,&h,&o98,&x,&y,&z);
            LogBuffer m; log_init(&m); log_prefix(&m);
            log_text(&m, "V157 COFFIN RESOLVER #"); log_uint(&m, (UINT64)idx);
            log_text(&m, ": handle20="); log_uint(&m, handle);
            log_text(&m, ", result="); log_uint(&m, (UINT64)result);
            log_text(&m, ", genuineCoffin="); log_uint(&m, genuine ? 1u : 0u);
            log_text(&m, ", entry="); log_uint(&m, (UINT64)inspectEntry);
            log_text(&m, ", entryWorld="); log_uint(&m, (UINT64)(UINT32)(unsigned short)world);
            log_text(&m, ", flagsEC="); log_uint(&m, ef);
            log_text(&m, ", score88="); log_float2(&m, score);
            log_text(&m, ", type49="); log_uint(&m, type);
            log_text(&m, ". Native resolver result preserved.\r\n"); append_log(&m);
        }
    }

    if (firstCapture) {
        BYTE* lastOutput = (BYTE*)__atomic_load_n(&g_v157LastOutput, __ATOMIC_ACQUIRE);
        v157_snapshot_cached_entry(lastOutput, g_v157LastQueryCount, true);
    }
    return result;
}

static bool install_v157_baggage_root_trace(HMODULE executable) {
    if (!g_enableOutsideNetworkMount) return true;
    BYTE* base = (BYTE*)executable;
    BYTE* queryCall = base + RVA_V157_BAGGAGE_QUERY_CALL;
    BYTE* queryNative = base + RVA_V157_BAGGAGE_QUERY_NATIVE;
    BYTE* resolveCall = base + RVA_V157_BAGGAGE_RESOLVE_CALL;
    BYTE* resolveNative = base + RVA_V157_BAGGAGE_RESOLVE_NATIVE;
    int queryDisp = 0, resolveDisp = 0;
    if (!v151_validate_call(queryCall, queryNative, &queryDisp) ||
        !v151_validate_call(resolveCall, resolveNative, &resolveDisp)) return false;

    g_nativeV157BaggageQuery = (V157BaggageQueryFn)queryNative;
    g_nativeV157BaggageResolve = (V157BaggageResolveFn)resolveNative;
    BYTE* queryRelay = (BYTE*)allocate_call_relay_near(queryCall, (void*)&v157_baggage_query_scope);
    BYTE* resolveRelay = (BYTE*)allocate_call_relay_near(resolveCall, (void*)&v157_baggage_resolve_scope);
    if (!queryRelay || !resolveRelay) return false;
    if (!v151_patch_call(queryCall, queryRelay, queryDisp)) return false;
    g_v157QueryTraceInstalled = 1;
    if (!v151_patch_call(resolveCall, resolveRelay, resolveDisp)) return false;
    g_v157ResolverTraceInstalled = 1;
    return true;
}

static float v151_read_raw_action_value(int action, int channel, bool* ok) {
    if (ok) *ok = false;
    if (!g_executable || action < 0 || channel < 0 || channel > 5) return 0.0f;
    BYTE* registry = (BYTE*)g_executable + RVA_V151_ACTION_REGISTRY;
    BYTE** slot = (BYTE**)(registry + (SIZE_T)(UINT32)action * sizeof(void*));
    if (!readable_range(slot, sizeof(void*))) return 0.0f;
    BYTE* registration = *slot;
    if (!registration || !readable_range(registration + 0x10u, sizeof(void*))) return 0.0f;
    BYTE* state = *(BYTE**)(registration + 0x10u);
    if (!state) return 0.0f;
    BYTE* value = state + 0x1F8u + (SIZE_T)(UINT32)channel * sizeof(float);
    if (!readable_range(value, sizeof(float))) return 0.0f;
    float result = *(float*)value;
    if (ok) *ok = true;
    return result;
}

static bool v151_current_outside_coffin_candidate(void** managerOut, void** selectedOut,
                                                   UINT32* d4Out, UINT32* flagsOut) {
    UINT64 cachedAction = __atomic_load_n(&g_v151ValidatedCoffinAction, __ATOMIC_ACQUIRE);
    UINT64 cachedManager = __atomic_load_n(&g_v151ValidatedCoffinManager, __ATOMIC_ACQUIRE);
    UINT64 cachedCandidate = __atomic_load_n(&g_v151ValidatedCoffinCandidate, __ATOMIC_ACQUIRE);
    if (!cachedAction || !cachedManager || !cachedCandidate) return false;

    BYTE* action = (BYTE*)cachedAction;
    BYTE* manager = (BYTE*)cachedManager;
    BYTE* selected = (BYTE*)cachedCandidate;
    if (!readable_range(action + OFF_COFFIN_ACTION_MANAGER, sizeof(void*)) ||
        *(void**)(action + OFF_COFFIN_ACTION_MANAGER) != manager ||
        !readable_range(manager + 0x230u, sizeof(void*)) ||
        !readable_range(manager + 0x2F0u, sizeof(UINT32)) ||
        *(void**)(manager + 0x230u) != selected ||
        !readable_range(selected, sizeof(void*)) ||
        !readable_range(selected + 0x08u, sizeof(UINT64)) ||
        !readable_range(selected + 0xD4u, sizeof(UINT32))) return false;

    if (*(void**)selected != (void*)((BYTE*)g_executable + RVA_COFFIN_BAGGAGE_INFO_VTABLE) ||
        *(UINT64*)(selected + 0x08u) != COFFIN_BAGGAGE_Q08_SIGNATURE) return false;

    // Use the native pre-open gate words captured by the successful 1000BD0
    // scope. Reading the live words here could race the very short interval in
    // which that wrapper has temporarily ORed them for the native producer.
    UINT32 d4 = __atomic_load_n(&g_v151ValidatedNativeD4, __ATOMIC_ACQUIRE);
    UINT32 flags = __atomic_load_n(&g_v151ValidatedNativeManagerFlags, __ATOMIC_ACQUIRE);
    // This correction is outside-network only. Inside, leave the native rule
    // system entirely untouched even if a physical E3 press exists.
    if ((d4 & COFFIN_BAGGAGE_D4_MOUNT_BIT) != 0u || (flags & 0x2u) != 0u) return false;

    if (managerOut) *managerOut = manager;
    if (selectedOut) *selectedOut = selected;
    if (d4Out) *d4Out = d4;
    if (flagsOut) *flagsOut = flags;
    return true;
}

static void __fastcall coffin_v151_qpid_scope(void* physics, UINT32 queryArg) {
    if (!g_nativeV151Qpid) return;
    g_nativeV151Qpid(physics, queryArg);
    if (!g_enableNetworkTraversal || !physics) return;

    BYTE* p = (BYTE*)physics;
    if (!readable_range(p + 0x514u, sizeof(UINT32)) ||
        !readable_range(p + 0x624u, sizeof(float)) ||
        !readable_range(p + 0x62Du, sizeof(BYTE))) return;

    UINT32 limitBits = *(UINT32*)(p + 0x514u);
    if (limitBits != 0x40B1C71Cu) return; // exact F43700 wall-hit branch

    float old514 = *(float*)(p + 0x514u);
    float old624 = *(float*)(p + 0x624u);
    BYTE old62D = *(BYTE*)(p + 0x62Du);
    *(float*)(p + 0x514u) = -1.0f;
    *(float*)(p + 0x624u) = -1.0f;
    *(BYTE*)(p + 0x62Du) = 0u;

    LONG index = __atomic_fetch_add(&g_v151QpidHits, 1, __ATOMIC_ACQ_REL);
    if (index < 24) {
        LogBuffer m; log_init(&m); log_prefix(&m);
        log_text(&m, "V151 QPID ROOT #"); log_uint(&m, (UINT64)index);
        log_text(&m, ": physics="); log_uint(&m, (UINT64)physics);
        log_text(&m, ", +514="); log_float2(&m, old514);
        log_text(&m, ", +624="); log_float2(&m, old624);
        log_text(&m, ", +62D="); log_uint(&m, old62D);
        log_text(&m, " -> -1,-1,0. +62E and wall geometry remain native.\r\n");
        append_log(&m);
    }
}

static void __fastcall coffin_v151_rule_merge_scope(void* stateRoot) {
    if (g_nativeV151RuleMerge) g_nativeV151RuleMerge(stateRoot);
    if (!g_enableOutsideNetworkMount || !stateRoot) return;

    bool rawReadable = false;
    float raw = v151_read_raw_action_value((int)COFFIN_NETWORK_E3_REQUEST, 2, &rawReadable);
    if (!rawReadable || !(raw > 0.0f) || raw > 1000.0f) return;

    void* manager = 0;
    void* selected = 0;
    UINT32 d4 = 0, flags = 0;
    bool candidate = v151_current_outside_coffin_candidate(&manager, &selected, &d4, &flags);

    BYTE* root = (BYTE*)stateRoot;
    if (!readable_range(root + 0x67FCu, sizeof(UINT32)) ||
        !readable_range(root + 0x6E94u, sizeof(UINT32))) return;
    UINT32 beforeBase = *(UINT32*)(root + 0x67FCu);
    UINT32 block = *(UINT32*)(root + 0x6E94u);
    bool baseAlready = (beforeBase & 0x8u) != 0u;
    bool blocked = (block & 0x8u) != 0u;
    bool injected = false;

    if (candidate && !baseAlready && !blocked) {
        *(UINT32*)(root + 0x67FCu) = beforeBase | 0x8u;
        injected = true;
        __atomic_fetch_add(&g_v151E3BaseInjections, 1, __ATOMIC_ACQ_REL);
    }

    LONG index = __atomic_fetch_add(&g_v151E3PressLogs, 1, __ATOMIC_ACQ_REL);
    if (index < 48) {
        LogBuffer m; log_init(&m); log_prefix(&m);
        log_text(&m, "V151 E3 PHYSICAL #"); log_uint(&m, (UINT64)index);
        log_text(&m, ": rawChannel2="); log_float2(&m, raw);
        log_text(&m, ", candidate="); log_uint(&m, candidate ? 1u : 0u);
        log_text(&m, ", selected="); log_uint(&m, (UINT64)selected);
        log_text(&m, ", D4="); log_uint(&m, d4);
        log_text(&m, ", manager2F0="); log_uint(&m, flags);
        log_text(&m, ", baseBefore="); log_uint(&m, (beforeBase >> 3) & 1u);
        log_text(&m, ", block="); log_uint(&m, (block >> 3) & 1u);
        log_text(&m, ", injected="); log_uint(&m, injected ? 1u : 0u);
        log_text(&m, ". F68750/F689B0 remain native.\r\n");
        append_log(&m);
    }
}

static UINT32 v151_read_u32(BYTE* p, UINT32 fallback) {
    return p && readable_range(p, sizeof(UINT32)) ? *(UINT32*)p : fallback;
}
static BYTE v151_read_u8(BYTE* p, BYTE fallback) {
    return p && readable_range(p, sizeof(BYTE)) ? *p : fallback;
}

static UINT64 __fastcall coffin_v151_edfe10_passive(void* manager, float deltaSeconds,
                                                     UINT32 request, void* stateEntry) {
    UINT64 nativeResult = g_nativeV151Edfe10
        ? g_nativeV151Edfe10(manager, deltaSeconds, request, stateEntry) : 0ull;
    if (request != COFFIN_NETWORK_E3_REQUEST || !manager || !stateEntry) return nativeResult;

    BYTE* stateRoot = 0;
    if (readable_range((BYTE*)manager + 0x25068u, sizeof(void*)))
        stateRoot = *(BYTE**)((BYTE*)manager + 0x25068u);

    UINT32 baseWord = 0u, blockWord = 0u, rawBWord = 0u, risingAWord = 0u, suppressWord = 0u;
    if (stateRoot) {
        baseWord = v151_read_u32(stateRoot + 0x67FCu, 0u);
        blockWord = v151_read_u32(stateRoot + 0x6E94u, 0u);
        rawBWord = v151_read_u32(stateRoot + 0x0290u, 0u);
        risingAWord = v151_read_u32(stateRoot + 0x02C0u, 0u);
        suppressWord = v151_read_u32(stateRoot + 0x7314u, 0u);
    }
    BYTE* record = (BYTE*)manager + 0x48870u + (SIZE_T)request * 0x4D0u;
    UINT32 descriptor = v151_read_u32(record + 0x08u, 0xffffffffu);
    BYTE state12 = v151_read_u8((BYTE*)stateEntry + 0x12u, 0xffu);
    BYTE active = v151_read_u8((BYTE*)stateEntry + 0xA8u, 0xffu);
    BYTE reject = v151_read_u8((BYTE*)stateEntry + 0x16u, 0xffu);

    UINT64 sig = (nativeResult & 0xFFu)
        | ((UINT64)(descriptor & 0xFFu) << 8)
        | ((UINT64)(state12 & 0x3u) << 16)
        | ((UINT64)(reject & 1u) << 18)
        | ((UINT64)(active & 1u) << 19)
        | ((UINT64)((baseWord >> 3) & 1u) << 20)
        | ((UINT64)((blockWord >> 3) & 1u) << 21)
        | ((UINT64)((rawBWord >> 3) & 1u) << 22)
        | ((UINT64)((risingAWord >> 3) & 1u) << 23)
        | ((UINT64)((suppressWord >> 3) & 1u) << 24);
    UINT64 previous = __atomic_exchange_n(&g_v151EdfeLastSignature, sig, __ATOMIC_ACQ_REL);
    if (previous != sig) {
        LONG index = __atomic_fetch_add(&g_v151EdfeChangeLogs, 1, __ATOMIC_ACQ_REL);
        if (index < 128) {
            LogBuffer m; log_init(&m); log_prefix(&m);
            log_text(&m, "V153 EDFE10 CHANGE #"); log_uint(&m, (UINT64)index);
            log_text(&m, ": result="); log_uint(&m, nativeResult & 0xFFu);
            log_text(&m, ", descriptor="); log_uint(&m, descriptor);
            log_text(&m, ", state12="); log_uint(&m, state12);
            log_text(&m, ", reject="); log_uint(&m, reject);
            log_text(&m, ", active="); log_uint(&m, active);
            log_text(&m, ", E3[base,block,rawB,risingA,suppress]=");
            log_uint(&m, (baseWord >> 3) & 1u); log_char(&m, ',');
            log_uint(&m, (blockWord >> 3) & 1u); log_char(&m, ',');
            log_uint(&m, (rawBWord >> 3) & 1u); log_char(&m, ',');
            log_uint(&m, (risingAWord >> 3) & 1u); log_char(&m, ',');
            log_uint(&m, (suppressWord >> 3) & 1u);
            log_text(&m, ". Read-only.\r\n");
            append_log(&m);
        }
    }
    return nativeResult;
}

static bool install_v151_qpid_scope(HMODULE executable) {
    if (!g_enableNetworkTraversal) return true;
    BYTE* base = (BYTE*)executable;
    BYTE* call = base + RVA_V151_QPID_LIMITER_CALL;
    BYTE* nativeTarget = base + RVA_V151_QPID_LIMITER_NATIVE;
    int oldDisp = 0;
    if (!v151_validate_call(call, nativeTarget, &oldDisp)) return false;
    g_nativeV151Qpid = (V151QpidFn)nativeTarget;
    BYTE* relay = (BYTE*)allocate_call_relay_near(call, (void*)&coffin_v151_qpid_scope);
    if (!relay || !v151_patch_call(call, relay, oldDisp)) return false;
    g_v151QpidInstalled = 1;
    return true;
}

static bool install_v151_rule_merge_scope(HMODULE executable) {
    if (!g_enableOutsideNetworkMount) return true;
    BYTE* base = (BYTE*)executable;
    BYTE* call = base + RVA_V151_RULE_MERGE_CALL;
    BYTE* nativeTarget = base + RVA_V151_RULE_MERGE_NATIVE;
    int oldDisp = 0;
    if (!v151_validate_call(call, nativeTarget, &oldDisp)) return false;
    g_nativeV151RuleMerge = (V151RuleMergeFn)nativeTarget;
    BYTE* relay = (BYTE*)allocate_call_relay_near(call, (void*)&coffin_v151_rule_merge_scope);
    if (!relay || !v151_patch_call(call, relay, oldDisp)) return false;
    g_v151RuleMergeInstalled = 1;
    return true;
}

static bool install_v151_edfe10_passive(HMODULE executable) {
    if (!g_enableOutsideNetworkMount) return true;
    BYTE* base = (BYTE*)executable;
    BYTE* call = base + RVA_V151_EDFE10_CALL;
    BYTE* nativeTarget = base + RVA_V151_EDFE10_NATIVE;
    int oldDisp = 0;
    if (!v151_validate_call(call, nativeTarget, &oldDisp)) return false;
    g_nativeV151Edfe10 = (V151Edfe10Fn)nativeTarget;
    BYTE* relay = (BYTE*)allocate_call_relay_near(call, (void*)&coffin_v151_edfe10_passive);
    if (!relay || !v151_patch_call(call, relay, oldDisp)) return false;
    g_v151EdfePassiveInstalled = 1;
    return true;
}


// -----------------------------------------------------------------------------
// v1.52.0 / V17-closed roots.
//
// Crossing:
//   V17 proves the native PhysicsCollisionIgnoreChiralWallFilter rejects exactly
//   collision layer (object+0x50)&0x7F == 0x16. The actual generic vehicle contact
//   path calls DSPhysicsCoffinInstance vfunc +0x10 (F480A0) with a contiguous
//   0x50-byte hit list before the native contact is committed. Filter only 0x16
//   from that Coffin-only list while an exact F43700 QPID wall hit is recent, then
//   let F480A0 and 24776E0 run normally. All non-Chiral-Wall contacts stay native.
//
// Legacy v1.52/v1.53 NxAction/contact constants remain below only so the historical
// research helpers still compile. They are not installed by the v1.54 worker.
// V23.2 proved the active E3 source belongs to the live AF2D00 profile rule instead.
static const UINT32 RVA_V152_NX_ACTIONS_SINGLETON = 0x0A1BB608u;
static const UINT32 RVA_V152_NX_ACTIONS_VTABLE = 0x03228BA0u;
static const UINT32 RVA_V152_NX_ACTION_LOOKUP = 0x00D3D380u;
static const UINT32 RVA_V152_CONTACT_SEMANTIC_HELPER = 0x01F47A90u;
static const SIZE_T V152_COFFIN_CONTACT_SLOT = 0x10u;
static const SIZE_T V152_CONTACT_RECORD_SIZE = 0x50u;
static const UINT32 V152_CHIRAL_WALL_LAYER = 0x16u;

// v1.54.0 roots closed by V22 + live V23.2 profile capture.
// DSVehicleCoffin::1F76D80 performs a second QPID query on the desired
// Coffin position and enters the transform/recovery block when that query
// succeeds.  Keep the query itself native but force its post-query branch to
// the native no-recovery path.
static const UINT32 RVA_V154_ENTITY_SECOND_QUERY_CMP = 0x01F76E80u;
static const UINT32 RVA_V154_ENTITY_SECOND_QUERY_BRANCH = 0x01F76E83u;
static const UINT32 RVA_V154_ENTITY_SECOND_QUERY_SKIP = 0x01F77169u;
// AF2D00 Type-3 source mapping table.  V23.2 captured the live normal E3 rule
// as source 0x75; this table maps 0x75 -> state slot 6.  The code below reads
// the current live E3 Type-3 rule rather than hardcoding only that source.
static const UINT32 RVA_V154_PROFILE_SOURCE_MAP = 0x033E787Cu;
static const UINT32 V154_E3_ACTION = 0xE3u;
static const SIZE_T OFF_V154_PRIMARY_RULE_CONTEXT = 0x4EA0u;
static const SIZE_T OFF_V154_SECONDARY_RULE_CONTEXT = 0x5B40u;
static const SIZE_T OFF_V154_E3_BASE_WORD = 0x67FCu;
static const SIZE_T OFF_V154_E3_BLOCK_WORD = 0x6E94u;

static volatile UINT64 g_v152RecentQpidPhysics = 0;
static volatile UINT64 g_v152UseCoffinAction = 0;
static volatile UINT64 g_v152NxActionsImpl = 0;
static volatile UINT64 g_v152InputManager = 0;
static volatile LONG g_v152InputResolveLogs = 0;
static volatile LONG g_v152PhysicalPressLogs = 0;
static volatile LONG g_v152BaseInjectionLogs = 0;
static volatile LONG g_v152ContactFilterLogs = 0;
static volatile LONG g_v152ContactRemoved = 0;
static int g_v152QpidInstalled = 0;
static int g_v152ContactFilterInstalled = 0;
static int g_v152RuleMergeInstalled = 0;
static int g_v154EntityBypassInstalled = 0;

typedef void (__fastcall* V152QpidFn)(void*, UINT32);
typedef void (__fastcall* V152RuleMergeFn)(void*);
typedef void (__fastcall* V152CoffinContactFn)(void*, void*, int*, void*, int);
typedef void* (__fastcall* V152ActionLookupFn)(void*, const char*);
typedef UINT32 (__fastcall* V152InputFlagsFn)(void*, int, int);
typedef float (__fastcall* V152InputValueFn)(void*, int, int);
static V152QpidFn g_nativeV152Qpid = 0;
static V152RuleMergeFn g_nativeV152RuleMerge = 0;
static V152CoffinContactFn g_nativeV152CoffinContact = 0;

struct V152InputSnapshot {
    int binding0;
    int binding1;
    int context;
    UINT32 flags0;
    UINT32 flags1;
    float value0;
    float value1;
};

static bool v152_ascii_equal(const char* value, const char* expected) {
    if (!value || !expected) return false;
    for (SIZE_T i = 0; i < 128u; ++i) {
        char a = value[i], b = expected[i];
        if (a != b) return false;
        if (a == '\0') return true;
    }
    return false;
}

static bool v152_pointer_in_game(const void* p) {
    UINT64 base = (UINT64)g_executable;
    UINT64 value = (UINT64)p;
    return base && value >= base && value < base + (UINT64)EXPECTED_IMAGE_SIZE;
}

static void* v152_resolve_use_coffin_action() {
    UINT64 cached = __atomic_load_n(&g_v152UseCoffinAction, __ATOMIC_ACQUIRE);
    if (cached) return (void*)cached;
    if (!g_executable) return 0;

    BYTE* base = (BYTE*)g_executable;
    void** singleton = (void**)(base + RVA_V152_NX_ACTIONS_SINGLETON);
    if (!readable_range(singleton, sizeof(void*))) return 0;
    BYTE* impl = (BYTE*)*singleton;
    if (!impl || !readable_range(impl, 0x18u) || *(void**)impl != (void*)(base + RVA_V152_NX_ACTIONS_VTABLE)) return 0;

    // Validate the lookup-only D3D380 prologue/hash seed before calling it.
    static const BYTE lookupPrefix[16] = {
        0x44,0x0F,0xB6,0x12,0x4C,0x8B,0xCA,0x4C,
        0x8B,0xD9,0x41,0xB8,0xEF,0xEF,0xCD,0xCD
    };
    BYTE* lookupAddress = base + RVA_V152_NX_ACTION_LOOKUP;
    if (!readable_range(lookupAddress, sizeof(lookupPrefix)) ||
        !bytes_equal(lookupAddress, lookupPrefix, sizeof(lookupPrefix))) return 0;
    V152ActionLookupFn lookup = (V152ActionLookupFn)lookupAddress;

    const char* registryKey = "Action_UseCoffinBoard";
    BYTE* action = (BYTE*)lookup(impl, registryKey);
    const char* matched = registryKey;
    if (action) {
        if (!readable_range(action + 0x3Cu, 0x80u) ||
            !v152_ascii_equal((const char*)(action + 0x3Cu), registryKey)) action = 0;
    }
    if (!action) return 0;

    BYTE* inputManager = *(BYTE**)(impl + 0x08u);
    if (!inputManager || !readable_range(inputManager, sizeof(void*))) return 0;
    void** inputVtable = *(void***)inputManager;
    if (!inputVtable || !readable_range((BYTE*)inputVtable + 0xE0u, sizeof(void*))) return 0;
    void* flagsFn = *(void**)((BYTE*)inputVtable + 0xD8u);
    void* valueFn = *(void**)((BYTE*)inputVtable + 0xE0u);
    if (!v152_pointer_in_game(flagsFn) || !v152_pointer_in_game(valueFn)) return 0;

    __atomic_store_n(&g_v152NxActionsImpl, (UINT64)impl, __ATOMIC_RELEASE);
    __atomic_store_n(&g_v152InputManager, (UINT64)inputManager, __ATOMIC_RELEASE);
    __atomic_store_n(&g_v152UseCoffinAction, (UINT64)action, __ATOMIC_RELEASE);

    LONG logIndex = __atomic_fetch_add(&g_v152InputResolveLogs, 1, __ATOMIC_ACQ_REL);
    if (logIndex < 4) {
        LogBuffer m; log_init(&m); log_prefix(&m);
        log_text(&m, "V153 INPUT RESOLVED: action="); log_uint(&m, (UINT64)action);
        log_text(&m, ", name="); log_text(&m, matched ? matched : "?");
        log_text(&m, ", binding0="); log_uint(&m, (UINT64)(UINT32)*(int*)(action + 0xCCu));
        log_text(&m, ", binding1="); log_uint(&m, (UINT64)(UINT32)*(int*)(action + 0xD0u));
        log_text(&m, ", context="); log_uint(&m, (UINT64)*(UINT32*)(action + 0xC8u));
        log_text(&m, ". Read-only native NxAction lookup.\r\n"); append_log(&m);
    }
    return action;
}

static bool v152_read_use_coffin_physical(V152InputSnapshot* snapshot) {
    if (snapshot) memset(snapshot, 0, sizeof(*snapshot));
    BYTE* action = (BYTE*)v152_resolve_use_coffin_action();
    BYTE* inputManager = (BYTE*)__atomic_load_n(&g_v152InputManager, __ATOMIC_ACQUIRE);
    if (!action || !inputManager) return false;

    void** inputVtable = *(void***)inputManager;
    if (!inputVtable) return false;
    V152InputFlagsFn readFlags = (V152InputFlagsFn)*(void**)((BYTE*)inputVtable + 0xD8u);
    V152InputValueFn readValue = (V152InputValueFn)*(void**)((BYTE*)inputVtable + 0xE0u);
    if (!readFlags || !readValue || !v152_pointer_in_game((void*)readFlags) || !v152_pointer_in_game((void*)readValue)) return false;

    int context = *(int*)(action + 0xC8u);
    int bindings[2] = { *(int*)(action + 0xCCu), *(int*)(action + 0xD0u) };
    UINT32 rawFlags[2] = { 0u, 0u };
    float values[2] = { 0.0f, 0.0f };
    bool active = false;
    for (UINT32 slot = 0; slot < 2u; ++slot) {
        if (bindings[slot] == -1) continue;
        rawFlags[slot] = readFlags(inputManager, bindings[slot], context);
        if ((rawFlags[slot] & 0x4u) != 0u) {
            values[slot] = readValue(inputManager, bindings[slot], context);
            if (values[slot] > 0.0f && values[slot] < 1000.0f) active = true;
        }
    }
    if (snapshot) {
        snapshot->binding0 = bindings[0]; snapshot->binding1 = bindings[1]; snapshot->context = context;
        snapshot->flags0 = rawFlags[0]; snapshot->flags1 = rawFlags[1];
        snapshot->value0 = values[0]; snapshot->value1 = values[1];
    }
    return active;
}

static void __fastcall coffin_v152_qpid_scope(void* physics, UINT32 queryArg) {
    if (!g_nativeV152Qpid) return;
    g_nativeV152Qpid(physics, queryArg);
    if (!g_enableNetworkTraversal || !physics) return;

    BYTE* p = (BYTE*)physics;
    if (!readable_range(p + 0x514u, sizeof(UINT32)) ||
        !readable_range(p + 0x624u, sizeof(float)) ||
        !readable_range(p + 0x62Du, sizeof(BYTE))) return;
    UINT32 limitBits = *(UINT32*)(p + 0x514u);
    if (limitBits != 0x40B1C71Cu) return; // exact native F43700 QPID-wall hit

    float old514 = *(float*)(p + 0x514u);
    float old624 = *(float*)(p + 0x624u);
    BYTE old62D = *(BYTE*)(p + 0x62Du);
    *(float*)(p + 0x514u) = -1.0f;
    *(float*)(p + 0x624u) = -1.0f;
    *(BYTE*)(p + 0x62Du) = 0u;

    LONG index = __atomic_fetch_add(&g_v151QpidHits, 1, __ATOMIC_ACQ_REL);
    if (index < 24) {
        LogBuffer m; log_init(&m); log_prefix(&m);
        log_text(&m, "V154 QPID PHYSICS ROOT #"); log_uint(&m, (UINT64)index);
        log_text(&m, ": physics="); log_uint(&m, (UINT64)physics);
        log_text(&m, ", +514="); log_float2(&m, old514);
        log_text(&m, ", +624="); log_float2(&m, old624);
        log_text(&m, ", +62D="); log_uint(&m, old62D);
        log_text(&m, " -> -1,-1,0. Entity recovery is handled separately.\r\n"); append_log(&m);
    }
}

static void v152_copy_contact_record(BYTE* dst, const BYTE* src) {
    for (SIZE_T offset = 0; offset < V152_CONTACT_RECORD_SIZE; offset += sizeof(UINT64))
        *(UINT64*)(dst + offset) = *(const UINT64*)(src + offset);
}

static volatile LONG g_v153ProvenanceLogs = 0;

static void __fastcall coffin_v152_contact_filter(void* vehicle, void* hitArray, int* hitCount,
                                                   void* param4, int param5) {
    bool armed = false;
    int countBefore = 0;
    if (g_enableNetworkTraversal && vehicle && hitArray && hitCount && g_resolvePhysicsCollisionObject) {
        UINT64 armedPhysics = __atomic_load_n(&g_v152RecentQpidPhysics, __ATOMIC_ACQUIRE);
        armed = armedPhysics == (UINT64)vehicle;
        countBefore = *hitCount;
        if (armed && countBefore > 0 && countBefore <= 32) {
            BYTE* hits = (BYTE*)hitArray;
            for (int i = 0; i < countBefore; ++i) {
                BYTE* record = hits + (SIZE_T)(UINT32)i * V152_CONTACT_RECORD_SIZE;
                void* resolved = g_resolvePhysicsCollisionObject(record);
                UINT32 layer = 0xFFFFFFFFu;
                UINT32 objectField50 = 0u;
                if (resolved && readable_range((BYTE*)resolved + 0x50u, sizeof(UINT32))) {
                    objectField50 = *(UINT32*)((BYTE*)resolved + 0x50u);
                    layer = objectField50 & 0x7Fu;
                }
                LONG li = __atomic_fetch_add(&g_v153ProvenanceLogs, 1, __ATOMIC_ACQ_REL);
                if (li < 160) {
                    LogBuffer m; log_init(&m); log_prefix(&m);
                    log_text(&m, "V153 CONTACT ORIGIN #"); log_uint(&m, (UINT64)li);
                    log_text(&m, ": vehicle="); log_uint(&m, (UINT64)vehicle);
                    log_text(&m, ", slot="); log_uint(&m, (UINT64)(UINT32)param5);
                    log_text(&m, ", hit="); log_uint(&m, (UINT64)(UINT32)i);
                    log_text(&m, "/"); log_uint(&m, (UINT64)(UINT32)countBefore);
                    log_text(&m, ", record="); log_uint(&m, (UINT64)record);
                    log_text(&m, ", raw0=");
                    log_uint(&m, readable_range(record, sizeof(UINT64)) ? *(UINT64*)record : 0ull);
                    log_text(&m, ", resolved="); log_uint(&m, (UINT64)resolved);
                    log_text(&m, ", field50="); log_uint(&m, (UINT64)objectField50);
                    log_text(&m, ", layer="); log_uint(&m, (UINT64)layer);
                    if (layer == 0x16u) log_text(&m, "(Chiral_Wall)");
                    else if (layer == 0x1Bu) log_text(&m, "(Vehicle_stopper)");
                    else if (layer == 0x4Eu) log_text(&m, "(Vehicle_stopper_without_coffinboard)");
                    else if (layer == 0x5Cu) log_text(&m, "(Enemy_Damage_Blocker)");
                    log_text(&m, ". READ-ONLY before native F480A0.\r\n"); append_log(&m);
                }
            }
        }
    }

    if (g_nativeV152CoffinContact) g_nativeV152CoffinContact(vehicle, hitArray, hitCount, param4, param5);

    // Tie the World hit to the internal Coffin contact slot after native F480A0.
    if (armed && vehicle && param5 >= 0 && param5 < 32) {
        BYTE* p = (BYTE*)vehicle;
        WORD flags = 0u;
        BYTE activeEE = 0u;
        UINT64 records = 0ull;
        if (readable_range(p + 0x480u + (SIZE_T)(UINT32)param5 * 2u, sizeof(WORD)))
            flags = *(WORD*)(p + 0x480u + (SIZE_T)(UINT32)param5 * 2u);
        if (readable_range(p + 0x80u, sizeof(UINT64))) records = *(UINT64*)(p + 0x80u);
        if (records) {
            BYTE* rec = (BYTE*)records + (SIZE_T)(UINT32)param5 * 0x100u;
            if (readable_range(rec + 0xEEu, sizeof(BYTE))) activeEE = *(BYTE*)(rec + 0xEEu);
        }
        LONG li = __atomic_fetch_add(&g_v153ProvenanceLogs, 1, __ATOMIC_ACQ_REL);
        if (li < 160) {
            LogBuffer m; log_init(&m); log_prefix(&m);
            log_text(&m, "V153 CONTACT COMMIT #"); log_uint(&m, (UINT64)li);
            log_text(&m, ": vehicle="); log_uint(&m, (UINT64)vehicle);
            log_text(&m, ", slot="); log_uint(&m, (UINT64)(UINT32)param5);
            log_text(&m, ", countBefore="); log_uint(&m, (UINT64)(UINT32)(countBefore < 0 ? 0 : countBefore));
            log_text(&m, ", countAfter="); log_uint(&m, (UINT64)(UINT32)((hitCount && *hitCount >= 0) ? *hitCount : 0));
            log_text(&m, ", flags480="); log_uint(&m, (UINT64)flags);
            log_text(&m, ", activeEE="); log_uint(&m, (UINT64)activeEE);
            log_text(&m, ". Native F480A0 result; no mutation.\r\n"); append_log(&m);
        }
    }
}

struct V154ProfileSourceSnapshot {
    BYTE* set;
    BYTE* context;
    UINT32 setKind;
    UINT32 entryIndex;
    int sourceId;
    int mappedSlot;
    UINT32 sourceState;
};

static bool v154_read_e3_profile_source(BYTE* root, V154ProfileSourceSnapshot* snap) {
    if (snap) memset(snap, 0, sizeof(*snap));
    if (!root || !g_executable) return false;
    const SIZE_T contexts[2] = { OFF_V154_PRIMARY_RULE_CONTEXT, OFF_V154_SECONDARY_RULE_CONTEXT };
    BYTE* base = (BYTE*)g_executable;
    for (UINT32 kind = 0; kind < 2u; ++kind) {
        BYTE** setSlot = (BYTE**)(root + contexts[kind]);
        BYTE** ctxSlot = (BYTE**)(root + contexts[kind] + 8u);
        if (!readable_range(setSlot, sizeof(void*)) || !readable_range(ctxSlot, sizeof(void*))) continue;
        BYTE* set = *setSlot;
        BYTE* ctx = *ctxSlot;
        if (!set || !ctx || !readable_range(set, 0x20u)) continue;
        // Type-3 rules are enabled by rule-set header byte +4 in AF2D00.
        if (*(BYTE*)(set + 4u) == 0u) continue;
        UINT32 count = *(UINT32*)(set + 8u);
        BYTE* entries = *(BYTE**)(set + 0x10u);
        if (count > 4096u || (count && (!entries || !readable_range(entries, (SIZE_T)count * 0x58u)))) continue;
        for (UINT32 i = 0; i < count; ++i) {
            BYTE* entry = entries + (SIZE_T)i * 0x58u;
            if (*(UINT32*)(entry + 0x34u) != V154_E3_ACTION) continue;
            for (UINT32 c = 0; c < 2u; ++c) {
                BYTE* cond = entry + (SIZE_T)c * 0x18u;
                if (*(int*)(cond + 4u) != 3) continue;
                // Reproduce AF2D00's optional state restrictions. V23.2's E3
                // rule has both fields zero, but keep the native semantics.
                int needA = *(int*)(cond + 8u);
                int needB = *(int*)(cond + 0x0Cu);
                if (needA != 0) {
                    if (!readable_range(ctx + 0x22Cu, sizeof(int)) || *(int*)(ctx + 0x22Cu) != needA) continue;
                }
                if (needB != 0) {
                    if (!readable_range(ctx + 0x230u, sizeof(int)) || *(int*)(ctx + 0x230u) != needB) continue;
                }
                int sourceId = *(int*)(cond + 0x14u);
                if (sourceId < 0x6D || sourceId > 0x8C) continue;
                BYTE* mapAddr = base + RVA_V154_PROFILE_SOURCE_MAP + (SIZE_T)(UINT32)sourceId * sizeof(int);
                if (!readable_range(mapAddr, sizeof(int))) continue;
                int mapped = *(int*)mapAddr;
                if (mapped < 0 || mapped > 0x400) continue;
                BYTE* stateAddr = ctx + 0x488u + (SIZE_T)(UINT32)mapped * sizeof(UINT32);
                if (!readable_range(stateAddr, sizeof(UINT32))) continue;
                UINT32 state = *(UINT32*)stateAddr;
                if ((state & 1u) == 0u) continue;
                if (snap) {
                    snap->set = set; snap->context = ctx; snap->setKind = kind;
                    snap->entryIndex = i; snap->sourceId = sourceId;
                    snap->mappedSlot = mapped; snap->sourceState = state;
                }
                return true;
            }
        }
    }
    return false;
}

static void v155_log_profile_state(BYTE* root) {
    if (!root || !g_executable) return;
    V154ProfileSourceSnapshot s; memset(&s,0,sizeof(s));
    bool found=false, active=false;
    const SIZE_T contexts[2] = { OFF_V154_PRIMARY_RULE_CONTEXT, OFF_V154_SECONDARY_RULE_CONTEXT };
    BYTE* base=(BYTE*)g_executable;
    for(UINT32 kind=0; kind<2u && !found; ++kind){
        BYTE** setSlot=(BYTE**)(root+contexts[kind]); BYTE** ctxSlot=(BYTE**)(root+contexts[kind]+8u);
        if(!readable_range(setSlot,sizeof(void*))||!readable_range(ctxSlot,sizeof(void*))) continue;
        BYTE* set=*setSlot; BYTE* ctx=*ctxSlot; if(!set||!ctx||!readable_range(set,0x20u)) continue;
        UINT32 count=*(UINT32*)(set+8u); BYTE* entries=*(BYTE**)(set+0x10u);
        if(count>4096u || (count && (!entries||!readable_range(entries,(SIZE_T)count*0x58u)))) continue;
        for(UINT32 i=0;i<count && !found;++i){
            BYTE* entry=entries+(SIZE_T)i*0x58u; if(*(UINT32*)(entry+0x34u)!=V154_E3_ACTION) continue;
            for(UINT32 c=0;c<2u;++c){
                BYTE* cond=entry+(SIZE_T)c*0x18u; if(*(int*)(cond+4u)!=3) continue;
                int needA=*(int*)(cond+8u), needB=*(int*)(cond+0x0Cu);
                if(needA && (!readable_range(ctx+0x22Cu,sizeof(int))||*(int*)(ctx+0x22Cu)!=needA)) continue;
                if(needB && (!readable_range(ctx+0x230u,sizeof(int))||*(int*)(ctx+0x230u)!=needB)) continue;
                int sourceId=*(int*)(cond+0x14u); if(sourceId<0x6D||sourceId>0x8C) continue;
                BYTE* mapAddr=base+RVA_V154_PROFILE_SOURCE_MAP+(SIZE_T)(UINT32)sourceId*sizeof(int);
                if(!readable_range(mapAddr,sizeof(int))) continue; int mapped=*(int*)mapAddr; if(mapped<0||mapped>0x400) continue;
                BYTE* stateAddr=ctx+0x488u+(SIZE_T)(UINT32)mapped*sizeof(UINT32); if(!readable_range(stateAddr,sizeof(UINT32))) continue;
                UINT32 state=*(UINT32*)stateAddr; found=true; active=(state&1u)!=0u;
                s.set=set;s.context=ctx;s.setKind=kind;s.entryIndex=i;s.sourceId=sourceId;s.mappedSlot=mapped;s.sourceState=state; break;
            }
        }
    }
    UINT32 baseWord=readable_range(root+OFF_V154_E3_BASE_WORD,sizeof(UINT32))?*(UINT32*)(root+OFF_V154_E3_BASE_WORD):0;
    UINT32 blockWord=readable_range(root+OFF_V154_E3_BLOCK_WORD,sizeof(UINT32))?*(UINT32*)(root+OFF_V154_E3_BLOCK_WORD):0;
    UINT64 sig=0x1553000ull; sig=v155_mix(sig,found?1u:0u);sig=v155_mix(sig,active?1u:0u);sig=v155_mix(sig,s.sourceState);
    sig=v155_mix(sig,(baseWord>>3)&1u);sig=v155_mix(sig,(blockWord>>3)&1u);sig=v155_mix(sig,(UINT64)s.set);
    UINT64 old=__atomic_exchange_n(&g_v155ProfileLastSignature,sig,__ATOMIC_ACQ_REL); if(old==sig) return;
    LONG idx=__atomic_fetch_add(&g_v155ProfileTraceLogs,1,__ATOMIC_ACQ_REL); if(idx>=96) return;
    LogBuffer m;log_init(&m);log_prefix(&m);log_text(&m,"V155 PROFILE E3 #");log_uint(&m,(UINT64)idx);
    log_text(&m,": found=");log_uint(&m,found?1u:0u);log_text(&m,", active=");log_uint(&m,active?1u:0u);
    log_text(&m,", setKind=");log_uint(&m,s.setKind);log_text(&m,", set=");log_uint(&m,(UINT64)s.set);
    log_text(&m,", entry=");log_uint(&m,s.entryIndex);log_text(&m,", source=");log_uint(&m,(UINT64)(UINT32)s.sourceId);
    log_text(&m,", mappedSlot=");log_uint(&m,(UINT64)(UINT32)s.mappedSlot);log_text(&m,", sourceState=");log_uint(&m,s.sourceState);
    log_text(&m,", base=");log_uint(&m,(baseWord>>3)&1u);log_text(&m,", block=");log_uint(&m,(blockWord>>3)&1u);
    log_text(&m,". READ-ONLY profile observation after native F67ED0.\r\n");append_log(&m);
}

// v1.56: V25-V28 + full master export proved the live Type-3 E3 source maps
// to slot 6 -> native input ID 0x19A (EMenuInputFunction::FUNCTION_ACCEPT).
// The previous v1.54/v1.55 source-state test used InputManager +0xE0, which
// only exposes action-state bit 0x4. Native consumers also use +0xD8 and test
// bit 0x2 for the actual Accept press. For the validated OUTSIDE Coffin
// candidate only, read the native raw action state and expose E3 Base when
// the native active bit 0x4 is present. D28350 returns 1 for inactive and 4 for active in the 400-range. Raw/current/rising and
// EDFE10 remain entirely native.
static const UINT32 RVA_V156_INPUT_MANAGER_GLOBAL = 0x0A1BB600u;
static const UINT32 RVA_V156_INPUT_MANAGER_VTABLE = 0x03225B10u;
static const UINT32 RVA_V156_INPUT_STATE_NATIVE = 0x00D28350u;
static const UINT32 RVA_V156_PROFILE_INPUT_CONTEXT = 0x06281C50u;
static const UINT32 V156_FUNCTION_ACCEPT = 0x19Au;
static volatile LONG g_v156AcceptLogs = 0;
static volatile LONG g_v156BaseInjections = 0;
static volatile UINT64 g_v156LastAcceptSignature = ~0ull;

typedef UINT32 (__fastcall* V156InputStateFn)(void*, UINT32, UINT32);

static bool v156_read_accept_state(UINT32* stateOut, UINT32* deviceOut) {
    if (stateOut) *stateOut = 0u;
    if (deviceOut) *deviceOut = 0xFFFFFFFFu;
    if (!g_executable) return false;

    BYTE* base = (BYTE*)g_executable;
    void** globalSlot = (void**)(base + RVA_V156_INPUT_MANAGER_GLOBAL);
    if (!readable_range(globalSlot, sizeof(void*))) return false;
    BYTE* inputManager = (BYTE*)*globalSlot;
    if (!inputManager || !readable_range(inputManager, sizeof(void*))) return false;
    void* vtable = *(void**)inputManager;
    if (vtable != (void*)(base + RVA_V156_INPUT_MANAGER_VTABLE)) return false;

    V156InputStateFn readState = (V156InputStateFn)(base + RVA_V156_INPUT_STATE_NATIVE);
    if (!readable_range((void*)readState, 16u)) return false;

    UINT32 preferred = 0xFFFFFFFFu;
    BYTE* inputContext = base + RVA_V156_PROFILE_INPUT_CONTEXT;
    if (readable_range(inputContext + 0x6Cu, sizeof(UINT32))) {
        preferred = *(UINT32*)(inputContext + 0x6Cu);
    }

    // First query exactly the device used by the native 213AB10 -> 213BE10
    // profile snapshot. If that device has no active bit, scan the other
    // native device slots only as a fallback for mixed keyboard/gamepad input.
    if (preferred < 20u) {
        UINT32 state = readState(inputManager, V156_FUNCTION_ACCEPT, preferred);
        if ((state & 0x4u) != 0u) {
            if (stateOut) *stateOut = state;
            if (deviceOut) *deviceOut = preferred;
            return true;
        }
        if (stateOut) *stateOut = state;
        if (deviceOut) *deviceOut = preferred;
    }

    for (UINT32 device = 0; device < 20u; ++device) {
        if (device == preferred) continue;
        UINT32 state = readState(inputManager, V156_FUNCTION_ACCEPT, device);
        if ((state & 0x4u) != 0u) {
            if (stateOut) *stateOut = state;
            if (deviceOut) *deviceOut = device;
            return true;
        }
    }
    return false;
}

static void __fastcall coffin_v152_rule_merge_scope(void* stateRoot) {
    if (g_nativeV152RuleMerge) g_nativeV152RuleMerge(stateRoot);
    if (!g_enableOutsideNetworkMount || !stateRoot) return;
    v155_log_profile_state((BYTE*)stateRoot);

    // Scope BEFORE reading input: this path must never affect normal/inside
    // gameplay, menus, or another RideFloater/Baggage candidate.
    void* manager = 0;
    void* selected = 0;
    UINT32 d4 = 0, managerFlags = 0;
    if (!v151_current_outside_coffin_candidate(&manager, &selected, &d4, &managerFlags)) return;

    UINT32 acceptState = 0u;
    UINT32 acceptDevice = 0xFFFFFFFFu;
    bool acceptActive = v156_read_accept_state(&acceptState, &acceptDevice);

    BYTE* root = (BYTE*)stateRoot;
    if (!readable_range(root + OFF_V154_E3_BASE_WORD, sizeof(UINT32)) ||
        !readable_range(root + OFF_V154_E3_BLOCK_WORD, sizeof(UINT32))) return;
    UINT32 beforeBase = *(UINT32*)(root + OFF_V154_E3_BASE_WORD);
    UINT32 block = *(UINT32*)(root + OFF_V154_E3_BLOCK_WORD);
    bool baseAlready = (beforeBase & 0x8u) != 0u;
    bool blocked = (block & 0x8u) != 0u;
    bool injected = false;

    if (acceptActive && !baseAlready && !blocked) {
        *(UINT32*)(root + OFF_V154_E3_BASE_WORD) = beforeBase | 0x8u;
        injected = true;
        __atomic_fetch_add(&g_v156BaseInjections, 1, __ATOMIC_ACQ_REL);
    }

    UINT64 acceptSignature = ((UINT64)acceptDevice << 32) | (UINT64)acceptState;
    acceptSignature ^= ((UINT64)((beforeBase >> 3) & 1u) << 60);
    acceptSignature ^= ((UINT64)((block >> 3) & 1u) << 61);
    acceptSignature ^= ((UINT64)(injected ? 1u : 0u) << 62);
    UINT64 previousAcceptSignature = __atomic_exchange_n(&g_v156LastAcceptSignature, acceptSignature, __ATOMIC_ACQ_REL);
    if (previousAcceptSignature != acceptSignature) {
        LONG index = __atomic_fetch_add(&g_v156AcceptLogs, 1, __ATOMIC_ACQ_REL);
        if (index < 96) {
            LogBuffer m; log_init(&m); log_prefix(&m);
            log_text(&m, "V156 OUTSIDE ACCEPT #"); log_uint(&m, (UINT64)index);
            log_text(&m, ": selected="); log_uint(&m, (UINT64)selected);
            log_text(&m, ", device="); log_uint(&m, (UINT64)acceptDevice);
            log_text(&m, ", rawState="); log_uint(&m, (UINT64)acceptState);
            log_text(&m, ", activeBit4="); log_uint(&m, (acceptState & 0x4u) ? 1u : 0u);
            log_text(&m, ", baseBefore="); log_uint(&m, (beforeBase >> 3) & 1u);
            log_text(&m, ", block="); log_uint(&m, (block >> 3) & 1u);
            log_text(&m, ", injected="); log_uint(&m, injected ? 1u : 0u);
            log_text(&m, ". Base-only; F68750/F689B0 and EDFE10 remain native.\r\n");
            append_log(&m);
        }
    }
}

// v1.70: native Accept -> E3 source bridge.
//
// Static proof from the master export:
//   * The live E3 Type-3 rule uses source 0x75 and maps to source-state slot 6.
//   * AF2D00 consumes the low two bits of each source-state DWORD as
//       0 = idle, 1 = held, 2 = released, 3 = pressed/rising.
//   * NxInputImpl::D28350 is the native raw-state function. For the validated
//     FUNCTION_ACCEPT input ID 0x19A it returns bit 0x4 while the real Accept
//     control is active. In contrast, input ID 1000 is a sentinel: D28350
//     returns 1 and D28550 returns 0, so Action_UseCoffinBoard's persisted
//     binding=1000 can never be used as a physical input source.
//
// Only while the exact sticky/native Coffin candidate is selected after the
// QPID crossing do we mirror FUNCTION_ACCEPT into slot 6 for the duration of
// native F67ED0/AF2D00. The original slot byte is restored immediately after
// the native call. We synthesize only the source transition code from the
// real native Accept state; E3 Base/Block/Raw outputs and EDFE10 remain native.
static const UINT32 V170_E3_SLOT = 6u;
static volatile LONG g_v170AcceptBridgeLogs = 0;
static volatile LONG g_v170AcceptBridgeHits = 0;
static volatile UINT64 g_v170LastAcceptBridgeSignature = ~0ull;
static bool g_v170AcceptWasActive = false;

static bool v166_selected_sticky_coffin(BYTE** managerOut, BYTE** candidateOut) {
    if (!g_enableOutsideNetworkMount || !g_executable ||
        __atomic_load_n(&g_v151QpidHits, __ATOMIC_ACQUIRE) <= 0) return false;
    BYTE* cache = (BYTE*)__atomic_load_n(&g_v164StickyCache, __ATOMIC_ACQUIRE);
    BYTE* candidate = (BYTE*)__atomic_load_n(&g_v164StickyCandidate, __ATOMIC_ACQUIRE);
    if (!cache || !candidate || !v159_is_genuine_coffin_candidate_early(candidate) ||
        !readable_range(cache, 0xB0u) ||
        *(void**)cache != (void*)((BYTE*)g_executable + RVA_COFFIN_BAGGAGE_INFO_CACHE_VTABLE)) return false;
    BYTE* manager = cache - 0x228u;
    if (!readable_range(manager + 0x230u, sizeof(void*)) ||
        !readable_range(manager + 0x2D0u, sizeof(UINT32))) return false;
    if (*(void**)(manager + 0x230u) != candidate || *(UINT32*)(manager + 0x2D0u) == 0u) return false;
    if (managerOut) *managerOut = manager;
    if (candidateOut) *candidateOut = candidate;
    return true;
}

static bool v166_read_dynamic_e3_state(BYTE* root, int* slotOut, UINT32* stateOut) {
    if (slotOut) *slotOut = -1;
    if (stateOut) *stateOut = 0u;
    if (!root || !g_executable) return false;
    BYTE* registry = (BYTE*)g_executable + RVA_V151_ACTION_REGISTRY;
    BYTE** regSlot = (BYTE**)(registry + (SIZE_T)COFFIN_NETWORK_E3_REQUEST * sizeof(void*));
    if (!readable_range(regSlot, sizeof(void*)) || !*regSlot ||
        !readable_range(*regSlot + 4u, sizeof(int))) return false;
    int dynamicSlot = *(int*)(*regSlot + 4u);
    if (dynamicSlot < 0 || dynamicSlot >= 0x400) return false;
    BYTE* dyn = root + 0x31F0u + 0x488u + (SIZE_T)(UINT32)dynamicSlot * sizeof(UINT32);
    if (!readable_range(dyn, sizeof(UINT32))) return false;
    if (slotOut) *slotOut = dynamicSlot;
    if (stateOut) *stateOut = *(UINT32*)dyn;
    return true;
}

static void __fastcall coffin_v166_rule_merge_source_bridge(void* stateRoot) {
    BYTE* root = (BYTE*)stateRoot;
    BYTE* manager = 0;
    BYTE* candidate = 0;
    bool selected = root && v166_selected_sticky_coffin(&manager, &candidate);

    UINT32 acceptState = 0u;
    UINT32 acceptDevice = 0xFFFFFFFFu;
    bool acceptActive = selected ? v156_read_accept_state(&acceptState, &acceptDevice) : false;

    int dynamicSlotBefore = -1;
    UINT32 dynamicStateBefore = 0u;
    if (root) v166_read_dynamic_e3_state(root, &dynamicSlotBefore, &dynamicStateBefore);

    BYTE* slot = root ? root + 0x31F0u + 0x488u + (SIZE_T)V170_E3_SLOT * sizeof(UINT32) : 0;
    BYTE oldByte = 0u;
    bool slotReadable = slot && readable_range(slot, sizeof(UINT32));
    if (slotReadable) oldByte = *slot;

    BYTE bridgeState = 0u;
    bool bridged = false;
    if (selected && slotReadable) {
        if (acceptActive) bridgeState = g_v170AcceptWasActive ? 1u : 3u;
        else if (g_v170AcceptWasActive) bridgeState = 2u;
        if (bridgeState != 0u) {
            *slot = bridgeState;
            bridged = true;
            __atomic_fetch_add(&g_v170AcceptBridgeHits, 1, __ATOMIC_ACQ_REL);
        }
        g_v170AcceptWasActive = acceptActive;
    } else {
        g_v170AcceptWasActive = false;
    }

    if (g_nativeV152RuleMerge) g_nativeV152RuleMerge(stateRoot);

    UINT32 baseWord = (root && readable_range(root + OFF_V154_E3_BASE_WORD, sizeof(UINT32)))
        ? *(UINT32*)(root + OFF_V154_E3_BASE_WORD) : 0u;
    UINT32 blockWord = (root && readable_range(root + OFF_V154_E3_BLOCK_WORD, sizeof(UINT32)))
        ? *(UINT32*)(root + OFF_V154_E3_BLOCK_WORD) : 0u;
    UINT32 dynamicStateAfter = 0u;
    int dynamicSlotAfter = -1;
    if (root) v166_read_dynamic_e3_state(root, &dynamicSlotAfter, &dynamicStateAfter);

    if (bridged && slotReadable) *slot = oldByte;

    if (g_enableOutsideNetworkMount && root) v155_log_profile_state(root);

    if (selected || acceptActive || bridged) {
        UINT64 sig = (((UINT64)acceptState & 0xFFFFu)
            | ((UINT64)acceptDevice << 16)
            | ((UINT64)bridgeState << 24)
            | ((UINT64)(oldByte & 0xFFu) << 32)
            | ((UINT64)((baseWord >> 3) & 1u) << 40)
            | ((UINT64)((blockWord >> 3) & 1u) << 41)
            | ((UINT64)(bridged ? 1u : 0u) << 42))
            ^ ((UINT64)(UINT32)(dynamicSlotAfter + 1) << 43)
            ^ ((UINT64)(dynamicStateAfter & 0xFFu) << 8);
        UINT64 prev = __atomic_exchange_n(&g_v170LastAcceptBridgeSignature, sig, __ATOMIC_ACQ_REL);
        if (prev != sig) {
            LONG idx = __atomic_fetch_add(&g_v170AcceptBridgeLogs, 1, __ATOMIC_ACQ_REL);
            if (idx < 128) {
                LogBuffer m; log_init(&m); log_prefix(&m);
                log_text(&m, "V170 NATIVE ACCEPT E3 #"); log_uint(&m, (UINT64)idx);
                log_text(&m, ": candidate="); log_uint(&m, (UINT64)candidate);
                log_text(&m, ", manager="); log_uint(&m, (UINT64)manager);
                log_text(&m, ", device="); log_uint(&m, (UINT64)acceptDevice);
                log_text(&m, ", rawState="); log_uint(&m, (UINT64)acceptState);
                log_text(&m, ", activeBit4="); log_uint(&m, acceptActive ? 1u : 0u);
                log_text(&m, ", slot6Before="); log_uint(&m, (UINT64)oldByte);
                log_text(&m, ", bridgeState="); log_uint(&m, (UINT64)bridgeState);
                log_text(&m, ", dynamicSlot="); log_uint(&m, (UINT64)(UINT32)dynamicSlotBefore);
                log_text(&m, ", dynamicBefore="); log_uint(&m, (UINT64)dynamicStateBefore);
                log_text(&m, ", dynamicAfter="); log_uint(&m, (UINT64)dynamicStateAfter);
                log_text(&m, ", bridged="); log_uint(&m, bridged ? 1u : 0u);
                log_text(&m, ", baseAfter="); log_uint(&m, (baseWord >> 3) & 1u);
                log_text(&m, ", blockAfter="); log_uint(&m, (blockWord >> 3) & 1u);
                log_text(&m, ". Native FUNCTION_ACCEPT 0x19A -> E3 slot 6 only around F67ED0; slot restored immediately.\r\n");
                append_log(&m);
            }
        }
    }
}

static bool install_v166_rule_merge_source_bridge(HMODULE executable) {
    if (!g_enableOutsideNetworkMount) return true;
    BYTE* base = (BYTE*)executable;
    BYTE* call = base + RVA_V151_RULE_MERGE_CALL;
    BYTE* nativeTarget = base + RVA_V151_RULE_MERGE_NATIVE;
    int oldDisp = 0;
    if (!v151_validate_call(call, nativeTarget, &oldDisp)) return false;
    g_nativeV152RuleMerge = (V152RuleMergeFn)nativeTarget;
    BYTE* relay = (BYTE*)allocate_call_relay_near(call, (void*)&coffin_v166_rule_merge_source_bridge);
    if (!relay || !v151_patch_call(call, relay, oldDisp)) return false;
    g_v152RuleMergeInstalled = 1;
    return true;
}

static bool install_v167_coffin_entity_capture(HMODULE executable) {
    if (!g_enableOutsideNetworkMount) return true;
    BYTE* base = (BYTE*)executable;
    void** slot = (void**)(base + RVA_V161_COFFIN_VEHICLE_VTABLE + RVA_V167_COFFIN_UPDATE_SLOT);
    void* expected = (void*)(base + RVA_V167_COFFIN_UPDATE_NATIVE);
    if (!readable_range(slot, sizeof(void*)) || *slot != expected) return false;
    g_nativeV167CoffinUpdate = (V167CoffinUpdateFn)*slot;
    if (!freeze_for_patch((const BYTE*)slot, sizeof(void*))) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(slot, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtection)) {
        unfreeze_after_patch(); return false;
    }
    *slot = (void*)&v167_coffin_update_capture;
    FlushInstructionCache((HANDLE)(INT64)-1, slot, sizeof(void*));
    bool ok = *slot == (void*)&v167_coffin_update_capture;
    if (!ok) *slot = (void*)g_nativeV167CoffinUpdate;
    DWORD ignored = 0; VirtualProtect(slot, sizeof(void*), oldProtection, &ignored);
    unfreeze_after_patch();
    if (ok) g_v167CoffinUpdateInstalled = 1;
    return ok;
}

static bool install_v167_edb900_direct_admit(HMODULE executable) {
    if (!g_enableOutsideNetworkMount) return true;
    BYTE* base = (BYTE*)executable;
    BYTE* call = base + RVA_V167_EDB900_CALL;
    BYTE* nativeTarget = base + RVA_V167_EDB900_NATIVE;
    int oldDisp = 0;
    if (!v151_validate_call(call, nativeTarget, &oldDisp)) return false;
    g_nativeV167Edb900 = (V167Edb900Fn)nativeTarget;
    g_nativeV169ComponentLookup = (V169ComponentLookupFn)(base + RVA_V169_COMPONENT_LOOKUP);
    BYTE* relay = (BYTE*)allocate_call_relay_near(call, (void*)&v167_edb900_scope);
    if (!relay || !v151_patch_call(call, relay, oldDisp)) return false;
    g_v167Edb900Installed = 1;
    return true;
}

// v1.83: 1F76D80 stores the native QPID coverage result at Coffin+126E.
// Base vehicle update 1F66E10 -> 1F5F0A0 computes mount availability from
// damage/state/occupancy/resource conditions. Coffin-only update 1F794F0 then
// calls 1F77180, whose tail clears availability bit0 solely when +126E == 0.
// The 08:44 v1.82 trace shows the same Coffin change 4E8=0x11 -> 0x10,
// causing BEEC80 -> A8 to disable 0x158. Remove only this final network veto;
// never set availability, fake coverage, or alter the shared BEEC80 predicate.
static int g_v183AvailabilityVetoRemoved = 0;
static const UINT32 RVA_V183_AVAILABILITY_TAIL = 0x01F77311u;
static const BYTE V183_AVAILABILITY_TAIL[17] = {
    0x80,0xB9,0x6E,0x12,0,0,0, // cmp byte ptr [rcx+126E],0
    0x75,0x07,                 // jnz ret
    0x83,0xA1,0xE8,0x04,0,0,0xFE, // and dword ptr [rcx+4E8],~1
    0xC3
};

static bool install_v183_coffin_availability(HMODULE executable) {
    if (!g_enableOutsideNetworkMount) return true;
    BYTE* base = (BYTE*)executable;
    BYTE** slot = (BYTE**)(base + RVA_V161_COFFIN_VEHICLE_VTABLE + 0x178u);
    BYTE* tail = base + RVA_V183_AVAILABILITY_TAIL;
    if (!readable_range(slot, sizeof(void*)) || *slot != base + 0x1F794F0u ||
        !readable_range(tail, sizeof(V183_AVAILABILITY_TAIL)) ||
        !bytes_equal(tail, V183_AVAILABILITY_TAIL, sizeof(V183_AVAILABILITY_TAIL))) return false;
    BYTE* target = tail + 9u;
    static const BYTE replacement[7] = {0x90,0x90,0x90,0x90,0x90,0x90,0x90};
    if (!freeze_for_patch(target, sizeof(replacement))) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(target, sizeof(replacement), PAGE_EXECUTE_READWRITE, &oldProtection)) {
        unfreeze_after_patch(); return false;
    }
    memcpy(target, replacement, sizeof(replacement));
    FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(replacement));
    bool ok = bytes_equal(target, replacement, sizeof(replacement));
    if (!ok) {
        memcpy(target, V183_AVAILABILITY_TAIL + 9u, sizeof(replacement));
        FlushInstructionCache((HANDLE)(INT64)-1, target, sizeof(replacement));
    }
    DWORD ignored = 0;
    VirtualProtect(target, sizeof(replacement), oldProtection, &ignored);
    unfreeze_after_patch();
    if (ok) g_v183AvailabilityVetoRemoved = 1;
    return ok;
}

static bool install_v154_entity_boundary_bypass(HMODULE executable) {
    if (!g_enableNetworkTraversal) return true;
    BYTE* base = (BYTE*)executable;
    BYTE* cmp = base + RVA_V154_ENTITY_SECOND_QUERY_CMP;
    BYTE* branch = base + RVA_V154_ENTITY_SECOND_QUERY_BRANCH;
    BYTE* target = base + RVA_V154_ENTITY_SECOND_QUERY_SKIP;
    if (!readable_range(cmp, 9u)) return false;
    static const BYTE cmpExpected[3] = {0x83,0xF8,0xFF};
    if (!bytes_equal(cmp, cmpExpected, sizeof(cmpExpected))) return false;
    if (branch[0] != 0x0Fu || branch[1] != 0x84u) return false;
    int oldDisp = 0; memcpy(&oldDisp, branch + 2u, sizeof(oldDisp));
    if (branch + 6u + oldDisp != target) return false;
    INT64 newDisp64 = (INT64)target - (INT64)(branch + 5u);
    if (newDisp64 < -0x80000000ll || newDisp64 > 0x7fffffffll) return false;
    int newDisp = (int)newDisp64;
    BYTE replacement[6] = {0xE9,0,0,0,0,0x90};
    memcpy(replacement + 1u, &newDisp, sizeof(newDisp));
    if (!freeze_for_patch(branch, 6u)) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(branch, 6u, PAGE_EXECUTE_READWRITE, &oldProtection)) {
        unfreeze_after_patch(); return false;
    }
    BYTE original[6]; memcpy(original, branch, 6u);
    memcpy(branch, replacement, 6u);
    FlushInstructionCache((HANDLE)(INT64)-1, branch, 6u);
    bool ok = bytes_equal(branch, replacement, 6u);
    if (!ok) { memcpy(branch, original, 6u); FlushInstructionCache((HANDLE)(INT64)-1, branch, 6u); }
    DWORD ignored = 0; VirtualProtect(branch, 6u, oldProtection, &ignored);
    unfreeze_after_patch();
    if (ok) g_v154EntityBypassInstalled = 1;
    return ok;
}

static bool install_v152_qpid_scope(HMODULE executable) {
    if (!g_enableNetworkTraversal) return true;
    BYTE* base = (BYTE*)executable;
    BYTE* call = base + RVA_V151_QPID_LIMITER_CALL;
    BYTE* nativeTarget = base + RVA_V151_QPID_LIMITER_NATIVE;
    int oldDisp = 0;
    if (!v151_validate_call(call, nativeTarget, &oldDisp)) return false;
    g_nativeV152Qpid = (V152QpidFn)nativeTarget;
    BYTE* relay = (BYTE*)allocate_call_relay_near(call, (void*)&coffin_v152_qpid_scope);
    if (!relay || !v151_patch_call(call, relay, oldDisp)) return false;
    g_v152QpidInstalled = 1;
    return true;
}

static bool install_v152_contact_filter(HMODULE executable) {
    if (!g_enableNetworkTraversal) return true;
    BYTE* base = (BYTE*)executable;
    void** vtable = (void**)(base + RVA_COFFIN_PHYSICS_VTABLE);
    if (!readable_range(vtable, 4u * sizeof(void*)) ||
        vtable[0] != (void*)(base + RVA_COFFIN_VFUNC_0) ||
        vtable[1] != (void*)(base + RVA_COFFIN_VFUNC_1) ||
        vtable[2] != (void*)(base + RVA_COFFIN_VFUNC_2) ||
        vtable[3] != (void*)(base + RVA_COFFIN_VFUNC_3)) return false;

    // V17 layout proof: F47A90 resolves the same 0x50-byte contact record and
    // reads object+0x50 & 0x7F; validate both the resolver call and 0x50 access.
    BYTE* semantic = base + RVA_V152_CONTACT_SEMANTIC_HELPER;
    if (!readable_range(semantic + 0x1Bu, 22u)) return false;
    if (semantic[0x1Bu] != 0xE8u) return false;
    int resolverDisp = 0; memcpy(&resolverDisp, semantic + 0x1Cu, sizeof(resolverDisp));
    if (semantic + 0x20u + resolverDisp != base + RVA_RESOLVE_PHYSICS_COLLISION_OBJECT) return false;
    static const BYTE layerRead[11] = { 0x8B,0x40,0x50,0x83,0xE0,0x7F,0x3C,0x5C,0x0F,0x85,0x00 };
    // The final branch displacement byte varies as part of the rel32; compare
    // only the stable MOV/AND/CMP opcode prefix.
    if (!readable_range(semantic + 0x29u, 8u) ||
        !bytes_equal(semantic + 0x29u, layerRead, 8u)) return false;

    // Independent V17 semantic proof: native IgnoreChiralWall compares layer 0x16.
    static const BYTE chiralGate[11] = {0x8B,0x43,0x50,0x83,0xE0,0x7F,0x3C,0x16,0x0F,0x95,0xC0};
    BYTE* ignoreGate = base + RVA_PHYSICS_COLLISION_IGNORE_CHIRAL_WALL_PREDICATE + 0xF0u;
    if (!readable_range(ignoreGate, sizeof(chiralGate)) ||
        !bytes_equal(ignoreGate, chiralGate, sizeof(chiralGate))) return false;

    void** slot = (void**)((BYTE*)vtable + V152_COFFIN_CONTACT_SLOT);
    g_nativeV152CoffinContact = (V152CoffinContactFn)*slot;
    g_resolvePhysicsCollisionObject = (ResolvePhysicsCollisionObjectFn)(base + RVA_RESOLVE_PHYSICS_COLLISION_OBJECT);
    if (!freeze_for_patch((const BYTE*)slot, sizeof(void*))) return false;
    DWORD oldProtection = 0;
    if (!VirtualProtect(slot, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtection)) {
        unfreeze_after_patch(); return false;
    }
    *slot = (void*)&coffin_v152_contact_filter;
    FlushInstructionCache((HANDLE)(INT64)-1, slot, sizeof(void*));
    bool installed = *slot == (void*)&coffin_v152_contact_filter;
    if (!installed) *slot = (void*)g_nativeV152CoffinContact;
    DWORD ignored = 0; VirtualProtect(slot, sizeof(void*), oldProtection, &ignored);
    unfreeze_after_patch();
    if (installed) g_v152ContactFilterInstalled = 1;
    return installed;
}

static bool install_v152_rule_merge_scope(HMODULE executable) {
    if (!g_enableOutsideNetworkMount) return true;
    BYTE* base = (BYTE*)executable;
    BYTE* call = base + RVA_V151_RULE_MERGE_CALL;
    BYTE* nativeTarget = base + RVA_V151_RULE_MERGE_NATIVE;
    int oldDisp = 0;
    if (!v151_validate_call(call, nativeTarget, &oldDisp)) return false;

    g_nativeV152RuleMerge = (V152RuleMergeFn)nativeTarget;
    BYTE* relay = (BYTE*)allocate_call_relay_near(call, (void*)&coffin_v152_rule_merge_scope);
    if (!relay || !v151_patch_call(call, relay, oldDisp)) return false;
    g_v152RuleMergeInstalled = 1;
    return true;
}

#ifdef COFFIN_AB_DIAGNOSTIC
#error The AB diagnostic wrapper for E0E230 does not preserve the live XMM1 argument; do not build or install it.
// One-session, read-only inside/outside trace. Every wrapper calls the exact
// native function and records only the gates that decide the normal mount.
typedef void (__fastcall* AbMoverFn)(void*, void*, BYTE);
typedef void (__fastcall* AbStateGateFn)(void*);
typedef void (__fastcall* AbStateTickFn)(void*);
typedef void (__fastcall* AbPhase5Fn)(void*);
typedef void (__fastcall* AbRideOnFn)(void*);
typedef BYTE (__fastcall* AbReservationFn)(void*, void*, BYTE, void*, BYTE);

static AbMoverFn g_abNativeMover = 0;
static AbStateGateFn g_abNativeStateGate = 0;
static AbStateTickFn g_abNativeStateTick = 0;
static AbPhase5Fn g_abNativePhase5 = 0;
static AbRideOnFn g_abNativeRideOn = 0;
static AbReservationFn g_abNativeReservation = 0;
static volatile LONG g_abMoverCalls[2] = {};
static volatile LONG g_abStateCalls = 0;
static volatile LONG g_abRideOnCalls[2] = {};
static volatile LONG g_abPhase5Calls[2] = {};
static volatile LONG g_abReserveCalls[2] = {};
static volatile UINT64 g_abLastMover = ~0ull;
static volatile UINT64 g_abLastPlugin = ~0ull;
static volatile LONG g_abLastGate = -1;
static volatile LONG g_abLastSuppressor = -1;
static volatile LONG g_abLastSide = -1;

static int ab_side() {
    return __atomic_load_n(&g_v151QpidHits,__ATOMIC_ACQUIRE) > 0 ? 1 : 0;
}

static UINT64 ab_read_u64(BYTE* base,SIZE_T offset) {
    return base && readable_range(base+offset,sizeof(UINT64)) ? *(UINT64*)(base+offset) : ~0ull;
}

static BYTE* ab_read_ptr(BYTE* base,SIZE_T offset) {
    UINT64 value=ab_read_u64(base,offset);
    return value==~0ull ? 0 : (BYTE*)value;
}

static UINT64 ab_module_rva(UINT64 address) {
    UINT64 base=(UINT64)g_executable;
    return address>=base && address<base+EXPECTED_IMAGE_SIZE ? address-base : ~0ull;
}

static UINT32 ab_read_u32(BYTE* base,SIZE_T offset) {
    return base && readable_range(base+offset,sizeof(UINT32)) ? *(UINT32*)(base+offset) : 0xffffffffu;
}

static void ab_log_vehicle(LogBuffer* m,BYTE* vehicle) {
    log_text(m," vehicle="); log_uint(m,(UINT64)vehicle);
    if (!vehicle || !readable_range(vehicle,0x918u)) return;
    UINT64 vtable=*(UINT64*)vehicle;
    log_text(m," vt="); log_uint(m,vtable);
    log_text(m," vtRva="); log_uint(m,ab_module_rva(vtable));
    log_text(m," type="); log_uint(m,ab_read_u32(vehicle,0x490u));
    log_text(m," flags98="); log_uint(m,ab_read_u64(vehicle,0x98u));
    log_text(m," bit9="); log_uint(m,((ab_read_u64(vehicle,0x98u)&0x200ull)!=0)?1:0);
    log_text(m," state4E4="); log_uint(m,ab_read_u32(vehicle,0x4E4u));
    log_text(m," flags4E8="); log_uint(m,ab_read_u32(vehicle,0x4E8u));
    log_text(m," trigger4F4="); log_uint(m,ab_read_u32(vehicle,0x4F4u));
    log_text(m," reserved4FC="); log_uint(m,ab_read_u32(vehicle,0x4FCu));
    log_text(m," owner4B0="); log_uint(m,ab_read_u64(vehicle,0x4B0u));
    log_text(m," context5C0="); log_uint(m,ab_read_u64(vehicle,0x5C0u));
    log_text(m," field910="); log_uint(m,ab_read_u64(vehicle,0x910u));
}

static void* ab_find_ridevehicle_plugin(BYTE* state) {
    if (!state || !readable_range(state+0xAA8u,sizeof(void*))) return 0;
    BYTE* entries=*(BYTE**)(state+0xAA8u);
    if (!entries || !readable_range(entries,42u*sizeof(void*))) return 0;
    for (UINT32 i=1;i<42u;++i) {
        BYTE* plugin=((BYTE**)entries)[i];
        if (plugin && readable_range(plugin,0x10u) &&
            *(void**)plugin==(BYTE*)g_executable+RVA_RIDEVEHICLE_PLUGIN_VTABLE) return plugin;
    }
    return 0;
}

static void __fastcall ab_mover_switch(void* owner,void* newMover,BYTE mode) {
    BYTE* oldMover=ab_read_ptr((BYTE*)owner,0xC0u);
    UINT64 oldVtable=ab_read_u64(oldMover,0);
    UINT64 requestedVtable=ab_read_u64((BYTE*)newMover,0);
    if (g_abNativeMover) g_abNativeMover(owner,newMover,mode);
    int side=ab_side();
    LONG idx=__atomic_fetch_add(&g_abMoverCalls[side],1,__ATOMIC_ACQ_REL);
    if (idx>=64) return;
    BYTE* actual=ab_read_ptr((BYTE*)owner,0xC0u);
    LogBuffer m; log_init(&m); log_prefix(&m);
    log_text(&m,"AB MOVER side="); log_uint(&m,side);
    log_text(&m," #"); log_uint(&m,(UINT64)idx);
    log_text(&m," owner="); log_uint(&m,(UINT64)owner);
    log_text(&m," old="); log_uint(&m,(UINT64)oldMover);
    log_text(&m," oldVt="); log_uint(&m,oldVtable);
    log_text(&m," oldVtRva="); log_uint(&m,ab_module_rva(oldVtable));
    log_text(&m," requested="); log_uint(&m,(UINT64)newMover);
    log_text(&m," requestedVt="); log_uint(&m,requestedVtable);
    log_text(&m," requestedVtRva="); log_uint(&m,ab_module_rva(requestedVtable));
    log_text(&m," actual="); log_uint(&m,(UINT64)actual);
    UINT64 actualVtable=ab_read_u64(actual,0);
    log_text(&m," actualVt="); log_uint(&m,actualVtable);
    log_text(&m," actualVtRva="); log_uint(&m,ab_module_rva(actualVtable));
    log_text(&m," mode="); log_uint(&m,mode);
    log_text(&m,"\r\n"); append_log(&m);
}

static void ab_state_snapshot(void* stateObject) {
    BYTE* state=(BYTE*)stateObject;
    BYTE* owner=ab_read_ptr(state,0x30u);
    BYTE* player=ab_read_ptr(state,0x298u);
    BYTE* mover=ab_read_ptr(owner,0xC0u);
    BYTE* plugin=(BYTE*)ab_find_ridevehicle_plugin(state);
    LONG gate=player && readable_range(player+0x739Du,1u) ? player[0x739Du] : -1;
    LONG suppressor=state && readable_range(state+0xAB0u,1u) ? state[0xAB0u] : -1;
    int side=ab_side();
    LONG idx=__atomic_fetch_add(&g_abStateCalls,1,__ATOMIC_ACQ_REL);
    bool changed=__atomic_exchange_n(&g_abLastMover,(UINT64)mover,__ATOMIC_ACQ_REL)!=(UINT64)mover;
    changed|=__atomic_exchange_n(&g_abLastPlugin,(UINT64)plugin,__ATOMIC_ACQ_REL)!=(UINT64)plugin;
    changed|=__atomic_exchange_n(&g_abLastGate,gate,__ATOMIC_ACQ_REL)!=gate;
    changed|=__atomic_exchange_n(&g_abLastSuppressor,suppressor,__ATOMIC_ACQ_REL)!=suppressor;
    changed|=__atomic_exchange_n(&g_abLastSide,side,__ATOMIC_ACQ_REL)!=side;
    if (!changed && idx>=4 && idx%180!=0) return;
    LogBuffer m; log_init(&m); log_prefix(&m);
    log_text(&m,"AB STATE side="); log_uint(&m,side);
    log_text(&m," qpidHits="); log_uint(&m,(UINT64)__atomic_load_n(&g_v151QpidHits,__ATOMIC_ACQUIRE));
    log_text(&m," #"); log_uint(&m,(UINT64)idx);
    log_text(&m," state="); log_uint(&m,(UINT64)state);
    log_text(&m," owner30="); log_uint(&m,(UINT64)owner);
    UINT64 ownerVtable=ab_read_u64(owner,0);
    log_text(&m," ownerVt="); log_uint(&m,ownerVtable);
    log_text(&m," ownerVtRva="); log_uint(&m,ab_module_rva(ownerVtable));
    log_text(&m," moverC0="); log_uint(&m,(UINT64)mover);
    UINT64 moverVtable=ab_read_u64(mover,0);
    log_text(&m," moverVt="); log_uint(&m,moverVtable);
    log_text(&m," moverVtRva="); log_uint(&m,ab_module_rva(moverVtable));
    log_text(&m," player298="); log_uint(&m,(UINT64)player);
    log_text(&m," gate739D="); log_uint(&m,(UINT64)gate);
    log_text(&m," suppressAB0="); log_uint(&m,(UINT64)suppressor);
    log_text(&m," plugin="); log_uint(&m,(UINT64)plugin);
    if (plugin) {
        log_text(&m," active8="); log_uint(&m,plugin[8]);
        log_text(&m," disableA="); log_uint(&m,plugin[0xA]);
    }
    log_text(&m," phase5In="); log_uint(&m,(UINT64)__atomic_load_n(&g_abPhase5Calls[0],__ATOMIC_ACQUIRE));
    log_text(&m," phase5Out="); log_uint(&m,(UINT64)__atomic_load_n(&g_abPhase5Calls[1],__ATOMIC_ACQUIRE));
    log_text(&m," rideOnIn="); log_uint(&m,(UINT64)__atomic_load_n(&g_abRideOnCalls[0],__ATOMIC_ACQUIRE));
    log_text(&m," rideOnOut="); log_uint(&m,(UINT64)__atomic_load_n(&g_abRideOnCalls[1],__ATOMIC_ACQUIRE));
    log_text(&m," reserveIn="); log_uint(&m,(UINT64)__atomic_load_n(&g_abReserveCalls[0],__ATOMIC_ACQUIRE));
    log_text(&m," reserveOut="); log_uint(&m,(UINT64)__atomic_load_n(&g_abReserveCalls[1],__ATOMIC_ACQUIRE));
    ab_log_vehicle(&m,(BYTE*)__atomic_load_n(&g_diagCandidateVehicle,__ATOMIC_ACQUIRE));
    log_text(&m,"\r\n"); append_log(&m);
}

static void __fastcall ab_state_gate(void* stateObject) {
    if (g_abNativeStateGate) g_abNativeStateGate(stateObject);
    ab_state_snapshot(stateObject);
}

static void __fastcall ab_state_tick(void* stateObject) {
    ab_state_snapshot(stateObject);
    if (g_abNativeStateTick) g_abNativeStateTick(stateObject);
}

static void __fastcall ab_phase5_dispatch(void* state) {
    int side=ab_side();
    LONG idx=__atomic_fetch_add(&g_abPhase5Calls[side],1,__ATOMIC_ACQ_REL);
    if (idx<6 || idx%180==0) {
        BYTE* player=ab_read_ptr((BYTE*)state,0x298u);
        BYTE* plugin=(BYTE*)ab_find_ridevehicle_plugin((BYTE*)state);
        LogBuffer m; log_init(&m); log_prefix(&m);
        log_text(&m,"AB PHASE5 side="); log_uint(&m,side);
        log_text(&m," #"); log_uint(&m,(UINT64)idx);
        log_text(&m," state="); log_uint(&m,(UINT64)state);
        log_text(&m," gate739D="); log_uint(&m,player && readable_range(player+0x739Du,1u) ? player[0x739Du] : 0xffu);
        log_text(&m," suppressAB0="); log_uint(&m,state && readable_range((BYTE*)state+0xAB0u,1u) ? ((BYTE*)state)[0xAB0u] : 0xffu);
        log_text(&m," plugin="); log_uint(&m,(UINT64)plugin);
        if (plugin && readable_range((BYTE*)plugin,0xBu)) {
            log_text(&m," active8="); log_uint(&m,((BYTE*)plugin)[8]);
            log_text(&m," disableA="); log_uint(&m,((BYTE*)plugin)[0xA]);
        }
        log_text(&m,"\r\n"); append_log(&m);
    }
    if (g_abNativePhase5) g_abNativePhase5(state);
}

static void __fastcall ab_rideon_event6(void* state) {
    int side=ab_side();
    LONG idx=__atomic_fetch_add(&g_abRideOnCalls[side],1,__ATOMIC_ACQ_REL);
    if (idx<24) {
        LogBuffer m; log_init(&m); log_prefix(&m);
        log_text(&m,"AB RIDEON6 entry side="); log_uint(&m,side);
        log_text(&m," #"); log_uint(&m,(UINT64)idx);
        log_text(&m," state="); log_uint(&m,(UINT64)state);
        log_text(&m," player98="); log_uint(&m,ab_read_u64((BYTE*)state,0x98u));
        ab_log_vehicle(&m,(BYTE*)__atomic_load_n(&g_diagCandidateVehicle,__ATOMIC_ACQUIRE));
        log_text(&m,"\r\n"); append_log(&m);
    }
    if (g_abNativeRideOn) g_abNativeRideOn(state);
}

static BYTE __fastcall ab_reservation(void* vehicle,void* owner,BYTE acquire,void* context,BYTE fifth) {
    int side=ab_side();
    LONG idx=__atomic_fetch_add(&g_abReserveCalls[side],1,__ATOMIC_ACQ_REL);
    UINT64 beforeFlags=ab_read_u64((BYTE*)vehicle,0x98u);
    UINT32 beforeState=ab_read_u32((BYTE*)vehicle,0x4E4u);
    UINT32 beforeOccupied=ab_read_u32((BYTE*)vehicle,0x4FCu);
    BYTE result=g_abNativeReservation ? g_abNativeReservation(vehicle,owner,acquire,context,fifth) : 0;
    if (idx<24) {
        LogBuffer m; log_init(&m); log_prefix(&m);
        log_text(&m,"AB RESERVE side="); log_uint(&m,side);
        log_text(&m," #"); log_uint(&m,(UINT64)idx);
        log_text(&m," acquire="); log_uint(&m,acquire);
        log_text(&m," result="); log_uint(&m,result);
        log_text(&m," preBit9="); log_uint(&m,(beforeFlags&0x200ull)?1:0);
        log_text(&m," preState="); log_uint(&m,beforeState);
        log_text(&m," preOccupied="); log_uint(&m,beforeOccupied);
        log_text(&m," ownerArg="); log_uint(&m,(UINT64)owner);
        log_text(&m," contextArg="); log_uint(&m,(UINT64)context);
        ab_log_vehicle(&m,(BYTE*)vehicle);
        log_text(&m,"\r\n"); append_log(&m);
    }
    return result;
}

static bool ab_patch_vtable(BYTE* base,UINT32 tableRva,SIZE_T slot,UINT32 expectedRva,void* wrapper,void** original) {
    BYTE** cell=(BYTE**)(base+tableRva+slot);
    BYTE* expected=base+expectedRva;
    if (!readable_range(cell,sizeof(void*)) || *cell!=expected) return false;
    *original=expected;
    DWORD oldProtection=0;
    if (!VirtualProtect(cell,sizeof(void*),PAGE_EXECUTE_READWRITE,&oldProtection)) return false;
    *cell=(BYTE*)wrapper;
    FlushInstructionCache((HANDLE)(INT64)-1,cell,sizeof(void*));
    bool installed=*cell==(BYTE*)wrapper;
    if (!installed) *cell=expected;
    DWORD ignored=0; VirtualProtect(cell,sizeof(void*),oldProtection,&ignored);
    return installed;
}

static bool ab_install_phase5(BYTE* base) {
    BYTE* target=base+0xE15D30u;
    static const BYTE expected[16]={
        0x40,0x56,0x48,0x83,0xEC,0x30,0x48,0x8B,
        0x81,0x98,0x02,0x00,0x00,0x48,0x8B,0xF1
    };
    if (!readable_range(target,sizeof(expected)) || !bytes_equal(target,expected,sizeof(expected))) return false;
    BYTE* trampoline=(BYTE*)VirtualAlloc(0,32u,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);
    if (!trampoline) return false;
    memcpy(trampoline,expected,sizeof(expected));
    static const BYTE absoluteJump[6]={0xFF,0x25,0,0,0,0};
    memcpy(trampoline+16u,absoluteJump,sizeof(absoluteJump));
    UINT64 continuation=(UINT64)(target+sizeof(expected));
    memcpy(trampoline+22u,&continuation,sizeof(continuation));
    FlushInstructionCache((HANDLE)(INT64)-1,trampoline,30u);
    g_abNativePhase5=(AbPhase5Fn)trampoline;
    BYTE replacement[16]={0xFF,0x25,0,0,0,0,0,0,0,0,0,0,0,0,0x90,0x90};
    UINT64 wrapper=(UINT64)&ab_phase5_dispatch;
    memcpy(replacement+6u,&wrapper,sizeof(wrapper));
    if (!freeze_for_patch(target,sizeof(replacement))) return false;
    DWORD oldProtection=0;
    if (!VirtualProtect(target,sizeof(replacement),PAGE_EXECUTE_READWRITE,&oldProtection)) {
        unfreeze_after_patch(); return false;
    }
    memcpy(target,replacement,sizeof(replacement));
    FlushInstructionCache((HANDLE)(INT64)-1,target,sizeof(replacement));
    bool installed=bytes_equal(target,replacement,sizeof(replacement));
    if (!installed) {
        memcpy(target,expected,sizeof(expected));
        FlushInstructionCache((HANDLE)(INT64)-1,target,sizeof(expected));
    }
    DWORD ignored=0; VirtualProtect(target,sizeof(replacement),oldProtection,&ignored);
    unfreeze_after_patch();
    return installed;
}

static bool ab_install(HMODULE executable) {
    BYTE* base=(BYTE*)executable;
    BYTE* reservationCall=base+0xF9AD51u;
    BYTE* reservationTarget=base+0x1F6BDE0u;
    int originalDisp=0;
    if (!v151_validate_call(reservationCall,reservationTarget,&originalDisp)) return false;
    if (!ab_patch_vtable(base,0x3233158u,0x38u,0xDC0470u,(void*)&ab_mover_switch,(void**)&g_abNativeMover)) return false;
    if (!ab_patch_vtable(base,0x323DC48u,0x38u,0xE0E060u,(void*)&ab_state_gate,(void**)&g_abNativeStateGate)) return false;
    if (!ab_patch_vtable(base,0x323DC48u,0x40u,0xE0E230u,(void*)&ab_state_tick,(void**)&g_abNativeStateTick)) return false;
    if (!ab_install_phase5(base)) return false;
    if (!ab_patch_vtable(base,0x325B6F0u,0xD8u,0xF9A390u,(void*)&ab_rideon_event6,(void**)&g_abNativeRideOn)) return false;
    g_abNativeReservation=(AbReservationFn)reservationTarget;
    BYTE* relay=(BYTE*)allocate_call_relay_near(reservationCall,(void*)&ab_reservation);
    return relay && v151_patch_call(reservationCall,relay,originalDisp);
}
#endif

#ifdef COFFIN_MOUNT_TRACE
#include "coffin_mount_trace.inc"
#endif

#undef worker
#pragma clang diagnostic pop
static DWORD __stdcall worker(LPVOID) {
    read_configuration();

    // v1.81 production path: normal RideVehicle frontend + plugin activation root + read-only A8 builder proof. All old
    // RideFloater E3/C8/input experiments remain compiled for auditability but
    // are deliberately not installed by this worker.
    g_enableNetworkTrace = 0;
    g_enableMountCallerTrace = 0;
    g_enableNetworkStateTrace = 0;
    g_enableQpidBorderEscapeClamp = 0;
    g_enableCoffinE3Descriptor = 0;
    g_enableNativeE3Producer = 0;
    g_enableCoffinCandidateCursorTrace = 0;
    g_enableCoffinCandidateInsertTrace = 0;
    g_enableCoffinCacheInsertTrace = 0;
    g_testClearActivity5D0Bit2 = 0;
    g_testHoldRideScalar = 0;
    g_enableActivity5D0XrefScan = 0;

    log_line("DS2 Coffin Board v1.83.0 NETWORK AVAILABILITY FIX loaded.");
    log_line("V181 ROOTS: v1.54 crossing + normal DSPlayerRideVehicleActionPlugin frontend + pre-collector plugin keepalive + strict real-DSBaggageCarrier linked-object semantics.");
    log_line("V182 SCOPE: 1006010 F4D290 exception requires an exact live Coffin type-3 capture. BB is a transient inhibit signal; neither BB nor QPID history is used to label the player's geographical side. Native action descriptors and state progression remain authoritative.");
    if (!g_enabled) { log_line("Disabled in INI; no game memory was changed."); return 0; }

    HMODULE executable = GetModuleHandleW(0);
    if (!executable || !validate_target_build(executable)) {
        log_line("ERROR: Unsupported DS2.exe. Expected DS2.exe v1.10.89.0; no patch applied.");
        return 0;
    }
    g_executable = executable;

    bool carrierLink = install_floating_carrier_link_preserve(executable);
    bool carrierDetach = install_floating_carrier_detach_guard(executable);
    bool carrierWarn = install_floating_carrier_warning_guard(executable);
    bool carrierGate = install_floating_carrier_gate(executable);
    if (!(carrierLink && carrierDetach && carrierWarn && carrierGate)) {
        log_line("ERROR: one or more strict Floating Carrier hooks failed; aborting before RideVehicle root.");
        return 0;
    }
    log_line("V179 FLOATING CARRIER: exceptions apply only to a linked object with a real DSBaggageCarrierComponent; strand/Sam links keep vanilla rejection semantics.");

    if (!install_drive_force_hook(executable)) {
        log_line("ERROR: exact Coffin drive-force hook anchor/vtable did not match; aborting.");
        return 0;
    }

    if (!install_v152_qpid_scope(executable)) {
        log_line("ERROR: F43A90->F43700 QPID boundary callsite mismatch; crossing root not installed.");
        return 0;
    }
    log_line("V154 QPID PHYSICS ROOT installed: exact F43700 wall-hit state +514/+624/+62D neutralized.");

    if (!install_v154_entity_boundary_bypass(executable)) {
        log_line("ERROR: DSVehicleCoffin 1F76D80 second-QPID recovery branch anchor mismatch; crossing entity root not installed.");
        return 0;
    }
    log_line("V154 ENTITY CROSSING ROOT installed: second desired-position QPID hit follows native no-recovery path; first QPID query remains native.");

    if (!install_v183_coffin_availability(executable)) {
        log_line("ERROR: Coffin-only 1F7731A availability veto anchor mismatch; outside mount fix not installed.");
        return 0;
    }
    if (g_v183AvailabilityVetoRemoved)
        log_line("V183 AVAILABILITY ROOT installed: Coffin-only 1F7731A network veto removed. Native 1F5F0A0 eligibility, actual +126E coverage and A8 descriptor remain native.");

    if (!install_v167_coffin_entity_capture(executable)) {
        log_line("ERROR: DSVehicleCoffin vtable +0x180 capture anchor mismatch.");
        return 0;
    }
    log_line("V179 COFFIN ENTITY CAPTURE installed: passive exact DSVehicleCoffin instance capture; native method preserved.");

    if (!install_v176_ridevehicle_root(executable)) {
        log_line("ERROR: RideVehicle 10047E3/10047EA network-root anchor mismatch; normal outside Coffin mount not installed.");
        return 0;
    }
    log_line("V182 RIDEVEHICLE ROOT installed: local 10047EA inhibit branch opens for a live exact Coffin; global +0x2264BB is never modified.");

    if (!install_v177_ridevehicle_cancel_scope(executable)) {
        log_line("ERROR: RideVehicle 10060C4 lifecycle-disable call anchor mismatch; normal outside Coffin mount lifecycle not installed.");
        return 0;
    }
    log_line("V182 RIDEVEHICLE ACTIVATION installed: 10060C4 F4D290 exception requires a live exact Coffin type-3; native 10079D0 owns target selection.");

    if (!install_v179_ridevehicle_action_builder_observer(executable)) {
        log_line("ERROR: RideVehicle vtable +0x110 native 10079D0 builder observer anchor mismatch; aborting to keep this validation build self-contained.");
        return 0;
    }
    log_line("V182 RIDEVEHICLE A8 BUILDER observer installed: bounded descriptor transitions and periodic samples, without BB or QPID filtering; native descriptor untouched.");
#ifdef COFFIN_MOUNT_TRACE
    if (!install_mount_trace(executable)) {
        log_line("ERROR: mount trace anchor mismatch; stop this test and keep the log.");
        return 0;
    }
    log_line("MOUNT TRACE installed: native frontend, activation/deactivation, sampled phase5, RideOn6 and reservation result. No E0E230 or input hook.");
#endif
#ifdef COFFIN_AB_DIAGNOSTIC
    if (!ab_install(executable)) {
        log_line("ERROR: AB diagnostic hook anchor mismatch; stop this test and keep the log.");
        return 0;
    }
    log_line("AB DIAGNOSTIC installed: state gate/tick, mover switch, exact phase-5 dispatcher, RideOn event 6, and reservation call. All observers forward to native code.");
#endif

    // Explicitly leave all alternate RideFloater mount machinery native/off.
    g_networkMountGateInstalled = 0;
    g_networkTraceInstalled = 0;
    g_coffinE3RecordProducerScopeInstalled = 0;
    g_v164ProviderInstalled = 0;
    g_v151EdfePassiveInstalled = 0;
    g_coffinBit20EventHandlerProbeInstalled = 0;
    g_coffinCacheInsertTraceInstalled = 0;
    log_line("V181 RIDEFLOATER EXPERIMENTS: disabled. FFFD03, bit20 owner, E3 producer/provider, EDFE10 observer/force and C8 retry/replay are untouched.");

    UINT64 globalAddress = find_streaming_manager_global(executable);
    if (!globalAddress || !readable_range((void*)globalAddress, sizeof(void*))) {
        log_line("ERROR: exact StreamingManager anchor did not match; runtime roots remain installed but speed resource patch unavailable.");
        return 0;
    }
    void** managerGlobal = (void**)globalAddress;
    void* manager = 0;
    for (int attempt = 0; attempt < 1200; ++attempt) {
        if (readable_range(managerGlobal, sizeof(void*))) manager = *managerGlobal;
        if (manager) break;
        Sleep(50);
    }
    if (!manager) { log_line("ERROR: StreamingManager did not initialize within 60 seconds."); return 0; }
    if (!register_streaming_listener(manager)) {
        log_line("ERROR: could not register resource listener; runtime roots remain installed but speed resource patch unavailable.");
        return 0;
    }

    {
        LogBuffer m; log_init(&m); log_prefix(&m);
        log_text(&m, "V181 active. speed="); log_uint(&m, (UINT64)g_speedPercent);
        log_text(&m, "%, acceleration="); log_uint(&m, (UINT64)g_accelerationPercent);
        log_text(&m, "%, drive="); log_uint(&m, (UINT64)g_driveForcePercent);
        log_text(&m, "%, AllowFloatingCarrier="); log_uint(&m, (UINT64)(g_allowFloatingCarrier != 0));
        log_text(&m, ", QpidPhysicsRoot="); log_uint(&m, (UINT64)g_v152QpidInstalled);
        log_text(&m, ", EntityRecoveryBypass="); log_uint(&m, (UINT64)g_v154EntityBypassInstalled);
        log_text(&m, ", CoffinAvailabilityVetoRemoved="); log_uint(&m, (UINT64)g_v183AvailabilityVetoRemoved);
        log_text(&m, ", DirectVehicleCapture="); log_uint(&m, (UINT64)g_v167CoffinUpdateInstalled);
        log_text(&m, ", RideVehicleRoot="); log_uint(&m, (UINT64)g_v176RideVehicleRootInstalled);
        log_text(&m, ", RideVehicleLifecycle="); log_uint(&m, (UINT64)g_v177RideVehicleCancelScopeInstalled);
        log_text(&m, ", RideVehicleBuilderObserver="); log_uint(&m, (UINT64)g_v179RideVehicleBuilderObserverInstalled);
        log_text(&m, ", StrictCarrierGate="); log_uint(&m, (UINT64)g_carrierGateInstalled);
        log_text(&m, ", StrictCarrierLink="); log_uint(&m, (UINT64)g_carrierLinkPreserveInstalled);
        log_text(&m, ", RideFloaterMountGate=0, Bit20=0, E3=0, C8=0, EDFE10=0");
        log_text(&m, ".\r\n"); append_log(&m);
    }

    log_line("LIFECYCLE: waiting for Coffin resources across menu/load delays.");
    for (UINT32 poll = 0; !patch_complete_acquire(); ++poll) {
        if ((poll % 4u) == 0u) {
            inspect_already_loaded_group(g_streamingSystem, COFFIN_PHYSICS_GROUP_ID);
            inspect_already_loaded_group(g_streamingSystem, COFFIN_RIDE_CONFIG_GROUP_ID);
        }
        if (!patch_complete_acquire()) Sleep(250);
    }
    if (!unregister_streaming_listener()) {
        log_line("LIFECYCLE ERROR: native resource listener removal failed; restart DS2 before changing this ASI.");
        return 0;
    }
    log_line("LIFECYCLE: resource patch complete; listener removed after successful patch, independent of menu/load duration.");
    return 0;
}

extern "C" __declspec(dllexport) void InitializeASI() {}
extern "C" int __stdcall DllMain(HINSTANCE instance,DWORD reason,LPVOID){if(reason==DLL_PROCESS_ATTACH){if(!is_main_game_process())return TRUE;g_module=(HMODULE)instance;DisableThreadLibraryCalls(g_module);g_mutex=CreateMutexW(0,FALSE,L"Local\\DS2_CoffinBoardAllTerrainSpeed_v1_77_0_ridevehicle_lifecycle");if(!g_mutex)return TRUE;if(GetLastError()==ERROR_ALREADY_EXISTS){CloseHandle(g_mutex);g_mutex=0;return TRUE;}HANDLE thread=CreateThread(0,0,worker,0,0,0);if(thread)CloseHandle(thread);}else if(reason==DLL_PROCESS_DETACH){if(g_mutex){CloseHandle(g_mutex);g_mutex=0;}}return TRUE;}
