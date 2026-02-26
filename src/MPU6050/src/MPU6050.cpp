#include "MPU6050.h"

bool MPU6050::begin() {
  if(bus.read(MPU6050_REG_WHO_AM_I) != MPU6050_CHIP_ID) return false;

  reset();

  setAccelScale(accelScale);
  setGyroScale(gyroScale);

  bus.readBurst(MPU6050_REG_OFFSET_ACCEL, rawAccelOffset, 6);
  accelOffset.x = (rawData[0] << 8) | rawData[1];
  accelOffset.y = (rawData[2] << 8) | rawData[3];
  accelOffset.z = (rawData[4] << 8) | rawData[5];

  bus.readBurst(MPU6050_REG_OFFSET_GYRO, rawGyroOffset, 6);
  gyroOffset.x = (rawData[0] << 8) | rawData[1];
  gyroOffset.y = (rawData[2] << 8) | rawData[3];
  gyroOffset.z = (rawData[4] << 8) | rawData[5];

  return true;
}
void MPU6050::update() {
  bus.readBurst(MPU6050_REG_DATA, rawData, 14);
  accel.x = (rawData[0] << 8) | rawData[1];
  accel.y = (rawData[2] << 8) | rawData[3];
  accel.z = (rawData[4] << 8) | rawData[5];
  temp = (rawData[6] << 8) | rawData[7];
  gyro.x = (rawData[8] << 8) | rawData[9];
  gyro.y = (rawData[10] << 8) | rawData[11];
  gyro.z = (rawData[12] << 8) | rawData[13];
}

void MPU6050::reset() {
  bus.writeField(MPU6050_REG_PWR_MGMT_1, 1, 7, 7);
}
void MPU6050::setAccelScale(uint8_t scale) {
  byte scaleIdx;
  switch (scale) {
    case 2:
      scaleIdx = 0;
      break;
    case 4:
      scaleIdx = 1;
      break;
    case 8:
      scaleIdx = 2;
      break;
    case 16:
      scaleIdx = 3;
      break;
    default:
      return;
  }
  bus.writeField(MPU6050_REG_CONFIG_ACCEL, scaleIdx, 4, 3);
}
void MPU6050::setGyroScale(uint16_t scale) {
  byte scaleIdx;
  switch (scale) {
    case 250:
      scaleIdx = 0;
      break;
    case 500:
      scaleIdx = 1;
      break;
    case 1000:
      scaleIdx = 2;
      break;
    case 2000:
      scaleIdx = 3;
      break;
    default:
      return;
  }
  bus.writeField(MPU6050_REG_CONFIG_GYRO, scaleIdx, 4, 3);
}
