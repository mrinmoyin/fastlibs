#pragma once

#ifndef MPU6050_H
#define MPU6050_H

#include <Arduino.h>
#include <Wire.h>

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
