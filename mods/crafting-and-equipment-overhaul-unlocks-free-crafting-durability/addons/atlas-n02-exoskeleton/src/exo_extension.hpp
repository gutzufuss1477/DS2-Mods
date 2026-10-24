#pragma once
struct ExoExtensionV1 {unsigned size,version;const PortableFile*files;const char*graphHash;const EmbeddedPayload*(*find)(const wchar_t*);};
