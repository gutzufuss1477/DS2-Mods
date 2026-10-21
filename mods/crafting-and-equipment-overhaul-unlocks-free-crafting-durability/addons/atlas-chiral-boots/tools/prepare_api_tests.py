from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
s=(ROOT/'tests/runtime_tests.cpp').read_text()
a=s.index(' hostBase=');b=s.index(' alignas(8)u8 gameData',a)
s=s[:a]+''' const u8 prologueV[]={0x48,0x83,0xec,0x38,0x48,0x8b,0x41,0x60,0x48,0x83,0xc4,0x38,0xc3};
 copy((void*)(image+VisibilityRva),prologueV,sizeof(prologueV));
 writeAbs((u8*)(image+0x3478e0),u64(&recordController));
'''+s[b:]
a=s.index(' u8 entryBefore');b=s.index(' check(iconBoundList==u64(list)',a)
s=s[:a]+''' check(prepare());check(install()==1);
 fixture(false);hostCalls=0;observedResource=0;
 worn(component,[](void*p){++hostCalls;observedResource=at<u64>(p,0x30);});
 check(hostCalls==1&&observedResource==at<u64>(component,0x30)&&observedResource!=u64(hostResource));
 check(copyCount==1);
 fixture(true);observedResource=0;
 hung(component,[](void*p){++hostCalls;observedResource=at<u64>(p,0x30);});
 check(hostCalls==2&&observedResource==at<u64>(component,0x30)&&observedResource!=u64(hostResource));
 check(catalogueWithChiralIcon(component,fakeHostCatalogue)==component&&catalogueCalls==1);
'''+s[b:]
s=s.replace('auto lookup=(IconLookup)(image+IconRva);','auto lookup=[](u64 c,const char*const*n){return chiralCatalogueIcon(c,n,fakeHostIcon);};')
a=s.index(' check(verifiedHost(');b=s.index(' char selfDigest',a)
s=s[:a]+''' using namespace crafting_addon;
 HostV1 host={sizeof(HostV1),Version,NativeLayout,103,0xB7AE21D89446A581ull,0x4A634CBDCE790267ull};
 AddonV1 callbacks={};callbacks.size=sizeof(callbacks);callbacks.version=Version;
 check(DS2CraftingAddonQueryV1(&host,&callbacks)&&callbacks.worn&&callbacks.hanging&&callbacks.icon&&callbacks.mergeGraph);
 host.version=2;check(!DS2CraftingAddonQueryV1(&host,&callbacks));host.version=1;
 host.atlasBootId=104;check(!DS2CraftingAddonQueryV1(&host,&callbacks));host.atlasBootId=103;
 host.nativeLayout++;check(!DS2CraftingAddonQueryV1(&host,&callbacks));host.nativeLayout--;
 host.size=4;check(!DS2CraftingAddonQueryV1(&host,&callbacks));host.size=sizeof(host);
 callbacks.size=4;check(!DS2CraftingAddonQueryV1(&host,&callbacks));callbacks.size=sizeof(callbacks);
 check(!DS2CraftingAddonQueryV1(nullptr,&callbacks)&&!DS2CraftingAddonQueryV1(&host,nullptr));
'''+s[b:]
a=s.index(' check(overlayIndex(CacheFiles');b=s.index(' check(pathEqual(',a)
s=s[:a]+''' check(portablePaths());
 check(overlayIndex(CacheFiles[0].original)==0&&overlayIndex(CacheFiles[1].original)==1);
 check(overlayIndex(L"LocalCacheWinGame/package/streaming_graph.core")==-1);
 check(overlayIndex(L".\\\\LocalCacheWinGame\\\\package\\\\streaming_links.stream")==1);
 check(overlayIndex(L"C:\\\\other\\\\streaming_graph.core")==-1&&overlayIndex(L"")==-1&&overlayIndex(nullptr)==-1);
'''+s[b:]
a=s.index(' check(initializeHostCacheName()');b=s.index(' menuPolicyTests();',a)
s=s[:a]+''' overlayState=0;check(!verifyOverlay());commitGraph(1);check(verifyOverlay());commitGraph(0);check(!verifyOverlay());
 fixture(false);
'''+s[b:]
s=s.replace('CHIRAL_RUNTIME_TESTS','CHIRAL_API_RUNTIME_TESTS')
(ROOT/'tests/api_runtime_tests.cpp').write_text(s)
print('Runtime regression tests migrated to public callbacks; obsolete trampoline/host-SHA tests replaced by ABI checks.')
