#pragma once
using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;
using i32 = int;
using i64 = long long;
using DWORD = unsigned long;
using SIZE_T = u64;
using BOOL = int;
using HANDLE = void*;
using HMODULE = void*;
struct MemoryInfo {
    void* base;
    void* allocation;
    DWORD allocationProtect;
    DWORD alignment;
    SIZE_T size;
    DWORD state;
    DWORD protect;
    DWORD type;
    DWORD padding;
};
extern "C" {
__declspec(dllimport) HMODULE __stdcall GetModuleHandleW(const wchar_t*);
__declspec(dllimport) BOOL __stdcall GetModuleHandleExW(DWORD, const wchar_t*, HMODULE*);
__declspec(dllimport) DWORD __stdcall GetModuleFileNameW(HMODULE, wchar_t*, DWORD);
__declspec(dllimport) BOOL __stdcall DisableThreadLibraryCalls(HMODULE);
__declspec(dllimport) HANDLE __stdcall CreateThread(void*, SIZE_T, DWORD (__stdcall*)(void*), void*, DWORD, DWORD*);
__declspec(dllimport) BOOL __stdcall CloseHandle(HANDLE);
__declspec(dllimport) HANDLE __stdcall GetCurrentProcess();
__declspec(dllimport) BOOL __stdcall ReadProcessMemory(HANDLE, const void*, void*, SIZE_T, SIZE_T*);
__declspec(dllimport) SIZE_T __stdcall VirtualQuery(const void*, MemoryInfo*, SIZE_T);
__declspec(dllimport) void* __stdcall VirtualAlloc(void*, SIZE_T, DWORD, DWORD);
__declspec(dllimport) BOOL __stdcall VirtualFree(void*, SIZE_T, DWORD);
__declspec(dllimport) BOOL __stdcall VirtualProtect(void*, SIZE_T, DWORD, DWORD*);
__declspec(dllimport) BOOL __stdcall FlushInstructionCache(HANDLE, const void*, SIZE_T);
__declspec(dllimport) DWORD __stdcall GetPrivateProfileStringW(const wchar_t*, const wchar_t*, const wchar_t*, wchar_t*, DWORD, const wchar_t*);
__declspec(dllimport) HANDLE __stdcall CreateFileW(const wchar_t*, DWORD, DWORD, void*, DWORD, DWORD, HANDLE);
__declspec(dllimport) BOOL __stdcall WriteFile(HANDLE, const void*, DWORD, DWORD*, void*);
unsigned char _InterlockedCompareExchange128(volatile i64*, i64, i64, i64*);
long _InterlockedCompareExchange(volatile long*, long, long);
}
#pragma intrinsic(_InterlockedCompareExchange128)
#pragma intrinsic(_InterlockedCompareExchange)

constexpr DWORD kReadWrite = 0x04;
constexpr DWORD kExecuteRead = 0x20;
constexpr DWORD kExecuteReadWrite = 0x40;
constexpr DWORD kCommit = 0x1000;
constexpr DWORD kReserve = 0x2000;
constexpr DWORD kRelease = 0x8000;
