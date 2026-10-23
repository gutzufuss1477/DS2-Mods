#pragma once
// Strict canonical DS2 archive/links file paths, never localized l200_aus.
namespace coffin_archive_path {
constexpr wchar_t canonical[]=L"LocalCacheWinGame\\package\\package.42.02.core.stream";
constexpr wchar_t canonicalLinks[]=L"LocalCacheWinGame\\package\\streaming_links.stream";
constexpr unsigned limit=2048u;
constexpr wchar_t norm(wchar_t c){
 if(c==L'/')return L'\\';
 return c>=L'A'&&c<=L'Z'?c+32:c;
}
// Explicit pattern lengths avoid CRT wcslen dependency in the lean ASI build.
// Supports wide & ANSI names without 4KiB scratch buffers or __chkstk.
template<class Char>
inline bool matchesPath(const Char*path,const wchar_t*pattern,unsigned length){
 if(!path||!pattern||!length)return false;
 unsigned count=0;
 while(count<limit&&path[count])++count;
 if(count>=limit||count<length)return false;
 if(count>length&&norm((wchar_t)path[count-length-1])!=L'\\')return false;
 for(unsigned i=0;i<length;++i){
  if(norm((wchar_t)path[count-length+i])!=norm(pattern[i]))return false;
 }
 return true;
}
inline bool matchesCanonical(const wchar_t*p){
 return matchesPath(p,canonical,unsigned(sizeof(canonical)/sizeof(wchar_t)-1u));
}
inline bool matchesCanonicalLinks(const wchar_t*p){
 return matchesPath(p,canonicalLinks,unsigned(sizeof(canonicalLinks)/sizeof(wchar_t)-1u));
}
inline bool matchesCanonicalLinksASCII(const char*p){
 return matchesPath(p,canonicalLinks,unsigned(sizeof(canonicalLinks)/sizeof(wchar_t)-1u));
}
}
