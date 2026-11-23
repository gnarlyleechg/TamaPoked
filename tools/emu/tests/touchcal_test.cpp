#include <stdio.h>
#include "touchcal.h"

static int failures = 0;
static void check(bool ok, const char *name) {
  if (!ok) { printf("FAIL: %s\n", name); failures++; }
}

int main() {
  TouchCalibration defaults;
  check(defaults.valid(), "factory calibration is valid");
  const CalTouchPoint center = defaults.correct({233, 233});
  check(center.x == 233 && center.y == 233, "factory calibration keeps center aligned");

  const CalTouchPoint samples[5] = {
    {50, 240}, {450, 240}, {250, 40}, {250, 440}, {250, 240}
  };
  TouchCalibration fitted;
  check(fitted.fit(samples), "five-point calibration accepts a consistent panel");
  const CalTouchPoint left = fitted.correct(samples[0]);
  const CalTouchPoint right = fitted.correct(samples[1]);
  check(left.x == TOUCH_TARGETS[0].x && left.y == TOUCH_TARGETS[0].y,
        "calibration maps left target");
  check(right.x == TOUCH_TARGETS[1].x && right.y == TOUCH_TARGETS[1].y,
        "calibration maps right target");

  const CalTouchPoint badSamples[5] = {
    {50, 240}, {450, 240}, {250, 40}, {250, 440}, {290, 280}
  };
  TouchCalibration unchanged;
  check(!unchanged.fit(badSamples), "calibration rejects an inconsistent center sample");
  check(unchanged.left == 70 && unchanged.right == 396,
        "rejected calibration leaves current mapping unchanged");

  TouchCalibration impossible{200, 300, 200, 300};
  check(!impossible.valid(), "calibration rejects an implausibly small touch area");
  if (!failures) puts("touch calibration checks passed");
  return failures ? 1 : 0;
}
