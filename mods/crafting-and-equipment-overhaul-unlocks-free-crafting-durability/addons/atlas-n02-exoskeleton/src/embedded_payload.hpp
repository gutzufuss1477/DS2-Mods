#pragma once
// Built from the verified Test15 payloads; no external asset files at runtime.
struct EmbeddedPayload {const unsigned char* data; unsigned size; const char* sha256;};
const EmbeddedPayload* findEmbeddedPayload(const wchar_t* name);
