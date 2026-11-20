// Included inside the runtime namespace. Runs on the native initialization
// thread, before catalogue construction and before the item manager is built.
AtlasBundle* atlas[3];
u8 bootVisualId=DefaultBootVisualId;
u64 originalBootName=0;
bool germanText(){
 char text[32]={};u64 str=read<u64>(originalBootName+0x20);
 return readmem(str,text,17) && !memcmp(text,"Transportstiefel",16);
}
bool scanSource(u64 depot,AtlasSources& s,u32 depth){
 if(depth>16 || ++s.visited>8192){log("ATLAS_SCAN_BLOCKED: resource traversal limit.");return false;}
 i32 n=read<i32>(depot+0x38);u64 table=read<u64>(depot+0x40);
 if(n<0||n>100000||(!table&&n)){log("ATLAS_SCAN_BLOCKED: invalid resource array.");return false;}
 using Type=u64(*)(u64);
 for(i32 i=0;i<n;++i){
  u64 p=read<u64>(table+8*i),vt=read<u64>(p),fn=read<u64>(vt);if(!p)continue;
  if(fn<image||fn>=image+0x2B00000){log("ATLAS_SCAN_BLOCKED: non-native type getter.");return false;}
  u64 type=((Type)fn)(p);
  if(type==image+0x605F3A0){if(!scanSource(p,s,depth+1))return false;}
  else if(type==image+0x43320A0){
   u8 id=read<u8>(p+0x20);
   if(id==CombatBokkaId)s.hybridConflict=true;
   if(id==BattleLevel3Id){
    if(s.item[2] && s.item[2]!=p)s.hybridConflict=true;
    s.item[2]=p;
   }
   if(id==BootId||id==SkeletonId)s.conflict=true;
   if(id==SourceBootId||id==SourceSkeletonId){u32 k=id==SourceBootId?0:1;if(s.item[k]&&s.item[k]!=p){log("ATLAS_SCAN_BLOCKED: duplicate source item identity.");return false;}s.item[k]=p;}
   if(id==bootVisualId||id==skeletonVisualId){u32 k=id==bootVisualId?0:1;if(s.visual[k]&&s.visual[k]!=p){log("ATLAS_SCAN_BLOCKED: duplicate visual donor identity.");return false;}s.visual[k]=p;}
   if(id==DefaultBootVisualId){
    if(s.defaultBootVisual && s.defaultBootVisual!=p)s.defaultBootAmbiguous=true;
    s.defaultBootVisual=p;
   }
   u64 list=read<u64>(p+0x40);u32 listId=read<u32>(list+0x40),listCode=read<u32>(list+0x44);
   if(listId==CombatBokkaListId || listCode==CombatBokkaList)s.hybridConflict=true;
   if(listId==BootListId||listId==SkeletonListId||listCode==BootList||listCode==SkeletonList)s.conflict=true;
  }else if(type==image+0x43340C0){
   u32 code=read<u32>(p+0x20);
   if(code==CombatBokkaRecipe)s.hybridConflict=true;
   if(code==EquipmentRecipes[2].sourceRecipe){
    if(s.recipe[2] && s.recipe[2]!=p)s.hybridConflict=true;
    s.recipe[2]=p;
   }
   if(code==BootRecipe||code==SkeletonRecipe)s.conflict=true;
   if(code==SourceBootRecipe||code==SourceSkeletonRecipe){u32 k=code==SourceBootRecipe?0:1;if(s.recipe[k]&&s.recipe[k]!=p){log("ATLAS_SCAN_BLOCKED: duplicate source recipe identity.");return false;}s.recipe[k]=p;}
  }else if(type==image+0x4334010){
   u32 code=read<u32>(p+0x44);if(code==BootBag||code==SkeletonBag)s.conflict=true;
   if(code==CombatBokkaBag)s.hybridConflict=true;
  }
 }
 return true;
}
bool prepareDepot(DepotPlan& plan,u64 depot,u32 extra){
 if(!writable((void*)(depot+0x38),16))return false;
 // A19D0 is the ordinary native fresh allocation used immediately before the
 // intercepted constructor. Never pass existing data to native array reserve:
 // its realloc path requires TLS+1A08, which is null during this initialization.
 return prepareDepotCopy(plan,depot,extra,(u64(*)(u64))(image+0xA19D0),&readmem);
}
void discardDepot(DepotPlan& plan){
 if(plan.replacement)((void(*)(u64))(image+0xA0D10))(plan.replacement);
 plan.replacement=0;
}
void appendDepot(u64 depot,void* object){
 // CoreFile owns a reference; the mod separately retains the initial pin.
 _InterlockedIncrement((long*)((u8*)object+8));
 i32& n=at<i32>((void*)depot,0x38);at<u64>((void*)read<u64>(depot+0x40),n*8)=u64(object);++n;
}
bool prepareEquipment(u32 k,const AtlasSources& s,u8 bootCatalogueVisualId,AtlasBundle*& bundle,u64 (&pins)[9]){
  u8 item[0xA0],list[0xC0],bag[0x88],recipe[0xB0],name[0x38],desc[0x38],chart[0x30],rows[6][0x30];
  u64 ip=s.item[k],rp=s.recipe[k],lp=read<u64>(ip+0x40),bp=read<u64>(rp+0x28);
  u64 np=read<u64>(ip+0x28),dp=read<u64>(lp+0x28),cp=read<u64>(lp+0x98);
  if(!readmem(ip,item,sizeof(item))||!readmem(lp,list,sizeof(list))||!readmem(bp,bag,sizeof(bag))||
     !readmem(rp,recipe,sizeof(recipe))||!readmem(np,name,sizeof(name))||!readmem(dp,desc,sizeof(desc))||
     !readmem(cp,chart,sizeof(chart))||at<i32>(chart,0x20)!=6||read<u64>(bp+0x50)!=lp)return false;
  for(u32 j=0;j<6;++j)if(!readmem(read<u64>(at<u64>(chart,0x28)+j*8),rows[j],0x30))return false;
  if(!k)originalBootName=np;
  bundle=(AtlasBundle*)VirtualAlloc(nullptr,sizeof(AtlasBundle),0x3000,4);
  if(!bundle||!prepareAtlas(*bundle,EquipmentRecipes[k],item,list,bag,recipe,name,desc,chart,rows,germanText()))return false;
  atlasFabrication(*bundle,k==2?configuredCombatBokkaRecipe():recipesEnabled);
  // The hybrid inherits the entire native Battle Lv.3 presentation graph.
  // Only the two ATLAS items borrow artwork from a second donor.
  u64 vp=ip,vl=lp;
  if(k<2){
  u8 donor[0xA0],donorList[0xC0];vp=s.visual[k];vl=read<u64>(vp+0x40);
  if(!readmem(vp,donor,sizeof(donor))||!readmem(vl,donorList,sizeof(donorList))||!atlasVisuals(*bundle,donor,donorList,k==0?bootCatalogueVisualId:skeletonVisualId))return false;
  }
  u64 p[]={ip,lp,bp,rp,np,dp,cp,vp,vl};
  for(u32 j=0;j<9;++j){pins[j]=p[j];if(!writable((void*)(p[j]+8),4))return false;}
  return true;
}
void registerAtlas(){
 if(atlas[0]){log("ATLAS_REGISTRATION_SKIPPED: already registered in this process.");return;}
 u64 game=read<u64>(image+0x623E310),items=read<u64>(game+0x280),catalogue=read<u64>(game+0x270);
 AtlasSources s;
 if(!game||!items||!catalogue){log("ATLAS_BLOCKED: source depots unavailable before catalogue construction.");return;}
 bootVisualId=static_cast<u8>(configuredBootAppearance());
 if(bootVisualId!=DefaultBootVisualId && GetModuleHandleW(L"ds2_atlas_chiral_addon.asi")){
  // The published Chiral addon binds its own parts to the default donor row.
  bootVisualId=DefaultBootVisualId;
  log("ATLAS_BOOT_APPEARANCE: Chiral Boots addon takes priority over the INI model.");
 }
 if(!scanSource(items,s,0)||(items!=catalogue&&!scanSource(catalogue,s,0)))return;
 // A boot can have worn/clip rows even when its catalogue donor is not loaded.
 u8 bootCatalogueVisualId=bootVisualId;
 if(!s.visual[0] && bootVisualId!=DefaultBootVisualId && s.defaultBootVisual && !s.defaultBootAmbiguous){
  bootCatalogueVisualId=DefaultBootVisualId;s.visual[0]=s.defaultBootVisual;
  log("ATLAS_BOOT_APPEARANCE: using Default for the catalogue. Worn and clip models follow the INI.");
 }
 if(s.conflict){log("ATLAS_BLOCKED: an ATLAS item/list/recipe/baggage identity is already occupied.");return;}
 if(!s.item[0]||!s.item[1]||!s.recipe[0]||!s.recipe[1]||!s.visual[0]||!s.visual[1]){log("ATLAS_BLOCKED: source item/recipe or visual donor missing before catalogue initialization.");return;}
 AtlasBundle* pending[3]={};u64 pins[3][9]={};
 bool ok=prepareEquipment(0,s,bootCatalogueVisualId,pending[0],pins[0]) &&
         prepareEquipment(1,s,bootCatalogueVisualId,pending[1],pins[1]);
 u32 count=2;
 if(ok){
  if(!s.hybridConflict && s.item[2] && s.recipe[2] &&
     prepareEquipment(2,s,BattleLevel3Id,pending[2],pins[2]))++count;
  else{
   if(pending[2])VirtualFree(pending[2],0,0x8000);
   pending[2]=nullptr;
   log("BOKKA_COMBAT_SKIPPED: Battle Lv.3 donor missing, invalid, or custom identity occupied.");
  }
 }
 // Stage both fresh native allocations before changing either resource list.
 DepotPlan itemPlan,cataloguePlan;
 if(ok){
  log("ATLAS_SOURCES_READY: source item/list/recipe identities validated; preparing fresh pointer tables.");
  ok=prepareDepot(itemPlan,items,items==catalogue?4*count:count) && (items==catalogue||prepareDepot(cataloguePlan,catalogue,3*count));
 }
 if(!ok){discardDepot(itemPlan);discardDepot(cataloguePlan);for(auto*p:pending)if(p)VirtualFree(p,0,0x8000);log("ATLAS_BLOCKED: source validation or allocation failed; original resources unchanged.");return;}
 // Native initialization owns these lists synchronously at this hook.
 if(!publishDepot(itemPlan)|| (items!=catalogue&&!publishDepot(cataloguePlan))){
  log("ATLAS_BLOCKED: native resource lists changed during preparation; no ATLAS objects published.");return;
 }
 for(u32 k=0;k<count;++k){
  for(u64 p:pins[k])_InterlockedIncrement((long*)(p+8));
  auto*b=pending[k];atlas[k]=b;
  appendDepot(items,b->item);appendDepot(catalogue,b->list);appendDepot(catalogue,b->bag);appendDepot(catalogue,b->recipe);
 }
 if(atlas[2])log(configuredCombatBokkaRecipe()?"BOKKA_COMBAT_READY: recipe visible, item 105.":"BOKKA_COMBAT_READY: recipe hidden, item 105 retained.");
 log("ATLAS_RESOURCES_READY: independent item IDs 103/104; unique equipment, baggage, recipe and text resources; original items 11/21 unchanged.");
 log("ATLAS_VISUALS_READY: ATLAS boot base model");
 for(const auto& model:BootModels)if(static_cast<u8>(model.appearance)==bootVisualId){log(model.english);break;}
 log(skeletonVisualId==SkeletonGoldVisualId?"ATLAS_VISUALS_READY: gold Boost Skeleton Lv.3 (35).":"ATLAS_VISUALS_READY: normal Boost Skeleton Lv.3 (26).");
 log(recipesEnabled?"ATLAS_RECIPES_READY: available without progression gates; costs follow FreeCrafting.":"ATLAS_RECIPES_HIDDEN: Usage=None; resources retained for save compatibility.");
 log(germanText()?"ATLAS_LANGUAGE: German UI text.":"ATLAS_LANGUAGE: English UI text/fallback.");
}
void* catalogueConstructor(void* p){
 registerAtlas();return ((void*(*)(void*))(image+0xB6CB20))(p);
}
void definitions(){
 if(!atlas[0]||_InterlockedCompareExchange(&definitionBusy,1,0))return;
 const u64 now=GetTickCount64();
 if(now>=nextDefinitions){
  nextDefinitions=now+1000;bool german=germanText();for(auto*b:atlas)if(b)atlasLanguage(*b,german);
  u64 mgr=read<u64>(image+0x623E5B0);
  if(mgr){using Find=void*(*)(u64,u8);auto find=(Find)(image+0xBAD380);
   if(atlas[2] && find(mgr,CombatBokkaId)==atlas[2]->item)
    note(64,"BOKKA_COMBAT_LOOKUP_READY: native item manager resolves item 105.");
   if(find(mgr,BootId)==atlas[0]->item && find(mgr,SkeletonId)==atlas[1]->item)
    note(1,"ATLAS_LOOKUP_READY: both additional equipment IDs resolve through the native item manager.");
  }
 }
 _InterlockedExchange(&definitionBusy,0);
}

