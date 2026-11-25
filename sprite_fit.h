#pragma once
#include <stdint.h>

// Keep the old 2x minimum for low-resolution art when it fits, while allowing
// native 1x high-resolution frames. An integer scale never crops or resamples
// the authored frame; cap both dimensions so wide attack poses remain visible.
inline uint8_t spriteFitScale(int idleH, int w, int h, int maxScale,
                              int maxW = 250, int maxH = 250) {
  if (w <= 0 || h <= 0 || maxScale < 1 || maxW < w || maxH < h) return 0;
  int s = idleH > 0 ? 170 / idleH : 5;
  if (s < 2) s = 2;
  if (s > maxScale) s = maxScale;
  if (s > maxW / w) s = maxW / w;
  if (s > maxH / h) s = maxH / h;
  return static_cast<uint8_t>(s);
}
