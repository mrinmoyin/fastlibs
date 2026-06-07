#include "CC1101.h"

bool CC1101::begin() { 
  reset();
  delayMicroseconds(50);

  if(!getChipInfo() || 
      !getFreqBand(freq, freqTable) ||
      !(drate > drateTable[mod][0] && drate < drateTable[mod][1])) 
    return false;

  setMod(mod);
  setFreq(freq);
  setDrate(drate);
  setPwr(freqBand, pwr, pwrTable);

  // setCCA(true);
  setAddr(addr);
  setCRC(isCRC);
  setFEC(isFEC);
  setAutoCalib(isAutoCalib);
  setManchester(isManchester);
  setAppendStatus(isAppendStatus);
  setDataWhitening(isDataWhitening);
  setSync(syncMode, syncWord, preambleLen);
  setPktLenMode(pktLenMode); 
  setPktLen(pktLen);

  return true;
}

bool CC1101::read(uint8_t *buff) {
  if (getState() == STATE_RX) return false;
  bool isRead = readRxFifo(buff);
  setIdleState();
  // setState(STATE_IDLE);
  flushRxBuff();
  setRxState();
  // setState(STATE_RX);
  return isRead;
};
bool CC1101::write(uint8_t *buff) {
  // if (bus.readField(CC1101_REG_TXBYTES | CC1101_READ_BURST, 6, 0) != 0 || getState() != STATE_RX) {
  if (readStatusReg(CC1101_REG_TXBYTES) != 0 || getState() != STATE_RX) {
    setIdleState();
    // setState(STATE_IDLE);
    flushRxBuff();
    flushTxBuff();
  }
  writeTxFifo(buff);
  setTxState();
  // setState(STATE_TX);
  waitForState();
  flushTxBuff();
  setRxState();
  // setState(STATE_RX);
  return true;
};

void CC1101::reset() {
  digitalWrite(ss, HIGH);
  delayMicroseconds(5);
  digitalWrite(ss, LOW);
  delayMicroseconds(5);
  digitalWrite(ss, HIGH);
  delayMicroseconds(40);

  strobe(CC1101_REG_RES);
  // bus.strobe(CC1101_REG_RES | CC1101_WRITE_BURST);
};
void CC1101::flushRxBuff() {
  // if (getState() != (STATE_IDLE || STATE_RXFIFO_OVERFLOW)) return;
  if (getState() != (STATE_IDLE || STATE_RXFIFO_OVERFLOW)) setIdleState();
  strobe(CC1101_REG_FRX);
  // bus.strobe(CC1101_REG_FRX | CC1101_WRITE_BURST);
};
void CC1101::flushTxBuff() {
  if (getState() != (STATE_IDLE || STATE_TXFIFO_UNDERFLOW)) return;
  // if(getState() != (STATE_IDLE || STATE_TXFIFO_UNDERFLOW)) setIdleState();
  strobe(CC1101_REG_FTX);
  // bus.strobe(CC1101_REG_FTX | CC1101_WRITE_BURST);
};
void CC1101::waitForState(State state) {
  while (getState() != state) { 
    // Serial.printf("waiting for state: %d, current state is : %d", state, getState());
    // Serial.println();
  };
};

byte CC1101::getState() {
  // return (strobe(CC1101_REG_NOP) >> 4) & 0b00111;
  return (bus.strobe(CC1101_REG_NOP) >> 4) & 0b00111;
};
bool CC1101::getChipInfo() {
  partnum = readStatusReg(CC1101_REG_PARTNUM);
  version = readStatusReg(CC1101_REG_VERSION);

  if(partnum == CC1101_PARTNUM && version == CC1101_VERSION) return true;
  return false;
};
bool CC1101::getFreqBand(float freq, const float freqTable[][2]) {
  for(int i = 0; i < 4; i++) {
    if(freq >= freqTable[i][0] && freq <= freqTable[i][1]) {
      freqBand = (CC1101_FreqBand)i;
      return true;
    }
  }
  return false;
};
uint8_t CC1101::getPreambleIdx(uint8_t len) {
  switch (len) {
    case 16:
      return 0;
    break;
    case 24:
      return 1;
    break;
    case 32:
      return 2;
    break;
    case 48:
      return 3;
    break;
    case 64:
      return 4;
    break;
    case 96:
      return 5;
    break;
    case 128:
      return 6;
    break;
    case 192:
      return 7;
    break;
    default:
      return 0;;
  }
};

