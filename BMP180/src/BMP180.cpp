#include "BMP180.h"

bool BMP180::begin() {

  return true;
}

void BMP180::start() {
  wire.beginTransmission();
}
void BMP180::end() {
  wire.endTransmission();
}
