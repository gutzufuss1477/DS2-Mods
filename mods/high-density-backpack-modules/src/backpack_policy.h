#pragma once
// Pure layout rules shared by the ASI and native tests. No game pointers here.
namespace backpack {
struct Module { unsigned id; unsigned char type; int x,y,a,b; unsigned short orientation; bool active; };
inline bool cell(int x,int y) { return x>=0 && x<5 && y>=0 && y<6; }
inline unsigned footprint(const Module& m) {
 if (!cell(m.x,m.y)||m.a<1||m.b<1||m.a>16||m.b>16||m.orientation>3) return 0;
 int w=(m.orientation&1)?m.b:m.a,h=(m.orientation&1)?m.a:m.b;
 int dx=(m.orientation==2||m.orientation==3)?-1:1;
 int dy=(m.orientation==0||m.orientation==3)?-1:1;
 unsigned mask=0;
 for(int j=0;j<h;++j)for(int i=0;i<w;++i){int x=m.x+i*dx,y=m.y+j*dy;if(!cell(x,y))return 0;mask|=1u<<(y*5+x);}
 return mask;
}
inline int anchor(const unsigned ids[30], unsigned id, int x,int y) {
 if(!id||id==0xffffffffu)return -1;
 if(cell(x,y)&&(!ids[y*5+x]||ids[y*5+x]==id))return y*5+x;
 // Search every cell, including when the cursor lies far outside the grid.
 int best=-1; long long distance=0x7fffffffffffffffLL;
 for(int i=0;i<30;++i){if(ids[i]&&ids[i]!=id)continue;
  long long dx=(long long)(i%5)-x,dy=(long long)(i/5)-y;
  long long d=(dx<0?-dx:dx)+(dy<0?-dy:dy);
  if(d<distance){distance=d;best=i;}}
 return best;
}
inline unsigned visible(const Module modules[30]) {
 unsigned occupied=0,shown=0;bool types[256]={};
 // Prefer variety, then fill remaining space with additional representatives.
 for(int pass=0;pass<2;++pass)for(int i=0;i<30;++i){
  const Module&m=modules[i];if(!m.active||!m.id||(shown&(1u<<i)))continue;
  if(pass==0&&types[m.type])continue;
  unsigned shape=footprint(m);if(!shape||(shape&occupied))continue;
  occupied|=shape;shown|=1u<<i;types[m.type]=true;
 }
 return shown;
}
struct Layout { unsigned shown; int x[30],y[30]; };
inline void pack(const Module modules[30],Layout& result) {
 result.shown=0;for(int i=0;i<30;++i){result.x[i]=0;result.y[i]=0;}
 unsigned occupied=0;bool types[256]={};
 // Smaller representatives first make room for several types. Logical anchor
 // positions remain unchanged; these coordinates are for model placement only.
 for(int pass=0;pass<2;++pass) {
  unsigned visited=0;
  for(int step=0;step<30;++step) {
   int best=-1,area=257;
   for(int i=0;i<30;++i) {
    const Module&m=modules[i];
    if(!m.active||!m.id||m.id==0xffffffffu||m.a<1||m.b<1||m.a>16||m.b>16||m.orientation>3||
       ((visited|result.shown)&(1u<<i))||(pass==0&&types[m.type]))continue;
    if(m.a*m.b<area){best=i;area=m.a*m.b;}
   }
   if(best<0)break;
   visited|=1u<<best;Module m=modules[best];
   for(int c=0;c<30;++c){m.x=c%5;m.y=c/5;unsigned f=footprint(m);
    if(f && !(f&occupied)){result.shown|=1u<<best;occupied|=f;types[m.type]=true;
     result.x[best]=m.x;result.y[best]=m.y;break;}}
  }
 }
}
}
