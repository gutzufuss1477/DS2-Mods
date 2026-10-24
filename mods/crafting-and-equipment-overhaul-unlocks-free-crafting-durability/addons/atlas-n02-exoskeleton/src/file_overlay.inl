// Early game-IAT overlay. All archives remain read-only; verified private files
// are held open with read-only sharing for the entire process lifetime.
#include "portable_cache.inl"
using OpenW=HANDLE(WINAPI*)(const WCHAR*,DWORD,DWORD,void*,DWORD,DWORD,HANDLE);
using OpenA=HANDLE(WINAPI*)(const char*,DWORD,DWORD,void*,DWORD,DWORD,HANDLE);
OpenW nextOpenW=nullptr;OpenA nextOpenA=nullptr;
volatile long overlayState=0; // 0 unverified, 1 verifying, 2 active, -1 refused

bool overlayInstalled=false;

WCHAR pathChar(WCHAR c){if(c==L'/')return L'\\';return c>=L'A'&&c<=L'Z'?c+32:c;}
bool pathEqual(const WCHAR*a,const WCHAR*b){
 if(!a||!b)return false;
 // Win32 extended drive paths still address the same exact verified file.
 if(a[0]==L'\\'&&a[1]==L'\\'&&a[2]==L'?'&&a[3]==L'\\')a+=4;
 if(b[0]==L'\\'&&b[1]==L'\\'&&b[2]==L'?'&&b[3]==L'\\')b+=4;
 for(u32 i=0;i<2048;++i){if(pathChar(a[i])!=pathChar(b[i]))return false;if(!a[i])return true;}return false;
}
int overlayIndex(const WCHAR* path){
 for(u32 i=0;i<2;++i)if(pathEqual(path,CacheFiles[i].original))return int(i);
 // Game uses both package-relative and absolute paths.
 for(u32 i=0;i<2;++i){
  const WCHAR*p=CacheFiles[i].original;u32 n=0;while(p[n])++n;
  while(n&&p[n-1]!=L'\\'&&p[n-1]!=L'/')--n;
  WCHAR relative[256]=L"LocalCacheWinGame\\package\\";u32 k=0;while(relative[k])++k;
  for(u32 j=0;p[n+j]&&k<255;++j)relative[k++]=p[n+j];relative[k]=0;
  if(pathEqual(path,relative)||(path&&path[0]==L'.'&&(path[1]==L'\\'||path[1]==L'/')&&pathEqual(path+2,relative)))return int(i);
 }return -1;
}
bool verifyOverlay(){return _InterlockedCompareExchange(&overlayState,0,0)==2;}
[[maybe_unused]] void cacheDiagnostics(){
 long state=_InterlockedCompareExchange(&overlayState,0,0);
 log(state==2?"CHIRAL_CACHE_STATE: active.":state==-1?"CHIRAL_CACHE_STATE: refused during an earlier file open.":
     state==1?"CHIRAL_CACHE_STATE: verification in progress.":"CHIRAL_CACHE_STATE: no matching file open observed yet.");
}
void unmatchedPath(const WCHAR*path){
 if(!path)return;
 u32 n=0;while(n<1000&&path[n])++n;if(n==1000)return;
 u32 start=n;while(start&&path[start-1]!=L'\\'&&path[start-1]!=L'/')--start;
 const WCHAR*names[]={L"streaming_graph.core",L"streaming_links.stream",L"package.42.02.core.stream"};
 for(u32 i=0;i<3;++i)if(pathEqual(path+start,names[i])){
  static volatile long reported[3]={};if(_InterlockedExchange(&reported[i],1))return;
  char text[1000];for(u32 j=0;j<n;++j)text[j]=path[j]<128?char(path[j]):'?';text[n]=0;
  logValue("CHIRAL_UNMATCHED_RESOURCE_PATH: ",text);return;
 }
}
bool readOpen(DWORD access,DWORD disposition){
 return disposition==3&&(access&0x80000001u)&&!(access&(0x40000000u|0x116u));
}
HANDLE WINAPI openOverlayW(const WCHAR*p,DWORD a,DWORD s,void*sa,DWORD d,DWORD f,HANDLE t){
 int index=readOpen(a,d)?overlayIndex(p):-1;
 if(index<0&&readOpen(a,d))unmatchedPath(p);
 if(index>=0&&verifyOverlay()){
  static volatile long reported[3]={};
  if(!_InterlockedExchange(&reported[index],1))log(index==0?"CHIRAL_REDIRECT_ARCHIVE":"CHIRAL_REDIRECT_LINKS");
  return CreateFileW(CacheFiles[index].cache,a,s,sa,d,f,t);
 }
 return nextOpenW(p,a,s,sa,d,f,t);
}
HANDLE WINAPI openOverlayA(const char*p,DWORD a,DWORD s,void*sa,DWORD d,DWORD f,HANDLE t){
 WCHAR wide[1024];u32 n=0;
 if(p){for(;n<1023&&p[n]&&u8(p[n])<128;++n)wide[n]=u8(p[n]);wide[n]=0;}
 int index=p&&n<1023&&!p[n]&&readOpen(a,d)?overlayIndex(wide):-1;
 if(index<0&&p&&n<1023&&!p[n]&&readOpen(a,d))unmatchedPath(wide);
 if(index>=0&&verifyOverlay())return openOverlayW(wide,a,s,sa,d,f,t);
 return nextOpenA(p,a,s,sa,d,f,t);
}
bool earlyOverlay(){
 image=u64(GetModuleHandleW(nullptr));
 u32 pe=read<u32>(image+0x3c);
 if(pe<0x40||pe>0x1000||read<u32>(image+pe)!=0x4550||
    read<u32>(image+pe+8)!=0x6A3DAE46||read<u32>(image+pe+0x50)!=0xB292000)return false;
 // Pinned exact PE layout. Full SHA validation occurs before any redirection.
 u64 w=image+0x2C16F40,a=image+0x2C16F00;
 nextOpenW=(OpenW)read<u64>(w);nextOpenA=(OpenA)read<u64>(a);
 if(!nextOpenW||!nextOpenA)return false;
 DWORD old=0,ignored=0;
 // Both slots are on the same IAT page; one protection change, atomic writes.
 if(!VirtualProtect((void*)a,0x48,4,&old))return false;
 bool first=(u64)_InterlockedCompareExchange64((long long*)w,(long long)&openOverlayW,(long long)nextOpenW)==u64(nextOpenW);
 bool second=first&&(u64)_InterlockedCompareExchange64((long long*)a,(long long)&openOverlayA,(long long)nextOpenA)==u64(nextOpenA);
 if(!second&&first)_InterlockedCompareExchange64((long long*)w,(long long)nextOpenW,(long long)&openOverlayW);
 bool restored=VirtualProtect((void*)a,0x48,old,&ignored)!=0;
 if(!restored&&second){
  _InterlockedCompareExchange64((long long*)w,(long long)nextOpenW,(long long)&openOverlayW);
  _InterlockedCompareExchange64((long long*)a,(long long)nextOpenA,(long long)&openOverlayA);
  VirtualProtect((void*)a,0x48,old,&ignored);
 }
 overlayInstalled=second&&restored;
 return overlayInstalled;
}