struct MeshBundle {u64 original;u32 offset;alignas(8)u8 resource[0x50];alignas(8)u8 rows[65][0x28];};
MeshBundle* meshes[32];u32 meshCount=0;
volatile long meshBusy=0;
void meshFor(void* component,bool hanging){
 if(!atlas[0]||_InterlockedCompareExchange(&meshBusy,1,0))return;
 u64 resource=read<u64>(u64(component)+0x30);u32 off=hanging?0x20:0x28;
 MeshBundle* b=nullptr;
 for(u32 i=0;i<meshCount;++i){auto*m=meshes[i];if(m->offset==off && (m->original==resource||u64(m->resource)==resource)){b=m;break;}}
 if(!b && meshCount<32){
  i32 n=read<i32>(resource+off);u64 rows=read<u64>(resource+off+8);
  u8 snapshot[64][0x28];
  const bool valid=n>0 && n<=64 && readmem(rows,snapshot,n*sizeof(snapshot[0]));
  const i32 source=valid?bootMeshRow(snapshot,n,bootVisualId):-1;
  if(source>=0){
   b=(MeshBundle*)VirtualAlloc(nullptr,sizeof(MeshBundle),0x3000,4);
   const u32 size=hanging?0x50:0x48;
   if(b && readmem(resource,b->resource,size)&&writable((void*)(resource+8),4)){
    previewCopy(b->rows,snapshot,n*sizeof(snapshot[0]));
    if(snapshot[source][0]!=bootVisualId)
     note(hanging?32:16,hanging?"ATLAS_BOOT_APPEARANCE: clip lacks selected mesh, using Default.":"ATLAS_BOOT_APPEARANCE: worn table lacks selected mesh, using Default.");
    b->original=resource;b->offset=off;resourceIdentity(b->resource,BootId,hanging?21:20);
    previewCopy(b->rows[n],b->rows[source],0x28);b->rows[n][0]=BootId;
    at<i32>(b->resource,off)=n+1;at<i32>(b->resource,off+4)=n+1;at<u64>(b->resource,off+8)=u64(b->rows);
    _InterlockedIncrement((long*)(resource+8));meshes[meshCount++]=b;
   }else{if(b)VirtualFree(b,0,0x8000);b=nullptr;}
  }
 }
 if(b && resource!=u64(b->resource) && writable((u8*)component+0x30,8)){
  _InterlockedIncrement((long*)(b->resource+8));at<u64>(component,0x30)=u64(b->resource);
  // Retain displaced Ref: cloned rows share native arrays and art-part refs.
  note(hanging?8:4,hanging?"ATLAS_HANGING_MESH_READY: boot clip mesh bound to ID 103.":"ATLAS_WORN_MESH_READY: worn boot mesh bound to ID 103.");
 }
 _InterlockedExchange(&meshBusy,0);
}
void* wornMeshOriginal;void* hangingMeshOriginal;
void wornMesh(void* p){meshFor(p,false);addon_host::worn(p,(void(*)(void*))wornMeshOriginal);}
void hangingMesh(void* p){meshFor(p,true);addon_host::hanging(p,(void(*)(void*))hangingMeshOriginal);}
