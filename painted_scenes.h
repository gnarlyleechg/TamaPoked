#pragma once
#include "textured_scenes.h"
#include <stdio.h>
#include <string.h>

// Read-only optional TPA1 plate; no extra persistent framebuffer or SD writes.
namespace painted_scene {
constexpr int Count = 94;
constexpr int HeaderBytes = 20;
inline uint16_t le16(const uint8_t *b) { return uint16_t(b[0] | uint16_t(b[1]) << 8); }
inline uint32_t le32(const uint8_t *b) { return uint32_t(b[0]) | uint32_t(b[1]) << 8 | uint32_t(b[2]) << 16 | uint32_t(b[3]) << 24; }
inline uint32_t crc32(const uint8_t *data, size_t n) {
  uint32_t crc = 0xffffffffu;
  for (size_t i=0; i<n; ++i) {
    crc ^= data[i];
    for (int j=0;j<8;++j) crc = (crc>>1) ^ (0xedb88320u & (0u-(crc&1u)));
  }
  return ~crc;
}
constexpr int rowsFor(int id) { return id<0 || id>=Count ? 0 : id<3 ? 233 : 127; }
// A failed read may overwrite the scratch cache. The caller MUST invalidate
// its old key before loading and repaint from the procedural fallback on error.
inline bool read(FILE *f, int id, uint16_t *pixels) {
  const int rows=rowsFor(id); if(!f||!pixels||!rows)return false;
  uint8_t h[HeaderBytes];
  const size_t bytes=size_t(scene_texture::W)*rows*2;
  if(fread(h,1,sizeof(h),f)!=sizeof(h)||memcmp(h,"TPA1",4)||le16(h+4)!=id||le16(h+6)!=scene_texture::W||le16(h+8)!=rows||le16(h+10)!=0||le32(h+12)!=bytes)return false;
  auto *raw=reinterpret_cast<uint8_t*>(pixels);
  if(fread(raw,1,bytes,f)!=bytes||fgetc(f)!=EOF||ferror(f)||crc32(raw,bytes)!=le32(h+16))return false;
  // ESP32-S3 and the supported host platforms are little-endian; decode
  // explicitly anyway to keep the format portable.
  for(size_t i=0;i<bytes/2;++i)pixels[i]=le16(raw+i*2);
  return true;
}
inline bool load(const char *directory,int id,uint16_t *pixels) {
  if(!directory||!pixels||!rowsFor(id))return false;
  char path[512];
  int n=snprintf(path,sizeof(path),"%s/p%03d.rgb",directory,id);
  if(n<0||size_t(n)>=sizeof(path))return false;
  FILE *f=fopen(path,"rb"); if(!f)return false;
  bool ok=read(f,id,pixels);fclose(f);return ok;
}
inline void nightGrade(uint16_t *p,int rows) {
  for(int i=0;i<scene_texture::W*rows;++i){
    uint16_t c=p[i];int r=((c>>11)&31)*255/31,g=((c>>5)&63)*255/63,b=(c&31)*255/31;
    p[i]=scene_art::rgb(5+r*64/100,9+g*73/100,13+b*84/100);
  }
}
// Separate art key from the procedural cache's identity. Missing/corrupt art
// is attempted only once per selected scene, not each animation frame.
struct Selection {
  int id=-1;bool night=false;bool available=false;
  void invalidate(){id=-1;available=false;}
  bool select(scene_texture::Cache &cache,const char *directory,int next,bool n) {
    if(!cache.pixels||!rowsFor(next))return false;
    if(id==next&&night==n)return available;
    id=next;night=n;available=false;cache.id=-1;
    if(!load(directory,next,cache.pixels))return false;
    if(n)nightGrade(cache.pixels,rowsFor(next));
    available=true;return true;
  }
};
}
