#include "../../../training_scenes.h"
#include <cassert>
#include <set>
#include <iostream>
struct Canvas {
  uint64_t hash=1469598103934665603ULL;
  bool rows[466]={};
  template<class... A> void add(int tag,A... a){hash^=tag;hash*=1099511628211ULL;for(auto v:{int(a)...}){hash^=uint32_t(v);hash*=1099511628211ULL;}}
  void fillRect(int x,int y,int w,int h,uint16_t c){
    assert(w>0&&h>0);add(1,x,y,w,h,c);
    if(x<=0&&x+w>=466)for(int i=y;i<y+h;i++)if(i>=0&&i<466)rows[i]=true;
  }
  void covered(int h)const{for(int i=0;i<h;i++)assert(rows[i]);}
#define METHOD(n,t) template<class... A> void n(A... a){add(t,a...);}
  METHOD(drawRoundRect,2) METHOD(fillRoundRect,3) METHOD(fillCircle,4) METHOD(drawFastHLine,5) METHOD(drawLine,6) METHOD(fillTriangle,7)
#undef METHOD
};
int main(){
  std::set<uint64_t> gyms,training;
  std::set<int> themes;
  for(int r=0;r<7;r++)for(int t=0;t<13;t++){
    assert(scene_art::gymKey(r,t,7,13)==r*13+t);
    themes.insert(scene_art::locations[r][t]);
    for(bool n:{false,true}){Canvas c;scene_art::gym(&c,r,t,3,scene_art::rgb(100,140,180),n);c.covered(254);assert(gyms.insert(c.hash).second);}
  }
  for(auto mode:{scene_art::Strength,scene_art::Reaction,scene_art::Ball}){Canvas c;scene_art::training(&c,mode);c.covered(466);assert(training.insert(c.hash).second);}
  assert(themes.size()==26);
  assert(scene_art::gymKey(-1,0,7,13)==-1);assert(scene_art::gymKey(7,0,7,13)==-1);
  assert(scene_art::gymKey(0,-1,7,13)==-1);assert(scene_art::gymKey(0,13,7,13)==-1);
  std::cout<<gyms.size()<<" unique gym draw traces; "<<training.size()<<" training draw traces; "<<themes.size()<<" themes; full row coverage\n";
}
