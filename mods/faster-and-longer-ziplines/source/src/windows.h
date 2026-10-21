#pragma once
#ifndef DS2_ZIPLINE_RANGE_WINDOWS_H
#define DS2_ZIPLINE_RANGE_WINDOWS_H

#define WINAPI __stdcall
#define TRUE 1
#define FALSE 0

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef int s32;
typedef long long s64;
typedef unsigned int DWORD;
typedef unsigned long long SIZE_T;
typedef int BOOL;
typedef void* HANDLE;
typedef void* HMODULE;
typedef void* LPVOID;
typedef const void* LPCVOID;
typedef wchar_t WCHAR;
typedef const WCHAR* LPCWSTR;
typedef WCHAR* LPWSTR;
typedef DWORD* LPDWORD;
typedef DWORD (WINAPI *LPTHREAD_START_ROUTINE)(LPVOID);

struct MEMORY_BASIC_INFORMATION_X64 {
    LPVOID BaseAddress;
    LPVOID AllocationBase;
    DWORD AllocationProtect;
    u16 PartitionId;
    u16 Padding0;
    u64 RegionSize;
    DWORD State;
    DWORD Protect;
    DWORD Type;
    DWORD Padding1;
};

struct SYSTEM_INFO_X64 {
    u16 ProcessorArchitecture;
    u16 Reserved;
    DWORD PageSize;
    LPVOID MinimumApplicationAddress;
    LPVOID MaximumApplicationAddress;
    u64 ActiveProcessorMask;
    DWORD NumberOfProcessors;
    DWORD ProcessorType;
    DWORD AllocationGranularity;
    u16 ProcessorLevel;
    u16 ProcessorRevision;
};

#define INVALID_HANDLE_VALUE ((HANDLE)(s64)-1)
#define DLL_PROCESS_ATTACH 1u
#define MEM_COMMIT 0x1000u
#define MEM_RESERVE 0x2000u
#define MEM_FREE 0x10000u
#define MEM_RELEASE 0x8000u
#define PAGE_NOACCESS 0x01u
#define PAGE_READONLY 0x02u
#define PAGE_READWRITE 0x04u
#define PAGE_WRITECOPY 0x08u
#define PAGE_EXECUTE_READWRITE 0x40u
#define PAGE_EXECUTE_WRITECOPY 0x80u
#define PAGE_GUARD 0x100u
#define FILE_SHARE_READ 0x00000001u
#define GENERIC_WRITE 0x40000000u
#define CREATE_ALWAYS 2u
#define FILE_ATTRIBUTE_NORMAL 0x00000080u

extern "C" {
HMODULE WINAPI GetModuleHandleW(LPCWSTR);
DWORD WINAPI GetModuleFileNameW(HMODULE,LPWSTR,DWORD);
HANDLE WINAPI CreateThread(LPVOID,SIZE_T,LPTHREAD_START_ROUTINE,LPVOID,DWORD,LPDWORD);
BOOL WINAPI CloseHandle(HANDLE);
BOOL WINAPI DisableThreadLibraryCalls(HMODULE);
void WINAPI Sleep(DWORD);
void WINAPI GetSystemInfo(SYSTEM_INFO_X64*);
SIZE_T WINAPI VirtualQuery(LPCVOID,MEMORY_BASIC_INFORMATION_X64*,SIZE_T);
LPVOID WINAPI VirtualAlloc(LPVOID,SIZE_T,DWORD,DWORD);
BOOL WINAPI VirtualFree(LPVOID,SIZE_T,DWORD);
BOOL WINAPI VirtualProtect(LPVOID,SIZE_T,DWORD,LPDWORD);
HANDLE WINAPI GetCurrentProcess();
BOOL WINAPI FlushInstructionCache(HANDLE,LPCVOID,SIZE_T);
DWORD WINAPI GetPrivateProfileStringW(LPCWSTR,LPCWSTR,LPCWSTR,LPWSTR,DWORD,LPCWSTR);
HANDLE WINAPI CreateFileW(LPCWSTR,DWORD,DWORD,LPVOID,DWORD,DWORD,HANDLE);
BOOL WINAPI WriteFile(HANDLE,LPCVOID,DWORD,LPDWORD,LPVOID);
BOOL WINAPI FlushFileBuffers(HANDLE);
}

#endif
