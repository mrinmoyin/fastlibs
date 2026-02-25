#pragma once

#ifndef MPU6500_H
#define MPU6500_H

#include "../../fastutils.h"

#define MPU6500_CHIP_ID   0x70
#define MPU6500_REG_ADDR  0x68
#define MPU6500_REG_WHO_AM_I      0x75
#define MPU6500_REG_DATA          0x3B 
#define MPU6500_REG_CONFIG        0x1A 
#define MPU6500_REG_CONFIG_GYRO   0x1B 
#define MPU6500_REG_CONFIG_ACCEL  0x1C 
#define MPU6500_REG_CONFIG_ACCEL2 0x1D 
#define MPU6500_REG_OFFSET_GYRO   0x13 
#define MPU6500_REG_OFFSET_ACCEL  0x77 
#define MPU6500_REG_CTRL          0x6A
#define MPU6500_REG_PWR_MGMT_1    0x6B
#define MPU6500_REG_PWR_MGMT_2    0x6C

enum AccelScale {
  ACCEL_SCALE_2G  = 2,
  ACCEL_SCALE_4G  = 4,
  ACCEL_SCALE_8G  = 8,
  ACCEL_SCALE_16G = 16,
};
enum GyroScale {
  GYRO_SCALE_250DPS  = 250,
  GYRO_SCALE_500DPS  = 500,
  GYRO_SCALE_1000DPS = 1000,
  GYRO_SCALE_2000DPS = 2000,
};

class MPU6500 {
  public:
    MPU6500(uint8_t accelScale = 2, uint16_t gyroScale = 250, TwoWire &wire = Wire): 
      accelScale(accelScale), gyroScale(gyroScale), bus(MPU6500_REG_ADDR, wire) {};
    MPU6500(
        uint8_t accelScale = 2, uint16_t gyroScale = 250, byte ss = SS, byte miso = MISO, SPIClass &spi = SPI
        ): accelScale(accelScale), gyroScale(gyroScale), bus(ss, miso, spi) {};

  bool begin();
  void update();

  uint8_t accelScale;
  uint16_t gyroScale;
  XYZ accel, gyro;
  uint16_t temp;

  private: 
    Bus bus;
    byte rawData[14];

    void reset();
    void setAccelScale(uint8_t scale);
    void setGyroScale(uint16_t scale);
};

#endif
