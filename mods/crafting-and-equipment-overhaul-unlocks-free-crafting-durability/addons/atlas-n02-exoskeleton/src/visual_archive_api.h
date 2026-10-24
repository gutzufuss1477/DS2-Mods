#pragma once
// Optional additive resource capability; frozen Crafting HostV1 is unchanged.
// Returned only after verified private archives and the native file shim exist.
struct DS2VisualArchiveV1 {
 unsigned size,version;
 void* nativeFileOpen;
 const char* archiveSHA256;
 const char* linksSHA256;
};
