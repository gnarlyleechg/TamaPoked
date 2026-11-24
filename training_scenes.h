#pragma once
#include <stdint.h>
#include <initializer_list>

// Original lightweight scenic illustration; no allocation or external assets.
namespace scene_art {
constexpr uint16_t rgb(int r,int g,int b){return uint16_t(((r>>3)<<11)|((g>>2)<<5)|(b>>3));}
constexpr uint16_t mix(uint16_t a,uint16_t b,int n,int d){return uint16_t((((((a>>11)&31)*(d-n)+((b>>11)&31)*n)/d)<<11)|(((((a>>5)&63)*(d-n)+((b>>5)&63)*n)/d)<<5)|(((a&31)*(d-n)+(b&31)*n)/d));}
inline uint16_t shade(uint16_t c,int n){return mix(c,rgb(18,25,34),n,8);}
enum Training { Strength, Reaction, Ball };
enum Theme { Cave, Pool, Power, Garden, Dojo, Observatory, Volcano, Desert, Glacier, Temple, Arena, Aviary, Forest, Factory, Reef, Theater, Mine, Swamp, Harbor, Studio, Library, Castle, Summit, Beach, Circuit, Gallery };
// Explicit location order matches trainers.h, eight gyms then four elite rooms
// and champion. Original interpretations, not replicas of commercial game art.
constexpr Theme locations[7][13]={
 {Cave,Pool,Power,Garden,Dojo,Observatory,Volcano,Desert,Glacier,Dojo,Temple,Castle,Arena}, // Kanto
 {Aviary,Forest,Studio,Temple,Dojo,Factory,Glacier,Cave,Observatory,Garden,Dojo,Theater,Castle}, // Johto
 {Gallery,Beach,Power,Volcano,Arena,Aviary,Observatory,Reef,Theater,Temple,Glacier,Harbor,Pool}, // Hoenn
 {Mine,Garden,Theater,Dojo,Swamp,Factory,Glacier,Circuit,Forest,Desert,Volcano,Library,Castle}, // Sinnoh
 {Studio,Theater,Gallery,Circuit,Mine,Harbor,Castle,Reef,Library,Theater,Observatory,Dojo,Summit}, // Unova
 {Forest,Cave,Arena,Garden,Power,Gallery,Observatory,Glacier,Volcano,Pool,Castle,Temple,Theater}, // Kalos
 {Beach,Reef,Volcano,Forest,Observatory,Studio,Harbor,Desert,Dojo,Cave,Library,Aviary,Summit} // Alola
};
constexpr int gymKey(int r,int t,int nr,int nt){return r>=0&&r<nr&&t>=0&&t<nt?r*nt+t:-1;}
template<class G> void gradient(G*g,int y0,int y1,uint16_t a,uint16_t b){for(int y=y0;y<y1;y+=4)g->fillRect(0,y,466,y1-y<4?y1-y:4,mix(a,b,y-y0,y1-y0));}
template<class G> void rock(G*g,int x,int y,int w,int h,uint16_t c){
 g->fillTriangle(x-w,y,x-w/3,y-h,x+w,y,shade(c,4));
 g->fillTriangle(x-w/3,y-h,x+w/3,y-h*2/3,x+w,y,c);
 g->fillTriangle(x-w,y,x-w/3,y-h,x,y-h/3,shade(c,2));
}
template<class G> void tree(G*g,int x,int y,int h,uint16_t c){
 g->fillRect(x-3,y-h/2,7,h/2,shade(rgb(135,99,62),3));
 for(int i=0;i<3;i++){int yy=y-h+i*h/5, r=h/5+i*3;g->fillCircle(x-5,yy,r,shade(c,3));g->fillCircle(x+5,yy-3,r,shade(c,i));}
}
template<class G> void pillar(G*g,int x,int y,int h,int w,uint16_t c){
 g->fillRect(x,y,w,h,shade(c,3));g->fillRect(x+3,y,w-7,h,c);g->fillRect(x+5,y,3,h,mix(c,rgb(255,244,214),3,8));
 g->fillRect(x-4,y,w+8,6,shade(c,1));g->fillRect(x-5,y+h-6,w+10,6,shade(c,3));
}
template<class G> void floor(G*g,int top,int bottom,uint16_t c,int spacing){
 gradient(g,top,bottom,shade(c,2),c);
 for(int x=-120;x<=586;x+=spacing)g->drawLine(233+(x-233)/6,top,x,bottom-1,shade(c,2));
 for(int y=top+8;y<bottom;y+=12+(y-top)/4)g->drawFastHLine(0,y,466,shade(c,2));
}
template<class G> void window(G*g,int x,int y,int w,int h,uint16_t trim,uint16_t sky){
 g->fillRoundRect(x-4,y-4,w+8,h+8,6,shade(trim,3));g->fillRoundRect(x,y,w,h,4,sky);
 g->fillRect(x+w/2-2,y,4,h,trim);g->fillRect(x,y+h/2,w,3,trim);g->fillRect(x-7,y+h,w+14,6,trim);
}
template<class G> void training(G*g,Training mode){
 const auto cream=rgb(245,235,213), steel=rgb(100,121,139), wood=rgb(138,98,71);
 if(mode==Strength){
   gradient(g,0,260,rgb(178,149,124),cream);
   for(int y=20;y<244;y+=26){g->drawFastHLine(0,y,466,rgb(169,142,120));for(int x=(y/26%2)*30;x<466;x+=60)g->fillRect(x,y,2,26,rgb(169,142,120));}
   for(int x: {36,338}){window(g,x,52,92,98,wood,rgb(224,235,233));pillar(g,x-12,20,238,13,wood);}
   g->fillRect(74,43,318,13,shade(wood,3));g->fillRect(74,46,318,3,wood);
   floor(g,260,466,rgb(219,196,164),68);
   g->fillRoundRect(26,275,414,155,18,rgb(209,191,166));g->drawRoundRect(26,275,414,155,18,rgb(179,159,136));
   for(int x: {35,413}){pillar(g,x,178,106,15,steel);for(int y=188;y<260;y+=24)g->fillRect(x<100?0:x,y,x<100?88:53,5,rgb(158,81,68));}
 }else if(mode==Reaction){
   gradient(g,0,260,rgb(155,194,208),rgb(239,247,243));
   for(int x: {30,370}){pillar(g,x-12,20,235,12,steel);g->fillRoundRect(x,113,65,116,9,shade(steel,1));g->fillRoundRect(x+5,118,55,72,6,rgb(154,211,220));for(int y=128;y<182;y+=13)g->drawFastHLine(x+12,y,41,rgb(96,157,172));for(int i=0;i<3;i++)g->fillCircle(x+15+i*17,207,4,rgb(239,214,144));}
   floor(g,260,466,rgb(222,231,232),92);
   g->fillRoundRect(109,105,248,236,32,rgb(223,237,237));g->drawRoundRect(109,105,248,236,32,rgb(185,209,213));
 }else{
   gradient(g,0,280,rgb(128,190,223),rgb(240,245,221));
   for(int x: {54,355}){g->fillCircle(x,95,27,rgb(229,242,236));g->fillCircle(x+25,105,22,rgb(229,242,236));}
   for(int x=0;x<490;x+=37)tree(g,x,302,48+(x*7)%42,rgb(84,147,102));
   g->fillRect(0,302,466,8,rgb(143,167,129));
   gradient(g,310,466,rgb(128,173,98),rgb(193,216,145));
   for(int y=318;y<466;y+=32)g->fillRect(0,y,466,12,rgb(170,200,128));
   g->drawLine(112,310,26,465,cream);g->drawLine(354,310,440,465,cream);g->drawFastHLine(0,400,466,cream);
   g->drawRoundRect(166,271,134,43,3,cream);for(int x=180;x<300;x+=15)g->fillRect(x,277,1,33,rgb(197,218,186));
 }
 // Readable score and results without changing target positions.
 g->fillRoundRect(151,22,164,80,20,mode==Reaction?rgb(228,241,241):cream);
}

template<class G> void gym(G*g,int r,int t,int type,uint16_t accent,bool night){
 const int key=r*13+t;const Theme theme=locations[r][t];
 const auto stone=mix(accent,rgb(162,151,133),6,8), light=mix(stone,rgb(244,237,215),5,8);
 const auto dark=shade(stone,night?5:3), sky=night?rgb(42,61,94):rgb(145,199,222);
 const bool outdoor=theme==Aviary||theme==Forest||theme==Desert||theme==Summit||theme==Beach||theme==Harbor||theme==Swamp;
 gradient(g,0,184,outdoor?sky:dark,outdoor?rgb(196,211,204):light);
 if(theme==Cave||theme==Mine||theme==Volcano||theme==Glacier||theme==Desert||theme==Summit){
   auto mineral=theme==Glacier?rgb(161,213,231):theme==Volcano?rgb(131,76,58):theme==Desert?rgb(198,157,103):stone;
   for(int i=0;i<7;i++){int x=12+i*77,h=72+(key*11+i*31)%82;rock(g,x,184,62,h,shade(mineral,2));}
   for(int i=0;i<5;i++)rock(g,15+i*108,184,48,40+(key*7+i*29)%53,mineral);
   if(theme==Mine){for(int x: {55,370}){pillar(g,x,38,145,18,rgb(135,94,60));g->fillCircle(x+10,91,8,rgb(240,194,97));}g->fillRect(55,38,333,9,rgb(135,94,60));}
   if(theme==Volcano){for(int x=10;x<466;x+=58)g->fillTriangle(x,184,x+23,140+(x%37),x+37,184,rgb(228,116,53));}
 }else if(theme==Forest||theme==Garden||theme==Swamp||theme==Beach){
   const auto green=theme==Swamp?rgb(84,121,92):rgb(80,147,96);
   if(theme==Garden){for(int x=14;x<466;x+=110)window(g,x,25,94,122,stone,sky);}
   for(int i=0;i<9;i++)tree(g,8+i*58,184,52+(key*13+i*21)%67,shade(green,i%3));
   for(int i=0;i<8;i++){int x=20+i*63;g->fillCircle(x,180,14,green);if(theme==Garden)g->fillCircle(x,172,5,accent);}
 }else if(theme==Pool||theme==Reef||theme==Harbor){
   gradient(g,76,184,theme==Reef?rgb(73,150,174):rgb(124,189,207),rgb(177,214,213));
   for(int i=0;i<12;i++){int x=(key*17+i*47)%425;g->drawFastHLine(x,85+i*8,36,rgb(203,231,226));}
   for(int x: {35,380}){pillar(g,x,18,166,19,stone);if(theme==Reef)for(int i=0;i<3;i++)g->fillCircle(x+20+i*10,171-i*11,8,accent);}
   if(theme==Harbor){g->fillRect(48,135,370,8,dark);for(int x=65;x<420;x+=55)g->fillRect(x,135,5,49,dark);}
 }else if(theme==Power||theme==Factory||theme==Circuit){
   for(int i=0;i<4;i++){int x=18+i*116,y=45+(key+i*7)%24;g->fillRoundRect(x,y,82,114,7,dark);g->fillRoundRect(x+5,y+5,72,48,4,shade(accent,2));for(int j=0;j<4;j++)g->fillRect(x+13,y+62+j*9,56,3,light);g->fillCircle(x+19,y+24,6,rgb(246,204,95));}
   g->fillRect(0,20,466,10,stone);g->fillRect(0,28,466,3,light);
 }else if(theme==Observatory||theme==Aviary){
   g->fillRoundRect(83,16,300,151,45,dark);g->fillRoundRect(90,23,286,137,40,theme==Observatory?rgb(44,67,103):sky);
   if(theme==Observatory){for(int i=0;i<29;i++){int x=110+(key*13+i*37)%245,y=40+(i*29)%100;g->fillCircle(x,y,i%7==0?2:1,light);}g->fillCircle(294,61,19,rgb(228,223,183));}
   else for(int i=0;i<4;i++){int x=120+i*62;g->fillCircle(x,68+i%2*30,20,light);g->fillCircle(x+16,77+i%2*30,16,light);}
   for(int x: {44,402})pillar(g,x,18,166,18,stone);
 }else{
   // Architectural interiors: shrine, arena, theater, studio, library, castle and gallery.
   for(int i=0;i<4;i++){int x=13+i*116;window(g,x+17,30+(key+i*5)%21,72,104,stone,theme==Temple||theme==Castle?rgb(164,185,190):light);pillar(g,x,16,168,15,stone);
     if(theme==Library){for(int j=0;j<4;j++){g->fillRect(x+20,54+j*19,64,3,dark);for(int k=0;k<5;k++)g->fillRect(x+22+k*12,41+j*19,8,13,shade(accent,(k+j)%5));}}
     else if(theme==Theater){g->fillTriangle(x+14,25,x+91,25,x+14,144,shade(accent,2));g->fillTriangle(x+91,25,x+91,144,x+70,144,accent);}
     else if(theme==Dojo){g->fillRect(x+19,112,68,5,dark);g->fillRect(x+50,35,3,97,stone);}
     else if(theme==Gallery||theme==Studio){g->fillRoundRect(x+32,59,42,46,5,dark);g->fillTriangle(x+35,100,x+53,64,x+71,100,accent);}
   }
   if(theme==Arena)for(int y=136;y<184;y+=12)g->fillRect(0,y,466,6,shade(stone,(y/12)%3));
 }
 // Variation in platform material and perspective is keyed to the location.
 floor(g,184,254,mix(light,accent,1+(r%3),8),55+(key%7)*9);
 // Stable physical trim rather than symbols or text; distinguishes repeat themes.
 for(int i=0;i<3;i++)g->fillRect(0,182+i*3,466,i==1?2:1,shade(mix(stone,accent,(t+i)%7,8),i));
 (void)type;
}
}
