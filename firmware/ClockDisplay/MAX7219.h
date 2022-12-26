#pragma once

// библиотека для управления 7-сегментным графическим дисплеем
// берёт движок графики из GyverGFX

#include <SPI.h>
#include "GyverGFX.h"

// тут можно настроить частоту SPI
SPISettings MAX_SPI_SETT(1000000, MSBFIRST, SPI_MODE0);

template <byte CS, byte WW, byte HH>
struct MaxDisp : public GyverGFX {
  MaxDisp() : GyverGFX(WW * 4 * 4, HH * 2 * 6) {}

  void begin() {
    SPI.begin();
    pinMode(CS, OUTPUT);

    sendCMD(0xF, 0x00);  // отключить тест
    sendCMD(0x9, 0x00);  // откл декодирование
    sendCMD(0xA, 0x00);  // яркость мин
    sendCMD(0xB, 0x07);  // отображаем всё
    sendCMD(0xC, 0x01);  // включить
  }

  void setBright(uint8_t value) {
    sendCMD(0x0a, value);        // яркость 0-15
  }

  void beginData() {
    SPI.beginTransaction(MAX_SPI_SETT);
    digitalWrite(CS, 0);
  }

  void endData() {
    digitalWrite(CS, 1);
    SPI.endTransaction();
  }

  void dot(int x, int y, uint8_t fill = 1) {
    // выбираем индикатор
    byte& b = getByte(x >> 2, y / 6);   // x/4
    
    // x и y внутри индикатора (суб-матрица 6х4)
    x = x & 3;  // x%4
    y = y % 6;
    
    // сумма со сдвигом, уникальный адрес каждого пикселя
    byte z = x | (y << 2);
    switch (z) {
      case 0x9:  bitWrite(b, 0, fill); break;
      case 0x4:  bitWrite(b, 1, fill); break;
      case 0xC:  bitWrite(b, 2, fill); break;
      case 0x11: bitWrite(b, 3, fill); break;
      case 0xE:  bitWrite(b, 4, fill); break;
      case 0x6:  bitWrite(b, 5, fill); break;
      case 0x1:  bitWrite(b, 6, fill); break;
      //case 0x13: bitWrite(b, 7, fill); break;
    }
  }

  void setByte(int x, int y, byte b) {
    getByte(x, y) = b;
  }
  byte& getByte(int x, int y) {
    //return buf[(y >> 1) * (WW * 8) + (x >> 2) * 8 + (x % 4) + (y % 2) * 4];
    return buf[(y >> 1) * (WW << 3) + ((x >> 2) << 3) + (x & 3) + ((y & 1) << 2)];
  }

  void update() {
    for (int i = 0; i < 8; i++) {
      beginData();
      for (int j = 0; j < WW * HH; j++) sendData(i + 1, buf[((WW * HH - 1 - j) << 3) + i]);
      endData();
    }
  }

  void clear() {
    //fill(0);
    memset(buf, 0, WW * HH * 8);
  }

  void fill(byte b = 0) {
    for (int i = 0; i < 8; i++) {
      beginData();
      for (int j = 0; j < WW * HH; j++) sendData(i + 1, b);
      endData();
    }
  }

  void sendByte(uint8_t address, uint8_t value) {
    beginData();
    sendData(address + 1, value);
    endData();
  }

  void sendCMD(uint8_t address, uint8_t value) {
    beginData();
    for (int i = 0; i < WW * HH; i++) sendData(address, value);
    endData();
  }

  void sendData(uint8_t address, uint8_t value) {
    SPI.transfer(address);
    SPI.transfer(value);
  }

  byte buf[WW * HH * 8];
};
