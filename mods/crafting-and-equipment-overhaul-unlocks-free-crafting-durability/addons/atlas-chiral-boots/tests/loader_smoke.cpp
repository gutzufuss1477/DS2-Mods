#include "vendor/win_api.hpp"
API HMODULE WINAPI LoadLibraryW(const WCHAR*);
API HANDLE WINAPI GetStdHandle(DWORD);
static char bytes[32768];
bool contains(const char*data,DWORD n,const char*value){
 DWORD len=0;while(value[len])++len;
 for(DWORD i=0;i+len<=n;++i){DWORD j=0;while(j<len&&data[i+j]==value[j])++j;if(j==len)return true;}return false;
}
extern "C" void mainCRTStartup(){
 HMODULE module=LoadLibraryW(L".\\release\\ds2_atlas_chiral_addon.asi");
 if(!module)ExitProcess(1);
 bool verified=false;
 for(DWORD i=0;i<100&&!verified;++i){
  Sleep(50);
  HANDLE file=CreateFileW(L".\\release\\ds2_atlas_chiral_addon.log",0x80000000u,3,nullptr,3,0x80,nullptr);
  if(file==InvalidHandle)continue;
  DWORD n=0;if(ReadFile(file,bytes,sizeof(bytes),&n,nullptr))
   verified=contains(bytes,n,"CHIRAL_EARLY_OVERLAY_BLOCKED: no file redirection or visual hooks.")&&
            contains(bytes,n,"[pid=")&&contains(bytes,n,"ATLAS Chiral Addon Test14")&&
            !contains(bytes,n,"CHIRAL_TEST14_READY");
  CloseHandle(file);
 }
 const char good[]="CHIRAL_LOADER_SMOKE: DLL loaded; wrong executable refused before hooks.\r\n";
 const char bad[]="CHIRAL_LOADER_SMOKE FAILED\r\n";DWORD n=0;
 WriteFile(GetStdHandle((DWORD)-11),verified?good:bad,verified?sizeof(good)-1:sizeof(bad)-1,&n,nullptr);
 // Process termination owns cleanup. Never unload the DLL beneath its worker.
 ExitProcess(verified?0:2);
}
