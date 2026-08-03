extern "C" unsigned long long strlen(const char* s){unsigned long long n=0;if(s)while(s[n])++n;return n;}
extern "C" unsigned long long wcslen(const wchar_t* s){unsigned long long n=0;if(s)while(s[n])++n;return n;}
