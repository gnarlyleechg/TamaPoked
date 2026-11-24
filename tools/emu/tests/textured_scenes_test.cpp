#include "../../../textured_scenes.h"
#include <cassert>
#include <algorithm>
#include <array>
#include <set>
#include <iostream>
#include <fstream>
#include <chrono>
struct Display {
 int calls=0; const uint16_t *expected=nullptr;
 void draw16bitRGBBitmap(int x,int y,const uint16_t*p,int w,int h){
  assert(x==0&&y==calls*2&&w==466&&h==2);
  for(int i=0;i<466;i++)assert(p[i]==p[466+i]&&p[i]==expected[calls*233+i/2]);
  calls++;
 }
};
uint64_t fingerprint(const uint16_t*p,int n){uint64_t v=1469598103934665603ULL;for(int i=0;i<n;i++){v^=p[i];v*=1099511628211ULL;}return v;}
int main(int argc,char**argv){
 using namespace scene_texture;
 static_assert(CacheBytes==108578&&BlitBytes==1864,"budget");
 std::array<uint16_t,W*H+2> storage;Cache c;assert(!c.training(0));assert(!c.gym(0,0,0,0,false));
 c.pixels=storage.data()+1;std::set<uint64_t> unique;std::ofstream out;if(argc>1)out.open(argv[1],std::ios::binary);
 auto check=[&](int rows){assert(storage.front()==0xf81f&&storage.back()==0xf81f);assert(std::find(c.pixels,c.pixels+W*rows,0xf81f)==c.pixels+W*rows);auto f=fingerprint(c.pixels,W*rows);assert(unique.insert(f).second);Display d{0,c.pixels};c.blit(&d,rows);assert(d.calls==rows);if(out)out.write(reinterpret_cast<const char*>(c.pixels),W*rows*2);return f;};
 auto reset=[&](){storage.fill(0xf81f);};auto start=std::chrono::steady_clock::now();
 for(int r=0;r<7;r++)for(int t=0;t<13;t++)for(bool n:{false,true}){reset();assert(c.gym(r,t,3,scene_art::rgb(100,140,180),n));auto f=check(127);assert(c.gym(r,t,3,scene_art::rgb(100,140,180),n));assert(fingerprint(c.pixels,W*127)==f);}
 for(int i=0;i<3;i++){reset();assert(c.training(i));check(H);}
 auto last=fingerprint(c.pixels,W*H);assert(!c.gym(-1,0,0,0,false));assert(!c.training(3));assert(fingerprint(c.pixels,W*H)==last);
 c.id=-1;assert(c.training(2));assert(fingerprint(c.pixels,W*H)==last);
 std::cout<<unique.size()<<" unique scenes; guards, cache replay, blit, coverage; "<<CacheBytes<<" PSRAM bytes; "<<BlitBytes<<" blit stack bytes; host total "<<std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-start).count()<<" ms\n";
}
