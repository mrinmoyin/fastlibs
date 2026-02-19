#include "BMP180.h"

bool BMP180::begin() {
  return true;
}
bool BMP180::update() {
  return true;
}

void BMP180::start(byte addr, uint8_t length) {
  wire.beginTransmission(BMP180_ADDR);
  wire.write(addr);
  wire.endTransmission(false);
  wire.requestFrom(BMP180_ADDR, length);
}
void BMP180::end() {
  wire.endTransmission();
}

void BMP180::getCP() {
  start();
  ac1 <<= wire.read()
  ac1 |= wire.read()
  end();
} 
void BMP180::getUT() {}
void BMP180::getUP() {}
void BMP180::getTemp() {}
void BMP180::getPress() {}
