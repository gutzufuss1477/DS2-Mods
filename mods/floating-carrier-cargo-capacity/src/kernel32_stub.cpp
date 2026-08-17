typedef void* HANDLE;
typedef void* HMODULE;
typedef void* LPVOID;
typedef const wchar_t* LPCWSTR;
typedef unsigned long DWORD;
typedef int BOOL;
typedef unsigned long long SIZE_T;
typedef DWORD* LPDWORD;
#define WINAPI __stdcall
extern "C" __declspec(dllexport) HMODULE WINAPI GetModuleHandleW(LPCWSTR){return 0;}
extern "C" __declspec(dllexport) DWORD WINAPI GetModuleFileNameW(HMODULE,wchar_t*,DWORD){return 0;}
extern "C" __declspec(dllexport) void WINAPI Sleep(DWORD){}
extern "C" __declspec(dllexport) HANDLE WINAPI CreateThread(LPVOID,SIZE_T,DWORD (WINAPI*)(LPVOID),LPVOID,DWORD,LPDWORD){return 0;}
extern "C" __declspec(dllexport) BOOL WINAPI CloseHandle(HANDLE){return 0;}
extern "C" __declspec(dllexport) BOOL WINAPI DisableThreadLibraryCalls(HMODULE){return 0;}
extern "C" __declspec(dllexport) LPVOID WINAPI VirtualAlloc(LPVOID,SIZE_T,DWORD,DWORD){return 0;}
extern "C" __declspec(dllexport) BOOL WINAPI VirtualFree(LPVOID,SIZE_T,DWORD){return 0;}
extern "C" __declspec(dllexport) BOOL WINAPI VirtualProtect(LPVOID,SIZE_T,DWORD,LPDWORD){return 0;}
extern "C" __declspec(dllexport) SIZE_T WINAPI VirtualQuery(const void*,LPVOID,SIZE_T){return 0;}
extern "C" __declspec(dllexport) BOOL WINAPI FlushInstructionCache(HANDLE,const void*,SIZE_T){return 0;}
extern "C" __declspec(dllexport) HANDLE WINAPI GetCurrentProcess(){return 0;}
extern "C" BOOL WINAPI DllMain(HMODULE,DWORD,LPVOID){return 1;}
