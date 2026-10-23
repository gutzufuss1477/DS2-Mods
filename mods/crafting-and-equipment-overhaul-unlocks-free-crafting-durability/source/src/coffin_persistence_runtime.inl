// Included inside redcoffin_runtime, after the native model runtime.
namespace persistence_runtime {
using namespace coffin_persistence;
Entry entries[Capacity]={};u32 count=0;u64 generation=0,flushed=0;
u64 loaded[LoadedCapacity]={};u32 loadedCount=0;
void*dataLock=nullptr;void*fileLock=nullptr;
bool writable=false;volatile long active=0;
WCHAR metadataPath[2048]={},stagePath[2048]={};
volatile long savedLogs=0,loadedLogs=0,restoreLogs=0;
bool isLoaded(u64 cargo){
 if(!validCargo(cargo))return false;bool found=false;AcquireSRWLockExclusive(&dataLock);
 for(u32 i=0;i<loadedCount;++i)if(loaded[i]==cargo){found=true;break;}
 ReleaseSRWLockExclusive(&dataLock);return found;
}
void initialize(){
 if(!path(metadataPath,L"ds2_story_coffin_variants.bin")||!path(stagePath,L"ds2_story_coffin_variants.bin.tmp"))return;
 HANDLE file=CreateFileW(metadataPath,0x80000000,1,nullptr,3,0x80,nullptr);
 if(file==InvalidHandle){writable=GetLastError()==2;if(!writable)say("RED_COFFIN_V1956_METADATA_UNAVAILABLE: native save remains usable; no metadata overwrite.");return;}
 Header h;long long size=0;DWORD got=0;
 bool ok=GetFileSizeEx(file,&size)&&size>=sizeof(Header)&&size<=sizeof(Header)+sizeof(entries)&&
  ReadFile(file,&h,sizeof(h),&got,nullptr)&&got==sizeof(h)&&h.count<=Capacity;
 if(ok&&h.count)ok=ReadFile(file,entries,h.count*sizeof(Entry),&got,nullptr)&&got==h.count*sizeof(Entry);
 CloseHandle(file);
 ok=ok&&valid(h,entries,u64(size));
 if(!ok){memset(entries,0,sizeof(entries));say("RED_COFFIN_V1956_METADATA_INVALID: file retained; native appearance fallback; writing disabled.");return;}
 count=h.count;writable=true;Text m;m.add("RED_COFFIN_V1956_METADATA_READY: saved native-record fingerprints=");m.dec(count);log(m);
}
void flush(){
 AcquireSRWLockExclusive(&fileLock);AcquireSRWLockExclusive(&dataLock);
 if(!writable||generation==flushed){ReleaseSRWLockExclusive(&dataLock);ReleaseSRWLockExclusive(&fileLock);return;}
 const u64 gen=generation;const u32 n=count,bytes=sizeof(Header)+n*sizeof(Entry);
 auto*buffer=(u8*)VirtualAlloc(nullptr,bytes,0x3000,4);
 if(!buffer){ReleaseSRWLockExclusive(&dataLock);ReleaseSRWLockExclusive(&fileLock);return;}
 Header h=seal(entries,n);memcpy(buffer,&h,sizeof(h));memcpy(buffer+sizeof(h),entries,n*sizeof(Entry));
 ReleaseSRWLockExclusive(&dataLock);
 // Atomic replacement of this mod's metadata only. No path into save folders.
 HANDLE file=CreateFileW(stagePath,0x40000000,0,nullptr,2,0x80,nullptr);
 bool ok=file!=InvalidHandle;
 if(ok){ok=writeall(file,buffer,bytes)&&FlushFileBuffers(file);CloseHandle(file);}
 if(ok)ok=MoveFileExW(stagePath,metadataPath,1|8)!=0;
 if(!ok)say("RED_COFFIN_V1956_METADATA_WRITE_FAILED: previous metadata preserved; native save untouched; will retry next save.");
 AcquireSRWLockExclusive(&dataLock);if(ok)flushed=gen;ReleaseSRWLockExclusive(&dataLock);
 VirtualFree(buffer,0,0x8000);ReleaseSRWLockExclusive(&fileLock);
}
void saveRecord(u64 serialized,u64 record){
 ((void(*)(u64,u64))(image+0x1184650))(serialized,record);
 if(_InterlockedCompareExchange(&active,0,0)!=1)return;
 auto reader=[](u64 p,auto&v){return rd(p,v);};
 const u64 cargo=coffin_session::recordCargo(reader,image,record);if(!cargo)return;
 u8 bytes[RecordSize];if(!readmem(serialized,bytes,sizeof(bytes)))return;
 Entry e=fingerprint(bytes);if(e.cargo!=cargo)return;
 const bool red=coffin_direct_runtime::isStoryCargo(cargo);
 bool changed=false,full=false;AcquireSRWLockExclusive(&dataLock);
 if(writable){changed=put(entries,count,e,red);if(changed)++generation;full=red&&count==Capacity&&find(entries,count,e)<0;}
 ReleaseSRWLockExclusive(&dataLock);
 if(changed&&_InterlockedIncrement(&savedLogs)<=24){Text m;m.add("RED_COFFIN_V1956_SAVED_VARIANT: cargo=");m.hex(cargo,16);m.add(" red=");m.dec(red);m.add(" SHA256 of untouched native serialized record.");log(m);}
 if(full)say("RED_COFFIN_V1956_METADATA_FULL: existing saved associations retained; no eviction.");
}
void loadRecord(u64 serialized,u64 record){
 Entry e;u8 bytes[RecordSize];bool red=false;
 if(_InterlockedCompareExchange(&active,0,0)==1&&readmem(serialized,bytes,sizeof(bytes))){
  e=fingerprint(bytes);AcquireSRWLockExclusive(&dataLock);
  int at=find(entries,count,e);red=at>=0&&entries[at].red==1;ReleaseSRWLockExclusive(&dataLock);
 }
 ((void(*)(u64,u64))(image+0x1184b50))(serialized,record);
 auto reader=[](u64 p,auto&v){return rd(p,v);};
 if(!red||coffin_session::recordCargo(reader,image,record)!=e.cargo)return;
 AcquireSRWLockExclusive(&dataLock);bool found=false;
 for(u32 i=0;i<loadedCount;++i)if(loaded[i]==e.cargo)found=true;
 if(!found&&loadedCount<LoadedCapacity)loaded[loadedCount++]=e.cargo;
 ReleaseSRWLockExclusive(&dataLock);
 if(_InterlockedIncrement(&loadedLogs)<=24){Text m;m.add("RED_COFFIN_V1956_LOADED_VARIANT: cargo=");m.hex(e.cargo,16);m.add(" exact saved-record fingerprint matched.");log(m);}
}
void saveAll(u64 manager,u64 context){
 ((void(*)(u64,u64))(image+0x11e0b10))(manager,context);
 if(_InterlockedCompareExchange(&active,0,0)==1)flush();
}
void loadAll(u64 manager,u64 context){
 if(_InterlockedCompareExchange(&active,0,0)==1){
  AcquireSRWLockExclusive(&dataLock);loadedCount=0;memset(loaded,0,sizeof(loaded));ReleaseSRWLockExclusive(&dataLock);
  AcquireSRWLockExclusive(&coffin_direct_runtime::storyOwnersLock);
  memset(coffin_direct_runtime::storyOwners,0,sizeof(coffin_direct_runtime::storyOwners));
  ReleaseSRWLockExclusive(&coffin_direct_runtime::storyOwnersLock);
  _InterlockedExchange(&arm,0);_InterlockedExchange(&storyPending,0);coffin_direct_runtime::beginFabrication();
 }
 ((void(*)(u64,u64))(image+0x11e1850))(manager,context);
 if(_InterlockedCompareExchange(&active,0,0)==1&&loadedCount){
  if(!prepareDirect())say("RED_COFFIN_V1956_RESTORE_ASSETS_PENDING: will retry during native coffin model creation.");
 }
}
bool setup(){
 initialize();
 const u64 sites[]={0x11e0c3e,0x11e1a12,0x71529a,0x71675a};
 const u8 bytes[][5]={{0xe8,0x0d,0x3a,0xfa,0xff},{0xe8,0x39,0x31,0xfa,0xff},{0xe8,0x71,0xb8,0xac,0x00},{0xe8,0xf1,0xb0,0xac,0x00}};
 const u64 targets[]={u64(&saveRecord),u64(&loadRecord),u64(&saveAll),u64(&loadAll)};
 void*relays[4]={};u8 native[5];
 for(u32 i=0;i<4;++i)if(!readmem(image+sites[i],native,5)||memcmp(native,bytes[i],5)){say("RED_COFFIN_V1956_PERSISTENCE_REFUSED: native callsite mismatch.");return false;}
 for(u32 i=0;i<4;++i)if(!(relays[i]=relayNear(sites[i],targets[i]))){for(u32 j=0;j<i;++j)VirtualFree(relays[j],0,0x8000);return false;}
 for(u32 i=0;i<4;++i){bool ok=false;
  for(u32 attempt=0;attempt<12;++attempt){if(patchOnce(sites[i],bytes[i],relays[i])){ok=true;break;}Sleep(50);}
  if(!ok){for(u32 j=i;j<4;++j)VirtualFree(relays[j],0,0x8000);say("RED_COFFIN_V1956_PERSISTENCE_PARTIAL: all installed wrappers remain native; restart required.");return false;}
 }
 _InterlockedExchange(&active,1);say("RED_COFFIN_V1956_PERSISTENCE_READY: native serialization called once and unchanged; variant fingerprints stored only in ds2_story_coffin_variants.bin.");return true;
}
}