void CC1101::setCCA(bool en) {
  writeRegField(CC1101_REG_MCSM1, 5, 4, (uint8_t)en); // Set CCA
  if (en) {
    writeRegField(CC1101_REG_MCSM1, 1, 0, 0); // Set TXOFF to IDLE
    writeRegField(CC1101_REG_MCSM1, 3, 2, 0); // Set RXOFF to IDLE
  }
}
void CC1101::setCRC(bool en) {
  // writeRegField(CC1101_REG_PKTCTRL0, 2, 2, (uint8_t)en); /* CRC_EN */
  // writeRegField(CC1101_REG_PKTCTRL1, 3, 3, (uint8_t)en); /* Autoflush */
  bus.writeField(CC1101_REG_PKTCTRL0, CC1101_READ, CC1101_WRITE, 2, 2, (byte)en); /* CRC_EN */
  bus.writeField(CC1101_REG_PKTCTRL1, CC1101_READ, CC1101_WRITE, 3, 3, (byte)en); /* Autoflush */
};
void CC1101::setFEC(bool en) {
  if(isVariablePktLen) return;
  // writeRegField(CC1101_REG_MDMCFG1, 7, 7, (uint8_t)en);
  bus.writeField(CC1101_REG_MDMCFG1, CC1101_READ, CC1101_WRITE, 7, 7, (byte)en);
};
void CC1101::setAddr(byte addr) {
  // writeRegField(CC1101_REG_PKTCTRL1, 1, 0, addr > 0 ? 1 : 0);
  // writeReg(CC1101_REG_ADDR, addr);
  bus.writeField(CC1101_REG_PKTCTRL1, CC1101_READ, CC1101_WRITE, 1, 0, addr > 0 ? 1 : 0);
  bus.write(CC1101_REG_ADDR | CC1101_WRITE, addr);
};
void CC1101::setSync(CC1101_SyncMode syncMode, uint16_t syncWord, uint8_t preambleLen) {
  // writeRegField(CC1101_REG_MDMCFG2, 2, 0, (uint8_t)syncMode);
  // writeReg(CC1101_REG_SYNC0, (syncWord >> 0) & 0xff);
  // writeReg(CC1101_REG_SYNC1, (syncWord >> 8) & 0xff);
  // writeRegField(CC1101_REG_MDMCFG1, 6, 4, getPreambleIdx(preambleLen));
  bus.writeField(CC1101_REG_MDMCFG2, CC1101_READ, CC1101_WRITE, 2, 0, (byte)syncMode);
  bus.write(CC1101_REG_SYNC0 | CC1101_WRITE, (syncWord >> 0) & 0xff);
  bus.write(CC1101_REG_SYNC1 | CC1101_WRITE, (syncWord >> 8) & 0xff);
  bus.writeField(CC1101_REG_MDMCFG1, CC1101_READ, CC1101_WRITE, 6, 4, getPreambleIdx(preambleLen));
};
void CC1101::setAutoCalib(bool en) {
  // writeRegField(CC1101_REG_MCSM0, 5, 4, (uint8_t)en);
  bus.writeField(CC1101_REG_MCSM0, CC1101_READ, CC1101_WRITE, 5, 4, (byte)en);
};
void CC1101::setManchester(bool en) {
  if(mod != CC1101_MOD_MSK || mod != CC1101_MOD_4FSK)
    // writeRegField(CC1101_REG_MDMCFG2, 3, 3, (uint8_t)en);
    bus.writeField(CC1101_REG_MDMCFG2, CC1101_READ, CC1101_WRITE, 3, 3, (byte)en);
};
void CC1101::setAppendStatus(bool en) {
  // writeRegField(CC1101_REG_PKTCTRL1, 2, 2, (uint8_t)en);
  bus.writeField(CC1101_REG_PKTCTRL1, CC1101_READ, CC1101_WRITE, 2, 2, (byte)en);
};
void CC1101::setDataWhitening(bool en) {
  // writeRegField(CC1101_REG_PKTCTRL0, 6, 6, (uint8_t)en);
  bus.writeField(CC1101_REG_PKTCTRL0, CC1101_READ, CC1101_WRITE, 6, 6, (byte)en);
};
void CC1101::setPktLen(uint8_t len) {
  writeReg(CC1101_REG_PKTLEN, len);
  // bus.write(CC1101_REG_PKTLEN | CC1101_WRITE, len);
};
void CC1101::setPktLenMode(CC1101_PktLenMode mode) { // TODO: infinite 
  writeRegField(CC1101_REG_PKTCTRL0, 1, 0, (uint8_t)mode);
  // bus.writeField(CC1101_REG_PKTCTRL0, CC1101_READ, CC1101_WRITE, 1, 0, (uint8_t)mode);
  // bus.writeField(CC1101_REG_PKTCTRL0, CC1101_READ, CC1101_WRITE, 1, 0, (byte)isVariablePktLen); 
};
void CC1101::setMod(CC1101_Modulation mod) {
  // writeRegField(CC1101_REG_MDMCFG2, 6, 4, (uint8_t)mod);
  bus.writeField(CC1101_REG_MDMCFG2, CC1101_READ, CC1101_WRITE, 6, 4, (byte)mod);
};
void CC1101::setFreq(float freq) {
  uint32_t f = ((freq * 65536.0) / CC1101_CRYSTAL_FREQ); 

  // writeReg(CC1101_REG_FREQ0, f & 0xff);
  // writeReg(CC1101_REG_FREQ1, (f >> 8) & 0xff);
  // writeReg(CC1101_REG_FREQ2, (f >> 16) & 0xff);
  bus.write(CC1101_REG_FREQ0 | CC1101_WRITE, f & 0xff);
  bus.write(CC1101_REG_FREQ1 | CC1101_WRITE, (f >> 8) & 0xff);
  bus.write(CC1101_REG_FREQ2 | CC1101_WRITE, (f >> 16) & 0xff);

  /* TODO Deviation */ 
  // writeRegField(CC1101_REG_DEVIATN, CC1101_READ, CC1101_WRITE, 6, 4, devE);
  // writeRegField(CC1101_REG_DEVIATN, CC1101_READ, CC1101_WRITE, 2, 0, devM);
};
void CC1101::setDrate(float drate) {
  uint32_t xosc = CC1101_CRYSTAL_FREQ * 1000;
  uint8_t e = log2((drate * (double)((uint32_t)1 << 20)) / xosc);
  uint32_t m = round(drate * ((double)((uint32_t)1 << (28 - e)) / xosc) - 256.0);

  if (m == 256) {
    m = 0;
    e++;
  }

  // writeRegField(CC1101_REG_MDMCFG4, 3, 0, e);
  // writeRegField(CC1101_REG_MDMCFG3, 7, 0, (uint8_t)m);
  // writeReg(CC1101_REG_MDMCFG3, (uint8_t)m);
  bus.writeField(CC1101_REG_MDMCFG4, CC1101_READ, CC1101_WRITE, 3, 0, e);
  bus.writeField(CC1101_REG_MDMCFG3, CC1101_READ, CC1101_WRITE, 7, 0, (byte)m);
  // bus.write(CC1101_REG_MDMCFG3 | CC1101_WRITE, (byte)m);
};
void CC1101::setPwr(CC1101_FreqBand freqBand, CC1101_PowerMW pwr, const uint8_t pwrTable[][8]){
  // if(mod == CC1101_MOD_ASK_OOK) {
  //   uint8_t paTable[2] = { CC1101_WRITE, pwrTable[freqBand][pwr] };
  //   writeRegBurst(CC1101_REG_PATABLE, paTable, 2);
  //   writeRegField(CC1101_REG_FREND0, 2, 0, 1);
  // } else {
  //   writeReg(CC1101_REG_PATABLE, pwrTable[freqBand][pwr]);
  //   writeRegField(CC1101_REG_FREND0, 2, 0, 0);
  // }
  // writeRegField(CC1101_REG_FREND0, 2, 0, mod == CC1101_MOD_ASK_OOK ? 1 : 0);
  // writeReg(CC1101_REG_PATABLE, pwrTable[freqBand][pwr]);
  bus.writeField(CC1101_REG_FREND0, CC1101_READ, CC1101_WRITE, 2, 0, mod == CC1101_MOD_ASK_OOK ? 1 : 0);
  bus.write(CC1101_REG_PATABLE | CC1101_WRITE, pwrTable[freqBand][pwr]);
};

