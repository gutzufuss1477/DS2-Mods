extern "C" unsigned long long strlen(const char* s){unsigned long long n=0;if(s)while(s[n])++n;return n;}
extern "C" unsigned long long wcslen(const wchar_t* s){unsigned long long n=0;if(s)while(s[n])++n;return n;}
extern "C" void* memset(void* dst,int value,unsigned long long count){unsigned char*d=(unsigned char*)dst;for(unsigned long long i=0;i<count;++i)d[i]=(unsigned char)value;return dst;}
extern "C" void* memcpy(void* dst,const void* src,unsigned long long count){unsigned char*d=(unsigned char*)dst;const unsigned char*s=(const unsigned char*)src;for(unsigned long long i=0;i<count;++i)d[i]=s[i];return dst;}
