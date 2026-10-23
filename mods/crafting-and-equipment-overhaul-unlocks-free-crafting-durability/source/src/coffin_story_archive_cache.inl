// Main-mod-owned runtime caches. Original game and addon files are read-only.
#include "coffin_registered_group_stream.hpp"
#include "coffin_archive_profiles.hpp"
#include "coffin_embedded_private_links.hpp"
namespace coffin_archive_cache {
using namespace craft;
constexpr u32 RegisteredGroupDecoded=5337u;
const coffin_archive_profiles::Profile* profile=&coffin_archive_profiles::Native;
constexpr char ShoeAddonSha[]="f2193e540a04e9b30507ee69a325b91feacdbed97ecc4dd9fa6d70b5fc5b172b";
constexpr char StorageCoreSha[]="07a228f5c0d669ed39297bd001430b77ea66de894e4a4190d8e0e1b78b53e96b";
static_assert(coffin_group_embedded::PackedSize==1631u,"Registered resource metadata compression changed");
WCHAR sourcePath[2048]={},privatePath[2048]={},tempPath[2048]={},folderPath[2048]={},gameDir[2048]={};
WCHAR sourceLinksPath[2048]={},privateLinksPath[2048]={},tempLinksPath[2048]={};
HANDLE sourcePin=InvalidHandle,privatePin=InvalidHandle;
HANDLE sourceLinksPin=InvalidHandle,privateLinksPin=InvalidHandle;
volatile long cacheState=0;  // 2 = exact verified generated private cache
u64 read64(const u8*p){u64 n=0;memcpy(&n,p,8);return n;}
u32 read32(const u8*p){u32 n=0;memcpy(&n,p,4);return n;}
void put64(u8*p,u64 n){memcpy(p,&n,8);}
void put32(u8*p,u32 n){memcpy(p,&n,4);}
bool append(WCHAR*out,const WCHAR*base,const WCHAR*suffix){
 u32 n=0;for(;base[n];++n){if(n>=1800)return false;out[n]=base[n];}
 for(u32 j=0;suffix[j];++j){if(n>=2047)return false;out[n++]=suffix[j];}
 out[n]=0;return true;
}
bool locations(){
 u32 n=GetModuleFileNameW(nullptr,gameDir,1790);
 if(!n||n>=1790)return false;
 while(n&&gameDir[n-1]!=L'\\'&&gameDir[n-1]!=L'/')--n;
 gameDir[n]=0;
 return append(sourcePath,gameDir,profile->sourcePath)&&
        append(sourceLinksPath,gameDir,profile->linksPath)&&
        append(folderPath,gameDir,L"ds2_crafting_story_cache\\")&&
        append(privatePath,folderPath,profile->cacheName)&&
        append(tempPath,privatePath,L".building")&&
        append(privateLinksPath,folderPath,profile->linksName)&&
        append(tempLinksPath,privateLinksPath,L".building");
}
bool shaMatches(const WCHAR*path,u64 exactSize,const char*expect) {
 HANDLE input=CreateFileW(path,0x80000000u,1,nullptr,3,0x80,nullptr);
 if(input==InvalidHandle)return false;
 long long length=0;
 bool ok=GetFileSizeEx(input,&length)&&length>0&&(u64)length==exactSize;
 u8 *buf=ok?(u8*)VirtualAlloc(nullptr,65536,0x3000,4):nullptr;
 if(!buf)ok=false;
 Sha256 sha;
 for(u64 left=exactSize;ok&&left;){
  u32 take=left>65536?65536u:u32(left),got=0;
  if(!ReadFile(input,buf,take,&got,nullptr)||got!=take){ok=false;break;}
  sha.update(buf,take);left-=take;
 }
 if(buf)VirtualFree(buf,0,0x8000);
 CloseHandle(input);
 if(!ok)return false;
 u8 digest[32];sha.finish(digest);
 constexpr char hex[]="0123456789abcdef";
 for(u32 i=0;i<32;++i)
  if(hex[digest[i]>>4]!=expect[2*i]||hex[digest[i]&15]!=expect[2*i+1])return false;
 return expect[64]==0;
}
bool writeAll(HANDLE h,const u8*buf,u64 n){
 while(n){u32 chunk=n>1048576ull?1048576u:u32(n);DWORD done=0;
  if(!WriteFile(h,buf,chunk,&done,nullptr)||done!=chunk)return false;
  buf+=chunk;n-=chunk;
 }
 return true;
}
// Append the exact registered Story resource links to a read-only source.
bool buildPinnedLinkCache(){
 if(!shaMatches(sourceLinksPath,profile->linkSourceSize,profile->sourceLinksSha))return false;
 if(shaMatches(privateLinksPath,(profile->linkSourceSize+coffin_private_links::TailBytes),profile->targetLinksSha))return true;
 HANDLE source=CreateFileW(sourceLinksPath,0x80000000u,1,nullptr,3,0x80,nullptr);
 if(source==InvalidHandle)return false;
 HANDLE target=CreateFileW(tempLinksPath,0x40000000u,0,nullptr,2,0x80,nullptr);
 if(target==InvalidHandle){CloseHandle(source);return false;}
 u8* buf=(u8*)VirtualAlloc(nullptr,1048576u,0x3000,4);
 bool ok=buf!=nullptr;
 for(u64 left=profile->linkSourceSize;ok&&left;){
  u32 take=left>1048576ull?1048576u:(u32)left,got=0;
  if(!ReadFile(source,buf,take,&got,nullptr)||got!=take||
     !writeAll(target,buf,take)){ok=false;break;}
  left-=take;
 }
 if(ok)ok=writeAll(target,coffin_private_links::Tail,
                      coffin_private_links::TailBytes)&&FlushFileBuffers(target)!=0;
 CloseHandle(target);CloseHandle(source);
 if(buf)VirtualFree(buf,0,0x8000);
 if(ok)ok=shaMatches(tempLinksPath,(profile->linkSourceSize+coffin_private_links::TailBytes),profile->targetLinksSha);
 if(ok)ok=MoveFileExW(tempLinksPath,privateLinksPath,1|8)!=0;
 if(!ok){DeleteFileW(tempLinksPath);return false;}
 return shaMatches(privateLinksPath,(profile->linkSourceSize+coffin_private_links::TailBytes),profile->targetLinksSha);
}
bool buildFromPinnedSource() {
 if(!locations())return false;
 if(!CreateDirectoryW(folderPath,nullptr)&&GetLastError()!=183u)return false;
 if(!shaMatches(sourcePath,profile->oldPhysical,profile->sourceSha)||!buildPinnedLinkCache())return false;
 if(shaMatches(privatePath,profile->newPhysical,profile->targetSha))return true;
 HANDLE src=CreateFileW(sourcePath,0x80000000u,1,nullptr,3,0x80,nullptr);
 if(src==InvalidHandle)return false;
 HANDLE dest=CreateFileW(tempPath,0x40000000u,0,nullptr,2,0x80,nullptr);
 if(dest==InvalidHandle){CloseHandle(src);return false;}
 u8 header[32]={};
 u8 *table=(u8*)VirtualAlloc(nullptr,profile->oldChunks*32,0x3000,4);
 u8 *block=(u8*)VirtualAlloc(nullptr,1048576,0x3000,4);
 bool ok=table&&block;
 DWORD got=0;
 if(ok)ok=ReadFile(src,header,32,&got,nullptr)&&got==32&&
  read32(header)==0x52415344u&&
  header[4]==3&&header[5]==0&&header[6]==1&&header[7]==0&&
  read32(header+8)==profile->oldChunks&&read32(header+12)==(32u+32u*profile->oldChunks)&&
  read64(header+16)==profile->oldVirtual;
 if(ok)ok=ReadFile(src,table,profile->oldChunks*32,&got,nullptr)&&got==profile->oldChunks*32;
 u64 virtualAt=0;bool valid=ok;
 for(u32 i=0;ok&&i<profile->oldChunks;++i){
  u8 *row=table+i*32;
  u64 off=read64(row),physical=read64(row+8);
  u32 rawSize=read32(row+16),packed=read32(row+20);
  if(off!=virtualAt||!rawSize||rawSize>268435456u||!packed||
     physical<(32u+32u*profile->oldChunks)||physical+packed>profile->oldPhysical||row[24]!=3) {
   valid=false;break;
  }
  virtualAt+=rawSize;
  put64(row+8,physical+32);
 }
 if(!valid||virtualAt!=profile->oldVirtual)ok=false;
 if(ok){
  put32(header+8,(profile->oldChunks+1u));put32(header+12,(64u+32u*profile->oldChunks));
  put64(header+16,(profile->oldVirtual+RegisteredGroupDecoded));
  u8 finalRow[32]={};
  put64(finalRow,profile->oldVirtual);
  put64(finalRow+8,profile->oldPhysical+32);
  put32(finalRow+16,RegisteredGroupDecoded);
  put32(finalRow+20,coffin_group_embedded::PackedSize);
  finalRow[24]=3;
  ok=writeAll(dest,header,32)&&writeAll(dest,table,profile->oldChunks*32)&&
     writeAll(dest,finalRow,32);
 }
 if(ok)ok=SetFilePointerEx(src,(long long)(32u+32u*profile->oldChunks),nullptr,0)!=0;
 for(u64 left=profile->oldPhysical-(32u+32u*profile->oldChunks);ok&&left;){
  u32 take=left>1048576ull?1048576u:u32(left),read=0;
  if(!ReadFile(src,block,take,&read,nullptr)||read!=take||
     !writeAll(dest,block,take)){ok=false;break;}
  left-=take;
 }
 if(ok)ok=writeAll(dest,coffin_group_embedded::GroupLz4Block,coffin_group_embedded::PackedSize)&&
          FlushFileBuffers(dest)!=0;
 CloseHandle(dest);CloseHandle(src);
 if(table)VirtualFree(table,0,0x8000);
 if(block)VirtualFree(block,0,0x8000);
 if(ok)ok=shaMatches(tempPath,profile->newPhysical,profile->targetSha);
 if(ok)ok=MoveFileExW(tempPath,privatePath,1|8)!=0;
 if(!ok){DeleteFileW(tempPath);return false;}
 return shaMatches(privatePath,profile->newPhysical,profile->targetSha);
}
bool prepare(bool withAddon=false){
 long prior=_InterlockedCompareExchange(&cacheState,1,0);
 if(prior==2)return profile->addon==withAddon;
 if(prior==-1)return false;
 if(prior==1){
  for(u32 wait=0;wait<3000;++wait){Sleep(10);
   long s=_InterlockedCompareExchange(&cacheState,0,0);
   if(s!=1)return s==2&&profile->addon==withAddon;
  }
  return false;
 }
 profile=withAddon?&coffin_archive_profiles::Chiral:&coffin_archive_profiles::Native;
 bool ok=buildFromPinnedSource();
 if(ok){
  sourcePin=CreateFileW(sourcePath,0x80000000u,1,nullptr,3,0x80,nullptr);
  privatePin=CreateFileW(privatePath,0x80000000u,1,nullptr,3,0x80,nullptr);
  sourceLinksPin=CreateFileW(sourceLinksPath,0x80000000u,1,nullptr,3,0x80,nullptr);
  privateLinksPin=CreateFileW(privateLinksPath,0x80000000u,1,nullptr,3,0x80,nullptr);
  ok=sourcePin!=InvalidHandle&&privatePin!=InvalidHandle&&
     sourceLinksPin!=InvalidHandle&&privateLinksPin!=InvalidHandle;
 }
 if(!ok){
  if(sourcePin!=InvalidHandle)CloseHandle(sourcePin);
  if(privatePin!=InvalidHandle)CloseHandle(privatePin);
  if(sourceLinksPin!=InvalidHandle)CloseHandle(sourceLinksPin);
  if(privateLinksPin!=InvalidHandle)CloseHandle(privateLinksPin);
  sourcePin=privatePin=sourceLinksPin=privateLinksPin=InvalidHandle;
 }
 _InterlockedExchange(&cacheState,ok?2:-1);
 return ok;
}
bool ready(){return _InterlockedCompareExchange(&cacheState,0,0)==2;}
} // namespace coffin_archive_cache
