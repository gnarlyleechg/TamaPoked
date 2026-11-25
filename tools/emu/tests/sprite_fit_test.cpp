#include "sprite_fit.h"
#include <cassert>
#include <initializer_list>
#include <cstdio>
int main() {
  assert(spriteFitScale(32,32,32,5)==5);
  assert(spriteFitScale(85,85,85,6)==2);
  assert(spriteFitScale(170,170,170,6)==1);
  assert(spriteFitScale(192,192,192,6)==1);
  assert(spriteFitScale(32,160,60,5)==1);
  assert(spriteFitScale(32,32,160,5)==1);
  assert(spriteFitScale(32,251,1,5)==0);
  assert(spriteFitScale(32,1,251,5)==0);
  assert(spriteFitScale(32,0,32,5)==0);
  assert(spriteFitScale(32,32,32,0)==0);
  int checked=0;
  for(int idle : {0, 16, 32, 85, 170, 192})
    for(int w=1;w<=255;++w) for(int h=1;h<=255;++h)
      for(int cap=1;cap<=6;++cap) {
        int s=spriteFitScale(idle,w,h,cap);
        if(w>250||h>250) assert(s==0);
        else assert(s>=1 && s<=cap && w*s<=250 && h*s<=250);
        ++checked;
      }
  printf("PASS: %d frame dimensions / scale combinations\n",checked);
}
