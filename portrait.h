#pragma once
#include <stdint.h>
#include <stddef.h>

// TPA1: magic, little-endian dex, width, height, shiny flag, reserved byte;
// then width*height triples: RGB565 little-endian, alpha (0 or 255).
struct PortraitView {
  static constexpr int MaxSide = 192;
  static constexpr size_t Header = 12;
  static constexpr size_t MaxBytes = Header + 3u * MaxSide * MaxSide;
  const uint8_t *pixels = nullptr;
  uint16_t w = 0, h = 0;
  bool parse(const uint8_t *p, size_t n, int dex, bool shiny) {
    pixels = nullptr; w = h = 0;
    if (!p || n < Header || n > MaxBytes || dex < 1 || dex > 999 ||
        p[0]!='T' || p[1]!='P' || p[2]!='A' || p[3]!='1') return false;
    const unsigned id = p[4] | (unsigned(p[5]) << 8);
    const unsigned x = p[6] | (unsigned(p[7]) << 8);
    const unsigned y = p[8] | (unsigned(p[9]) << 8);
    if (id != unsigned(dex) || p[10] != unsigned(shiny) || p[11] != 0 ||
        !x || !y || x > MaxSide || y > MaxSide || n != Header + 3u*x*y) return false;
    bool visible = false;
    for (size_t i = Header + 2; i < n; i += 3) {
      if (p[i] == 255) visible = true;
      else if (p[i] != 0) return false;
    }
    if (!visible) return false;
    pixels = p + Header; w = x; h = y; return true;
  }
  // Decode only one row at a time, emitting contiguous opaque spans.
  template<typename Emit> void draw(Emit emit) const {
    if (!pixels) return;
    uint16_t row[MaxSide];
    for (int y = 0; y < h; ++y) {
      const uint8_t *p = pixels + size_t(y)*w*3;
      int x = 0;
      while (x < w) {
        if (!p[x*3+2]) { ++x; continue; }
        const int start = x;
        do { row[x-start] = p[x*3] | (uint16_t(p[x*3+1]) << 8); ++x; }
        while (x < w && p[x*3+2]);
        emit(start, y, x-start, row);
      }
    }
  }
};
