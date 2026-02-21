#include "BMP180.h"

bool BMP180::begin() {
  wire.beginTransmission(BMP180_ADDR);
  byte status = wire.endTransmission();

  if (status != 0) return false;

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
  Serial1.println(ac4);

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
  wire.beginTransmission(BMP180_ADDR);
  wire.write(0xF4);
  wire.write(0x2E);
  // wire.endTransmission();
  if (wire.endTransmission != 0) Serial1.println("Error endTransmission");
  delay(4.5);
  wire.beginTransmission(BMP180_ADDR);
  wire.write(0xF6);
  // wire.endTransmission();
  if (wire.endTransmission != 0) Serial1.println("Error endTransmission");
  wire.requestFrom(BMP180_ADDR, 2);
  ut = (wire.read() << 8) | wire.read();
  // ut <<= wire.read();
  // ut |= wire.read();   
}
void BMP180::getUP() {
  wire.beginTransmission(BMP180_ADDR);
  wire.write(0xF4);
  wire.write(0x34 | (oss << 6));
  wire.endTransmission();
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
  wire.write(0xF6);
  wire.endTransmission();
  wire.requestFrom(BMP180_ADDR, 3);
  up = (((wire.read() << 16) | wire.read() << 8) | wire.read()) >> (8 - oss);
}
void BMP180::getTemp() {
  x1 = (ut - ac6) * ac5 / (2 << 15);
  x2 = mc * (2 << 11) / (x1 + md);
  t = (b5 + 8) / (2 << 4);
}
void BMP180::getPress() {
  b6 = b5 - 4000;
  x1 = (b2 * (b6 * b6 / (2 << 12))) / (2 << 11);
  x2 = ac2 * b6 / (2 << 11);
  x3 = x1 + x2;
  b3 = (((ac1 * 4 + x3) << oss) + 2) / 4;
  x1 = ac3 * b6 / (2 << 13);
  x2 = (b1 * (b6 * b6 / (2 << 12))) / (2 << 16);
  x3 = ((x1 + x2) + 2) / (2 << 2);
  b4 = ac4 * (uint32_t)(up - b3) * (50000 >> 0);
  if (b7 < 0x80000000) p = (b7 * 2) / b4;
  else p = (b7 / b4) * 2;
  x1 = (p / (2 << 8)) * (p / (2 << 8));
  x1 = (x1 * 3038) / (2 << 16);
  x2 = (-7357 * p) / (2 << 16);
  p = p + (x1 + x2 + 3791) / (2 << 4);
}

byte BMP180::readReg(byte addr) {}
void BMP180::readRegBurst(byte addr, byte *buff, uint8_t len) {}
void BMP180::writeReg(byte addr, byte value) {}
