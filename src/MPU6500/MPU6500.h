#pragma once

#ifndef MPU6500_H
#define MPU6500_H

#include "../fastutils.h"

#define MPU6500_CHIP_ID           0x70
#define MPU6500_REG_ADDR          0x68
#define MPU6500_REG_WHO_AM_I      0x75
#define MPU6500_REG_DATA          0x3B 
#define MPU6500_REG_CONFIG        0x1A 
#define MPU6500_REG_CONFIG_GYRO   0x1B 
#define MPU6500_REG_CONFIG_ACCEL  0x1C 
#define MPU6500_REG_CONFIG_ACCEL2 0x1D 
#define MPU6500_REG_OFFSET_ACCEL  0x77 
#define MPU6500_REG_OFFSET_GYRO   0x13 
#define MPU6500_REG_CTRL          0x6A
#define MPU6500_REG_PWR_MGMT_1    0x6B
#define MPU6500_REG_PWR_MGMT_2    0x6C

class MPU6500 {
  public:
    MPU6500(uint8_t accelScale = 2, uint16_t gyroScale = 250, TwoWire &wire = Wire): 
      accelScale(accelScale), gyroScale(gyroScale), bus(MPU6500_REG_ADDR, wire) {};
    MPU6500(
        uint8_t accelScale = 2, uint16_t gyroScale = 250, byte ss = SS, byte miso = MISO, SPIClass &spi = SPI
        ): accelScale(accelScale), gyroScale(gyroScale), bus(ss, miso, spi) {};

  bool begin();
  void update();

  XYZ accel, gyro, accelOffset, gyroOffset;
  uint16_t temp;

  private: 
    Bus bus;
    byte rawData[14];
    byte rawAccelOffset[6];
    byte rawGyroOffset[6];
    uint8_t accelScale;
    uint16_t gyroScale;

    void reset();
    void setAccelScale(uint8_t scale);
    void setGyroScale(uint16_t scale);
};

#endif
