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
  setPwr(freqBand, pwr, powerTable);

  setAddr(addr);
  setCRC(isCRC);
  setFEC(isFEC);
  setAutoCalib(isAutoCalib);
  setManchester(isManchester);
  setAppendStatus(isAppendStatus);
  setDataWhitening(isDataWhitening);
  setSync(syncMode, syncWord, preambleLen);
  setPktLenMode(isVariablePktLen);
  setPktLen(pktLen);

  return true;
}

bool CC1101::read(uint8_t *buff) {
  Serial.printf("readState: %d\n", getState());
  Serial.println();
  if (getState() == STATE_RX) return false;
  bool isRead = readRxFifo(buff);
  Serial.printf("isRead: %d\n", isRead);
  Serial.println();
  setState();
  flushRxBuff();
  setState(STATE_RX);
  return isRead;
};
bool CC1101::write(uint8_t *buff) {
  // uint8_t txBytes = readStatus(CC1101_REG_TXBYTES);
  // if (txBytes != 0 || getState() != STATE_RX) {
  //   Serial.println("tx bytes is not empty, or state is not rx");
  //   strobe(CC1101_REG_IDLE);
  //   strobe(CC1101_REG_FRX);
  //   strobe(CC1101_REG_FTX);
  //   strobe(CC1101_REG_RX);
  // }
  //
  // strobe(CC1101_REG_TX);
  //
  // if (getState() == STATE_RX) {
  //   Serial.println("expected tx state but got rx");
  //   return false;
  // }
  //
  // writeTxFifo(buff);
  // Serial.println("written to txfifo");
  // waitForState();
  // strobe(CC1101_REG_FTX);
  // strobe(CC1101_REG_RX);
  // return true;

 
  if (readStatus(CC1101_REG_TXBYTES) != 0) {
    Serial.println("tx bytes is not empty");
    setState();
    flushTxBuff();
    waitForState();
  }
  writeTxFifo(buff);
  Serial.printf("written to tx fifo: %d",readStatus(CC1101_REG_TXBYTES));
  Serial.println();
  setState(STATE_TX);
  waitForState();
  return true;


  // setState();
  // flushTxBuff();
  // setState(STATE_TX);
  // writeTxFifo(buff);
  // waitForState();
  // return true;
};
// bool CC1101::read(uint8_t *buff){
  // uint8_t len;
  // setIdleState();
  // flushRxBuff();
  // setRxState();
  // if (!isVariablePktLen) {
  //   len = pktLen;
  // } else {
  //   while(!enoughRxBytes(1));
  //   len = bus.read(CC1101_REG_FIFO | CC1101_READ);
  //   Serial.print("pkt length is ");
  //   Serial.println(len);
  // }
  // waitForRxBytes(len);
  // if (!readRxFifo(buff, len)) return false; // crc mismatch
  // waitForState();
  // return true;
