// Isolated process. Uses the exact installed DirectStorage binaries (private copies)
// and unmodified Crafting graph source; never launches DS2 or reads saves.
#define CHIRAL_SELF_TEST
#include "../src/addon.cpp"
API HMODULE WINAPI LoadLibraryW(const WCHAR*);
API HANDLE WINAPI CreateFileA(const char*,DWORD,DWORD,void*,DWORD,DWORD,HANDLE);
API BOOL WINAPI DeleteFileW(const WCHAR*);
API BOOL WINAPI GetFileInformationByHandle(HANDLE,void*);
API HANDLE WINAPI GetStdHandle(DWORD);
#include "D:/DS2-Asset-Lab/analysis/red-coffin-v1922-live-original-resource-private/src/virtual_graph_runtime.inl"
namespace {
u32 cases=0,failures=0;
void print(const char*s){DWORD n=0,k=0;while(s[k])++k;WriteFile(GetStdHandle(DWORD(-11)),s,k,&n,nullptr);}
void check(bool ok,const char*label){++cases;if(!ok){++failures;print("FAILED: ");print(label);print("\r\n");}}
struct Guid {u32 a;u16 b,c;u8 d[8];};
constexpr Guid FactoryIID={0x6924ea0c,0xc3cd,0x4826,{0xb1,0x0a,0xf6,0x4f,0x4e,0xd9,0x27,0xc1}};
constexpr Guid FileIID={0x5de95e7b,0x955a,0x4868,{0xa7,0x3c,0x24,0x3b,0x29,0xf4,0xb8,0xda}};
struct FileInfo{u32 data[13];};
bool sameFile(const FileInfo&a,const FileInfo&b){
 // Volume, size, and file index; access timestamps can change after opening.
 return a.data[7]==b.data[7]&&a.data[8]==b.data[8]&&a.data[9]==b.data[9]&&a.data[11]==b.data[11]&&a.data[12]==b.data[12];
}
bool info(const WCHAR*path,FileInfo&result){
 HANDLE h=CreateFileW(path,0x80000000u,3,nullptr,3,0x80,nullptr);
 if(h==InvalidHandle)return false;bool ok=GetFileInformationByHandle(h,&result)!=0;CloseHandle(h);return ok;
}
void checkHandle(HANDLE h,const WCHAR*expected){
 FileInfo actual={},want={};
 check(h!=InvalidHandle&&GetFileInformationByHandle(h,&actual)&&info(expected,want)&&sameFile(actual,want),"Win32 returned exact expected file");
 if(h!=InvalidHandle)CloseHandle(h);
}
void storageOpen(void*factory,const WCHAR*request,const WCHAR*expected){
 auto vt=*(void***)factory;void*file=nullptr;
 auto open=(i32(WINAPI*)(void*,const WCHAR*,const void*,void**))vt[4];
 i32 hr=open(factory,request,&FileIID,&file);
 check(hr>=0&&file,"real IDStorageFactory::OpenFile");
 if(!file)return;
 auto ft=*(void***)file;FileInfo actual={},want={};
 auto getInfo=(i32(WINAPI*)(void*,void*))ft[4];
 check(getInfo(file,&actual)>=0&&info(expected,want)&&sameFile(actual,want),"DirectStorage returned exact expected file identity");
 ((u32(WINAPI*)(void*))ft[2])(file);
}
}
extern "C" void mainCRTStartup(){
 nextOpenW=CreateFileW;nextOpenA=CreateFileA;
 check(initializeHostCacheName(),"own host cache path");
 static WCHAR hostPath[2048]={};check(vgraph_runtime::temporaryPath(hostPath),"real host cache path");
 check(pathEqual(hostPath,hostCacheName)&&overlayIndex(hostPath)==1,"actual Crafting v1921 path recognized");
 vgraph_runtime::originalW=openOverlayW;vgraph_runtime::originalA=openOverlayA;
 copy(vgraph_runtime::virtualPath,hostPath,sizeof(hostPath));
 vgraph_runtime::cacheState=2;overlayState=2;
 checkHandle(vgraph_runtime::openW(CacheFiles[1].original,0x80000000u,3,nullptr,3,0x80,nullptr),CacheFiles[1].cache);
 checkHandle(vgraph_runtime::openA("LocalCacheWinGame/package/streaming_graph.core",0x80000000u,3,nullptr,3,0x80,nullptr),CacheFiles[1].cache);
 check(vgraph_runtime::successfulRedirects()==2,"real host chain redirects W and A");
 vgraph_runtime::originalW=CreateFileW;vgraph_runtime::originalA=CreateFileA;
 nextOpenW=vgraph_runtime::openW;nextOpenA=vgraph_runtime::openA;
 checkHandle(openOverlayW(CacheFiles[1].original,0x80000000u,3,nullptr,3,0x80,nullptr),CacheFiles[1].cache);
 // Full foreign-process gate still fails with a genuine DirectStorage library.
 u64 fakeImage=u64(VirtualAlloc(nullptr,0x2c18000,0x3000,4));
 check(fakeImage!=0,"isolated game import table allocation");
 if(!fakeImage)ExitProcess(2);
 image=fakeImage;
 at<u64>((void*)image,0x2c170d8)=u64(&GetProcAddress);
 at<u64>((void*)image,0x2c16f40)=u64(&openOverlayW);
 at<u64>((void*)image,0x2c16f00)=u64(&openOverlayA);
 check(earlyStorage(),"install factory resolver through game IAT");
 check(read<u64>(image+0x2c170d8)==u64(&resolveStorage),"resolver import targets addon");
 check(!exchangeImport(image+0x2c170d8,0,u64(&GetProcAddress))&&read<u64>(image+0x2c170d8)==u64(&resolveStorage),"unexpected import is not overwritten");
 HMODULE dll=LoadLibraryW(L"D:\\DS2-Asset-Lab\\mods\\atlas-chiral-addon\\build\\dstorage.dll");
 check(dll!=nullptr,"load private copy of actual DirectStorage");
 if(!dll)ExitProcess(2);
 auto factoryFn=(StorageFactory)((ResolveProc)read<u64>(image+0x2c170d8))(dll,"DStorageGetFactory");
 check(factoryFn==&storageFactory,"real factory export intercepted");
 void*factory=nullptr;i32 hr=factoryFn(&FactoryIID,&factory);
 check(hr>=0&&factory,"real DirectStorage factory created");
 check(storageState==2,"verified DirectStorageCore IAT installed");
 if(storageState!=2){
  auto core=GetModuleHandleW(L"DStorageCore.dll");char digest[65]={};
  check(core!=nullptr,"diagnostic core module exists");
  check(core&&moduleDigest(core,digest)&&sameDigest(digest,StorageCoreSHA),"diagnostic core hash");
  if(core){
   u64 base=u64(core);u32 pe=read<u32>(base+0x3c);
   check(read<u32>(base+pe+8)==0x34c782e3&&read<u32>(base+pe+0x50)==0x176000,"diagnostic core PE");
   auto pointer=read<u64>(base+0x58158);
   check(pointer==u64(GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"CreateFileW")),"diagnostic core import equals Kernel32");
   check(pointer==u64(GetProcAddress(GetModuleHandleW(L"kernelbase.dll"),"CreateFileW")),"diagnostic core import equals KernelBase");
  }
 }
 check(resolveStorage(dll,"DStorageSetConfiguration")==GetProcAddress(dll,"DStorageSetConfiguration"),"unrelated export unchanged");
 check(installStorageCore(),"storage hook idempotent");
 overlayState=0;check(!verifyOverlay()&&overlayState==-1,"foreign process cannot activate production validation");
 if(factory){
  storageOpen(factory,CacheFiles[0].original,CacheFiles[0].original);
  storageOpen(factory,CacheFiles[2].original,CacheFiles[2].original);
  // Test-only gate setup: exercises real native I/O, not game/host impersonation.
  overlayState=2;
  storageOpen(factory,CacheFiles[0].original,CacheFiles[0].cache);
  storageOpen(factory,CacheFiles[2].original,CacheFiles[2].cache);
  storageOpen(factory,CacheFiles[1].original,CacheFiles[1].cache);
  // Same basename from another package must remain native.
  const WCHAR other[]=L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\DEATH STRANDING 2 - ON THE BEACH\\LocalCacheWinGame\\package\\l200_aus\\package.42.02.core.stream";
  storageOpen(factory,other,other);
  ((u32(WINAPI*)(void*))(*(void***)factory)[2])(factory);
 }
 char result[128]="CHIRAL_IO_INTEGRATION ";u32 at=sizeof("CHIRAL_IO_INTEGRATION ")-1;
 auto number=[&](u32 n){char digits[12];u32 k=0;do{digits[k++]=char('0'+n%10);n/=10;}while(n);while(k)result[at++]=digits[--k];};
 number(cases);const char mid[]=" failures=";copy(result+at,mid,sizeof(mid)-1);at+=sizeof(mid)-1;number(failures);result[at++]='\r';result[at++]='\n';result[at]=0;print(result);
 ExitProcess(failures?1:0);
}
