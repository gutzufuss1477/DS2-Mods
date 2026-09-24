#pragma once
// No C++ runtime dependency: the same policy is compiled into the DLL and host tests.
namespace craft {
using u8=unsigned char; using u16=unsigned short; using u32=unsigned int;
using i32=int; using u64=unsigned long long;
static_assert(sizeof(u32)==4 && sizeof(u64)==8,"unsupported integer ABI");
constexpr u32 MaxRecipes=4096, MaxMenu=512, MaxRules=4096;
struct Instance { u64 vtable; u32 flags; u32 padding; };
struct Vector { i32 count; i32 capacity; Instance* data; };
static_assert(sizeof(Instance)==16 && sizeof(Vector)==16,"native layout");
struct Rule {u32 key; i32 value;}; // -1=inherit, 0=vanilla, 1=early
struct Settings { bool enabled; bool all; bool dump; u32 count; Rule rules[MaxRules]; };
struct Error {u32 line; const char* message;};
inline bool ws(char c){return c==' ' || c=='\t' || c=='\r';}
inline char lower(char c){return c>='A'&&c<='Z'?char(c+32):c;}
inline bool eq(const char* p,u32 n,const char* q){u32 i=0;for(;i<n&&q[i];++i)if(lower(p[i])!=lower(q[i]))return false;return i==n&&!q[i];}
inline void trim(const char*&p,u32&n){while(n&&ws(*p)){++p;--n;}while(n&&ws(p[n-1]))--n;}
inline bool key32(const char*p,u32 n,u32& v){if(n!=10||p[0]!='0'||lower(p[1])!='x')return false;v=0;for(u32 i=2;i<n;++i){char c=lower(p[i]);u32 d=c>='0'&&c<='9'?u32(c-'0'):c>='a'&&c<='f'?u32(c-'a'+10):99;if(d>15)return false;v=(v<<4)|d;}return true;}
inline bool parse(const char* p,u32 n,Settings&s,Error&e){
 s.enabled=false;s.all=false;s.dump=true;s.count=0;e={0,nullptr};
 u32 pos=0,line=0,section=0,seen=0;if(n>=3&&(u8)p[0]==239&&(u8)p[1]==187&&(u8)p[2]==191)pos=3;
 while(pos<n){++line;u32 begin=pos;while(pos<n&&p[pos]!='\n'){if(p[pos]==0){e={line,"NUL/UTF-16 is not supported; use UTF-8"};return false;}++pos;}u32 len=pos-begin;if(pos<n)++pos;const char*t=p+begin;
  for(u32 k=0;k<len;++k){if(t[k]==';'||t[k]=='#'){len=k;break;}}
  trim(t,len);if(!len)continue;
  if(t[0]=='['){if(len<3||t[len-1]!=']'){e={line,"invalid section"};return false;}const char*a=t+1;u32 l=len-2;trim(a,l);if(eq(a,l,"CraftingUnlocks"))section=1;else if(eq(a,l,"Items"))section=2;else{e={line,"unknown section"};return false;}continue;}
  u32 mid=0;while(mid<len&&t[mid]!='=')++mid;if(mid==len||!section){e={line,"expected setting in a section"};return false;}
  const char*k=t;u32 kl=mid;trim(k,kl);const char*v=t+mid+1;u32 vl=len-mid-1;trim(v,vl);i32 value=eq(v,vl,"inherit")?-1:(vl==1&&v[0]=='0')?0:(vl==1&&v[0]=='1')?1:-2;
  if(section==1){if(value<0){e={line,"global values must be 0 or 1"};return false;}u32 bit=eq(k,kl,"Enabled")?1:eq(k,kl,"DefaultUnlock")?2:eq(k,kl,"ExportCatalogue")?4:0;if(!bit||(seen&bit)){e={line,"unknown or duplicate global setting"};return false;}seen|=bit;if(bit==1)s.enabled=value!=0;else if(bit==2)s.all=value!=0;else s.dump=value!=0;}
  else {u32 id;if(!key32(k,kl,id)||value==-2){e={line,"item requires 0xXXXXXXXX = inherit, 0 or 1"};return false;}for(u32 i=0;i<s.count;++i)if(s.rules[i].key==id){e={line,"duplicate recipe key"};return false;}if(s.count==MaxRules){e={line,"too many item overrides"};return false;}s.rules[s.count++]={id,value};}
 }
 if((seen&3)!=3){e={line,"Enabled and DefaultUnlock are required"};s.enabled=false;return false;}return true;
}
inline bool selected(const Settings&s,u32 key){if(!s.enabled)return false;for(u32 i=0;i<s.count;++i)if(s.rules[i].key==key)return s.rules[i].value<0?s.all:s.rules[i].value!=0;return s.all;}
inline u8 facility_mask(u32 type,bool isNull=false){if(isNull)return 15;switch(type){case 0x100:case 0x118:return 2;case 0x101:return 1;case 0x102:case 0x202:case 0x402:return 4;case 0x103:case 0x108:case 0x203:case 0x403:return 8;default:return 0;}}
inline bool supported_usage(u8 u){return u==1||u==3||u==4||u==9;}
inline bool facility_allows(u8 usage,u8 mask){constexpr u8 masks[10]={0,3,6,11,7,0,0,0,0,15};return usage<10 && (masks[usage]&mask)!=0;}
struct Recipe {Instance instance;u64 resource;u64 baggage;u32 key;u32 baggageKey;u8 usage;u8 dlc;u8 caseType;u8 valid;bool mission;bool duplicate;u16 reserved;};
enum class Skip:u8 {None,NotSelected,UnsupportedPath,MissingResource,DlcSpecial,Quest,Duplicate,Facility,AlreadyNative};
inline Skip eligibility(const Settings&s,const Recipe&r,u8 mask){
 if(!selected(s,r.key))return Skip::NotSelected;
 if(!supported_usage(r.usage))return Skip::UnsupportedPath;
 if(!r.valid||!r.resource||!r.baggage||r.caseType==11)return Skip::MissingResource;
 // No DLC/entitlement predicate is replaced. Special DLC resources stay entirely native.
 if(r.dlc&1)return Skip::DlcSpecial;
 if(r.mission)return Skip::Quest;
 if(r.duplicate)return Skip::Duplicate;
 if(!facility_allows(r.usage,mask))return Skip::Facility;
 return Skip::None;
}
inline void mark_duplicates(Recipe*r,u32 n){for(u32 i=0;i<n;++i)r[i].duplicate=false;for(u32 i=0;i<n;++i)for(u32 j=i+1;j<n;++j)if(r[i].key==r[j].key){r[i].duplicate=true;r[j].duplicate=true;}}
struct Build {bool ok;u32 count;u32 added;u32 eligible;u32 overLimit;};
inline Build build(const Settings&s,const Recipe*r,u32 rn,const Instance* native,u32 nn,u8 mask,Instance*out){
 Build b={false,0,0,0,0};if(rn>MaxRecipes||nn>MaxMenu||(!native&&nn)||(!r&&rn)||!out)return b;
 for(u32 i=0;i<nn;++i)out[b.count++]=native[i];
 for(u32 i=0;i<rn;++i){if(eligibility(s,r[i],mask)!=Skip::None)continue;bool present=false;for(u32 j=0;j<b.count;++j)if((out[j].flags&0xFFF)==(r[i].instance.flags&0xFFF)){present=true;break;}if(present)continue;++b.eligible;
  if(b.count==MaxMenu){++b.overLimit;continue;}out[b.count]=r[i].instance;out[b.count].flags|=0x8000u;++b.count;++b.added;
 }
 // Do not silently truncate candidates: preserve the complete vanilla menu instead.
 if(b.overLimit){b.count=nn;b.added=0;return b;}b.ok=true;return b;
}
}
