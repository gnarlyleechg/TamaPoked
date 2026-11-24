#pragma once
#include <stdint.h>

// Original procedural scenery. No framebuffer, dynamic allocation or SD asset.
namespace scene_art {
constexpr uint16_t rgb(int r, int g, int b) { return uint16_t(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)); }
constexpr uint16_t mix(uint16_t a, uint16_t b, int n, int d) {
  return uint16_t((((((a >> 11) & 31) * (d-n) + ((b >> 11) & 31)*n)/d) << 11) |
                  (((((a >> 5) & 63) * (d-n) + ((b >> 5) & 63)*n)/d) << 5) |
                  (((a & 31)*(d-n) + (b & 31)*n)/d));
}
enum Training { Strength, Reaction, Ball };
template<class G> void gradient(G *g, int y0, int y1, uint16_t a, uint16_t b) {
  for (int y=y0; y<y1; y+=8) g->fillRect(0,y,466,(y1-y<8?y1-y:8),mix(a,b,y-y0,y1-y0));
}
template<class G> void training(G *g, Training mode) {
  const uint16_t wall = mode==Strength ? rgb(183,153,127) : mode==Reaction ? rgb(155,188,204) : rgb(155,201,220);
  const uint16_t edge = mode==Strength ? rgb(112,80,62) : mode==Reaction ? rgb(76,118,142) : rgb(69,126,87);
  const uint16_t light=rgb(243,239,220);
  gradient(g,0,mode==Ball?310:258,wall,light);
  if (mode==Strength) {
    // Boxing gym: brick side walls, ceiling beam, padded floor and ring ropes.
    for(int y=36;y<246;y+=30) for(int x=0;x<466;x+=54) if(x<100||x>348)
      g->drawRoundRect(x+(y%60==36?0:8),y,48,24,3,mix(wall,edge,1,3));
    g->fillRect(72,44,322,12,edge); g->fillRect(76,56,314,4,mix(edge,light,1,3));
    gradient(g,258,466,rgb(172,169,159),rgb(232,222,200));
    for(int x=36;x<466;x+=394) {g->fillRoundRect(x,164,18,148,5,edge);g->fillRoundRect(x+3,168,5,132,2,wall);}
    for(int y=182;y<=230;y+=24) {g->fillRect(0,y,82,4,edge);g->fillRect(384,y,82,4,edge);}
  } else if(mode==Reaction) {
    // Reaction lab: quiet, light central field; side instrument panels.
    for(int x=20;x<=390;x+=370) {g->fillRoundRect(x,104,56,138,12,edge);g->fillRoundRect(x+5,109,46,128,8,wall);
      for(int y=124;y<225;y+=24) {g->fillRoundRect(x+12,y,32,7,3,light);g->fillCircle(x+17,y+3,2,edge);}}
    gradient(g,258,466,rgb(180,202,207),rgb(234,239,231));
    for(int y=288;y<466;y+=38) g->drawFastHLine(0,y,466,mix(wall,light,1,2));
  } else {
    // Outdoor ball court: distant tree line, striped turf and touchlines.
    for(int x=0;x<466;x+=34) {g->fillCircle(x,286+(x%3)*5,29,mix(edge,wall,1,3));g->fillCircle(x+9,295,21,edge);}
    gradient(g,310,466,rgb(112,166,105),rgb(181,214,147));
    for(int y=320;y<466;y+=32) g->fillRect(0,y,466,12,mix(edge,light,2,3));
    g->drawLine(108,310,30,465,light);g->drawLine(358,310,436,465,light);
    g->drawFastHLine(0,400,466,light);
  }
  // Neutral score field, shared only as a legibility convention.
  g->fillRoundRect(152,22,162,80,20,mix(wall,light,3,4));
}
constexpr int gymKey(int region,int trainer,int regions,int trainers) {
  return region>=0&&region<regions&&trainer>=0&&trainer<trainers ? region*trainers+trainer : -1;
}
// Stable location identity; type controls the palette, location controls the
// architectural motif. No dependence on opponent species or difficulty.
template<class G> void gym(G *g,int region,int trainer,int type,uint16_t accent,bool night) {
  const int key=region*13+trainer;
  const uint16_t base=mix(accent,rgb(218,215,199),night?2:3,4);
  const uint16_t shadow=mix(accent,rgb(36,43,54),night?3:2,4);
  const uint16_t pale=mix(base,rgb(250,244,225),2,3);
  gradient(g,0,182,shadow,base);
  // Distinct room geometry per city: wall bays, windows, columns and trim.
  const int bays=3+region%4, width=466/bays;
  for(int i=0;i<bays;i++) {
    int x=i*width+8, w=width-16, h=65+(key*19+i*23)%61;
    g->fillRoundRect(x,20,w,h+8,8,shadow);g->fillRoundRect(x+4,24,w-8,h,7,base);
    g->fillRoundRect(x+10,30,w-20,h-12,6,pale);
    const int cy=30+(h-12)/2, cx=x+w/2;
    // Different motifs for types and room identities, drawn in restrained tones.
    switch((type+region)%6) {
      case 0: g->fillTriangle(cx,cy-19,cx-24,cy+18,cx+24,cy+18,accent);break;
      case 1: for(int j=-1;j<=1;j++) g->fillCircle(cx+j*13,cy+(j==0?-8:4),12,accent);break;
      case 2: for(int j=0;j<3;j++) g->fillRoundRect(cx-23,cy-18+j*13,46,5,2,accent);break;
      case 3: g->fillTriangle(cx-6,cy-20,cx+17,cy-20,cx-4,cy+8,accent);g->fillTriangle(cx+3,cy-5,cx-17,cy-5,cx-3,cy+20,accent);break;
      case 4: g->fillCircle(cx,cy,20,accent);g->fillCircle(cx+8,cy-5,15,pale);break;
      default: g->drawRoundRect(cx-20,cy-20,40,40,8,accent);g->fillCircle(cx,cy,9,accent);break;
    }
    const int stripe=5+(trainer%5)*2;
    g->fillRect(x,140,w,stripe,accent);
  }
  gradient(g,182,254,base,pale);
  for(int x=-140;x<=606;x+=74) g->drawLine(233+(x-233)/4,182,x,253,mix(base,shadow,1,3));
  g->drawFastHLine(0,182,466,shadow);
  g->drawFastHLine(0,225,466,mix(base,shadow,1,4));
  // One of 13 distinct floor inlays, aligned to the stable trainer slot.
  g->fillRoundRect(168,205,130,22,10,shadow);
  for(int i=0;i<=trainer;i++) g->fillRoundRect(173+i*9,210,5,12,2,pale);
}
}
