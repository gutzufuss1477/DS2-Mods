#pragma once
#ifndef MINIMAL_WINDOWS_COMPAT_H
#define MINIMAL_WINDOWS_COMPAT_H
#ifndef __cplusplus
typedef unsigned short wchar_t;
#endif
#define WINAPI __stdcall
#define CALLBACK __stdcall
#define __cdecl
#define TRUE 1
#define FALSE 0
#define NULL ((void*)0)
typedef signed char INT8; typedef short INT16; typedef int INT32; typedef long long INT64;
typedef unsigned char UINT8; typedef unsigned short UINT16; typedef unsigned int UINT32; typedef unsigned long long UINT64; typedef UINT32* PUINT32;
typedef unsigned char u8; typedef unsigned short u16; typedef unsigned int u32; typedef unsigned long long u64; typedef signed int i32; typedef signed long long i64; typedef wchar_t WCHAR;
typedef void* LPVOID; typedef const void* LPCVOID; typedef void* HANDLE; typedef void* HMODULE; typedef const wchar_t* LPCWSTR; typedef wchar_t* LPWSTR; typedef const char* LPCSTR; typedef unsigned char BYTE; typedef BYTE* LPBYTE; typedef unsigned char* PUCHAR; typedef unsigned long ULONG; typedef unsigned short WORD; typedef unsigned int DWORD; typedef int LONG; typedef unsigned long long ULONG_PTR; typedef unsigned long long DWORD_PTR; typedef unsigned long long SIZE_T; typedef int BOOL; typedef unsigned int UINT; typedef void VOID; typedef DWORD* LPDWORD; typedef UINT* PUINT; typedef UINT64 DWORD64;
typedef struct _LARGE_INTEGER { INT64 QuadPart; } LARGE_INTEGER;
typedef struct _MEMORY_BASIC_INFORMATION { LPVOID BaseAddress; LPVOID AllocationBase; DWORD AllocationProtect; UINT16 PartitionId; SIZE_T RegionSize; DWORD State; DWORD Protect; DWORD Type; } MEMORY_BASIC_INFORMATION;
typedef struct _CONTEXT { u8 raw[1232]; } CONTEXT;
typedef struct _EXCEPTION_RECORD { DWORD ExceptionCode; DWORD ExceptionFlags; struct _EXCEPTION_RECORD* ExceptionRecord; LPVOID ExceptionAddress; DWORD NumberParameters; ULONG_PTR ExceptionInformation[15]; } EXCEPTION_RECORD;
typedef struct _EXCEPTION_POINTERS { EXCEPTION_RECORD* ExceptionRecord; CONTEXT* ContextRecord; } EXCEPTION_POINTERS;
typedef LONG (WINAPI *PVECTORED_EXCEPTION_HANDLER)(EXCEPTION_POINTERS*);
typedef DWORD (WINAPI *LPTHREAD_START_ROUTINE)(LPVOID);
#define INVALID_HANDLE_VALUE ((HANDLE)(long long)-1)
#define MEM_COMMIT 0x1000u
#define PAGE_READONLY 2u
#define PAGE_READWRITE 4u
#define PAGE_WRITECOPY 8u
#define PAGE_EXECUTE 0x10u
#define PAGE_EXECUTE_READ 0x20u
#define PAGE_EXECUTE_READWRITE 0x40u
#define PAGE_EXECUTE_WRITECOPY 0x80u
#define PAGE_GUARD 0x100u
#define FILE_SHARE_READ 1u
#define GENERIC_WRITE 0x40000000u
#define CREATE_ALWAYS 2u
#define FILE_ATTRIBUTE_NORMAL 0x80u
#define EXCEPTION_BREAKPOINT 0x80000003u
#define EXCEPTION_CONTINUE_EXECUTION ((LONG)-1)
#define EXCEPTION_CONTINUE_SEARCH ((LONG)0)
#ifdef __cplusplus
extern "C" {
#endif
void* __cdecl memcpy(void*,const void*,SIZE_T); void* __cdecl memset(void*,int,SIZE_T);
LPVOID WINAPI AddVectoredExceptionHandler(ULONG,PVECTORED_EXCEPTION_HANDLER); ULONG WINAPI RemoveVectoredExceptionHandler(LPVOID);
HMODULE WINAPI GetModuleHandleW(LPCWSTR); DWORD WINAPI GetModuleFileNameW(HMODULE,LPWSTR,DWORD); HANDLE WINAPI GetCurrentProcess(void); DWORD WINAPI GetCurrentThreadId(void); HANDLE WINAPI CreateThread(LPVOID,SIZE_T,LPTHREAD_START_ROUTINE,LPVOID,DWORD,DWORD*); BOOL WINAPI CloseHandle(HANDLE); void WINAPI Sleep(DWORD); SIZE_T WINAPI VirtualQuery(LPCVOID,MEMORY_BASIC_INFORMATION*,SIZE_T); BOOL WINAPI VirtualProtect(LPVOID,SIZE_T,DWORD,DWORD*); BOOL WINAPI FlushInstructionCache(HANDLE,LPCVOID,SIZE_T); BOOL WINAPI ReadProcessMemory(HANDLE,LPCVOID,LPVOID,SIZE_T,SIZE_T*); HANDLE WINAPI CreateFileW(LPCWSTR,DWORD,DWORD,LPVOID,DWORD,DWORD,HANDLE); BOOL WINAPI WriteFile(HANDLE,LPCVOID,DWORD,DWORD*,LPVOID); BOOL WINAPI FlushFileBuffers(HANDLE); BOOL WINAPI DisableThreadLibraryCalls(HMODULE); BOOL WINAPI QueryPerformanceCounter(LARGE_INTEGER*); BOOL WINAPI QueryPerformanceFrequency(LARGE_INTEGER*); DWORD WINAPI GetPrivateProfileStringW(LPCWSTR,LPCWSTR,LPCWSTR,LPWSTR,DWORD,LPCWSTR);
#ifdef __cplusplus
}
#endif
static inline LONG InterlockedCompareExchange(volatile LONG*p,LONG e,LONG c){return __sync_val_compare_and_swap(p,c,e);} static inline LONG InterlockedExchange(volatile LONG*p,LONG v){return __sync_lock_test_and_set(p,v);} static inline LONG InterlockedIncrement(volatile LONG*p){return __sync_add_and_fetch(p,1);}
#endif
