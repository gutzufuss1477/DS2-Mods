#define CHIRAL_SELF_TEST
#include "../src/addon.cpp"
#include "../development/crafting-v198-addon-api/src/virtual_graph_core.hpp"
#include "../development/crafting-v198-addon-api/src/core.hpp"
craft::Settings parserSettings;
extern "C" __declspec(dllexport) int Config(const char*text,u32 n,u32*line){
 craft::Error error={};bool ok=craft::parse(text,n,parserSettings,error);*line=error.line;
 return ok&&parserSettings.enabled&&parserSettings.atlasEnabled;
}
extern "C" __declspec(dllexport) int Merge(const u8*s,u32 n,const u8*d,u32 dn,u8*out,u32 cap,u32*written){return chiral_graph::merge(s,n,d,dn,out,cap,*written);}
extern "C" __declspec(dllexport) int Host198(const u8*s,u32 n,u8*out,u32 cap){return vgraph::build(s,n,out,cap);}
extern "C" __declspec(dllexport) int Embedded(const WCHAR*name,const u8**out,u32*size){
 *out=nullptr;*size=0;const auto*p=findEmbeddedPayload(name);
 if(!p||!validatedEmbedded(name,p->sha256))return 0;
 *out=p->data;*size=p->size;return 1;
}
extern "C" __declspec(dllexport) int Cache(const WCHAR*package,const WCHAR*folder){
 if(!pathJoin(cacheFolder,folder,L"")||!ensureCacheDirectory())return 0;
 for(u32 i=0;i<2;++i){auto&f=CacheFiles[i];f.sourceHash=PortableFiles[i].originalHash;f.cacheHash=PortableFiles[i].resultHash;
  if(!pathJoin(f.original,package,PortableFiles[i].name)||!pathJoin(f.cache,folder,i?L"links-v1.cache":L"geometry-v1.cache"))return 0;}
 bool ok=buildPortableFile(0)&&buildPortableFile(1);closeCachePins();return ok;
}
extern "C" __declspec(dllexport) int RouteProbe(const WCHAR*source,int expected){
 overlayState=2;nextOpenW=&CreateFileW;nextStorageOpen=&CreateFileW;
 if(overlayIndex(source)!=expected)return 0;
 HANDLE h=openOverlayW(source,0x80000000u,1,nullptr,3,0x80,nullptr);long long n=0;
 bool ok=h!=InvalidHandle&&GetFileSizeEx(h,&n)&&n==(long long)PortableFiles[expected].resultSize;if(h!=InvalidHandle)CloseHandle(h);
 h=openStorageW(source,0x80000000u,1,nullptr,3,0x80,nullptr);n=0;
 ok=ok&&h!=InvalidHandle&&GetFileSizeEx(h,&n)&&n==(long long)PortableFiles[expected].resultSize;if(h!=InvalidHandle)CloseHandle(h);
 return ok;
}
extern "C" BOOL WINAPI DllMain(HMODULE,DWORD,void*){return 1;}
