// Production cache, graph and StorageCore hook in a disposable test process.
// Game inputs are read-only. All outputs and fake IAT memory belong to this test.
#include "../src/win_api.hpp"
#include "../src/sha256.hpp"
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <vector>
extern "C" long _InterlockedCompareExchange(long volatile*,long,long);
extern "C" long _InterlockedExchange(long volatile*,long);
extern "C" long _InterlockedIncrement(long volatile*);
extern "C" long long _InterlockedCompareExchange64(long long volatile*,long long,long long);
#pragma intrinsic(_InterlockedCompareExchange)
#pragma intrinsic(_InterlockedExchange)
#pragma intrinsic(_InterlockedIncrement)
#pragma intrinsic(_InterlockedCompareExchange64)

static constexpr const WCHAR* Game=L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\DEATH STRANDING 2 - ON THE BEACH\\";
static bool addon=false;
static unsigned sourceReads=0,shoeReads=0,writeViolations=0;
HANDLE WINAPI inputOnlyRedirect(const WCHAR*name,DWORD access,DWORD share,void*sec,DWORD mode,DWORD flags,HANDLE templ){
 const WCHAR*relative=nullptr;
 for(const auto*needle:{L"LocalCacheWinGame\\package\\package.42.02.core.stream",L"LocalCacheWinGame\\package\\streaming_links.stream",L"atlas_chiral\\geometry-v1.cache",L"atlas_chiral\\links-v1.cache"})
  if(const auto*p=std::wcsstr(name,needle))relative=p;
 WCHAR path[2048]={};
 if(relative){
  if(mode!=3||(access&0x40000000u)){++writeViolations;return InvalidHandle;}
  if(std::wcsstr(relative,L"atlas_chiral")){
   ++shoeReads;if(!addon)return InvalidHandle;
  }
  ++sourceReads;std::swprintf(path,2048,L"%ls%ls",Game,relative);name=path;
 }
 return CreateFileW(name,access,share,sec,mode,flags,templ);
}
#define CreateFileW inputOnlyRedirect
#include "../src/coffin_story_archive_cache.inl"
#undef CreateFileW

static unsigned char* fakeCore=nullptr;
HMODULE WINAPI testModule(const WCHAR*name){
 if(std::wcscmp(name,L"DStorageCore.dll")==0)return fakeCore;
 if(std::wcscmp(name,L"ds2_atlas_chiral_addon.asi")==0)return addon?GetModuleHandleW(nullptr):nullptr;
 return GetModuleHandleW(name);
}
DWORD WINAPI testModulePath(HMODULE module,WCHAR*out,DWORD size){
 const WCHAR*leaf=nullptr;
 if(module==fakeCore)leaf=L"DStorageCore.dll";
 else if(addon&&module==GetModuleHandleW(nullptr))leaf=L"ds2_atlas_chiral_addon.asi";
 if(!leaf)return GetModuleFileNameW(module,out,size);
 int n=std::swprintf(out,size,L"%ls%ls",Game,leaf);
 return n>0?(DWORD)n:0;
}
HANDLE WINAPI addonForward(const WCHAR*n,DWORD a,DWORD s,void*sa,DWORD d,DWORD f,HANDLE t){return CreateFileW(n,a,s,sa,d,f,t);}
#define GetModuleHandleW testModule
#define GetModuleFileNameW testModulePath
#include "../src/coffin_story_archive_storage.inl"
#undef GetModuleHandleW
#undef GetModuleFileNameW
#include "../src/addon_host.inl"
#include "../src/virtual_graph_runtime.inl"

