#include "portable_manifest.hpp"
#include "exo_extension.hpp"
extern "C" const ExoExtensionV1* AtlasVisualPayloadV1();
bool exoEnabled=true,bootsEnabled=false;
const PortableFile*activePortableFiles=nullptr;
const char*activeGraphHash=nullptr;
const EmbeddedPayload*(*activeFind)(const WCHAR*)=nullptr;
constexpr char PublishedBootSHA[]="f2193e540a04e9b30507ee69a325b91feacdbed97ecc4dd9fa6d70b5fc5b172b";
bool supportedRouter(){
 HMODULE host=GetModuleHandleW(L"ds2_crafting_unlocks.asi");
 using Router=unsigned(*)();auto f=host?(Router)GetProcAddress(host,"DS2CraftingAddonRouterV1"):nullptr;
 return f&&f()==1;
}
bool publishedBootModule(HMODULE module){
 if(!module||module!=GetModuleHandleW(L"ds2_atlas_chiral_addon.asi"))return false;
 char digest[65];return moduleDigest(module,digest)&&sameDigest(digest,PublishedBootSHA);
}
bool selectExo(){
 if(!supportedRouter()){log("EXO_ROUTER_REQUIRED: compatible optional-visual host required.");return false;}
 HMODULE boots=GetModuleHandleW(L"ds2_atlas_chiral_addon.asi");
 if(boots&&!publishedBootModule(boots)){log("EXO_COMPANION_REFUSED: unrecognized shoe addon; main appearance retained.");return false;}
 bootsEnabled=boots!=nullptr;
 const auto*e=AtlasVisualPayloadV1();
 if(!e||e->size!=sizeof(*e)||e->version!=1||!e->files||!e->graphHash||!e->find)return false;
 activePortableFiles=e->files;activeGraphHash=e->graphHash;activeFind=e->find;
 log(bootsEnabled?"EXO_MODE_BOTH: one resource owner; independent boots and skeleton visuals active.":"EXO_MODE_SKELETON_ONLY: shoe mesh and icon callbacks pass through unchanged.");return true;
}
struct CacheFile {WCHAR original[2048],cache[2048];const char* sourceHash;const char* cacheHash;};
CacheFile CacheFiles[2]={};
WCHAR cacheFolder[2048]={};
HANDLE cachePins[2]={},sourcePins[2]={};
bool pathJoin(WCHAR*out,const WCHAR*folder,const WCHAR*name){
 u32 n=0;for(;folder[n];++n){if(n>=1800)return false;out[n]=folder[n];}
 for(u32 j=0;name[j];++j){if(n>=2047)return false;out[n++]=name[j];}out[n]=0;return true;
}
bool moduleFolder(HMODULE module,WCHAR*out){u32 n=GetModuleFileNameW(module,out,1800);if(!n||n>=1800)return false;
 while(n&&out[n-1]!=L'\\'&&out[n-1]!=L'/')--n;out[n]=0;return n!=0;}
