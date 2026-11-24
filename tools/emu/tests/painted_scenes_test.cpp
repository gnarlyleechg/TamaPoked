#include "../../../painted_scenes.h"
#include <array>
#include <cassert>
#include <set>
#include <iostream>
#include <vector>
#include <string>
static uint64_t fingerprint(const uint16_t*p,int n){uint64_t v=1469598103934665603ULL;for(int i=0;i<n;i++){v^=p[i];v*=1099511628211ULL;}return v;}
static bool damaged(std::vector<uint8_t> b,int id,uint16_t *p){FILE*f=tmpfile();assert(f);assert(fwrite(b.data(),1,b.size(),f)==b.size());rewind(f);bool ok=painted_scene::read(f,id,p);fclose(f);return ok;}
int main(int argc,char**argv){
 using namespace scene_texture;using namespace painted_scene;
 static_assert(CacheBytes==108578,"cache budget");std::array<uint16_t,W*H+2> buf;buf.fill(0xf81f);Cache c;c.pixels=buf.data()+1;Selection s;
 const char *dir=argc>1?argv[1]:nullptr;
 if(!dir){ // default host suite tests the parser with a synthetic valid plate
   std::vector<uint8_t> b(HeaderBytes+CacheBytes,0);memcpy(b.data(),"TPA1",4);b[6]=233;b[8]=233;
   uint32_t n=CacheBytes, crc=crc32(b.data()+HeaderBytes,n);
   for(int i=0;i<4;++i){b[12+i]=uint8_t(n>>(8*i));b[16+i]=uint8_t(crc>>(8*i));}
   assert(damaged(b,0,c.pixels));assert(!damaged(b,1,c.pixels));b.back()=1;assert(!damaged(b,0,c.pixels));b.pop_back();assert(!damaged(b,0,c.pixels));
   assert(!s.select(c,"/nonexistent-tamapoke-scenes",0,false));assert(c.id==-1);assert(c.training(0));
   std::cout<<"TPA1 parser and fallback passed (optional pack not supplied)\n";return 0;
 }
 std::set<uint64_t> unique;
 for(int id=0;id<Count;++id){int rows=rowsFor(id);assert(s.select(c,dir,id,false));auto h=fingerprint(c.pixels,W*rows);assert(unique.insert(h).second);assert(s.select(c,dir,id,false));assert(fingerprint(c.pixels,W*rows)==h);if(id>=3){assert(s.select(c,dir,id,true));assert(unique.insert(fingerprint(c.pixels,W*rows)).second);}assert(buf.front()==0xf81f&&buf.back()==0xf81f);}
 assert(!s.select(c,dir,-1,false));assert(!s.select(c,dir,94,false));
 assert(!s.select(c,"/nonexistent-tamapoke-scenes",0,false));assert(c.id==-1);assert(c.training(0));auto fallback=fingerprint(c.pixels,W*H);assert(!s.select(c,"/nonexistent-tamapoke-scenes",0,false));assert(fingerprint(c.pixels,W*H)==fallback);
 s.invalidate();assert(s.select(c,dir,0,false));assert(fingerprint(c.pixels,W*H)!=fallback);
 std::string path=std::string(dir)+"/p000.rgb";FILE*f=fopen(path.c_str(),"rb");assert(f);std::vector<uint8_t>b(HeaderBytes+CacheBytes);assert(fread(b.data(),1,b.size(),f)==b.size());fclose(f);
 assert(damaged(b,0,c.pixels));assert(!damaged(b,1,c.pixels));auto bad=b;bad[0]='X';assert(!damaged(bad,0,c.pixels));bad=b;bad[6]--;assert(!damaged(bad,0,c.pixels));bad=b;bad.back()^=1;assert(!damaged(bad,0,c.pixels));bad=b;bad.pop_back();assert(!damaged(bad,0,c.pixels));bad=b;bad.push_back(0);assert(!damaged(bad,0,c.pixels));assert(buf.front()==0xf81f&&buf.back()==0xf81f);
 std::cout<<unique.size()<<" distinct day/night painted plates; checksum, truncation, wrong ID, geometry, trailing bytes, cache and fallback checks passed\n";
}
