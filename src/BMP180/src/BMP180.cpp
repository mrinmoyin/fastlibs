#include "BMP180.h"

bool BMP180::begin() {
  if (readReg(REG_CHIP_ID) != BMP180_CHIP_ID) return false;

  getCP();

  return true;
}
void BMP180::update() {
  getUT();
  getUP();
  getTemp();
  getPress();

  Serial1.print("ac1: ");
  Serial1.print(ac1);
  Serial1.print(" ac2: ");
  Serial1.print(ac2);
  Serial1.print(" ac3: ");
  Serial1.print(ac3);
  Serial1.print(" ac4: ");
  Serial1.print(ac4);
  Serial1.print(" ac5: ");
  Serial1.print(ac5);
  Serial1.print(" ac6: ");
  Serial1.print(ac6);
  Serial1.print(" b1: ");
  Serial1.print(b1);
  Serial1.print(" b2: ");
  Serial1.print(b2);
  Serial1.print(" mb: ");
  Serial1.print(mb);
  Serial1.print(" mc: ");
  Serial1.print(mc);
  Serial1.print(" md: ");
  Serial1.println(md);

  Serial1.print("UT: ");
  Serial1.print(ut);
  Serial1.print(" UP: ");
  Serial1.print(up);
  Serial1.print(" Temp: ");
  Serial1.print(t);
  Serial1.print(" Press: ");
  Serial1.println(p);
}

void BMP180::start() {
}
void BMP180::end() {
}

void BMP180::getCP() {
  wire.beginTransmission(BMP180_ADDR);
  wire.write(0xAA);
  wire.endTransmission();
  wire.requestFrom(BMP180_ADDR, 22);
  // byte calData[22];
  // wire.readBytes(calData, 22);
  // ac1 = (calData[0] < 8) | calData[1];
  ac1 = (wire.read() << 8) | wire.read();
  ac2 = (wire.read() << 8) | wire.read();
  ac3 = (wire.read() << 8) | wire.read();
  ac4 = (wire.read() << 8) | wire.read();
  ac5 = (wire.read() << 8) | wire.read();
  ac6 = (wire.read() << 8) | wire.read();
  b1 = (wire.read() << 8) | wire.read();
  b2 = (wire.read() << 8) | wire.read();
  mb = (wire.read() << 8) | wire.read();
  mc = (wire.read() << 8) | wire.read();
  md = (wire.read() << 8) | wire.read();
} 
void BMP180::getUT() {
  writeReg(0xF4, 0x2E);
  delay(4.5);
  ut = readReg16(REG_MEAS);
}
void BMP180::getUP() {
  writeReg(0xF4, 0x34 | (oss << 6));
  // writeReg(0xF4, 0x34);
  // writeReg(0xF4, 0x74);
  // writeReg(0xF4, 0xB4);
  // writeReg(0xF4, 0xF4);
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
  wire.beginTransmission(BMP180_ADDR);
  wire.write(REG_MEAS);
  wire.endTransmission();
  wire.requestFrom(BMP180_ADDR, 3);
  up = (((wire.read() << 16) | wire.read() << 8) | wire.read()) >> (8 - oss);
}
void BMP180::getTemp() {
  x1 = ((ut - ac6) * ac5) >> 15;
  x2 = (mc << 11) / (x1 + md);
  b5 = x1 + x2;
  t = (b5 + 8) >> 4;
}
void BMP180::getPress() {
  b6 = b5 - 4000;
  Serial1.print("b6: ");
  Serial1.println(b6);
  x1 = (b2 * (b6 * b6 >> 12)) >> 11;
  Serial1.print("x1: ");
  Serial1.println(x1);
  x2 = (ac2 * b6) >> 11;
  Serial1.print("x2: ");
  Serial1.println(x2);
  x3 = x1 + x2;
  Serial1.print("x3: ");
  Serial1.println(x3);
  b3 = ((((ac1 * 4) + x3) << oss) + 2) / 4;
  Serial1.print("b3: ");
  Serial1.println(b3);
  x1 = (ac3 * b6) >> 13;
  Serial1.print("x1: ");
  Serial1.println(x1);
  x2 = (b1 * ((b6 * b6) >> 12)) >> 16;
  Serial1.print("x2: ");
  Serial1.println(x2);
  x3 = ((x1 + x2) + 2) >> 2;
  Serial1.print("x3: ");
  Serial1.println(x3);
  b4 = (ac4 * (uint32_t)(x3 + 32768)) >> 15; 
  Serial1.print("b4: ");
  Serial1.println(b4);
  b7 = (uint32_t)(up - b3) * (50000 >> oss);
  Serial1.print("b7: ");
  Serial1.println(b7);
  if (b7 < 0x80000000) p = (b7 * 2) / b4;
  else p = (b7 / b4) * 2;
  Serial1.print("p: ");
  Serial1.println(p);
  x1 = (p >> 8) * (p >> 8);
  Serial1.print("x1: ");
  Serial1.println(x1);
  x1 = (x1 * 3038) >> 16;
  Serial1.print("x1: ");
  Serial1.println(x1);
  x2 = (-7357 * p) >> 16;
  Serial1.print("x2: ");
  Serial1.println(x2);
  p = (p + (x1 + x2 + 3791)) >> 4;
  Serial1.print("p: ");
  Serial1.println(p);
}

uint8_t BMP180::readReg(byte addr) {
  wire.beginTransmission(BMP180_ADDR);
  wire.write(addr);
  wire.endTransmission();
  wire.requestFrom(BMP180_ADDR, 1);
  return wire.read();
}
uint16_t BMP180::readReg16(byte addr) {
  wire.beginTransmission(BMP180_ADDR);
  wire.write(addr);
  wire.endTransmission();
  wire.requestFrom(BMP180_ADDR, 2);
  return (wire.read() << 8) | wire.read();
}
void BMP180::readRegBurst(byte addr, byte *buff, uint8_t len) {}
void BMP180::writeReg(byte addr, byte val) {
  wire.beginTransmission(BMP180_ADDR);
  wire.write(addr);
  wire.write(val);
  wire.endTransmission();
}
