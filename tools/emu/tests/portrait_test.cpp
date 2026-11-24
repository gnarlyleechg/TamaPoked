#include "portrait.h"
#include <cassert>
#include <cstdio>
#include <vector>
int main() {
  std::vector<uint8_t> b = {'T','P','A','1',2,1,3,0,2,0,1,0,
    0,248,255,0,0,0,31,0,255, 224,7,255,255,255,255,0,0,0};
  PortraitView v;
  assert(v.parse(b.data(),b.size(),258,true));
  int calls=0;
  uint16_t out[6]={1,1,1,1,1,1};
  v.draw([&](int x,int y,int len,const uint16_t *row){
    ++calls; for(int k=0;k<len;++k) out[y*3+x+k]=row[k];
  });
  assert(calls==3 && out[0]==0xf800 && out[1]==1 && out[2]==31 &&
         out[3]==0x07e0 && out[4]==0xffff && out[5]==1);
  for (size_t n=0;n<b.size();++n) assert(!v.parse(b.data(),n,258,true));
  assert(!v.parse(b.data(),b.size(),2,true));
  assert(!v.parse(b.data(),b.size(),258,false));
  for (int i : {0,4,6,8,10,11,14}) {
    auto bad=b; bad[i]=254;
    assert(!v.parse(bad.data(),bad.size(),258,true));
    assert(!v.pixels && !v.w && !v.h);
  }
  auto blank=b; for(size_t i=14;i<blank.size();i+=3) blank[i]=0;
  assert(!v.parse(blank.data(),blank.size(),258,true));
  b.push_back(0); assert(!v.parse(b.data(),b.size(),258,true));
  assert(!v.parse(nullptr,0,258,true));
  puts("PASS: RGB565 spans, transparency, high dex ID, shiny identity, truncation, corrupt headers and empty art");
}
