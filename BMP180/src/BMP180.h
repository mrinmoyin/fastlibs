#pragma once

#ifndef BMP180_H
#define BMP180_H

#include <Arduino.h>
#include <Wire.h>

#define REG_TEMPERATURE 0x2E
#define REG_OSS_0       0x34
#define REG_OSS_1       0x74
#define REG_OSS_2       0xB4
#define REG_OSS_3       0xF4

class BMP180 {
  public:
    BMP180(
        TwoWire &wire = Wire
        ):wire(wire) {};

  float alt, temp, pres;

  bool begin();

  private: 
    TwoWire &wire;

    void start();
    void end();
};

#endif
