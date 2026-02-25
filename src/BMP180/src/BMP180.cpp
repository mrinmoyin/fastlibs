#include "BMP180.h"

bool BMP180::begin() {
  if (bus.read(BMP180_REG_CHIP_ID) != BMP180_CHIP_ID) return false;

  getCP();

  return true;
}
void BMP180::update() {
  getUT();
  getUP();
  getTemp();
  getPress();
}

void BMP180::getCP() {
  bus.readBurst(0xAA, calData, 22);
  cal.ac1 = (calData[0] << 8) | calData[1];
  cal.ac2 = (calData[2] << 8) | calData[3];
  cal.ac3 = (calData[4] << 8) | calData[5];
  cal.ac4 = (calData[6] << 8) | calData[7];
  cal.ac5 = (calData[8] << 8) | calData[9];
  cal.ac6 = (calData[10] << 8) | calData[11];
  cal.b1 = (calData[12] << 8) | calData[13];
  cal.b2 = (calData[14] << 8) | calData[15];
  cal.mb = (calData[16] << 8) | calData[17];
  cal.mc = (calData[18] << 8) | calData[19];
  cal.md = (calData[20] << 8) | calData[21];

  // bus.wire.beginTransmission(BMP180_REG_ADDR);
  // bus.wire.write(0xAA);
  // bus.wire.endTransmission();
  // bus.wire.requestFrom(BMP180_REG_ADDR, 22);
  // ac1 = (bus.wire.read() << 8) | bus.wire.read();
  // ac2 = (bus.wire.read() << 8) | bus.wire.read();
  // ac3 = (bus.wire.read() << 8) | bus.wire.read();
  // ac4 = (bus.wire.read() << 8) | bus.wire.read();
  // ac5 = (bus.wire.read() << 8) | bus.wire.read();
  // ac6 = (bus.wire.read() << 8) | bus.wire.read();
  // b1 = (bus.wire.read() << 8) | bus.wire.read();
  // b2 = (bus.wire.read() << 8) | bus.wire.read();
  // mb = (bus.wire.read() << 8) | bus.wire.read();
  // mc = (bus.wire.read() << 8) | bus.wire.read();
  // md = (bus.wire.read() << 8) | bus.wire.read();

  // Serial1.print("ac1: ");
  // Serial1.print(ac1);
  // Serial1.print(" ac2: ");
  // Serial1.print(ac2);
  // Serial1.print(" ac3: ");
  // Serial1.print(ac3);
  // Serial1.print(" ac4: ");
  // Serial1.print(ac4);
  // Serial1.print(" ac5: ");
  // Serial1.print(ac5);
  // Serial1.print(" ac6: ");
  // Serial1.print(ac6);
  // Serial1.print(" b1: ");
  // Serial1.print(b1);
  // Serial1.print(" b2: ");
  // Serial1.print(b2);
  // Serial1.print(" mb: ");
  // Serial1.print(mb);
  // Serial1.print(" mc: ");
  // Serial1.print(mc);
  // Serial1.print(" md: ");
  // Serial1.println(md);
} 
void BMP180::getUT() {
  bus.write(0xF4, 0x2E);
  delay(4.5);
  ut = bus.read16(BMP180_REG_MEAS);
  Serial1.print("ut: ");
  Serial1.println(ut);
}
void BMP180::getUP() {
  bus.write(0xF4, 0x34 | (oss << 6));
  // bus.write(0xF4, 0x34);
  // bus.write(0xF4, 0x74);
  // bus.write(0xF4, 0xB4);
  // bus.write(0xF4, 0xF4);
  switch (oss) {
    case 3:
      delay(25.5);
      break;
    case 2:
      delay(13.5);
      break;
    case 1:
      delay(7.5);
      break;
    case 0:
      delay(4.5);
      break;
  }
  // bus.wire.beginTransmission(BMP180_REG_ADDR);
  // bus.wire.write(BMP180_REG_MEAS);
  // bus.wire.endTransmission();
  // bus.wire.requestFrom(BMP180_REG_ADDR, 3);
  // up = (((bus.wire.read() << 16) | bus.wire.read() << 8) | bus.wire.read()) >> (8 - oss);
  up = bus.read24(BMP180_REG_MEAS) >> (8 - oss);
  Serial1.print("up: ");
  Serial1.println(up);
}
void BMP180::getTemp() {
  x1 = ((ut - cal.ac6) * cal.ac5) >> 15;
  x2 = (cal.mc << 11) / (x1 + cal.md);
  b5 = x1 + x2;
  t = (b5 + 8) >> 4;
  Serial1.print("x1: ");
  Serial1.print(x1);
  Serial1.print(" x2: ");
  Serial1.print(x2);
  Serial1.print(" b5: ");
  Serial1.print(b5);
  Serial1.print(" t: ");
  Serial1.println(t);
}
void BMP180::getPress() {
  b6 = b5 - 4000;
  Serial1.print("b6: ");
  Serial1.print(b6);
  x1 = (cal.b2 * (b6 * b6 >> 12)) >> 11;
  Serial1.print(" x1: ");
  Serial1.print(x1);
  x2 = (cal.ac2 * b6) >> 11;
  Serial1.print(" x2: ");
  Serial1.print(x2);
  x3 = x1 + x2;
  Serial1.print(" x3: ");
  Serial1.print(x3);
  b3 = ((((cal.ac1 * 4) + x3) << oss) + 2) / 4;
  Serial1.print(" b3: ");
  Serial1.print(b3);
  x1 = (cal.ac3 * b6) >> 13;
  Serial1.print(" x1: ");
  Serial1.print(x1);
  x2 = (cal.b1 * ((b6 * b6) >> 12)) >> 16;
  Serial1.print(" x2: ");
  Serial1.print(x2);
  x3 = ((x1 + x2) + 2) >> 2;
  Serial1.print(" x3: ");
  Serial1.print(x3);
  b4 = (cal.ac4 * (uint32_t)(x3 + 32768)) >> 15; 
  Serial1.print(" b4: ");
  Serial1.print(b4);
  b7 = (uint32_t)(up - b3) * (50000 >> oss);
  Serial1.print(" b7: ");
  Serial1.print(b7);
  if (b7 < 0x80000000) p = (b7 * 2) / b4;
  else p = (b7 / b4) * 2;
  Serial1.print(" p: ");
  Serial1.print(p);
  x1 = (p >> 8) * (p >> 8);
  Serial1.print(" x1: ");
  Serial1.print(x1);
  x1 = (x1 * 3038) >> 16;
  Serial1.print(" x1: ");
  Serial1.print(x1);
  x2 = (-7357 * p) >> 16;
  Serial1.print(" x2: ");
  Serial1.print(x2);
  p = (p + ((x1 + x2) + 3791)) >> 4;
  Serial1.print(" p: ");
  Serial1.println(p);
}
