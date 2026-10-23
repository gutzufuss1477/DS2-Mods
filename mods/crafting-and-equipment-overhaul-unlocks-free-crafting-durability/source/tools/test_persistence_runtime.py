"""Execute production metadata IO and save/load wrappers in a disposable test process.
Only the four engine calls and engine memory reads are replaced with fixtures.
"""
from pathlib import Path
import subprocess
R=Path(__file__).resolve().parents[1]
source=(R/'src/coffin_persistence_runtime.inl').read_text()
for rva,name in [('0x1184650','nativeSaveRecord'),('0x1184b50','nativeLoadRecord'),('0x11e0b10','nativeSaveAll'),('0x11e1850','nativeLoadAll')]:
    old=f'((void(*)(u64,u64))(image+{rva}))'
    assert source.count(old)==1
    source=source.replace(old,name)
prefix=r'''
#include "../src/win_api.hpp"
#include "../src/coffin_persistence_policy.hpp"
#include <intrin.h>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
using namespace craft;
constexpr u64 image=0x140000000;
u8 record[0x160]={},bag[0x80]={},serialized[coffin_persistence::RecordSize]={},sourceBytes[coffin_persistence::RecordSize]={};
u64 uid=0x475a0000004b26;unsigned saveCalls=0,loadCalls=0,saveAllCalls=0,loadAllCalls=0;
bool freshRed=true,failMove=false;std::wstring root;
namespace persistence_runtime{bool isLoaded(u64);void saveRecord(u64,u64);void loadRecord(u64,u64);}
namespace coffin_direct_runtime{
 void*storyOwnersLock=nullptr;coffin_session::Identity storyOwners[2]={};
 void beginFabrication(){}
 bool isStoryCargo(u64 a){return a==uid&&(freshRed||persistence_runtime::isLoaded(a));}
}
volatile long arm=0,storyPending=0;
bool prepareDirect(){return true;}
struct Text{void add(const char*){}void dec(u64){}void hex(u64,unsigned){}};
void say(const char*){}void log(const Text&){}
template<class T>void field(u8*p,unsigned off,T value){std::memcpy(p+off,&value,sizeof(value));}
bool readmem(u64 a,void*out,u32 n){
 for(auto span: {std::pair<u8*,unsigned>{record,sizeof(record)},{bag,sizeof(bag)},{serialized,sizeof(serialized)}}){
  u64 begin=u64(span.first);if(a>=begin&&a+n>=a&&a+n<=begin+span.second){std::memcpy(out,(void*)a,n);return true;}}
 return false;
}
template<class T>bool rd(u64 a,T&v){return readmem(a,&v,sizeof(v));}
bool path(WCHAR*out,const WCHAR*name){auto s=root+name;if(s.size()>=2048)return false;std::memcpy(out,s.c_str(),(s.size()+1)*sizeof(WCHAR));return true;}
bool writeall(HANDLE h,const void*p,u32 n){DWORD got=0;return WriteFile(h,p,n,&got,nullptr)&&got==n;}
void nativeSaveRecord(u64 dst,u64){++saveCalls;std::memcpy((void*)dst,sourceBytes,sizeof(sourceBytes));}
void nativeLoadRecord(u64 src,u64 dst){++loadCalls;std::memcpy((void*)(dst+0x18),(void*)(src+0xd0),8);}
void nativeSaveAll(u64,u64){++saveAllCalls;persistence_runtime::saveRecord(u64(serialized),u64(record));}
void nativeLoadAll(u64,u64){++loadAllCalls;persistence_runtime::loadRecord(u64(serialized),u64(record));}
void*relayNear(u64,u64){return nullptr;}bool patchOnce(u64,const u8*,void*){return false;}
BOOL testMove(const WCHAR*a,const WCHAR*b,DWORD flags){return failMove?0:MoveFileExW(a,b,flags);}
#define MoveFileExW testMove
'''
suffix=r'''
#undef MoveFileExW
std::vector<u8> fileBytes(const WCHAR*p){auto*f=_wfopen(p,L"rb");if(!f)return {};std::fseek(f,0,SEEK_END);auto n=std::ftell(f);std::rewind(f);std::vector<u8>b(n);std::fread(b.data(),1,b.size(),f);std::fclose(f);return b;}
int main(){
 using namespace persistence_runtime;unsigned n=0,failed=0;auto ck=[&](bool b,const char*s){++n;if(!b){++failed;std::printf("FAIL %s\n",s);}};
 WCHAR tmp[2048];GetTempPathW(2048,tmp);root=std::wstring(tmp)+L"ds2-v1956-test-"+std::to_wstring(GetCurrentProcessId())+L"\\";
 ck(CreateDirectoryW(root.c_str(),nullptr)!=0,"isolated test directory created");
 field(record,0,image+0x327da68);field(record,0x18,uid);field(record,0x38,u64(bag));field(bag,0x44,u32(0x2a889790));
 field(sourceBytes,0xd0,uid);field(sourceBytes,0x11c,u32(0x2a889790));
 initialize();active=1;ck(writable&&count==0,"missing metadata starts empty");
 saveAll(0,0);ck(saveAllCalls==1&&saveCalls==1,"native saving called exactly once");
 ck(!std::memcmp(sourceBytes,serialized,sizeof(serialized)),"actual production wrapper leaves all serialized bytes unchanged");
 auto first=fileBytes(metadataPath);ck(first.size()==sizeof(Header)+sizeof(Entry)&&count==1&&flushed==generation,"atomic metadata commit completed");
 std::memset(entries,0,sizeof(entries));count=0;flushed=generation=0;writable=false;freshRed=false;
 initialize();ck(writable&&count==1,"new process recovers metadata from disk");
 loadAll(0,0);ck(loadAllCalls==1&&loadCalls==1,"native loading called exactly once");
 ck(isLoaded(uid),"existing red coffin survives simulated process restart and load");
 ck(!std::memcmp(sourceBytes,serialized,sizeof(serialized)),"load wrapper also leaves save bytes unchanged");
 serialized[0x40]^=1;loadAll(0,0);ck(!isLoaded(uid),"loading a different save clears stale red associations");
 ck(fileBytes(metadataPath)==first,"read-only load never writes metadata or saves");
 serialized[0x40]^=1;loadAll(0,0);ck(isLoaded(uid),"older exact red snapshot can be loaded again");
 loadedCount=0;sourceBytes[0x40]=2;saveAll(0,0);ck(count==1,"new normal record is not registered red");
 freshRed=true;failMove=true;sourceBytes[0x40]=3;saveAll(0,0);
 ck(fileBytes(metadataPath)==first&&generation!=flushed,"failed atomic rename preserves previous metadata");
 failMove=false;flush();ck(generation==flushed&&count==2,"next save can retry pending metadata");
 auto second=fileBytes(metadataPath);ck(second.size()==sizeof(Header)+2*sizeof(Entry),"retry file contains old and new fingerprints");
 // Real corrupted-file path must refuse to overwrite the file.
 auto*f=_wfopen(metadataPath,L"wb");std::fwrite("broken",1,6,f);std::fclose(f);
 count=0;writable=false;initialize();ck(!writable&&count==0,"corrupt file disables writes");
 sourceBytes[0x40]=4;saveAll(0,0);ck(fileBytes(metadataPath).size()==6,"corrupt metadata preserved for recovery");
 DeleteFileW(stagePath);DeleteFileW(metadataPath);
 std::printf("PRODUCTION_PERSISTENCE_ASSERTIONS=%u FAILURES=%u TEMP=%ls\n",n,failed,root.c_str());return failed?1:0;
}
'''
cpp=R/'build/persistence_actual_tests.cpp';cpp.write_text(prefix+source+suffix)
exe=R/'build/persistence_actual_tests.exe'
subprocess.run([r'C:\Program Files\LLVM\bin\clang-cl.exe','/nologo','/std:c++17','/EHsc','/MT','/O2',str(cpp),f'/Fe{exe}','/link','kernel32.lib'],cwd=R,check=True)
subprocess.run([str(exe)],cwd=R,check=True)