// };
// bool CC1101::read(uint8_t *buff, uint8_t len){
//   setIdleState();
//   setPktLenMode(false);
//   setPktLen(len);
//   flushRxBuff();
//   setRxState();
//   waitForRxBytes(len);
//   if (!readRxFifo(buff, len)) return false; // crc mismatch
//   waitForState();
//   return true;
// };
// bool CC1101::readUntil(uint8_t *buff, size_t timeoutMs){
//   uint8_t len;
//   setIdleState();
//   flushRxBuff();
//   setRxState();
//   if (!isVariablePktLen) {
//     len = pktLen;
//   } else {
//     while(!enoughRxBytes(1));
//     len = bus.read(CC1101_REG_FIFO | CC1101_READ);
//   }
//   if (!waitForRxBytes(len, timeoutMs)) return false; //timeout
//   if (!readRxFifo(buff, len)) return false; // crc mismatch
//   waitForState();
//   return true;
// };
// bool CC1101::readUntil(uint8_t *buff, uint8_t len, size_t timeoutMs){
//   setIdleState();
//   setPktLenMode(false);
//   setPktLen(len);
//   flushRxBuff();
//   setRxState();
//   if (!waitForRxBytes(len, timeoutMs)) return false; //timeout
//   if (!readRxFifo(buff, len)) return false; // crc mismatch
//   waitForState();
//   return true;
// };
// bool CC1101::write(uint8_t *buff) {
//   uint8_t len = !isVariablePktLen ? pktLen : sizeof(buff);
//   setIdleState();
//   flushTxBuff();
//   setTxState();
//   writeTxFifo(buff, len);
//   waitForState();
//   return true;
// };
// bool CC1101::write(uint8_t *buff, uint8_t len){
//   setIdleState();
//   setPktLen(len);
//   flushTxBuff();
//   setTxState();
//   writeTxFifo(buff, len);
//   waitForState();
//   return true;
// };
// bool CC1101::link(uint8_t *txBuff, uint8_t *rxBuff, size_t timeoutMs) {
//   uint32_t timer = millis();
//   setIdleState();
//   flushTxBuff();
//   setTxState();
//   writeTxFifo(txBuff, pktLen);
//   waitForState();
//   flushRxBuff();
//   setRxState();
//   while (!enoughRxBytes(pktLen)) {
//     if (timer + timeoutMs < millis()) {
//       return false; // timeout
//     }
//     delay(1); // avoid watchdog
//   }
//   readRxFifo(rxBuff, pktLen);
//   waitForState();
//   return true;
// };
// void CC1101::link2(uint8_t *txBuff, uint8_t *rxBuff, size_t timeoutMs) {
//   uint32_t timer;
//   setIdleState();
//   setTwoWay();
//   setTxState();
//   while(true) {
//     flushTxBuff();
//     writeTxFifo(txBuff, pktLen);
//     waitForState(STATE_RX);
//     Serial.println("Sent packet.");
//     flushRxBuff();
//     timer = millis();
//     while (true) { /* state goes to tx even when fifo is empty */
//       Serial.print("state: ");
//       Serial.println(getState());
//       if (bus.readField(CC1101_REG_RXBYTES | CC1101_READ_BURST, 6, 0) != 0) {
//         Serial.println("rxbytes > 0");
//         readRxFifo(rxBuff, pktLen);
//         waitForState(STATE_TX);
//         Serial.println("Received packet.");
//         break;
//       // } else if (timer + timeoutMs < millis()) {
//       //   setIdleState();
//       //   setTxState();
//       //   Serial.println("timeout");
//       //   break;
//       // } else {
//         delay(500);
//       }
//     }
//   }
// };

void CC1101::reset() {
  digitalWrite(ss, HIGH);
  delayMicroseconds(5);
  digitalWrite(ss, LOW);
  delayMicroseconds(5);
  digitalWrite(ss, HIGH);
  delayMicroseconds(40);

  // bus.strobe(CC1101_REG_RES);
  bus.strobe(CC1101_REG_RES | CC1101_WRITE_BURST);
};
void CC1101::flushRxBuff() {
  if (getState() != (STATE_IDLE || STATE_RXFIFO_OVERFLOW)) return;
  // if (getState() != (STATE_IDLE || STATE_RXFIFO_OVERFLOW)) setState();
  // if (getState() != (STATE_IDLE || STATE_RXFIFO_OVERFLOW)) waitForState();
  // bus.strobe(CC1101_REG_FRX);
  bus.strobe(CC1101_REG_FRX | CC1101_WRITE_BURST);
};
void CC1101::flushTxBuff() {
  if (getState() != (STATE_IDLE || STATE_TXFIFO_UNDERFLOW)) return;
  // if(getState() != (STATE_IDLE || STATE_TXFIFO_UNDERFLOW)) setState();
  // if(getState() != (STATE_IDLE || STATE_TXFIFO_UNDERFLOW)) waitForState();
  // bus.strobe(CC1101_REG_FTX);
  bus.strobe(CC1101_REG_FTX | CC1101_WRITE_BURST);
};
void CC1101::waitForState(State state) {
  while (getState() != state) { 
    Serial.printf("waiting for state: %d, current state is : %d", state, getState());
    Serial.println();
  };
};

