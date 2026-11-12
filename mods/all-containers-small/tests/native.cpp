#include "../src/win_api.hpp"
#include "../src/sha256.hpp"
using namespace smallcargo;
extern "C" void* memcpy(void*d,const void*s,SIZE_T n){auto*a=(volatile u8*)d;auto*b=(const volatile u8*)s;while(n--)*a++=*b++;return d;}
extern "C" void* memset(void*d,int v,SIZE_T n){auto*a=(volatile u8*)d;while(n--)*a++=u8(v);return d;}
bool rd(u64 p,void*out,u32 n){SIZE_T got=0;return ReadProcessMemory(GetCurrentProcess(),(void*)p,out,n,&got)&&got==n;}
void* denied=nullptr;
bool wr(void*p,u32){return p!=denied;}
extern "C" __declspec(dllexport) bool Transform(u8* bag){return transform(bag);}
extern "C" __declspec(dllexport) bool Patch(Array* a,u64 image,Counts* out,void* deny){denied=deny;return patchArray(a,image,rd,wr,*out);}
extern "C" __declspec(dllexport) void Hash(const void*p,u32 n,u8*out){craft::Sha256 hash;hash.update(p,n);hash.finish(out);}
extern "C" __declspec(dllexport) void* Model(void* manager,const u8* bag,SelectModel original){return model(manager,bag,original);}
extern "C" __declspec(dllexport) void* Appearance(void* context,const u8* instance,SelectAppearance original){return appearance(context,instance,original);}
extern "C" __declspec(dllexport) bool IsGamePath(const WCHAR* path,u32 n){return isGamePath(path,n);}
extern "C" BOOL WINAPI DllMain(HMODULE,DWORD,void*){return 1;}
