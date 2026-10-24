#pragma once
// Main-mod resources use the original game archives. The second profile only
// composes with the optional, unchanged ATLAS Chiral Boots addon.
namespace coffin_archive_profiles {
struct Profile {
 bool addon;
 unsigned oldChunks;
 unsigned long long oldPhysical, newPhysical, oldVirtual;
 unsigned long long linkSourceSize;
 const char *sourceSha,*targetSha,*sourceLinksSha,*targetLinksSha;
 const wchar_t *sourcePath,*linksPath,*cacheName,*linksName;
};
constexpr Profile Native={false,12171u,1532055407ull,1532057070ull,2103066486ull,30229145ull,
 "ea8eda102f61012975bcfd2822c22680dd3b78f1517e501808a23083b69bb16f",
 "4b53a6b218ff2e4c45cca48ef6d3ec5505c9464379c451bfc9acffe02907ae24",
 "6f4e41debb19b1bb702b2effc8932108dd77255ea6fec9dbc459a1d5a7bab28d",
 "6d9fb1262858d82c534d3d2920e1b6fe953e31a2dbcfbed18853c37aac7fa86f",
 L"LocalCacheWinGame\\package\\package.42.02.core.stream",
 L"LocalCacheWinGame\\package\\streaming_links.stream",
 L"story-native-v1.cache",L"links-native-v1.cache"};
constexpr Profile Chiral={true,12209u,1537599967ull,1537601630ull,2110830444ull,31725738ull,
 "14da2af2e3c86491b9a5909c8fa8740c6f93b46804eff52ac806d61668253753",
 "6c10c2d8c61ae1aae2f62f510692e66604b694ee21038190c0fecdb12482afd6",
 "8c3d7bc70e9eda0288f79e12cfc2da9448063ae35d38732962154be0c5f58ac7",
 "3d38aabec3120a6457834b46fe3933311f2fadb525ecc6e702e9f05a7e34f5b8",
 L"atlas_chiral\\geometry-v1.cache",L"atlas_chiral\\links-v1.cache",
 L"story-chiral-v1.cache",L"links-chiral-v1.cache"};

constexpr Profile Exoskeleton={true,12331u,1553423552ull,1553425215ull,2142335331ull,31769863ull,
 "32b5df483d39befa3f746262b830eb13c1c9fc44110e2646f9613a54f6a6dc80",
 "95bf8939bc4fe0f83356e5c4b1cdfccfe9708400106c67bf236ce57fb2b954d0",
 "3ced843f268376219b59fc32c0ebe0d13ce5669451cbecf9fc99779841ce4ab3",
 "0fabe2e54b3311ee2a3516f64511e5629d6ecbf4cc447d98226ffa5e17512837",
 L"atlas_exoskeleton\\geometry-v1.cache",L"atlas_exoskeleton\\links-v1.cache",
 L"story-exoskeleton-v1.cache",L"links-exoskeleton-v1.cache"};
}
