#pragma once
// Semantic append merge: retain current host arrays and preload lists verbatim.
// Only three declared native groups and their reservation records are replaced.
namespace chiral_graph {
using U=unsigned;using B=unsigned char;using Q=unsigned long long;
inline U get(const B*p){return U(p[0])|(U(p[1])<<8)|(U(p[2])<<16)|(U(p[3])<<24);}
inline void put(B*p,U v){for(U i=0;i<4;++i)p[i]=B(v>>(8*i));}
inline void cp(B*d,const B*s,U n){for(U i=0;i<n;++i)d[i]=s[i];}
inline bool eq(const B*a,const B*b,U n){for(U i=0;i<n;++i)if(a[i]!=b[i])return false;return true;}
inline bool digest(const B*p,U n,const B*expected){craft::Sha256 h;h.update(p,n);B out[32];h.finish(out);return eq(out,expected,32);}
struct Slice {U at=0,n=0;};
struct Cursor {
 const B*b;U n,p=0;bool ok=true;
 Slice take(Q count){if(!ok||count>n-p){ok=false;return {};};Slice s={p,U(count)};p+=U(count);return s;}
 U number(){auto s=take(4);return ok?get(b+s.at):0;}
 Slice array(U size){U start=p,count=number();take(Q(count)*size);return {start,p-start};}
};
enum Part {Uuid,Packed,Hashes,Types,LinkFile,LinkSize,Locators,Arrays,Spans,Groups,Subgroups,Roots,RootIndices,Files,PackOffsets,PackLengths,ObjectLocators,BlockSize,MaxBlock,UuidLinks,PartCount};
struct Parsed {Slice part[PartCount];U count=0;};
inline bool parse(const B*b,U n,Parsed&r){
 if(!b||n<16||n>128*1024*1024||get(b+8)!=n-16||get(b+n-4))return false;
 Cursor c{b,n-4,12};r.part[Uuid]=c.take(16);r.part[Packed]=c.take(1);
 r.part[Hashes]=c.array(8);r.part[Types]=c.array(1);r.part[LinkFile]=c.take(8);r.part[LinkSize]=c.take(4);
 r.part[Locators]=c.array(8);r.part[Arrays]=c.array(4);r.part[Spans]=c.array(12);
 U at=c.p;r.count=c.number();if(r.count>1000000)return false;
 for(U i=0;i<r.count&&c.ok;++i){c.take(64);c.array(16);}r.part[Groups]={at,c.p-at};
 r.part[Subgroups]=c.array(4);r.part[Roots]=c.array(16);r.part[RootIndices]=c.array(4);
 at=c.p;U count=c.number();if(count>1000000)return false;
 for(U i=0;i<count&&c.ok;++i){U size=c.number();if(size)c.take(Q(size)+4);}r.part[Files]={at,c.p-at};
 for(U k=PackOffsets;k<=PackLengths;++k){at=c.p;count=c.number();if(count>1000000)return false;
  for(U i=0;i<count&&c.ok;++i)c.array(4);r.part[k]={at,c.p-at};}
 r.part[ObjectLocators]=c.array(32);r.part[BlockSize]=c.take(4);r.part[MaxBlock]=c.take(4);r.part[UuidLinks]=c.array(16);
 return c.ok&&c.p==n-4;
}
struct Replacement {Slice oldRow,newRow,typeHash,locatorHash,oldArray,newArray;U rowAt=0,arrayAt=0;};
inline bool merge(const B*src,U size,const B*delta,U deltaSize,B*out,U capacity,U&written){
 written=0;Parsed g;if(!parse(src,size,g)||!delta||deltaSize<100||!out)return false;
 Cursor d{delta,deltaSize};if(d.number()!=0x31444743||d.number()!=1)return false;
 U base[3];for(auto&v:base)v=d.number();U oldLinks=d.number(),newLinks=d.number();
 if(get(src+g.part[LinkSize].at)!=oldLinks||newLinks<oldLinks)return false;
 const U identities[]={Hashes,Files};
 for(U part:identities){U n=d.number(),count=d.number();Slice hash=d.take(32);
  if(!d.ok||n<4||n>g.part[part].n||get(src+g.part[part].at)<count)return false;
  B header[4],value[32];put(header,count);craft::Sha256 h;h.update(header,4);
  h.update(src+g.part[part].at+4,n-4);h.finish(value);if(!eq(value,delta+hash.at,32))return false;}
 // Existing identities must retain their indices; future hosts may append.
 Slice tails[3];for(auto&t:tails){U n=d.number();t=d.take(n);}
 if(!d.ok||tails[0].n%2||tails[1].n%8||tails[2].n%12)return false;
 U replacementCount=d.number();if(replacementCount!=3&&replacementCount!=4)return false;
 if(g.part[Types].n<24)return false;
 U host[3]={get(src+g.part[Types].at+12),get(src+g.part[Locators].at),get(src+g.part[Spans].at)};
 if(g.part[Types].n!=24ull+2ull*host[0]||get(src+g.part[Types].at+16)!=host[0])return false;
 for(U k=0;k<3;++k)if(host[k]<base[k])return false;
 Replacement r[4];
 for(U ri=0;ri<replacementCount;++ri)if(auto&v=r[ri];true){v.oldRow=d.take(64);v.newRow=d.take(64);v.typeHash=d.take(32);v.locatorHash=d.take(32);
  U n=d.number();v.oldArray=d.take(n);v.newArray=d.take(n);if(!d.ok||n<4)return false;}
 if(!d.ok||d.p!=deltaSize)return false;
 Cursor rows{src,size,g.part[Groups].at+4},arrays{src,size,g.part[Arrays].at+4};U found=0;
 for(U i=0;i<g.count;++i){auto row=rows.take(64);rows.array(16);auto arr=arrays.array(12);
  if(!rows.ok||!arrays.ok)return false;
  for(U ri=0;ri<replacementCount;++ri)if(auto&v=r[ri];true)if(get(src+row.at)==get(delta+v.oldRow.at)){
   if(v.rowAt||!eq(src+row.at,delta+v.oldRow.at,64)||arr.n!=v.oldArray.n||!eq(src+arr.at,delta+v.oldArray.at,arr.n))return false;
   U ti=get(src+row.at+40),tn=get(src+row.at+44),li=get(src+row.at+56),ln=get(src+row.at+60);
   if(Q(ti)+tn>host[0]||Q(li)+ln>host[1]||!digest(src+g.part[Types].at+24+ti*2,tn*2,delta+v.typeHash.at)||
      !digest(src+g.part[Locators].at+4+li*8,ln*8,delta+v.locatorHash.at))return false;
   v.rowAt=row.at;v.arrayAt=arr.at;++found;
  }
 }
 if(found!=replacementCount||rows.p!=g.part[Groups].at+g.part[Groups].n||arrays.p!=g.part[Arrays].at+g.part[Arrays].n)return false;
 Q need=Q(size)+tails[0].n+tails[1].n+tails[2].n;if(need>capacity||need>128*1024*1024)return false;
 U target[PartCount]={};cp(out,src,12);U pos=12;
 for(U k=0;k<PartCount;++k){target[k]=pos;auto s=g.part[k];cp(out+pos,src+s.at,s.n);pos+=s.n;
  int t=k==Types?0:k==Locators?1:k==Spans?2:-1;
  if(t>=0){cp(out+pos,delta+tails[t].at,tails[t].n);pos+=tails[t].n;
   if(t==0){put(out+target[k],s.n-4+tails[t].n);put(out+target[k]+12,host[0]+tails[0].n/2);put(out+target[k]+16,host[0]+tails[0].n/2);}
   else put(out+target[k],host[t]+tails[t].n/(t==1?8:12));}
 }
 put(out+target[LinkSize],newLinks);
 for(U ri=0;ri<replacementCount;++ri)if(auto&v=r[ri];true){B*row=out+target[Groups]+v.rowAt-g.part[Groups].at;cp(row,delta+v.newRow.at,64);
  const U offsets[3]={40,56,32};for(U k=0;k<3;++k){U value=get(row+offsets[k]);if(value<base[k])return false;put(row+offsets[k],value+host[k]-base[k]);}
  cp(out+target[Arrays]+v.arrayAt-g.part[Arrays].at,delta+v.newArray.at,v.newArray.n);}
 put(out+pos,0);put(out+8,pos-12);written=pos+4;return true;
}
}
