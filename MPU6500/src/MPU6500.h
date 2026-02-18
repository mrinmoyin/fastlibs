#pragma once

#ifndef MPU6500_H
#define MPU6500_H

#include <Arduino.h>
#include <SPI.h>

#define REG_TEMPERATURE 0x2E
#define REG_OSS_0       0x34
#define REG_OSS_1       0x74
#define REG_OSS_2       0xB4
#define REG_OSS_3       0xF4

class MPU6500 {
  public:
    MPU6500(
        TwoWire &wire = Wire
        ):wire(wire) {};

  bool begin();

  private: 
    TwoWire &wire;

    void start();
    void end();
};

#endif
