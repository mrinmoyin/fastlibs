#pragma once

#ifndef MPU6050_H
#define MPU6050_H

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#define REG_TEMPERATURE 0x2E
#define REG_OSS_0       0x34
#define REG_OSS_1       0x74
#define REG_OSS_2       0xB4
#define REG_OSS_3       0xF4

class MPU6050 {
  public:
    MPU6050(
        TwoWire &wire = Wire
        ):wire(wire) {};

  bool begin();

  private: 
    TwoWire &wire;

    void start();
    void end();
};

#endif
