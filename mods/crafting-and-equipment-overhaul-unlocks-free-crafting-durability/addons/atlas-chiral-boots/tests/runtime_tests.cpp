#define CHIRAL_SELF_TEST
#include "../src/addon.cpp"
#include "equipment_type_fixture.hpp"
API HANDLE WINAPI GetStdHandle(DWORD);
namespace {
u32 tests=0,failures=0;
void check(bool value){++tests;if(!value)++failures;}
alignas(8)u8 hostResource[0x50],hostRows[3][RowSize],component[0x500],component2[0x500];
alignas(8)u8 bodyArt[0x100],bodyResource[0x100],bodyControllers[166*0x30],bodyRender[0x80],entity[0x100],parent[0x100];
u32 bodyNames[166]={};u8 visibleParts[55]={};
void fixtureBody(){
 memset(bodyArt,0,sizeof(bodyArt));memset(bodyResource,0,sizeof(bodyResource));
 memset(bodyControllers,0,sizeof(bodyControllers));memset(bodyRender,0,sizeof(bodyRender));
 memset(bodyMaps,0,sizeof(bodyMaps));memset(visibleParts,0,sizeof(visibleParts));
 at<u64>(component,0x48)=u64(entity);at<u64>(entity,0x80)=u64(parent);at<u64>(parent,0xd8)=u64(bodyArt);
 at<u64>(bodyArt,0x30)=u64(bodyResource);at<u64>(bodyArt,0x58)=u64(bodyControllers);
 at<u64>(bodyArt,0x48)=u64(parent);at<u64>(parent,0xc8)=u64(bodyRender);
 at<u64>(bodyResource,0x10)=0x99427894a809558dull;at<u64>(bodyResource,0x18)=0xd1d869139c5e09b4ull;
 at<i32>(bodyResource,0x88)=166;at<u64>(bodyResource,0x90)=u64(bodyNames);
 bodyNames[19]=1838528733;bodyNames[20]=718898234;bodyNames[53]=0x6c1a0005;bodyNames[54]=0x6c1a0006;
 for(u32 i=0;i<166;++i){u8*c=bodyControllers+i*0x30;at<u64>(c,8)=u64(parent);at<i32>(c,0x28)=i;c[0x2d]=1;}
}
void recordVisibility(u64,i32 part,u8 enabled){if(part>=0&&part<55)visibleParts[part]=enabled;}
void recordController(u64 c,u8 enabled){
 at<u8>((void*)c,0x2c)=at<u8>((void*)c,0x2d)=enabled;
 chainVisibility(read<u64>(read<u64>(c+8)+0xc8),read<i32>(c+0x28),enabled);
}
u32 wornNames[]={1063881013,55570813,254575095,856357311,983316578,112869418,991501977,121595601};
u32 hangingNames[]={824723485,932823509};
void fixture(bool hanging){
 memset(hostResource,0,sizeof(hostResource));memset(hostRows,0,sizeof(hostRows));memset(component,0,sizeof(component));
 hostRows[0][0]=11;hostRows[1][0]=20;hostRows[2][0]=103;
 at<i32>(hostRows[0],8)=at<i32>(hostRows[0],12)=hanging?2:8;
 at<u64>(hostRows[0],16)=hanging?u64(hangingNames):u64(wornNames);
 at<u32>(hostResource,8)=1;
 at<u64>(hostResource,0x10)=0x4A634CBDCE790000ull|(u64(hanging?21:20)<<8)|103;
 at<u64>(hostResource,0x18)=0xB7AE21D89446A581ull;
 const u32 off=hanging?0x20:0x28;
 at<i32>(hostResource,off)=at<i32>(hostResource,off+4)=3;
 at<u64>(hostResource,off+8)=u64(hostRows);
 at<u64>(component,0x30)=u64(hostResource);
 copyCount=0;meshBusy=0;announced[0]=announced[1]=blocked[0]=blocked[1]=false;
 overlayState=2;fixtureBody();
}
u64 observedResource=0;u32 hostCalls=0;
void(*fakeNativeWorn)(void*)=nullptr;void(*fakeNativeHung)(void*)=nullptr;
void fakeHostWorn(void*p){++hostCalls;fakeNativeWorn(p);}
void fakeHostHung(void*p){++hostCalls;fakeNativeHung(p);}
void writeAbs(u8* p,u64 address){const u8 op[]={0xff,0x25,0,0,0,0};copy(p,op,6);at<u64>(p,6)=address;}
u32 menuCalls=0,catalogueCalls=0;const char* observedIcon=nullptr;
u64 fakeHostIcon(u64,const char*const*name){++menuCalls;observedIcon=*name;return 0x12345678;}
void* fakeHostCatalogue(void*p){++catalogueCalls;return p;}
__declspec(noinline) void menuPolicyTests(){
 alignas(8)u8 list[0xc0]={},before[0xc0]={};IconString donor=iconString("ut_ui_icon_sam_boot_pizza");
 at<u64>(list,0x10)=0x4A634CBDCE790267ull;at<u64>(list,0x18)=0xB7AE21D89446A581ull;
 at<u32>(list,0x40)=1000103;at<u32>(list,0x44)=iconCode("DS2Mod.Atlas.Boots.Equipment.v1");
 at<u64>(list,0x50)=u64(donor.data);copy(before,list,sizeof(list));
 check(bindAtlasIconList(u64(list))&&iconFallbackName==donor.data);
 check(same(list,before,0x50)&&same(list+0x58,before+0x58,sizeof(list)-0x58));
 check(privateIconName(at<u64>(list,0x50))&&!privateIconName(u64(donor.data)));
 check(bindAtlasIconList(u64(list))); // repeat is idempotent
 copy(list,before,sizeof(list));at<u32>(list,0x40)=1000001;
 check(!bindAtlasIconList(u64(list))&&at<u64>(list,0x50)==u64(donor.data)); // ordinary boots untouched
 copy(list,before,sizeof(list));at<u64>(list,0x10)^=1;
 check(!bindAtlasIconList(u64(list))); // exact host UUID required
 copy(list,before,sizeof(list));at<u64>(list,0x50)=1;
 check(!bindAtlasIconList(u64(list))); // malformed pointer refused
 auto wrong=chiralIconName;wrong.hash^=1;
 check(!privateIconName(u64(wrong.data)));wrong.hash=0xffffffffu;
 check(privateIconName(u64(wrong.data)));wrong.length++;
 check(!privateIconName(u64(wrong.data)));
 IconGraph g={};u8 ui[0x38]={},gpu[0x188]={};
 at<u64>(g.ui,0)=1;at<u64>(g.texture,0)=2;at<u64>(g.gpu,0)=3;
 at<u32>(ui,0x24)=256;at<u32>(ui,0x28)=160;gpu[0x29]=75;
 check(fillIconGraph(g,ui,gpu,0x1234,0x5678));
 check(g.gpu[0x29]==68&&at<u32>(g.gpu,0x2c)==(0x20000000u|256|(160<<15)));
 check(at<u64>(g.ui,0x30)==u64(g.texture)&&at<u64>(g.texture,0x20)==u64(g.gpu));
 check(at<u64>(g.gpu,0x80)==0x1234&&at<u64>(g.gpu,0x90)==0x5678);
 check(!fillIconGraph(g,ui,gpu,0,0x5678));ui[0x20]=1;
 check(!fillIconGraph(g,ui,gpu,0x1234,0x5678));ui[0x20]=0;
 at<u32>(ui,0x24)=512;check(!fillIconGraph(g,ui,gpu,0x1234,0x5678));
 iconBoundList=0;iconFallbackName=nullptr;
}
__declspec(noinline) void fakeHostImage(){
 image=u64(VirtualAlloc(nullptr,0x6400000,0x3000,4));
 check(image!=0);if(!image)return;
 hostBase=u64(GetModuleHandleW(nullptr));u32 pe=at<u32>((void*)hostBase,0x3c);
 hostSize=at<u32>((void*)hostBase,pe+0x50);
 const u8 prologueW[]={0x48,0x89,0x5c,0x24,0x20};
 const u8 prologueH[]={0x40,0x53,0x55,0x57,0x48,0x81,0xec,0xd0,0,0,0};
 const u8 prologueV[]={0x48,0x83,0xec,0x38,0x48,0x8b,0x41,0x60,0x48,0x83,0xc4,0x38,0xc3};
 copy((void*)(image+VisibilityRva),prologueV,sizeof(prologueV));
 writeAbs((u8*)(image+0x3478e0),u64(&recordController));
 for(u32 i=0;i<2;++i){
  u32 rva=i?HungRva:WornRva,n=i?11:5;u8*stub=(u8*)(image+0x800000+i*256);
  writeAbs(stub,i?u64(&fakeHostHung):u64(&fakeHostWorn));
  copy(stub+128,i?prologueH:prologueW,n);writeAbs(stub+128+n,image+rva+n);
  check(relativeJump((u8*)(image+rva),image+rva,u64(stub)));
  // Simulated native continuation records the resource pointer actually seen.
  u8*body=(u8*)(image+rva+n);body[0]=0x48;body[1]=0xb8;at<u64>(body,2)=u64(&observedResource);
  const u8 record[]={0x48,0x8b,0x51,0x30,0x48,0x89,0x10};copy(body+10,record,sizeof(record));
  if(!i)body[17]=0xc3;
  else{const u8 epilogue[]={0x48,0x81,0xc4,0xd0,0,0,0,0x5f,0x5d,0x5b,0xc3};copy(body+17,epilogue,sizeof(epilogue));}
 }
 fakeNativeWorn=(void(*)(void*))(image+0x800080);fakeNativeHung=(void(*)(void*))(image+0x800180);
 const u8 menuPrologue[]={0x48,0x89,0x5c,0x24,0x08};
 for(u32 i=0;i<2;++i){
  u32 rva=i?IconRva:CatalogueRva;u8*stub=(u8*)(image+0x800400+i*256);
  writeAbs(stub,i?u64(&fakeHostIcon):u64(&fakeHostCatalogue));
  copy(stub+64,menuPrologue,5);writeAbs(stub+69,image+rva+5);
  check(relativeJump((u8*)(image+rva),image+rva,u64(stub)));
 }
 alignas(8)u8 gameData[0x300]={},depot[0x50]={},list[0xc0]={},ordinary[0xc0]={};
 u64 pointers[]={u64(ordinary),u64(list)};u64 getters[2]={image+0x800800,image+NativeEquipmentGetterRva};
 u8*depotGetter=(u8*)getters[0];depotGetter[0]=0x48;depotGetter[1]=0xb8;
 at<u64>(depotGetter,2)=image+0x605f3a0;depotGetter[10]=0xc3;
 copy((void*)getters[1],NativeEquipmentGetter,sizeof(NativeEquipmentGetter));
 at<u64>(depot,0)=u64(getters);at<u64>(list,0)=at<u64>(ordinary,0)=u64(getters+1);
 at<u64>((void*)image,0x623e310)=u64(gameData);
 at<u64>(gameData,0x270)=at<u64>(gameData,0x280)=u64(depot);
 at<i32>(depot,0x38)=2;at<u64>(depot,0x40)=u64(pointers);
 IconString source=iconString("ut_ui_icon_sam_boot_pizza");
 at<u64>(list,0x10)=0x4A634CBDCE790267ull;at<u64>(list,0x18)=0xB7AE21D89446A581ull;
 at<u32>(list,0x40)=1000103;at<u32>(list,0x44)=iconCode("DS2Mod.Atlas.Boots.Equipment.v1");
 at<u64>(list,0x50)=at<u64>(ordinary,0x50)=u64(source.data);
 DWORD old=0;check(VirtualProtect((void*)image,0x6400000,0x20,&old)!=0);
 check(FlushInstructionCache(GetCurrentProcess(),(void*)image,0x1800000)!=0);
 check(iconType(u64(list))==0x4333710&&iconType(u64(list))!=0x4333a00);
 u8 entryBefore[2][5];copy(entryBefore[0],(void*)(image+WornRva),5);copy(entryBefore[1],(void*)(image+HungRva),5);
 check(prepare());check(install()==1);
 fixture(false);hostCalls=0;observedResource=0;
 ((void(*)(void*))(image+WornRva))(component);
 check(hostCalls==1&&observedResource==at<u64>(component,0x30)&&observedResource!=u64(hostResource));
 check(copyCount==1);
 fixture(true);observedResource=0;
 ((void(*)(void*))(image+HungRva))(component);
 check(hostCalls==2&&observedResource==at<u64>(component,0x30)&&observedResource!=u64(hostResource));
 check(same(entryBefore[0],(void*)(image+WornRva),5)&&same(entryBefore[1],(void*)(image+HungRva),5));
 check(((void*(*)(void*))(image+CatalogueRva))(component)==component&&catalogueCalls==1);
 check(iconBoundList==u64(list)&&at<u64>(list,0x50)==u64(chiralIconName.data));
 check(at<u64>(ordinary,0x50)==u64(source.data)); // exact scope through native initialization chain
 static IconSurvey survey;
 check(findAtlasIcon(u64(depot),survey)&&survey.list==u64(list));
 copy(ordinary,list,sizeof(list));survey.count=0;survey.list=0;
 check(!findAtlasIcon(u64(depot),survey)); // conflicting identity fails closed
 at<i32>(depot,0x38)=-1;survey.count=0;survey.list=0;
 check(!findAtlasIcon(u64(depot),survey));
 auto lookup=(IconLookup)(image+IconRva);
 IconString original=iconString("ut_ui_icon_sam_boot");const char*name=original.data;
 check(lookup(1,&name)==0x12345678&&menuCalls==1&&observedIcon==original.data);
 iconBoundList=1;iconFallbackName=original.data;iconFailed=true;name=chiralIconName.data;
 check(lookup(1,&name)==0x12345678&&menuCalls==2&&observedIcon==original.data);
 // A host-private weapon icon still reaches the host chain unchanged.
 IconString other=iconString("ut_ui_icon_wep_srai_higgsred");name=other.data;
 check(lookup(1,&name)==0x12345678&&menuCalls==3&&observedIcon==other.data);
}
}
extern "C" void mainCRTStartup(){
 check(verifiedHost("e7084292f784477dd032f5eeca0b9ab50551ca101ad8bf9a508dc93e4b014839")==&VerifiedHosts[0]);
 check(verifiedHost("091faec058829178ecae65fb142ce8b3bf78a01f6be49d51dfd8ea0391bf6c9d")==&VerifiedHosts[1]);
 check(verifiedHost("01e6b66c6d90a815a8f32ee39c18153b184a8d722ddce50931ccd142b95a526e")==&VerifiedHosts[2]);
 check(verifiedHost("217c74857e7ddd19072bc49728778c1b6ee6d13957bb3c31622cfb00736df4b3")==&VerifiedHosts[3]);
 check(verifiedCacheHost(VerifiedHosts[2].sha256,CacheBaseGraphSHA)==&VerifiedHosts[2]);
 check(verifiedCacheHost(VerifiedHosts[3].sha256,CacheBaseGraphSHA)==&VerifiedHosts[3]);
 check(!verifiedCacheHost(VerifiedHosts[0].sha256,CacheBaseGraphSHA)&&!verifiedCacheHost(VerifiedHosts[1].sha256,CacheBaseGraphSHA));
 check(!verifiedCacheHost(VerifiedHosts[3].sha256,GameSHA)&&!verifiedCacheHost(VerifiedHosts[3].sha256,nullptr));
 char unknown[65];copy(unknown,VerifiedHosts[1].sha256,65);unknown[63]='0';
 check(verifiedHost(unknown)==nullptr);
 check(!verifiedCacheHost(unknown,CacheBaseGraphSHA));
 check(verifiedHost("")==nullptr&&verifiedHost(nullptr)==nullptr);
 check(!sameDigest("091faec",VerifiedHosts[1].sha256));
 check(!sameDigest("091faec058829178ecae65fb142ce8b3bf78a01f6be49d51dfd8ea0391bf6c9d0",VerifiedHosts[1].sha256));
 char selfDigest[65];check(moduleDigest(nullptr,selfDigest)&&selfDigest[64]==0&&selfDigest[0]!=0);
 check(!sameDigest(selfDigest,GameSHA));
 char unavailable[65];check(!hashFile(L"Z:\\__chiral_missing_hash_probe__",unavailable)&&unavailable[0]==0);
 alignas(8)static u8 output[3][RowSize],original[3][RowSize];
 fixture(false);copy(original,hostRows,sizeof(original));
 check(makeMarkerRows((u8*)output,(u8*)hostRows,3));
 check(same(original,hostRows,sizeof(original))); // no source mutation
 check(same(output[0],hostRows[0],RowSize)&&same(output[1],hostRows[1],RowSize));
 check(output[2][0]==103&&same(output[2]+1,hostRows[0]+1,RowSize-1));
 hostRows[1][0]=103;check(!makeMarkerRows((u8*)output,(u8*)hostRows,3));
 hostRows[1][0]=11;check(!makeMarkerRows((u8*)output,(u8*)hostRows,3));
 fixture(false);hostRows[2][0]=99;check(!makeMarkerRows((u8*)output,(u8*)hostRows,3));
 check(!makeMarkerRows((u8*)output,(u8*)hostRows,65));
 check(!makeMarkerRows((u8*)output,(u8*)hostRows,0));
 check(markerNames(wornNames,8,false)&&markerNames(hangingNames,2,true));
 wornNames[0]^=1;check(!markerNames(wornNames,8,false));wornNames[0]^=1;
 u8 stub[14]={};u64 target=0;writeAbs(stub,0x100050);
 check(absoluteHostRelay(stub,0x100000,0x100,target));
 check(!absoluteHostRelay(stub,0x200000,0x100,target));
 stub[0]=0xe9;check(!absoluteHostRelay(stub,0x100000,0x100,target));
 u8 jump[5];check(relativeJump(jump,0x100000,0x200000));
 check(!relativeJump(jump,0x100000,0x900000000ull));
 fixture(false);bindMarker(component,false);
 check(copyCount==1&&at<u64>(component,0x30)!=u64(hostResource));
 auto*c=copies[0];check(c&&same(c->rows[0],hostRows[0],RowSize)&&same(c->rows[1],hostRows[1],RowSize));
 check(c&&c->rows[2][0]==103&&at<u64>(c->rows[2],16)==u64(ChiralWornNames));
 bindMarker(component,false);check(copyCount==1);
 memset(component2,0,sizeof(component2));at<u64>(component2,0x30)=u64(hostResource);
 bindMarker(component2,false);check(copyCount==1&&at<u64>(component2,0x30)==at<u64>(component,0x30));
 fixture(false);at<u64>(component,0x30)=0x12345;bindMarker(component,false);
 check(copyCount==0&&at<u64>(component,0x30)==0x12345);
 fixture(false);at<u64>(hostResource,0x10)=0;bindMarker(component,false);check(copyCount==0);
 fixture(false);wornNames[0]^=1;bindMarker(component,false);check(copyCount==0);wornNames[0]^=1;
 fixture(true);bindMarker(component,true);check(copyCount==1&&copies[0]->offset==0x20);
 check(at<u64>(copies[0]->rows[2],16)==u64(ChiralHungNames));
 fixture(false);overlayState=-1;bindMarker(component,false);check(copyCount==0);
 fixture(false);chainVisibility=recordVisibility;setControllerVisibility=recordController;
 check(syncBody(component,true)&&!bodyMaps[0].active&&visibleParts[19]&&!visibleParts[53]);
 component[0x51]=AtlasId;
 check(syncBody(component,true)&&bodyMaps[0].active&&!visibleParts[19]&&visibleParts[53]);
 visibility(u64(bodyRender),19,0);check(!visibleParts[19]&&!visibleParts[53]);
 visibility(u64(bodyRender),19,1);check(!visibleParts[19]&&visibleParts[53]);
 visibility(u64(bodyRender),10,1);check(visibleParts[10]); // attached equipment is untouched
 component[0x51]=11;check(syncBody(component,true)&&!bodyMaps[0].active&&visibleParts[19]&&!visibleParts[53]);
 at<u64>(bodyArt,0x30)=0;check(!syncBody(component,true)&&!validBody(bodyMaps[0]));
 fixture(false);BodyMap actualBody={};check(findBody(u64(bodyArt),actualBody)&&actualBody.entity==u64(parent));
 at<u64>(bodyControllers+53*0x30,8)=u64(bodyArt);check(!findBody(u64(bodyArt),actualBody));
 at<u64>(bodyControllers+53*0x30,8)=u64(parent);
 at<u64>(bodyArt,0x48)=0;check(!findBody(u64(bodyArt),actualBody));
 at<u64>(bodyArt,0x48)=u64(parent);at<u64>(parent,0xc8)=0;check(!validBody(actualBody)&&!findBody(u64(bodyArt),actualBody));
 at<u64>(parent,0xc8)=u64(bodyRender);at<u64>(parent,0xd8)=0;check(!validBody(actualBody)&&!findBody(u64(bodyArt),actualBody));
 at<u64>(parent,0xd8)=u64(bodyArt);check(findBody(u64(bodyArt),actualBody)&&validBody(actualBody));
 check(readOpen(0x80000000u,3)&&!readOpen(0xc0000000u,3)&&!readOpen(0x80000000u,2));
 check(overlayIndex(CacheFiles[0].original)==0&&overlayIndex(CacheFiles[1].original)==1&&overlayIndex(CacheFiles[2].original)==2);
 check(overlayIndex(L"LocalCacheWinGame/package/streaming_graph.core")==1);
 check(overlayIndex(L".\\LocalCacheWinGame\\package\\streaming_links.stream")==2);
 check(overlayIndex(L"C:\\other\\streaming_graph.core")==-1&&overlayIndex(L"")==-1&&overlayIndex(nullptr)==-1);
 check(pathEqual(L"\\\\?\\C:\\Game\\FILE",L"C:/game/file")&&pathEqual(L"C:/game/file",L"\\\\?\\C:\\Game\\FILE"));
 check(!pathEqual(L"\\\\?\\D:\\Game\\FILE",L"C:/game/file"));
 check(!pathEqual(L"\\",L"")&&!pathEqual(L"\\\\",L"")&&!pathEqual(L"\\\\?",L""));
 check(factoryName("DStorageGetFactory")&&!factoryName("DStorageGetFactoryCore"));
 check(!factoryName(nullptr)&&!factoryName((const char*)1)&&!factoryName("DStorageGet"));
 check(initializeHostCacheName()&&overlayIndex(hostCacheName)==1);
 storageState=0;overlayState=0;check(!verifyOverlay()&&overlayState==-1); // Unbound storage never activates.
 storageState=2;overlayState=0;check(!verifyOverlay()&&overlayState==-1); // A non-game process never activates.
 fixture(false);
 HANDLE cached=openOverlayW(CacheFiles[1].original,0x80000000u,3,nullptr,3,0x80,nullptr);
 long long size=0;check(cached!=InvalidHandle&&GetFileSizeEx(cached,&size)&&size>28334985);if(cached!=InvalidHandle)CloseHandle(cached);
 menuPolicyTests();fakeHostImage();
 char line[120]="CHIRAL_RUNTIME_TESTS ";u32 n=sizeof("CHIRAL_RUNTIME_TESTS ")-1;
 auto number=[&](u32 v){char digits[16];u32 k=0;do{digits[k++]=char('0'+v%10);v/=10;}while(v);while(k)line[n++]=digits[--k];};
 number(tests);const char middle[]=" failures=";copy(line+n,middle,sizeof(middle)-1);n+=sizeof(middle)-1;number(failures);line[n++]='\r';line[n++]='\n';
 DWORD done;WriteFile(GetStdHandle((DWORD)-11),line,n,&done,nullptr);ExitProcess(failures?1:0);
}
