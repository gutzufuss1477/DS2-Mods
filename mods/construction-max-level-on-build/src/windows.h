#pragma once
#ifndef DS2_CONSTRUCTION_MAX_LEVEL_WINDOWS_H
#define DS2_CONSTRUCTION_MAX_LEVEL_WINDOWS_H

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

#define INVALID_HANDLE_VALUE ((HANDLE)(s64)-1)
#define DLL_PROCESS_ATTACH 1u
#define MEM_COMMIT 0x1000u
#define PAGE_NOACCESS 0x01u
#define PAGE_READWRITE 0x04u
#define PAGE_WRITECOPY 0x08u
#define PAGE_EXECUTE 0x10u
#define PAGE_EXECUTE_READ 0x20u
#define PAGE_EXECUTE_READWRITE 0x40u
#define PAGE_EXECUTE_WRITECOPY 0x80u
#define PAGE_GUARD 0x100u
#define FILE_SHARE_READ 0x00000001u
#define GENERIC_WRITE 0x40000000u
#define CREATE_ALWAYS 2u
#define FILE_ATTRIBUTE_NORMAL 0x00000080u
#define TLS_OUT_OF_INDEXES 0xFFFFFFFFu
#define WAIT_OBJECT_0 0u

extern "C" {
HMODULE WINAPI GetModuleHandleW(LPCWSTR);
DWORD WINAPI GetModuleFileNameW(HMODULE,LPWSTR,DWORD);
HANDLE WINAPI CreateThread(LPVOID,SIZE_T,LPTHREAD_START_ROUTINE,LPVOID,DWORD,LPDWORD);
BOOL WINAPI CloseHandle(HANDLE);
BOOL WINAPI DisableThreadLibraryCalls(HMODULE);
void WINAPI Sleep(DWORD);
SIZE_T WINAPI VirtualQuery(LPCVOID,MEMORY_BASIC_INFORMATION_X64*,SIZE_T);
BOOL WINAPI VirtualProtect(LPVOID,SIZE_T,DWORD,LPDWORD);
DWORD WINAPI WaitForSingleObject(HANDLE,DWORD);
DWORD WINAPI TlsAlloc();
LPVOID WINAPI TlsGetValue(DWORD);
BOOL WINAPI TlsSetValue(DWORD,LPVOID);
DWORD WINAPI GetPrivateProfileStringW(LPCWSTR,LPCWSTR,LPCWSTR,LPWSTR,DWORD,LPCWSTR);
HANDLE WINAPI CreateFileW(LPCWSTR,DWORD,DWORD,LPVOID,DWORD,DWORD,HANDLE);
BOOL WINAPI WriteFile(HANDLE,LPCVOID,DWORD,LPDWORD,LPVOID);
BOOL WINAPI FlushFileBuffers(HANDLE);
}

#endif
