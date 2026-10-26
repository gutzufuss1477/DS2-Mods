#include "boot_companion_api.h"
AtlasBootCompanionV1 bootCompanion={};
bool bootCompanionReady=false;
bool bootFunctionOwned(HMODULE module,const void* function){
 HMODULE owner=nullptr;
 return function&&GetModuleHandleExW(6,(const WCHAR*)function,&owner)&&owner==module;
}
bool bootCompanionShape(const AtlasBootCompanionV1&c,const char*assetHash){
 return c.size==sizeof(c)&&c.version==1&&c.nativeLayout==crafting_addon::NativeLayout&&c.atlasBootId==103&&
  sameDigest(assetHash,GraphPayloadSHA)&&c.storageOpenShim&&c.prepareMaterials&&c.syncMaterials&&c.isPassive;
}
bool queryBootCompanion(HMODULE module,AtlasBootCompanionV1&out){
 out={};if(!module||module!=GetModuleHandleW(L"ds2_atlas_chiral_addon.asi"))return false;
 auto query=(AtlasBootCompanionQuery)GetProcAddress(module,"DS2AtlasBootCompanionQueryV1");
 if(!query)return false;
 AtlasBootCompanionV1 candidate={};char assetHash[65]={};
 if(!readmem(u64(query(1)),&candidate,sizeof(candidate))||
   !readmem(u64(candidate.graphPayloadSHA256),assetHash,sizeof(assetHash))||!bootCompanionShape(candidate,assetHash)||
   !bootFunctionOwned(module,candidate.storageOpenShim)||!bootFunctionOwned(module,(void*)candidate.prepareMaterials)||
   !bootFunctionOwned(module,(void*)candidate.syncMaterials)||!bootFunctionOwned(module,(void*)candidate.isPassive)||
   !candidate.isPassive())return false;
 out=candidate;return true;
}
bool compatibleBootModule(HMODULE module,const void*storageShim=nullptr){
 if(!module||module!=GetModuleHandleW(L"ds2_atlas_chiral_addon.asi"))return false;
 AtlasBootCompanionV1 peer={};
 if(queryBootCompanion(module,peer))return !storageShim||storageShim==peer.storageOpenShim;
 // Legacy release remains supported without pretending it has the new material fix.
 char digest[65];return moduleDigest(module,digest)&&sameDigest(digest,PublishedBootSHA);
}
bool prepareBootCompanion(){
 bootCompanionReady=false;
 if(!bootsEnabled||!bootCompanion.prepareMaterials)return true;
 HMODULE module=GetModuleHandleW(L"ds2_atlas_chiral_addon.asi"),pinned=nullptr;
 if(!module||!GetModuleHandleExW(5,(const WCHAR*)module,&pinned)||!bootCompanion.isPassive()||!bootCompanion.prepareMaterials())return false;
 bootCompanionReady=true;log("EXO_BOOT_MATERIALS_READY: negotiated boot runtime supplies current suit overrides.");return true;
}
void syncBootCompanion(u64 art,bool active){
 if(bootsEnabled&&bootCompanionReady)bootCompanion.syncMaterials(art,active?1:0);
}
