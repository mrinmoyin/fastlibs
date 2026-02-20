#pragma once

#ifndef MPU6500_H
#define MPU6500_H

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

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
