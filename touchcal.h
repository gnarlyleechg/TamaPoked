#pragma once
#include <stdint.h>

struct CalTouchPoint { int16_t x, y; };
constexpr CalTouchPoint TOUCH_TARGETS[] = {{70,233}, {396,233}, {233,70}, {233,396}, {233,233}};

struct TouchCalibration {
  int16_t left = 70, right = 396, top = 70, bottom = 396;

  bool valid() const {
    return left >= 0 && right <= 465 && top >= 0 && bottom <= 465 &&
           right - left >= 240 && right - left <= 410 &&
           bottom - top >= 240 && bottom - top <= 410;
  }

  static int16_t map(int16_t value, int16_t low, int16_t high) {
    int32_t pixel = 70 + (int32_t)(value - low) * 326 / (high - low);
    return pixel < 0 ? 0 : pixel > 465 ? 465 : pixel;
  }

  CalTouchPoint correct(CalTouchPoint p) const {
    return {map(p.x, left, right), map(p.y, top, bottom)};
  }

  bool fit(const CalTouchPoint *sample) {
    TouchCalibration candidate{sample[0].x, sample[1].x, sample[2].y, sample[3].y};
    if (!candidate.valid()) return false;
    for (int i = 0; i < 5; ++i) {
      const CalTouchPoint corrected = candidate.correct(sample[i]);
      int dx = corrected.x - TOUCH_TARGETS[i].x;
      int dy = corrected.y - TOUCH_TARGETS[i].y;
      if (dx * dx + dy * dy > 24 * 24) return false;
    }
    *this = candidate;
    return true;
  }
};