std::vector<unsigned char> read(const char*path){
 FILE*f=std::fopen(path,"rb");if(!f)return {};
 std::fseek(f,0,SEEK_END);auto n=std::ftell(f);std::rewind(f);
 std::vector<unsigned char>v(n);bool ok=std::fread(v.data(),1,v.size(),f)==v.size();std::fclose(f);
 return ok?v:std::vector<unsigned char>{};
}
int main(int argc,char**argv){
 if(argc!=4)return 2;
 addon=std::strcmp(argv[1],"addon")==0;
 unsigned tests=0,fail=0;
 auto check=[&](bool ok,const char*label){++tests;if(!ok){++fail;std::printf("FAIL %s\n",label);}};
 using namespace coffin_archive_cache;
 check(prepare(addon)&&ready(),"actual production cache generated and SHA verified");
 check(profile->addon==addon,"selected requested profile");
 check(prepare(addon)&&!prepare(!addon),"repeat preparation stable, wrong profile rejected");
 check(sourcePin!=InvalidHandle&&privatePin!=InvalidHandle&&sourceLinksPin!=InvalidHandle&&privateLinksPin!=InvalidHandle,"four read pins");
 check(shaMatches(privatePath,profile->newPhysical,profile->targetSha),"archive exact reference hash");
 check(shaMatches(privateLinksPath,profile->linkSourceSize+coffin_private_links::TailBytes,profile->targetLinksSha),"links exact reference hash");
 check(!writeViolations,"no writes to source archives");
 check(sourceReads>0&&(addon?shoeReads>0:shoeReads==0),"standalone never opens addon caches");
 auto src=read(argv[2]),expected=read(argv[3]);
 unsigned char*out=nullptr;unsigned size=0;
 bool graph=vgraph_runtime::buildStoryRegisteredGraph(src.data(),(unsigned)src.size(),&out,&size,addon);
 check(graph&&out&&size==expected.size()&&!std::memcmp(out,expected.data(),size),"production graph exact independent generated reference");
 if(out)VirtualFree(out,0,0x8000);
 check(!vgraph_runtime::buildStoryRegisteredGraph(src.data(),(unsigned)src.size(),&out,&size,!addon),"wrong graph profile rejected");
 if(!src.empty())src[src.size()/2]^=1;
 check(!vgraph_runtime::buildStoryRegisteredGraph(src.data(),(unsigned)src.size(),&out,&size,addon),"modified input graph rejected");
 using namespace coffin_archive_storage;
 auto os=(craft::u64)GetProcAddress(GetModuleHandleW(L"kernelbase.dll"),"CreateFileW");
 auto module=GetModuleHandleW(nullptr);
 check(trustedPrior(false,os,nullptr,nullptr,os,os),"native import accepted without addon");
 check(!trustedPrior(false,(craft::u64)&addonForward,module,nullptr,os,os),"unknown hook rejected without addon");
 check(!trustedPrior(false,os,nullptr,module,os,os),"uncommitted installed addon rejected");
 check(trustedPrior(true,(craft::u64)&addonForward,module,module,os,os),"verified addon chain accepted");
 check(!trustedPrior(true,os,nullptr,module,os,os),"missing addon hook rejected");
 check(!trustedPrior(true,(craft::u64)&addonForward,nullptr,module,os,os),"wrong hook owner rejected");
 fakeCore=(unsigned char*)VirtualAlloc(nullptr,CoreImageSize,0x3000,4);
 if(!fakeCore)return 3;
 unsigned pe=0x100,signature=0x4550;
 std::memcpy(fakeCore+0x3c,&pe,4);std::memcpy(fakeCore+pe,&signature,4);
 std::memcpy(fakeCore+pe+8,&CoreTimestamp,4);std::memcpy(fakeCore+pe+0x50,&CoreImageSize,4);
 auto prior=addon?(craft::u64)&addonForward:os;
 std::memcpy(fakeCore+ImportRva,&prior,8);
 check(install(addon)&&coffin_archive_storage::ready(),"actual production installer on private fake IAT");
 check(*(craft::u64*)(fakeCore+ImportRva)==(craft::u64)&readOverlay&&(craft::u64)originalOpenW==prior,"hook installed, exact previous callback retained");
 const WCHAR*canonical=L"C:\\unused\\LocalCacheWinGame\\package\\package.42.02.core.stream";
 auto handle=readOverlay(canonical,0x80000000u,1,nullptr,3,0x80,nullptr);
 long long length=0;
 check(handle!=InvalidHandle&&GetFileSizeEx(handle,&length)&&(craft::u64)length==profile->newPhysical,"production hook opens selected main-mod archive");
 if(handle!=InvalidHandle)CloseHandle(handle);
 check(install(addon)&&!install(!addon),"installed profile stable");
 std::printf("STORY_INDEPENDENCE mode=%s ASSERTIONS=%u FAILURES=%u shoeReads=%u sourceWrites=%u\n",addon?"addon":"standalone",tests,fail,shoeReads,writeViolations);
 return fail?1:0;
}
