#pragma once
#include <stdint.h>

// Step counting from the board's QMI8658 accelerometer.
// imuStepsBegin() after Wire.begin(); imuStepsPump(now) every loop pass (it
// rate-limits itself); imuStepsTake() returns the steps counted since last call.
void imuStepsBegin();
void imuStepsPump(uint32_t now);
uint16_t imuStepsTake();
bool imuStepsOk();
