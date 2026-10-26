#define CHIRAL_SELF_TEST
#include "../src/addon.cpp"
#include "host_router_under_test.inl"
extern "C" __declspec(dllexport) int Selected(){auto h=addon_host::preferredModule();if(!h)return 0;return h==GetModuleHandleW(L"ds2_atlas_exoskeleton_addon.asi")?2:1;}
extern "C" __declspec(dllexport) int Mode(){return selectExo()?(bootsEnabled?3:2):-1;}
extern "C" __declspec(dllexport) int AcceptStorage(const void*p){return compatibleBootModule(GetModuleHandleW(L"ds2_atlas_chiral_addon.asi"),p);}
extern "C" __declspec(dllexport) int HasPeer(){return bootCompanion.prepareMaterials?1:0;}
extern "C" __declspec(dllexport) int PreparePeer(){return prepareBootCompanion();}
extern "C" BOOL WINAPI DllMain(HMODULE,DWORD,void*){return 1;}
