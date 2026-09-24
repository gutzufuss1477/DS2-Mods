#pragma once
#include "core.hpp"
using craft::u8;using craft::u16;using craft::u32;using craft::u64;using craft::i32;
using HANDLE=void*;using HMODULE=void*;using DWORD=u32;using BOOL=int;using SIZE_T=unsigned long long;using WCHAR=wchar_t;
#define WINAPI __stdcall
#define API extern "C" __declspec(dllimport)
API HMODULE WINAPI GetModuleHandleW(const WCHAR*);
API BOOL WINAPI GetModuleHandleExW(DWORD,const WCHAR*,HMODULE*);
API DWORD WINAPI GetModuleFileNameW(HMODULE,WCHAR*,DWORD);
API HANDLE WINAPI GetCurrentProcess();
API DWORD WINAPI GetCurrentProcessId();
API DWORD WINAPI GetCurrentThreadId();
API HANDLE WINAPI CreateThread(void*,SIZE_T,DWORD(WINAPI*)(void*),void*,DWORD,DWORD*);
API BOOL WINAPI DisableThreadLibraryCalls(HMODULE);
API BOOL WINAPI CloseHandle(HANDLE);
API HANDLE WINAPI CreateFileW(const WCHAR*,DWORD,DWORD,void*,DWORD,DWORD,HANDLE);
API BOOL WINAPI ReadFile(HANDLE,void*,DWORD,DWORD*,void*);
API BOOL WINAPI WriteFile(HANDLE,const void*,DWORD,DWORD*,void*);
API BOOL WINAPI GetFileSizeEx(HANDLE,long long*);
API BOOL WINAPI SetFilePointerEx(HANDLE,long long,long long*,DWORD);
API BOOL WINAPI FlushFileBuffers(HANDLE);
API BOOL WINAPI ReadProcessMemory(HANDLE,const void*,void*,SIZE_T,SIZE_T*);
API void* WINAPI VirtualAlloc(void*,SIZE_T,DWORD,DWORD);
API BOOL WINAPI VirtualFree(void*,SIZE_T,DWORD);
API BOOL WINAPI VirtualProtect(void*,SIZE_T,DWORD,DWORD*);
API BOOL WINAPI FlushInstructionCache(HANDLE,const void*,SIZE_T);
API DWORD WINAPI GetLastError();
API u64 WINAPI GetTickCount64();
API void WINAPI Sleep(DWORD);
struct ThreadEntry {DWORD size,usage,tid,pid;i32 basePriority,deltaPriority;DWORD flags;};
API HANDLE WINAPI CreateToolhelp32Snapshot(DWORD,DWORD);
API BOOL WINAPI Thread32First(HANDLE,ThreadEntry*);
API BOOL WINAPI Thread32Next(HANDLE,ThreadEntry*);
API HANDLE WINAPI OpenThread(DWORD,BOOL,DWORD);
API DWORD WINAPI SuspendThread(HANDLE);
API DWORD WINAPI ResumeThread(HANDLE);
// AMD64 CONTEXT: integer/control prefix at documented offsets; extended tail retained.
struct alignas(16) Context {u64 home[6];u32 flags,mxcsr;u16 seg[6];u32 eflags;u64 debug[6];u64 rax,rcx,rdx,rbx,rsp,rbp,rsi,rdi,r8,r9,r10,r11,r12,r13,r14,r15,rip;u8 extended[976];};
static_assert(__builtin_offsetof(Context,rip)==248 && sizeof(Context)==1232,"Windows AMD64 CONTEXT layout");
API BOOL WINAPI GetThreadContext(HANDLE,Context*);
API u8 WINAPI TryAcquireSRWLockShared(void*);
API void WINAPI ReleaseSRWLockShared(void*);
#define InvalidHandle ((HANDLE)(long long)-1)
API DWORD WINAPI GetProcessIdOfThread(HANDLE);
API DWORD WINAPI GetThreadId(HANDLE);
API BOOL WINAPI GetExitCodeThread(HANDLE,DWORD*);
