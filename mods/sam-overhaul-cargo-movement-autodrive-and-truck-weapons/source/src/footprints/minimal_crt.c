// Local byte primitives for the existing no-CRT Sam Overhaul build.
// Volatile accesses prevent the compiler from folding these loops into themselves.
#include <stddef.h>
void* __cdecl memcpy(void* destination,const void* source,size_t length) {
    volatile unsigned char* out=(volatile unsigned char*)destination;
    const volatile unsigned char* in=(const volatile unsigned char*)source;
    for(size_t i=0;i<length;++i) out[i]=in[i];
    return destination;
}
void* __cdecl memset(void* destination,int value,size_t length) {
    volatile unsigned char* out=(volatile unsigned char*)destination;
    for(size_t i=0;i<length;++i) out[i]=(unsigned char)value;
    return destination;
}
