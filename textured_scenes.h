#pragma once
#include "training_scenes.h"
#include <stddef.h>

// A single half-resolution RGB565 surface shared by every trainer and training
// scene. The owner allocates it in PSRAM; this renderer never allocates.
namespace scene_texture {
constexpr int W=233,H=233;
constexpr size_t CacheBytes=size_t(W)*H*sizeof(uint16_t);
constexpr size_t BlitBytes=466*2*sizeof(uint16_t);
inline int clamp(int x,int lo,int hi){return x<lo?lo:x>hi?hi:x;}
inline int absval(int x){return x<0?-x:x;}
struct Raster {
 uint16_t *p;
 void pixel(int x,int y,uint16_t c){if(x>=0&&x<W&&y>=0&&y<H)p[y*W+x]=c;}
 void fillRect(int x,int y,int w,int h,uint16_t c){
  if(w<=0||h<=0)return;
  int l=clamp(x/2,0,W),r=clamp((x+w+1)/2,0,W),a=clamp(y/2,0,H),b=clamp((y+h+1)/2,0,H);
  if(x+w<=0||y+h<=0)return;
  for(int j=a;j<b;j++)for(int i=l;i<r;i++)p[j*W+i]=c;
 }
 void drawFastHLine(int x,int y,int w,uint16_t c){fillRect(x,y,w,1,c);}
 void drawLine(int x,int y,int X,int Y,uint16_t c){
  x/=2;y/=2;X/=2;Y/=2;int dx=absval(X-x),dy=-absval(Y-y),sx=x<X?1:-1,sy=y<Y?1:-1,e=dx+dy;
  for(;;){pixel(x,y,c);if(x==X&&y==Y)break;int q=2*e;if(q>=dy){e+=dy;x+=sx;}if(q<=dx){e+=dx;y+=sy;}}
 }
 void fillCircle(int x,int y,int r,uint16_t c){
  for(int j=clamp((y-r)/2,0,H);j<clamp((y+r+2)/2,0,H);j++)for(int i=clamp((x-r)/2,0,W);i<clamp((x+r+2)/2,0,W);i++){
   int dx=2*i-x,dy=2*j-y;if(dx*dx+dy*dy<=r*r)pixel(i,j,c);}
 }
 static bool inside(int x,int y,int l,int t,int w,int h,int r){
  if(x<l||x>=l+w||y<t||y>=t+h)return false;
  int dx=x<l+r?l+r-x:x>=l+w-r?x-(l+w-r-1):0;
  int dy=y<t+r?t+r-y:y>=t+h-r?y-(t+h-r-1):0;
  return dx*dx+dy*dy<=r*r||!dx||!dy;
 }
 void rounded(int x,int y,int w,int h,int r,uint16_t c,bool border){
  r=clamp(r,0,(w<h?w:h)/2);
  for(int j=clamp(y/2,0,H);j<clamp((y+h+1)/2,0,H);j++)for(int i=clamp(x/2,0,W);i<clamp((x+w+1)/2,0,W);i++)
   if(inside(2*i,2*j,x,y,w,h,r)&&(!border||!inside(2*i,2*j,x+2,y+2,w-4,h-4,r>2?r-2:0)))pixel(i,j,c);
 }
 void fillRoundRect(int x,int y,int w,int h,int r,uint16_t c){rounded(x,y,w,h,r,c,false);}
 void drawRoundRect(int x,int y,int w,int h,int r,uint16_t c){rounded(x,y,w,h,r,c,true);}
 static int edge(int x,int y,int a,int b,int A,int B){return (x-a)*(B-b)-(y-b)*(A-a);}
 void fillTriangle(int x,int y,int X,int Y,int u,int v,uint16_t c){
  int l=x<X?x:X;l=l<u?l:u;int r=x>X?x:X;r=r>u?r:u;
  int t=y<Y?y:Y;t=t<v?t:v;int b=y>Y?y:Y;b=b>v?b:v;
  for(int j=clamp(t/2,0,H);j<clamp((b+2)/2,0,H);j++)for(int i=clamp(l/2,0,W);i<clamp((r+2)/2,0,W);i++){
   int a=edge(2*i,2*j,x,y,X,Y),d=edge(2*i,2*j,X,Y,u,v),e=edge(2*i,2*j,u,v,x,y);
   if((a>=0&&d>=0&&e>=0)||(a<=0&&d<=0&&e<=0))pixel(i,j,c);}
 }
};
inline uint32_t hash(uint32_t x,uint32_t y,uint32_t seed){uint32_t a=x*0x9e3779b1u^y*0x85ebca77u^seed*0xc2b2ae3du;a^=a>>16;a*=0x7feb352du;a^=a>>15;return a;}
inline int noise(int x,int y,int scale,uint32_t seed){
 int u=x/scale,v=y/scale,fx=x%scale,fy=y%scale;
 int a=int(hash(u,v,seed)&255),b=int(hash(u+1,v,seed)&255),c=int(hash(u,v+1,seed)&255),d=int(hash(u+1,v+1,seed)&255);
 return ((a*(scale-fx)+b*fx)*(scale-fy)+(c*(scale-fx)+d*fx)*fy)/(scale*scale)-128;
}
// Material-specific multiscale texture, soft directional light and RGB565 dithering.
// Applied only to the background, before the UI and sprites are drawn.
inline void finish(uint16_t*p,int rows,int theme,uint32_t seed,bool night){
 for(int y=0;y<rows;y++)for(int x=0;x<W;x++){
  uint16_t c=p[y*W+x];int r=((c>>11)&31)*255/31,g=((c>>5)&63)*255/63,b=(c&31)*255/31;
  int coarse=noise(x,y,24,seed),mid=noise(x,y,7,seed+1),fine=int(hash(x,y,seed+2)&31)-16;
  bool green=g>r+12&&g>b+9, water=b>r+12&&g>r+9;
  int delta=coarse/9+mid/13+fine/3;
  if(green)delta+=mid/6+fine/2;
  else if(water)delta+=noise(x/2,y*3,13,seed+3)/9;
  else if(theme==scene_art::Cave||theme==scene_art::Mine||theme==scene_art::Volcano||theme==scene_art::Desert||theme==scene_art::Glacier)delta+=noise(x+y/2,y*2,11,seed+4)/7;
  else if(theme==26)delta+=noise(x/3,y*2,11,seed+4)/10;
  int dx=x-95,dy=y-55;delta-= (dx*dx+dy*dy)/2400;
  if(night){r=r*81/100;g=g*86/100;b=b*94/100;}
  p[y*W+x]=scene_art::rgb(clamp(r+delta,0,255),clamp(g+delta,0,255),clamp(b+delta,0,255));
 }
}
// IDs 0..2 are training; trainer scenes use 3+region*13+trainer.
struct Cache {
 uint16_t *pixels=nullptr; int id=-1; uint16_t accent=0; bool night=false;
 bool training(int mode){if(!pixels||mode<0||mode>2)return false;if(id!=mode){Raster r{pixels};scene_art::training(&r,scene_art::Training(mode));finish(pixels,H,26+mode,100+mode,false);id=mode;night=false;}return true;}
 bool gym(int region,int trainer,int type,uint16_t color,bool n){
  int key=scene_art::gymKey(region,trainer,7,13);if(!pixels||key<0)return false;
  if(id!=key+3||accent!=color||night!=n){Raster r{pixels};scene_art::gym(&r,region,trainer,type,color,n);finish(pixels,127,scene_art::locations[region][trainer],200+key,n);id=key+3;accent=color;night=n;}return true;
 }
 template<class G> void blit(G*g,int rows)const{
  uint16_t line[466*2];for(int y=0;y<rows;y++){for(int x=0;x<W;x++)line[x*2]=line[x*2+1]=pixels[y*W+x];for(int x=0;x<466;x++)line[466+x]=line[x];g->draw16bitRGBBitmap(0,y*2,line,466,2);}
 }
};
}
