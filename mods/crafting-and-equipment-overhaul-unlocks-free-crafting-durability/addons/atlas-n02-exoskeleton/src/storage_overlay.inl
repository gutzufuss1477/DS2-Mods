// DS2 opens resource archives through DirectStorageCore, not its own file IAT.
// Bind after the factory is created, before the caller can open any files.
using ResolveProc=void*(WINAPI*)(HMODULE,const char*);
using StorageFactory=i32(WINAPI*)(const void*,void**);
ResolveProc nextResolve=nullptr;
StorageFactory nextFactory=nullptr;
OpenW nextStorageOpen=nullptr;
constexpr char StorageSHA[]="66c5ffc2f525fb1ce33f9fec4eceda122545201a14c5397c42cd91fc956c7c6a";
constexpr char StorageCoreSHA[]="07a228f5c0d669ed39297bd001430b77ea66de894e4a4190d8e0e1b78b53e96b";
bool exchangeImport(u64 slot,u64 expected,u64 replacement){
 DWORD old=0,ignored=0;
 if(!VirtualProtect((void*)slot,8,4,&old))return false;
 bool changed=u64(_InterlockedCompareExchange64((long long*)slot,(long long)replacement,(long long)expected))==expected;
 if(!VirtualProtect((void*)slot,8,old,&ignored)){
  if(changed)_InterlockedCompareExchange64((long long*)slot,(long long)expected,(long long)replacement);
  VirtualProtect((void*)slot,8,old,&ignored);return false;
 }
 return changed;
}
HANDLE WINAPI openStorageW(const WCHAR*p,DWORD a,DWORD s,void*sa,DWORD d,DWORD f,HANDLE t){
 int index=readOpen(a,d)?overlayIndex(p):-1;
 if(index<0&&readOpen(a,d))unmatchedPath(p);
 if(index>=0&&verifyOverlay()){
  static volatile long reported[3]={};
  if(!_InterlockedExchange(&reported[index],1))log(index==0?"CHIRAL_DSTORAGE_ARCHIVE":"CHIRAL_DSTORAGE_LINKS");
  return nextStorageOpen(CacheFiles[index].cache,a,s,sa,d,f,t);
 }
 return nextStorageOpen(p,a,s,sa,d,f,t);
}
bool installStorageCore(){
 if(!supportedRouter())return false;
 long state=_InterlockedCompareExchange(&storageState,1,0);
 if(state==2)return true;if(state==-1)return false;
 if(state==1){while((state=_InterlockedCompareExchange(&storageState,1,1))==1)Sleep(1);return state==2;}
 HMODULE core=GetModuleHandleW(L"DStorageCore.dll");char digest[65];
 u64 base=u64(core);u32 pe=core?read<u32>(base+0x3c):0;
 bool ok=core&&pe>=0x40&&pe<=0x1000&&read<u32>(base+pe)==0x4550&&
  read<u32>(base+pe+8)==0x34c782e3&&read<u32>(base+pe+0x50)==0x176000&&
  moduleDigest(core,digest)&&sameDigest(digest,StorageCoreSHA);
 if(ok){
  // This api-ms-win-core-file import resolves directly to KernelBase;
  // Kernel32's exported compatibility wrapper has a different address.
  auto expected=GetProcAddress(GetModuleHandleW(L"kernelbase.dll"),"CreateFileW");
  nextStorageOpen=(OpenW)read<u64>(base+0x58158);
  bool native=expected&&u64(nextStorageOpen)==u64(expected);
  HMODULE owner=nullptr;
  bool passiveBoot=!native&&nextStorageOpen&&GetModuleHandleExW(6,(const WCHAR*)nextStorageOpen,&owner)&&publishedBootModule(owner);
  // The unchanged published shoe ASI installs early IAT shims at load time.
  // The host selects ONLY this runtime when both are present. Its boot peer
  // never commits a graph and remains a forwarding-only shim.
  ok=native||passiveBoot;
  if(passiveBoot)log("EXO_LEGACY_BOOT_CHAIN: verified published boot file-open shim retained, no second active cache.");
  HMODULE pinned=nullptr;
  ok=ok&&GetModuleHandleExW(5,(const WCHAR*)core,&pinned);
  if(ok)ok=exchangeImport(base+0x58158,u64(nextStorageOpen),u64(&openStorageW));
 }
 _InterlockedExchange(&storageState,ok?2:-1);
 log(ok?"CHIRAL_DSTORAGE_READY: exact DirectStorageCore file import bound before factory return.":
        "CHIRAL_DSTORAGE_BLOCKED: unknown library/import; private cache disabled.");
 return ok;
}
i32 WINAPI storageFactory(const void*iid,void**result){
 i32 hr=nextFactory(iid,result);
 installStorageCore();
 return hr;
}
bool factoryName(const char*name){
 if(u64(name)<0x10000)return false; // GetProcAddress also accepts ordinals.
 const char expected[]="DStorageGetFactory";
 for(u32 i=0;i<sizeof(expected);++i)if(name[i]!=expected[i])return false;
 return true;
}
void* WINAPI resolveStorage(HMODULE module,const char*name){
 void* actual=nextResolve(module,name);
 if(!supportedRouter())return actual;
 if(!actual||!factoryName(name)||module!=GetModuleHandleW(L"dstorage.dll"))return actual;
 char digest[65];
 if(!moduleDigest(module,digest)||!sameDigest(digest,StorageSHA)){
  _InterlockedExchange(&storageState,-1);log("CHIRAL_DSTORAGE_BLOCKED: unverified factory library.");return actual;
 }
 if(nextFactory&&u64(nextFactory)!=u64(actual))return actual;
 nextFactory=(StorageFactory)actual;
 log("CHIRAL_DSTORAGE_FACTORY: verified factory export intercepted.");
 return (void*)&storageFactory;
}
bool earlyStorage(){
 nextResolve=(ResolveProc)read<u64>(image+0x2c170d8);
 if(nextResolve&&exchangeImport(image+0x2c170d8,u64(nextResolve),u64(&resolveStorage)))return true;
 // No partial early installation on failure.
 exchangeImport(image+0x2c16f40,u64(&openOverlayW),u64(nextOpenW));
 exchangeImport(image+0x2c16f00,u64(&openOverlayA),u64(nextOpenA));
 return false;
}
