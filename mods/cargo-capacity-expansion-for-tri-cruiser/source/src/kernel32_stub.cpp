extern "C" {
__declspec(dllexport) void GetModuleHandleW(){}
__declspec(dllexport) void VirtualAlloc(){}
__declspec(dllexport) void VirtualFree(){}
__declspec(dllexport) void VirtualProtect(){}
__declspec(dllexport) void FlushInstructionCache(){}
__declspec(dllexport) void GetCurrentProcess(){}
__declspec(dllexport) void CreateThread(){}
__declspec(dllexport) void Sleep(){}
__declspec(dllexport) void CreateFileW(){}
__declspec(dllexport) void WriteFile(){}
__declspec(dllexport) void CloseHandle(){}
__declspec(dllexport) void DisableThreadLibraryCalls(){}
__declspec(dllexport) void ReadProcessMemory(){}
}
extern "C" int __stdcall DllMain(void*,unsigned long,void*) { return 1; }
