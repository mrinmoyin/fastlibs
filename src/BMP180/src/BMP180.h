#pragma once

#ifndef BMP180_H
#define BMP180_H

#include <Arduino.h>
#include <Wire.h>

#define BMP180_ADDR 0x77

#define REG_TEMPERATURE 0x2E
#define REG_OSS_0       0x34
#define REG_OSS_1       0x74
#define REG_OSS_2       0xB4
#define REG_OSS_3       0xF4

#define REG_CAL_AC1     0xAA 
#define REG_CAL_AC2     0xAC 
#define REG_CAL_AC3     0xAE 
#define REG_CAL_AC4     0xB0 
#define REG_CAL_AC5     0xB2 
#define REG_CAL_AC6     0xB4 
#define REG_CAL_B1      0xB6 
#define REG_CAL_B2      0xB8 
#define REG_CAL_MB      0xBA 
#define REG_CAL_MC      0xBC 
#define REG_CAL_MD      0xBE

class BMP180 {
  public:
    BMP180(
        TwoWire &wire = Wire
        ):wire(wire),
          oss(0) {};

  float alt, temp, press;

  bool begin();
  void update();

  private: 
    TwoWire &wire;
    uint8_t oss;

    int16_t ac1, ac2, ac3, b1, b2, mb, mc, md;
    uint16_t ac4, ac5, ac6;
    int32_t x1, x2, x3, b3, b4, b5, b6, b7;
    int32_t ut, up, t, p;

    void start();
    void end();

    void getCP(); // get calibraation data
    void getUT(); // get uncompensated temperature value
    void getUP(); // get uncompensated pressure value
    void getTemp(); // get temperature in 0.1C
    void getPress(); // get pressure in Pa
                     
    byte readReg(byte addr);
    void readRegBurst(byte addr, byte *buff, uint8_t len);
    void writeReg(byte addr, byte value);
};

#endif
