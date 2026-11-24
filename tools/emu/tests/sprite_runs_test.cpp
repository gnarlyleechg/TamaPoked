#include "sprite_runs.h"
#include <cassert>
#include <cstdio>
#include <random>
#include <vector>

static void check(const std::vector<uint8_t>& src, int w, int h, bool sil, int scale) {
  const int pitch = w * scale + 6, rows = h * scale + 6;
  std::vector<uint16_t> expected(pitch * rows, 0x1234), actual = expected;
  const auto color = [sil](uint8_t i) -> uint16_t { return sil ? 0x0020 : uint16_t(i * 193u); };
  int visible = 0, calls = 0;
  for (int y = 0; y < h; ++y) for (int x = 0; x < w; ++x) {
    const uint8_t i = src[y*w+x];
    if (i == 255) continue;
    ++visible;
    for (int yy = 0; yy < scale; ++yy) for (int xx = 0; xx < scale; ++xx)
      expected[(3+y*scale+yy)*pitch+3+x*scale+xx] = color(i);
  }
  forSpriteRuns(src.data(), w, h, sil, [&](int x, int y, int len, uint8_t i) {
    assert(i != 255 && len > 0 && x >= 0 && x+len <= w && y >= 0 && y < h);
    ++calls;
    for (int yy = 0; yy < scale; ++yy) for (int xx = 0; xx < len*scale; ++xx)
      actual[(3+y*scale+yy)*pitch+3+x*scale+xx] = color(i);
  });
  assert(actual == expected && calls <= visible);
  if (visible == w*h && sil) assert(calls == h);
}

int main() {
  std::mt19937 rng(1729);
  int cases = 0;
  for (int h : {1, 2, 9, 37}) for (int w : {1, 2, 8, 31, 80}) {
    for (int variant = 0; variant < 5; ++variant) {
      std::vector<uint8_t> src(w*h);
      for (auto& i : src) i = variant == 0 ? 255 : variant == 1 ? 7 :
        variant == 2 ? uint8_t(rng()%255) : variant == 3 ? uint8_t(rng()%4) :
        uint8_t(rng()%3 == 0 ? 255 : rng()%6);
      for (bool sil : {false, true}) for (int scale : {1, 2, 4, 6}) {
        check(src, w, h, sil, scale); ++cases;
      }
    }
  }
  int invalidCalls = 0;
  forSpriteRuns(nullptr, 2, 2, false, [&](int,int,int,uint8_t){ ++invalidCalls; });
  assert(invalidCalls == 0);
  printf("PASS %d pixel-exact cases and null-input guard\n", cases);
}
