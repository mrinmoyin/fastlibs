#pragma once

#ifndef BMP180_H
#define BMP180_H

#include <Arduino.h>
#include <SPI.h>

class BMP180 {
  public:
    BMP180(
        TwoWire &wire = Wire
        ):wire(wire) {};

  float alt, temp, pres;

  bool begin();

  private: 
    TwoWire &spi;
};

#endif
