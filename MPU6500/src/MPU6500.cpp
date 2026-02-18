#include "MPU6500.h"

bool MPU6500::begin() {

  return true;
}

void MPU6500::start() {
  wire.beginTransmission();
}
void MPU6500::end() {
  wire.endTransmission();
}
