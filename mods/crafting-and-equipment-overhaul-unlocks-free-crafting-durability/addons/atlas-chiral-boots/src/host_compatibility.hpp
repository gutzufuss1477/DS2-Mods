#pragma once
#include "core.hpp"
namespace chiral {
struct VerifiedHost {const char* sha256;const char* version;const char* baseGraphSha256;};
// Only source-compared builds with an exact installed/release binary match.
// Evidence: evidence/test01-diagnosis-test02-compatibility.json and
// evidence/test03-diagnosis-test04-host-compatibility.json.
constexpr VerifiedHost VerifiedHosts[]={
 {"e7084292f784477dd032f5eeca0b9ab50551ca101ad8bf9a508dc93e4b014839",
  "1.9.19-red-coffin-native-render-donor-private-test01",nullptr},
 {"091faec058829178ecae65fb142ce8b3bf78a01f6be49d51dfd8ea0391bf6c9d",
  "1.9.20-red-coffin-story-visual-selector-private-test01",nullptr},
 {"01e6b66c6d90a815a8f32ee39c18153b184a8d722ddce50931ccd142b95a526e",
  "1.9.21-red-coffin-original-story-streamed-private-test01",
  "2479bf350a16973a73f37d7da1be8409b6f9621baf852e454f215b978a564dd5"},
 {"217c74857e7ddd19072bc49728778c1b6ee6d13957bb3c31622cfb00736df4b3",
  "1.9.22-red-coffin-loaded-story-resource-private-test01",
  "2479bf350a16973a73f37d7da1be8409b6f9621baf852e454f215b978a564dd5"}
};
inline bool sameDigest(const char* actual,const char* expected){
 if(!actual||!expected)return false;
 for(craft::u32 i=0;i<64;++i)if(!actual[i]||actual[i]!=expected[i])return false;
 return actual[64]==0&&expected[64]==0;
}
inline const VerifiedHost* verifiedHost(const char* digest){
 for(const auto& host:VerifiedHosts)if(sameDigest(digest,host.sha256))return &host;
 return nullptr;
}
inline const VerifiedHost* verifiedCacheHost(const char* digest,const char* baseGraph){
 const auto*host=verifiedHost(digest);
 return host&&sameDigest(host->baseGraphSha256,baseGraph)?host:nullptr;
}
}
