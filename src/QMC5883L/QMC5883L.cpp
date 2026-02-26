#include "QMC5883L.h"

bool QMC5883L::begin() {
  if(bus.read(QMC5883L_REG_CHIP_ID) != QMC5883L_CHIP_ID) return false;

  reset();

  setDrate(drate);
  setScale(scale);
  setBandwidth(bandwidth);
  setMode(true);

  update();

  return true;
}
void QMC5883L::update() {
  bus.readBurst(QMC5883L_REG_DATA, rawData, 6);
  mag.x = (rawData[0] << 8) | rawData[1];
  mag.y = (rawData[2] << 8) | rawData[3];
  mag.z = (rawData[4] << 8) | rawData[5];

  temp = bus.read16(QMC5883L_REG_TEMP);
}

void QMC5883L::reset() {
  bus.write(QMC5883L_REG_RESET, 1);
}
void QMC5883L::setMode(bool isContinuous) {
  bus.writeField(QMC5883L_REG_CTRL, byte(isContinuous), 7, 6);
}
void QMC5883L::setDrate(byte drate) {
  byte drateIdx;
  switch (drate) {
    case 10:
      drateIdx = 0;
      break;
    case 50:
      drateIdx = 1;
      break;
    case 100:
      drateIdx = 2;
      break;
    case 200:
      drateIdx = 3;
      break;
  }
  bus.writeField(QMC5883L_REG_CTRL, drateIdx, 5, 4);
}
void QMC5883L::setScale(byte scale) {
  byte scaleIdx;
  switch (scale) {
    case 2:
      scaleIdx = 0;
      break;
    case 8:
      scaleIdx = 1;
      break;
  }
  bus.writeField(QMC5883L_REG_CTRL, scaleIdx, 3, 2);
}
void QMC5883L::setBandwidth(uint16_t bandwidth) {
  byte bandwidthIdx;
  switch (bandwidth) {
    case 512:
      bandwidthIdx = 0;
      break;
    case 256:
      bandwidthIdx = 1;
      break;
    case 128:
      bandwidthIdx = 2;
      break;
    case 64:
      bandwidthIdx = 3;
      break;
  }
  bus.writeField(QMC5883L_REG_CTRL, bandwidthIdx, 1, 0);
}
