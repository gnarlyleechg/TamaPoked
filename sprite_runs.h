#pragma once
#include <stdint.h>

// Emit contiguous visible runs of palette indices. Transparent index 255
// never touches the destination. In silhouette mode every visible pixel has
// the same color, so adjacent opaque indices can share one draw call.
template <typename Emit>
inline void forSpriteRuns(const uint8_t *pixels, int width, int height,
                          bool silhouette, Emit emit) {
  if (!pixels || width <= 0 || height <= 0) return;
  for (int y = 0; y < height; ++y) {
    const uint8_t *row = pixels + (uint32_t)y * width;
    int x = 0;
    while (x < width) {
      const uint8_t index = row[x];
      if (index == 255) { ++x; continue; }
      const int start = x++;
      while (x < width && row[x] != 255 &&
             (silhouette || row[x] == index)) ++x;
      emit(start, y, x - start, index);
    }
  }
}
