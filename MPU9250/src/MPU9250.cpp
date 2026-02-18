#include "MPU9250.h"

bool MPU9250::begin() {

  return true;
}

void MPU9250::start() {
  wire.beginTransmission();
}
void MPU9250::end() {
  wire.endTransmission();
}
