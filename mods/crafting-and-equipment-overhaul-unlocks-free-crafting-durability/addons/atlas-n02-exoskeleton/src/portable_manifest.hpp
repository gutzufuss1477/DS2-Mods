#pragma once
struct PayloadSegment {const wchar_t* name; unsigned long long offset,length; const char* hash;};
struct PortableFile {const wchar_t* name; const char* originalHash; const char* resultHash; unsigned long long resultSize; unsigned count; PayloadSegment segments[3];};
constexpr PortableFile PortableFiles[]={
{L"package.42.02.core.stream","ea8eda102f61012975bcfd2822c22680dd3b78f1517e501808a23083b69bb16f","14da2af2e3c86491b9a5909c8fa8740c6f93b46804eff52ac806d61668253753",1537599967ull,3,{{L"geometry-table.bin",0ull,390720ull,"c7bd89a30cff8ab312a90c562d6625f82221f4daa6aff4e942a4b16e044c8fa1"},{nullptr,389504ull,1531665903ull,nullptr},{L"geometry-append.bin",0ull,5543344ull,"3772953420a7e740f4b82657b760bd47b13a17b3499c03dfc86117b589e1b52c"}}},
{L"streaming_links.stream","6f4e41debb19b1bb702b2effc8932108dd77255ea6fec9dbc459a1d5a7bab28d","8c3d7bc70e9eda0288f79e12cfc2da9448063ae35d38732962154be0c5f58ac7",31725738ull,2,{{nullptr,0ull,30229145ull,nullptr},{L"links-delta.bin",0ull,1496593ull,"022090e36c25f7ca565246a938110942e0b392433601570ebb140cfe2270c7a6"}}},
};
constexpr char GraphPayloadSHA[]="769f7357c42e38f8fcb5a273c0c7a5ffbd939e2f6bf7fc72c256961d7afeefe2";
