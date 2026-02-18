#include "BMP180.h"

bool BMP180::begin() {
  wire.begin();

  return true;
}
