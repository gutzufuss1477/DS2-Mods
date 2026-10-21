#include "../src/win_api.hpp"
#include "../src/sha256.hpp"
extern "C" long _InterlockedCompareExchange(long volatile*,long,long);
extern "C" long _InterlockedExchange(long volatile*,long);
extern "C" long _InterlockedIncrement(long volatile*);
#pragma intrinsic(_InterlockedCompareExchange)
#pragma intrinsic(_InterlockedExchange)
#pragma intrinsic(_InterlockedIncrement)
#include <cstring>
extern "C" __declspec(dllimport) HANDLE WINAPI CreateFileA(const char*,DWORD,DWORD,void*,DWORD,DWORD,HANDLE);
#include "../src/virtual_graph_runtime.inl"
#include <cstdio>
int main(){
 using namespace vgraph_runtime;
 const unsigned span=CreateFileW_IAT+0x1000;
 u8*fake=(u8*)VirtualAlloc(nullptr,span,0x3000,4);
 if(!fake)return 2;
 auto write=vgraph::set32;
 write(fake,0x00905A4Du);
 write(fake+0x3C,0x1C0u);
 write(fake+0x1C0,0x00004550u);
 write(fake+0x1C0+0x50,ExecutableImageSize);
 const u8 site[5]={0xE8,0x9E,0x1E,0x45,0xFF};
 for(unsigned i=0;i<5;++i)fake[0x171DB1D+i]=site[i];
 auto**w=(void**)(fake+CreateFileW_IAT),**a=(void**)(fake+CreateFileA_IAT);
 *w=(void*)(OpenW)&CreateFileW;
 *a=(void*)(OpenA)&CreateFileA;
 int cases=0,failed=0;
 auto check=[&](bool v,const char*name){++cases;if(!v){++failed;std::printf("FAILED %s\n",name);}};
 check(!earlyInstall((u64)fake),"refuse wrong executable timestamp");
 check(*w==(void*)(OpenW)&CreateFileW&&*a==(void*)(OpenA)&CreateFileA,"wrong executable does not change IAT");
 write(fake+0x1C0+8,ExecutableTimestamp);
 check(earlyInstall((u64)fake),"install on exact PE gate");
 check(*w==(void*)&openW&&*a==(void*)&openA,"both CreateFile imports redirected");
 check(originalW==(OpenW)&CreateFileW&&originalA==(OpenA)&CreateFileA,"original functions preserved");
 check(matchesGraphFile(L"C:\\Game\\LocalCacheWinGame\\package\\streaming_graph.core"),"match Unicode graph path");
 check(matchesGraphFile("LOCALCACHEWINGAME/package/STREAMING_GRAPH.CORE"),"match ANSI graph path");
 check(!matchesGraphFile(L"C:\\Other\\graph_snapshot.core"),"ignore unrelated files");
 check(!matchesGraphFile("other-streaming_graph.core"),"require full filename boundary");
 check(!matchesGraphFile(L"C:\\Game\\LocalCacheWinGame\\package\\not_graph.core"),"skip other CORE files");
 VirtualFree(fake,0,0x8000);
 std::printf("VIRTUAL_GRAPH_IAT_TESTS %d ERRORS %d\n",cases,failed);
 return failed?1:0;
}