bool portablePaths(){
 // Host serializes resource preparation; avoid large startup stack frames.
 static WCHAR folder[2048],game[2048],package[2048];
 if(!moduleFolder(selfModule,folder)||!moduleFolder(nullptr,game)||!pathJoin(cacheFolder,folder,L"atlas_exoskeleton\\")||
    !pathJoin(package,game,L"LocalCacheWinGame\\package\\"))return false;
 for(u32 i=0;i<2;++i){auto&f=CacheFiles[i];f.sourceHash=activePortableFiles[i].originalHash;f.cacheHash=activePortableFiles[i].resultHash;
  if(!pathJoin(f.original,package,activePortableFiles[i].name)||!pathJoin(f.cache,cacheFolder,i?L"links-v1.cache":L"geometry-v1.cache"))return false;}
 return true;
}
bool ensureCacheDirectory(){
 if(CreateDirectoryW(cacheFolder,nullptr))return true;
 DWORD attributes=GetFileAttributesW(cacheFolder);
 return attributes!=0xffffffffu&&(attributes&0x10)!=0;
}
const EmbeddedPayload* validatedEmbedded(const WCHAR*name,const char*expected){
 const auto*p=activeFind(name);
 if(!p||!p->data||!p->size||!sameDigest(p->sha256,expected))return nullptr;
 craft::Sha256 hash;hash.update(p->data,p->size);u8 bytes[32];hash.finish(bytes);
 char digest[65];constexpr char hex[]="0123456789abcdef";
 for(u32 i=0;i<32;++i){digest[2*i]=hex[bytes[i]>>4];digest[2*i+1]=hex[bytes[i]&15];}digest[64]=0;
 return sameDigest(digest,expected)?p:nullptr;
}
bool buildPortableFile(u32 index){
 const auto&spec=activePortableFiles[index];auto&f=CacheFiles[index];char hash[65];
 sourcePins[index]=CreateFileW(f.original,0x80000000u,1,nullptr,3,0x80,nullptr);
 if(sourcePins[index]==InvalidHandle||!hashFile(f.original,hash)||!sameDigest(hash,spec.originalHash))return false;
 cachePins[index]=CreateFileW(f.cache,0x80000000u,1,nullptr,3,0x80,nullptr);
 if(cachePins[index]!=InvalidHandle&&hashFile(f.cache,hash)&&sameDigest(hash,spec.resultHash))return true;
 if(cachePins[index]!=InvalidHandle)CloseHandle(cachePins[index]);cachePins[index]=nullptr;
 // Own temporary file only. Original archives are never opened for writing.
 static WCHAR tmp[2048];if(!pathJoin(tmp,cacheFolder,index?L"links-v1.building":L"geometry-v1.building"))return false;
 HANDLE output=CreateFileW(tmp,0x40000000u,0,nullptr,2,0x80,nullptr);if(output==InvalidHandle)return false;
 u8*block=(u8*)VirtualAlloc(nullptr,1024*1024,0x3000,4);bool ok=block!=nullptr;u64 total=0;
 log("CHIRAL_CACHE_BUILD: reconstructing own cache from unchanged original + embedded assets.");
 for(u32 i=0;i<spec.count&&ok;++i){const auto&s=spec.segments[i];HANDLE input=sourcePins[index];const u8*embedded=nullptr;
  if(s.name){const auto*p=validatedEmbedded(s.name,s.hash);
   ok=p&&s.offset<=p->size&&s.length<=p->size-s.offset;if(ok)embedded=p->data+s.offset;}
  else if(ok)ok=SetFilePointerEx(input,(long long)s.offset,nullptr,0)!=0;
  for(u64 left=s.length;left&&ok;){u32 chunk=left>1024*1024?1024*1024:u32(left),got=0,wrote=0;
   const u8*data=block;
   if(embedded)data=embedded+(s.length-left);
   else ok=ReadFile(input,block,chunk,&got,nullptr)&&got==chunk;
   if(ok)ok=WriteFile(output,data,chunk,&wrote,nullptr)&&wrote==chunk;
   left-=chunk;total+=chunk;}
 }
 if(ok)ok=total==spec.resultSize&&FlushFileBuffers(output);CloseHandle(output);if(block)VirtualFree(block,0,0x8000);
 if(ok)ok=hashFile(tmp,hash)&&sameDigest(hash,spec.resultHash);
 if(ok)ok=MoveFileExW(tmp,f.cache,1|8)!=0;
 if(!ok){DeleteFileW(tmp);return false;}
 cachePins[index]=CreateFileW(f.cache,0x80000000u,1,nullptr,3,0x80,nullptr);return cachePins[index]!=InvalidHandle;
}
void closeCachePins(){for(auto&h:cachePins){if(h&&h!=InvalidHandle)CloseHandle(h);h=nullptr;}
 for(auto&h:sourcePins){if(h&&h!=InvalidHandle)CloseHandle(h);h=nullptr;}}
bool preparePortable(){
 if(!portablePaths()||!ensureCacheDirectory())return false;
 for(u32 i=0;i<2;++i)if(!buildPortableFile(i)){closeCachePins();return false;}
 log("CHIRAL_PORTABLE_CACHE_READY: native originals and own cache verified; no host hash dependency.");return true;
}
