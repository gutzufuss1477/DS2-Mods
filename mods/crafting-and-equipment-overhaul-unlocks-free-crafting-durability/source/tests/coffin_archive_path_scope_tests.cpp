#include "../src/coffin_archive_path_scope.hpp"
#include <cstdio>
using coffin_archive_path::matchesCanonical;
using coffin_archive_path::matchesCanonicalLinks;
int main(){
 unsigned checked=0,err=0;
 auto ck=[&](bool v){++checked;if(!v)++err;};
 ck(matchesCanonical(L"LocalCacheWinGame\\package\\package.42.02.core.stream"));
 ck(matchesCanonical(L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\DEATH STRANDING 2 - ON THE BEACH\\LocalCacheWinGame\\package\\package.42.02.core.stream"));
 ck(matchesCanonical(L".\\LocalCacheWinGame\\package\\package.42.02.core.stream"));
 ck(matchesCanonical(L"LOCALCACHEWINGAME\\PACKAGE\\PACKAGE.42.02.CORE.STREAM"));
 ck(matchesCanonical(L"C:/program files/Game/LocalCacheWinGame/package/package.42.02.core.stream"));
 ck(!matchesCanonical(L"LocalCacheWinGame\\package\\l200_aus\\package.42.02.core.stream"));
 ck(!matchesCanonical(L"C:\\Steam\\Game\\LocalCacheWinGame\\package\\l200_aus\\package.42.02.core.stream"));
 ck(!matchesCanonical(L"package.42.02.core.stream"));
 ck(!matchesCanonical(L"LocalCacheWinGame\\package\\package.42.00.core.stream"));
 ck(!matchesCanonical(L"LocalCacheWinGame\\package\\package.42.02.core.stream.bak"));
 ck(!matchesCanonical(L"AnotherLocalCacheWinGame\\package\\package.42.02.core.stream"));
 ck(!matchesCanonical(nullptr));
 ck(matchesCanonicalLinks(L"LocalCacheWinGame\\package\\streaming_links.stream"));
 ck(matchesCanonicalLinks(L"C:\\Steam\\Game\\LocalCacheWinGame\\package\\streaming_links.stream"));
 ck(matchesCanonicalLinks(L".\\LocalCacheWinGame\\package\\streaming_links.stream"));
 ck(matchesCanonicalLinks(L"LOCALCACHEWINGAME\\PACKAGE\\STREAMING_LINKS.STREAM"));
 ck(matchesCanonicalLinks(L"C:/Steam/Game/LocalCacheWinGame/package/streaming_links.stream"));
 ck(!matchesCanonicalLinks(L"LocalCacheWinGame\\package\\l200_aus\\streaming_links.stream"));
 ck(!matchesCanonicalLinks(L"C:\\Steam\\Game\\LocalCacheWinGame\\package\\l200_aus\\streaming_links.stream"));
 ck(!matchesCanonicalLinks(L"streaming_links.stream"));
 ck(!matchesCanonicalLinks(L"LocalCacheWinGame\\package\\streaming_graph.core"));
 ck(!matchesCanonicalLinks(L"LocalCacheWinGame\\package\\streaming_links.stream.bak"));
 ck(!matchesCanonicalLinks(L"AnotherLocalCacheWinGame\\package\\streaming_links.stream"));
 ck(!matchesCanonicalLinks(nullptr));
 ck(!matchesCanonical(L"LocalCacheWinGame\\package\\streaming_links.stream"));
 ck(!matchesCanonicalLinks(L"LocalCacheWinGame\\package\\package.42.02.core.stream"));
 ck(coffin_archive_path::matchesCanonicalLinksASCII("LocalCacheWinGame\\package\\streaming_links.stream"));
 ck(coffin_archive_path::matchesCanonicalLinksASCII("C:\\Steam\\Game\\LocalCacheWinGame\\package\\streaming_links.stream"));
 ck(coffin_archive_path::matchesCanonicalLinksASCII("C:/Steam/Game/LocalCacheWinGame/package/streaming_links.stream"));
 ck(!coffin_archive_path::matchesCanonicalLinksASCII("streaming_links.stream"));
 ck(!coffin_archive_path::matchesCanonicalLinksASCII("LocalCacheWinGame\\package\\l200_aus\\streaming_links.stream"));
 ck(!coffin_archive_path::matchesCanonicalLinksASCII("LocalCacheWinGame\\package\\streaming_links.stream.bak"));
 ck(!coffin_archive_path::matchesCanonicalLinksASCII(nullptr));
 std::printf("V1945_CANONICAL_GRAPH_AND_LINKS_PATH_%u_ASSERTIONS_ERRORS_%u\n",checked,err);
 return err?1:0;
}
