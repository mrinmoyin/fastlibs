#pragma once

#ifndef MPU9250_H
#define MPU9250_H

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

class MPU9250 {
  public:
    MPU9250(
        TwoWire &wire = Wire
        ):wire(wire) {};

  bool begin();

  private: 
    TwoWire &wire;

    void start();
    void end();
};

#endif