void CC1101::setState(State state) {
  byte currentState = getState();
  if (currentState == state) return;
  switch (state) {
    case STATE_IDLE: 
      // strobe(CC1101_REG_IDLE);
      bus.strobe(CC1101_REG_IDLE);
      break;
    case STATE_RX: 
      if (currentState == STATE_RXFIFO_OVERFLOW)
        // strobe(CC1101_REG_FRX);
        bus.strobe(CC1101_REG_FRX);
      // if (currentState == (STATE_CALIB || STATE_SETTLING)) setState(); 
      // strobe(CC1101_REG_RX);
      // if (currentState != STATE_CALIB || currentState != STATE_SETTLING) 
      else if (state != (STATE_CALIB || STATE_SETTLING)) 
        // strobe(CC1101_REG_RX);
        bus.strobe(CC1101_REG_RX);
      break;
    case STATE_TX: 
      if (currentState == STATE_TXFIFO_UNDERFLOW) 
        // strobe(CC1101_REG_FTX);
        bus.strobe(CC1101_REG_FTX);
      // if (currentState == (STATE_CALIB || STATE_SETTLING)) setState(); 
      // strobe(CC1101_REG_TX);
      // if (currentState != STATE_CALIB || currentState != STATE_SETTLING) 
      else if (state != (STATE_CALIB || STATE_SETTLING)) 
        // strobe(CC1101_REG_TX);
        bus.strobe(CC1101_REG_TX);
      break;
  }
  waitForState(state);
  // while (getState() != state);
};
void CC1101::setIdleState() {
  if (getState() == STATE_IDLE) return;
  // strobe(CC1101_REG_IDLE);
  bus.strobe(CC1101_REG_IDLE);
  waitForState();
};
void CC1101::setRxState() {
  byte state = getState();
  if (state == STATE_RX) return; 
  else if (state == STATE_RXFIFO_OVERFLOW) 
    // strobe(CC1101_REG_FRX);
    bus.strobe(CC1101_REG_FRX);
  else if (state != (STATE_CALIB || STATE_SETTLING)) 
    // strobe(CC1101_REG_RX);
    bus.strobe(CC1101_REG_RX);
  waitForState(STATE_RX);
};
void CC1101::setTxState() {
  byte state = getState();
  if (state == STATE_TX) return;
  else if (state == STATE_TXFIFO_UNDERFLOW) 
    // strobe(CC1101_REG_FTX);
    bus.strobe(CC1101_REG_FTX);
  else if (state != (STATE_CALIB || STATE_SETTLING)) 
    // strobe(CC1101_REG_TX);
    bus.strobe(CC1101_REG_TX);
  waitForState(STATE_TX);
};
bool CC1101::readRxFifo(uint8_t *buff) {
  uint8_t len = isVariablePktLen ? readReg(CC1101_REG_FIFO) : pktLen;
  if(addr) (void)readReg(CC1101_REG_FIFO);
  // if (bus.readField(CC1101_REG_RXBYTES | CC1101_READ_BURST, 6, 0) < len) return false;
  if (readStatusReg(CC1101_REG_RXBYTES) < len) return false;
  readRegBurst(CC1101_REG_FIFO, buff, len);
  if(isAppendStatus) {
    uint8_t r = readReg(CC1101_REG_FIFO);
    // if(r >= 128) rssi = ((rssi - 256) / 2) - CC1101_RSSI_OFFSET;
    // else rssi = (rssi / 2) - CC1101_RSSI_OFFSET;
    rssi = ((r >= 128 ? (r - 256) : r) / 2) - CC1101_RSSI_OFFSET;  
    lqi = readReg(CC1101_REG_FIFO) & 0x7f;
    if(!(r >> 7) & 1) return false; // CRC Mismatch
  }
  return true;
};
void CC1101::writeTxFifo(uint8_t *buff) {
  uint8_t len = isVariablePktLen ? sizeof(buff) : pktLen;
  if (isVariablePktLen) writeReg(CC1101_REG_FIFO, len);
  if (addr) writeReg(CC1101_REG_FIFO, addr);
  // writeReg(CC1101_REG_FIFO, len);
  writeRegBurst(CC1101_REG_FIFO, buff, len);
};

