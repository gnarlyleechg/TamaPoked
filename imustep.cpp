// Step counting from the QMI8658's FIFO. The register-level FIFO reading and the
// step detector (steps.h) are ported from the Life sim fork of TamaPoke
// (MIT, (c) 2026 Quique Tortosa and contributors); see CREDITS.md.
#include "imustep.h"
#include "pin_config.h"
#include "steps.h"
#include <Arduino.h>
#include <Wire.h>
#include <ImuDrv.hpp>

#define IMU_SAMPLE_MS 16        // 62.5 Hz
#define IMU_LSB_PER_G 4096.0f   // +-8 g

static SensorQMI8658 imu;
static bool imuOk = false, imuFifoOk = false;
static uint8_t imuAddr = 0, imuFifoCtrl = 0;
static StepDetector stepper;

static bool imuReg(uint8_t reg, uint8_t *buf, uint8_t n) {
  Wire.beginTransmission(imuAddr);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom((int)imuAddr, (int)n) != n) return false;
  for (uint8_t k = 0; k < n; k++) buf[k] = Wire.read();
  return true;
}
static bool imuPut(uint8_t reg, uint8_t v) {
  Wire.beginTransmission(imuAddr);
  Wire.write(reg);
  Wire.write(v);
  return Wire.endTransmission() == 0;
}
// CTRL9 command with its handshake (STATUSINT.bit7 = done; then ACK)
static bool imuCtrl9(uint8_t cmd) {
  if (!imuPut(0x0A, cmd)) return false;
  bool done = false;
  uint8_t st;
  for (int k = 0; k < 50 && !done; k++) {
    if (imuReg(0x2D, &st, 1) && (st & 0x80)) done = true;
    else delayMicroseconds(100);
  }
  imuPut(0x0A, 0x00);  // CTRL_CMD_ACK
  for (int k = 0; k < 50; k++) {
    if (imuReg(0x2D, &st, 1) && !(st & 0x80)) break;
    delayMicroseconds(100);
  }
  return done;
}

void imuStepsBegin() {
  if (imu.begin(Wire, QMI8658_L_SLAVE_ADDRESS, IIC_SDA, IIC_SCL)) imuAddr = QMI8658_L_SLAVE_ADDRESS;
  else if (imu.begin(Wire, QMI8658_H_SLAVE_ADDRESS, IIC_SDA, IIC_SCL)) imuAddr = QMI8658_H_SLAVE_ADDRESS;
  imuOk = imuAddr != 0;
  if (!imuOk) { Serial.println("QMI8658 not found (no step counting)"); return; }
  imu.configAccel(AccelFullScaleRange::FS_8G, 56.0f, SensorQMI8658::LpfMode::MODE_3);
  imu.enableAccel();
  imuFifoOk = imu.configFifo(SensorQMI8658::FifoMode::STREAM, SensorQMI8658::FifoSamples::SAMPLES_128) &&
              imuReg(0x14, &imuFifoCtrl, 1);
  imuFifoCtrl &= 0x7F;  // not in FIFO read mode
  if (!imuFifoOk) Serial.println("QMI8658 FIFO unavailable");
}

bool imuStepsOk() { return imuOk && imuFifoOk; }

void imuStepsPump(uint32_t now) {
  static uint32_t last = 0, lastT = 0;
  if (!imuOk || !imuFifoOk || now - last < 50) return;
  last = now;
  uint8_t c[2];
  if (!imuReg(0x15, c, 2)) return;
  uint16_t n = (uint16_t)(((c[1] & 0x03) << 8) | c[0]) * 2 / 6;
  if (!n) return;
  if (n > 128) n = 128;
  bool ok = imuCtrl9(0x05);  // CTRL_CMD_REQ_FIFO: read mode
  uint8_t buf[30];
  for (uint16_t i = 0; ok && i < n;) {
    uint8_t k = n - i > 5 ? 5 : (uint8_t)(n - i);
    if (!imuReg(0x17, buf, k * 6)) break;
    for (uint8_t j = 0; j < k; j++, i++) {
      const uint8_t *b = buf + j * 6;
      float gx = (int16_t)(b[0] | b[1] << 8) / IMU_LSB_PER_G;
      float gy = (int16_t)(b[2] | b[3] << 8) / IMU_LSB_PER_G;
      float gz = (int16_t)(b[4] | b[5] << 8) / IMU_LSB_PER_G;
      uint32_t t = now - (uint32_t)(n - 1 - i) * IMU_SAMPLE_MS;
      if ((int32_t)(t - lastT) <= 0) t = lastT + 1;
      lastT = t;
      stepper.feed(sqrtf(gx * gx + gy * gy + gz * gz), t);
    }
  }
  imuPut(0x14, imuFifoCtrl);  // leave read mode so samples flow again
}

uint16_t imuStepsTake() { return stepper.take(); }
