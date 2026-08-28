#pragma once
#ifndef DS2_WEAPONS_ANYWHERE_WINDOWS_H
#define DS2_WEAPONS_ANYWHERE_WINDOWS_H

#define WINAPI __stdcall
#define TRUE 1
#define FALSE 0

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
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
typedef DWORD (WINAPI *LPTHREAD_START_ROUTINE)(LPVOID);

#define INVALID_HANDLE_VALUE ((HANDLE)(long long)-1)
#define PAGE_EXECUTE_READWRITE 0x40u
#define FILE_SHARE_READ 0x00000001u
#define GENERIC_WRITE 0x40000000u
#define CREATE_ALWAYS 2u
#define FILE_ATTRIBUTE_NORMAL 0x00000080u

extern "C" {
HMODULE WINAPI GetModuleHandleW(LPCWSTR);
DWORD WINAPI GetModuleFileNameW(HMODULE,LPWSTR,DWORD);
HANDLE WINAPI GetCurrentProcess(void);
HANDLE WINAPI CreateThread(LPVOID,SIZE_T,LPTHREAD_START_ROUTINE,LPVOID,DWORD,DWORD*);
BOOL WINAPI CloseHandle(HANDLE);
BOOL WINAPI DisableThreadLibraryCalls(HMODULE);
BOOL WINAPI VirtualProtect(LPVOID,SIZE_T,DWORD,DWORD*);
BOOL WINAPI FlushInstructionCache(HANDLE,LPCVOID,SIZE_T);
DWORD WINAPI GetPrivateProfileStringW(LPCWSTR,LPCWSTR,LPCWSTR,LPWSTR,DWORD,LPCWSTR);
HANDLE WINAPI CreateFileW(LPCWSTR,DWORD,DWORD,LPVOID,DWORD,DWORD,HANDLE);
BOOL WINAPI WriteFile(HANDLE,LPCVOID,DWORD,DWORD*,LPVOID);
BOOL WINAPI FlushFileBuffers(HANDLE);
}

#endif