uint8_t CC1101::strobe(byte addr) {
  // return bus.strobe(addr);
  // return bus.strobe(CC1101_WRITE | (addr & 0b111111));
  return bus.strobe(addr | CC1101_WRITE_BURST);
};
uint8_t CC1101::readReg(byte addr) {
  return bus.read(addr | CC1101_READ);
  // return bus.read(CC1101_READ | (addr & 0b111111));
};
uint8_t CC1101::readStatusReg(byte addr) {
  // uint8_t header = CC1101_READ | (addr & 0b111111);
  // header |= CC1101_BURST;
  return bus.read(addr | CC1101_READ_BURST);
  // return bus.read(CC1101_READ | CC1101_BURST | addr);
};
uint8_t CC1101::readRegField(byte addr, byte lo, byte hi) {
  // return bus.readField(addr | CC1101_BURST, lo, hi);
  return (readReg(addr) >> lo) & ((1 << (hi - lo + 1)) - 1);
};
void CC1101::readRegBurst(byte addr, uint8_t *buff, size_t len) {
  bus.readBurst(addr | CC1101_READ_BURST, buff, len);
  // bus.readBurst(CC1101_READ | CC1101_BURST | (addr & 0b111111), buff, len);
};
void CC1101::writeReg(byte addr, uint8_t val) {
  bus.write(addr | CC1101_WRITE, val);
  // bus.write(CC1101_WRITE | (addr & 0b111111), val);
};
void CC1101::writeRegField(byte addr, byte lo, byte hi, uint8_t val) {
  bus.writeField(addr, CC1101_READ, CC1101_WRITE, hi, lo, val);
  // uint8_t mask = ((1 << (hi - lo + 1)) -1) << lo;
  // writeReg(addr, (readReg(addr) & ~mask) | ((val <<= lo) & mask));
};
void CC1101::writeRegBurst(byte addr, uint8_t *buff, size_t len) {
  bus.writeBurst(addr | CC1101_WRITE_BURST, buff, len);
  // bus.writeBurst(CC1101_WRITE | CC1101_BURST | (addr & 0b111111), buff, len);
};