byte CC1101::getState() {
  // return (strobe(CC1101_REG_NOP) >> 4) & 0b00111;
  return (bus.strobe(CC1101_REG_NOP) >> 4) & 0b00111;
};
bool CC1101::getChipInfo() {
  partnum = readStatus(CC1101_REG_PARTNUM);
  version = readStatus(CC1101_REG_VERSION);

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

void CC1101::setCRC(bool en) {
  writeRegField(CC1101_REG_PKTCTRL0, 2, 2, (uint8_t)en); /* CRC_EN */
  writeRegField(CC1101_REG_PKTCTRL1, 3, 3, (uint8_t)en); /* Autoflush */
};
void CC1101::setFEC(bool en) {
  if(isVariablePktLen) return;
  writeRegField(CC1101_REG_MDMCFG1, 7, 7, (uint8_t)en);
};
void CC1101::setAddr(byte addr) {
  writeRegField(CC1101_REG_PKTCTRL1, 1, 0, addr > 0 ? 1 : 0);
  writeReg(CC1101_REG_ADDR, addr);
};
void CC1101::setSync(CC1101_SyncMode syncMode, uint16_t syncWord, uint8_t preambleLen) {
  writeRegField(CC1101_REG_MDMCFG2, 2, 0, (uint8_t)syncMode);

  writeReg(CC1101_REG_SYNC0, (syncWord >> 0) & 0xff);
  writeReg(CC1101_REG_SYNC1, (syncWord >> 8) & 0xff);

  writeRegField(CC1101_REG_MDMCFG1, 6, 4, getPreambleIdx(preambleLen));
};
void CC1101::setAutoCalib(bool en) {
  writeRegField(CC1101_REG_MCSM0, 5, 4, (uint8_t)en);
};
void CC1101::setManchester(bool en) {
  if(mod != CC1101_MOD_MSK || mod != CC1101_MOD_4FSK)
    writeRegField(CC1101_REG_MDMCFG2, 3, 3, (uint8_t)en);
};
void CC1101::setAppendStatus(bool en) {
  writeRegField(CC1101_REG_PKTCTRL1, 2, 2, (uint8_t)en);
};
void CC1101::setDataWhitening(bool en) {
  writeRegField(CC1101_REG_PKTCTRL0, 6, 6, (uint8_t)en);
};
void CC1101::setPktLen(uint8_t len) {
  writeReg(CC1101_REG_PKTLEN, len);
};
void CC1101::setPktLenMode(bool isVariablePktLen) {
  writeRegField(CC1101_REG_PKTCTRL0, 1, 0, (uint8_t)isVariablePktLen); /* TODO: infinite */
};
void CC1101::setMod(CC1101_Modulation mod){
  writeRegField(CC1101_REG_MDMCFG2, 6, 4, (uint8_t)mod);
};
void CC1101::setFreq(float freq){
  uint32_t f = ((freq * 65536.0) / CC1101_CRYSTAL_FREQ); 

  writeReg(CC1101_REG_FREQ0, f & 0xff);
  writeReg(CC1101_REG_FREQ1, (f >> 8) & 0xff);
  writeReg(CC1101_REG_FREQ2, (f >> 16) & 0xff);

  /* TODO Deviation */ 
  // writeRegField(CC1101_REG_DEVIATN, CC1101_READ, CC1101_WRITE, 6, 4, devE);
  // writeRegField(CC1101_REG_DEVIATN, CC1101_READ, CC1101_WRITE, 2, 0, devM);
};
void CC1101::setDrate(float drate){
  uint32_t xosc = CC1101_CRYSTAL_FREQ * 1000;
  uint8_t e = log2((drate * (double)((uint32_t)1 << 20)) / xosc);
  uint32_t m = round(drate * ((double)((uint32_t)1 << (28 - e)) / xosc) - 256.0);

  if (m == 256) {
    m = 0;
    e++;
  }

  writeRegField(CC1101_REG_MDMCFG4, 3, 0, e);
  writeRegField(CC1101_REG_MDMCFG3, 7, 0, (uint8_t)m);
  // write(CC1101_REG_MDMCFG3 | CC1101_WRITE, (uint8_t)m);
};
void CC1101::setPwr(CC1101_FreqBand freqBand, CC1101_PowerMW pwr, const uint8_t pwrTable[][8]){
  // if(mod == CC1101_MOD_ASK_OOK) {
  //   uint8_t paTable[2] = {CC1101_WRITE, pwrTable[freqBand][pwr]};
  //   writeBurst(CC1101_REG_PATABLE | CC1101_WRITE_BURST, paTable, 2);
  //   writeRegField(CC1101_REG_FREND0, CC1101_READ, CC1101_WRITE, 2, 0, 1);
  // } else {
  //   write(CC1101_REG_PATABLE | CC1101_WRITE, pwrTable[freqBand][pwr]);
  //   writeRegField(CC1101_REG_FREND0, CC1101_READ, CC1101_WRITE, 2, 0, 0);
  // }
  writeRegField(CC1101_REG_FREND0, 2, 0, mod == CC1101_MOD_ASK_OOK ? 1 : 0);
  writeReg(CC1101_REG_PATABLE, pwrTable[freqBand][pwr]);
};
void CC1101::setState(State state) {
  Serial.printf("setState: %d", state);
  Serial.println();
  byte currentState = getState();
  if (currentState == state) return;
  switch (state) {
    case STATE_IDLE: 
      Serial.printf("setting state to idle, currentState: %d", currentState);
      Serial.println();
      bus.strobe(CC1101_REG_IDLE);
      break;
    case STATE_RX: 
      Serial.printf("setting state to rx, currentState: %d", currentState);
      Serial.println();
      if (currentState == STATE_RXFIFO_OVERFLOW) {
        Serial.println("rx fifo is overflowing, flushing");
        bus.strobe(CC1101_REG_FRX);
      };
      // if (currentState == (STATE_CALIB || STATE_SETTLING)) setState(); 
      // strobe(CC1101_REG_RX);
      if (currentState != STATE_CALIB || currentState != STATE_SETTLING) {
        Serial.println("setting to rx state");
        bus.strobe(CC1101_REG_RX);
      } 
      break;
    case STATE_TX: 
      Serial.printf("setting state to tx, currentState: %d", currentState);
      Serial.println();
      if (currentState == STATE_TXFIFO_UNDERFLOW) {
        Serial.println("tx fifo is underflowing, flushing");
        bus.strobe(CC1101_REG_FTX);
      } 
      // if (currentState == (STATE_CALIB || STATE_SETTLING)) setState(); 
      // strobe(CC1101_REG_TX);
      if (currentState != STATE_CALIB || currentState != STATE_SETTLING) {
        Serial.println("setting to tx state");
        bus.strobe(CC1101_REG_TX);
      };
      break;
  }
  waitForState(state);
  // while (getState() != state);
};
// void CC1101::setIdleState() {
//   if (getState() == STATE_IDLE) return;
//   strobe(CC1101_REG_IDLE);
//   while (getState() != STATE_IDLE);
// };
// void CC1101::setRxState() {
//     byte state = getState();
//     if (state == STATE_RX) return; 
//     else if (state == STATE_RXFIFO_OVERFLOW) strobe(CC1101_REG_FRX);
//     else if (state != (STATE_CALIB || STATE_SETTLING)) strobe(CC1101_REG_RX);
//     while (getState() != STATE_RX);
// };
// void CC1101::setTxState() {
//     byte state = getState();
//     if (state == STATE_TX) return;
//     else if (state == STATE_TXFIFO_UNDERFLOW) strobe(CC1101_REG_FTX);
//     else if (state != (STATE_CALIB || STATE_SETTLING)) strobe(CC1101_REG_TX);
//     while (getState() != STATE_TX);
// };
void CC1101::setTwoWay() {
    writeRegField(CC1101_REG_MCSM1, 5, 4, 0); // Disabl CCA
    writeRegField(CC1101_REG_MCSM1, 1, 0, 3); // Set TXOFF to RX
    writeRegField(CC1101_REG_MCSM1, 3, 2, 2); // Set RXOFF to TX
};

bool CC1101::enoughRxBytes(uint8_t len) {
  if (readRegField(CC1101_REG_RXBYTES, 6, 0) < len)
    return false;
  return true;
};
void CC1101::waitForRxBytes(uint8_t len) {
  // if (isVariablePktLen) {
  //   while (!enoughRxBytes(1));
  //   len = read(CC1101_REG_FIFO | CC1101_READ);
  // }
  // if (addr > 0) len++;
  while (!enoughRxBytes(len));
};
bool CC1101::waitForRxBytes(uint8_t len, size_t timeoutMs) {
  // if (isVariablePktLen) {
  //   while (!enoughRxBytes(1));
  //   len = read(CC1101_REG_FIFO | CC1101_READ);
  // }
  // if (addr > 0) len++;
  uint32_t timer = millis();
  while(!enoughRxBytes(len)) {
    if((timer + timeoutMs) < millis()) {
      setState(); // timeout
      return false;
    }
  }
  setState();
  return true;
};
// bool CC1101::readRxFifo(uint8_t *buff, uint8_t len) {
//   Serial.print("bytes in rxfifo: ");
//   Serial.println(readField(CC1101_REG_RXBYTES | CC1101_READ_BURST, 6, 0));
//   if (!readField(CC1101_REG_RXBYTES | CC1101_READ_BURST, 6, 0)) return false;
//   if(isVariablePktLen) (void)read(CC1101_REG_FIFO | CC1101_READ);
//   if(addr) (void)read(CC1101_REG_FIFO | CC1101_READ);
//   readBurst(CC1101_REG_FIFO | CC1101_READ_BURST, buff, len);
//   if(isAppendStatus) {
//     uint8_t r = read(CC1101_REG_FIFO | CC1101_READ);
//     // if(r >= 128) rssi = ((rssi - 256) / 2) - CC1101_RSSI_OFFSET;
//     // else rssi = (rssi / 2) - CC1101_RSSI_OFFSET;
//     rssi = ((r >= 128 ? (r - 256) : r) / 2) - CC1101_RSSI_OFFSET;  
//     lqi = read(CC1101_REG_FIFO | CC1101_READ) & 0x7f;
//     if(!(r >> 7) & 1) return false; // CRC Mismatch
//   }
//   Serial.print("rxbytes still has: ");
//   Serial.println(readField(CC1101_REG_RXBYTES | CC1101_READ_BURST, 6, 0));
//   return true;
// };
// void CC1101::writeTxFifo(uint8_t *buff, uint8_t len) {
//   if(isVariablePktLen) write(CC1101_REG_FIFO | CC1101_WRITE, len);
//   if(addr) write(CC1101_REG_FIFO | CC1101_WRITE, addr);
//   writeBurst(CC1101_REG_FIFO | CC1101_WRITE_BURST, buff, len);
// };
bool CC1101::readRxFifo(uint8_t *buff) {
  Serial.print("bytes in rxfifo: ");
  Serial.println(readRegField(CC1101_REG_RXBYTES, 6, 0));
  if (!readRegField(CC1101_REG_RXBYTES, 6, 0)) return false;
  uint8_t len = isVariablePktLen ? readReg(CC1101_REG_FIFO) : pktLen;
  // if(isVariablePktLen) (void)read(CC1101_REG_FIFO | CC1101_READ);
  if(addr) (void)readReg(CC1101_REG_FIFO);
  readRegBurst(CC1101_REG_FIFO, buff, len);
  if(isAppendStatus) {
    uint8_t r = readReg(CC1101_REG_FIFO);
    // if(r >= 128) rssi = ((rssi - 256) / 2) - CC1101_RSSI_OFFSET;
    // else rssi = (rssi / 2) - CC1101_RSSI_OFFSET;
    rssi = ((r >= 128 ? (r - 256) : r) / 2) - CC1101_RSSI_OFFSET;  
    lqi = readReg(CC1101_REG_FIFO) & 0x7f;
    if(!(r >> 7) & 1) return false; // CRC Mismatch
  }
  Serial.print("rxbytes still has: ");
  Serial.println(readRegField(CC1101_REG_RXBYTES, 6, 0));
  return true;
};
void CC1101::writeTxFifo(uint8_t *buff) {
  uint8_t len = isVariablePktLen ? sizeof(buff) : pktLen;
  if(isVariablePktLen) writeReg(CC1101_REG_FIFO, len);
  if(addr) writeReg(CC1101_REG_FIFO, addr);
  writeRegBurst(CC1101_REG_FIFO, buff, len);
};

uint8_t CC1101::strobe(byte addr) {
  return bus.strobe(addr);
  // return bus.strobe(addr | CC1101_WRITE_BURST);
};
uint8_t CC1101::readReg(byte addr) {
  return bus.read(addr | CC1101_READ);
};
uint8_t CC1101::readStatus(byte addr) {
  return bus.read(addr | CC1101_READ_BURST);
};
uint8_t CC1101::readRegField(byte addr, byte lo, byte hi) {
  return bus.readField(addr | CC1101_READ_BURST, lo, hi);
};
void CC1101::readRegBurst(byte addr, uint8_t *buff, size_t len) {
  bus.readBurst(addr | CC1101_READ_BURST, buff, len);
};
void CC1101::writeReg(byte addr, uint8_t val) {
  bus.write(addr | CC1101_WRITE, val);
};
void CC1101::writeRegField(byte addr, byte lo, byte hi, uint8_t val) {
  bus.writeField(addr, CC1101_READ, CC1101_WRITE, hi, lo, val);
};
void CC1101::writeRegBurst(byte addr, uint8_t *buff, size_t len) {
  bus.writeBurst(addr | CC1101_WRITE_BURST, buff, len);
};
