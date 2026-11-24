#include "../../../training_scenes.h"
#include <cassert>
#include <set>
#include <iostream>
struct Canvas { uint64_t hash=1469598103934665603ULL; template<class... A> void add(int tag,A... a){hash^=tag;hash*=1099511628211ULL;for(auto v:{int(a)...}){hash^=uint32_t(v);hash*=1099511628211ULL;}}
#define METHOD(n,t) template<class... A> void n(A... a){add(t,a...);}
METHOD(fillRect,1) METHOD(drawRoundRect,2) METHOD(fillRoundRect,3) METHOD(fillCircle,4) METHOD(drawFastHLine,5) METHOD(drawLine,6) METHOD(fillTriangle,7)
};
int main(){std::set<uint64_t> gyms,training;for(int r=0;r<7;r++)for(int t=0;t<13;t++){assert(scene_art::gymKey(r,t,7,13)==r*13+t);for(bool n:{false,true}){Canvas c;scene_art::gym(&c,r,t,3,scene_art::rgb(100,140,180),n);assert(gyms.insert(c.hash).second);}} for(auto mode:{scene_art::Strength,scene_art::Reaction,scene_art::Ball}){Canvas c;scene_art::training(&c,mode);assert(training.insert(c.hash).second);}assert(scene_art::gymKey(-1,0,7,13)==-1);assert(scene_art::gymKey(7,0,7,13)==-1);assert(scene_art::gymKey(0,13,7,13)==-1);std::cout<<gyms.size()<<" unique gym draw traces; "<<training.size()<<" training draw traces\n";}
