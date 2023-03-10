/*
  Скетч к проекту "Часы из часов"
  Страница проекта (схемы, описания): https://alexgyver.ru/disp-clock/
  Исходники на GitHub: https://github.com/AlexGyver/SegmentMonitor
  Нравится, как написан и закомментирован код? Поддержи автора! https://alexgyver.ru/support_alex/
  Автор: AlexGyver Technologies, 2022
  http://AlexGyver.ru/
*/

// - проведена оптимизация бегущей строки и вообще расчётов
// - оптимизировано деление и остаток на 6

// ВНИМАНИЕ! Это "playground", а не полноценный проект. Реализован
// вывод как на графический дисплей, подключен GyverGFX, можно рисовать
// и выводить битмапы. Также сделан вывод часов (шрифт в комплекте)
// САМИ ЧАСЫ Я НЕ ДЕЛАЛ, ЭТО ПРОСТО ДЕМОНСТРАЦИЯ!
// если вам нужно - пишите, я помогу

#include "MAX7219.h"
#include "dig3.h"   // шрифт для часов. Можно переключить между dig1, dig2, dig3

#define MAX_CS D8  // пин CS
#define DW 6      // кол-во МИКРОСХЕМ по горизонтали
#define DH 3      // кол-во МИКРОСХЕМ по вертикали

#define DWW (DW*4)  // расчёт кол-ва индикаторов гориз.
#define DHH (DH*2)  // расчёт кол-ва индикаторов верт.

// указать пин CS!
MaxDisp<MAX_CS, DW, DH> disp;

// битмап логотипа
// сделан в ImageProcessor https://github.com/AlexGyver/imageProcessor
// с настройками 8pix/byte, BW
const uint8_t wrench_43x36[] PROGMEM = {
  0x00, 0x1F, 0xF8, 0x00, 0x3F, 0x00,
  0x00, 0x7F, 0xE0, 0x00, 0x7F, 0x80,
  0x01, 0xFF, 0x00, 0x00, 0xFE, 0x00,
  0x03, 0xFC, 0x00, 0x01, 0xF8, 0x00,
  0x07, 0xF8, 0x00, 0x01, 0xF8, 0x00,
  0x07, 0xF8, 0x00, 0x01, 0xF8, 0x00,
  0x0F, 0xF8, 0x00, 0x03, 0xFC, 0x20,
  0x0F, 0xFC, 0x00, 0x03, 0x8C, 0xE0,
  0x0F, 0xFE, 0x00, 0x07, 0x0F, 0xE0,
  0x3F, 0xFE, 0x00, 0x07, 0x0F, 0xE0,
  0x7F, 0x7F, 0x00, 0x0F, 0x1F, 0xC0,
  0x7E, 0x1F, 0x80, 0x1F, 0xFF, 0x80,
  0xFC, 0x0F, 0xC0, 0x3F, 0xFC, 0x00,
  0x7C, 0x07, 0xE0, 0x7F, 0xE0, 0x00,
  0x38, 0x03, 0xF0, 0xFF, 0x80, 0x00,
  0x00, 0x01, 0xF8, 0xFF, 0x00, 0x00,
  0x00, 0x00, 0xFF, 0x7E, 0x00, 0x00,
  0x00, 0x00, 0x7F, 0xDC, 0x00, 0x00,
  0x00, 0x00, 0x3F, 0xE8, 0x00, 0x00,
  0x00, 0x00, 0x1F, 0xF0, 0x00, 0x00,
  0x00, 0x00, 0x5F, 0xF8, 0x00, 0x00,
  0x00, 0x00, 0xFF, 0xFC, 0x00, 0x00,
  0x00, 0x01, 0xEF, 0xFE, 0x00, 0x00,
  0x00, 0x03, 0xF7, 0xFF, 0x00, 0x00,
  0x00, 0x07, 0xFB, 0xFF, 0x80, 0x00,
  0x00, 0x0F, 0xFD, 0xFF, 0xC0, 0x00,
  0x00, 0x1F, 0xF8, 0xFF, 0xE0, 0x00,
  0x00, 0x3F, 0xF0, 0x7F, 0xF0, 0x00,
  0x00, 0xFF, 0xE0, 0x3F, 0xF8, 0x00,
  0x01, 0xFF, 0xC0, 0x1F, 0xFC, 0x00,
  0x01, 0xFF, 0xC0, 0x0F, 0xFE, 0x00,
  0x03, 0x9F, 0x80, 0x07, 0xFE, 0x00,
  0x03, 0x0F, 0x00, 0x03, 0xFE, 0x00,
  0x03, 0x8E, 0x00, 0x01, 0xFC, 0x00,
  0x01, 0xDC, 0x00, 0x00, 0xF8, 0x00,
  0x00, 0xF8, 0x00, 0x00, 0x70, 0x00,
};

void setup() {
  randomSeed(analogRead(0));
  disp.begin();
  disp.setBright(10);
  disp.textDisplayMode(GFX_ADD);
}

void loop() {
  // выбери эффект, который будет воспроизводиться
  //bigBall();
  //net();
  //party();
  clock_e();
  //running();
  //segTest1();
  //segTest2();
}

void segTest2() {
  uint8_t b = 1;
  for (int i = 0; i < 8; i++) {
    disp.fill(b);
    disp.update();
    delay(300);
    b <<= 1;
  }
}

void segTest1() {
  for (int y = 0; y < DHH; y++) {
    for (int x = 0; x < DWW; x++) {
      disp.clear();
      disp.setByte(x, y, 0xff);
      disp.update();
      delay(100);
    }
  }
}

void running() {
  // бегущая
  disp.setScale(3);
  char* txt = "Лайк, подписка, колокольчик! =) AlexGyver Show";
  int w = strlen(txt) * 5 * 3 + disp.W();
  for (int x = disp.W(); x > -w; x--) {
    disp.clear();
    disp.setCursor(x, 6);
    disp.print(txt);
    disp.update();
    delay(10);
  }
}


void drawDigit(byte dig, int x) {
#ifdef __AVR__
  disp.drawBitmap(x, 0, (const uint8_t*)pgm_read_word(&(digs[dig])), d_width, 36, 0);
#else
  disp.drawBitmap(x, 0, (const uint8_t*)pgm_read_dword(&(digs[dig])), d_width, 36, 0);
#endif
}

void drawClock(byte h, byte m, bool dots) {
  disp.clear();
  if (h > 9) drawDigit(h / 10, 0);
  drawDigit(h % 10, d_width + 2);
  if (dots) {
    disp.setByte(11, 2, 0b0011101);
    disp.setByte(11, 4, 0b1100011);
  }
  drawDigit(m / 10, 95 - d_width * 2 - 3);
  drawDigit(m % 10, 95 - d_width);
  disp.update();
}

void clock_e() {
  // часы
  static int h = 22, m = 30;
  static volatile bool d;

  drawClock(h, m, d);
  d = !d;
  if (++m >= 60) m = 0, h++;
  if (h >= 24) h = 0;
  delay(500);
}

void party() {
  // вечеринка gyver
  disp.setScale(3);
  delay(700);
  disp.clear();
  disp.setCursor(14, 6);
  disp.print("Alex");
  disp.update();

  delay(700);
  disp.clear();
  disp.setCursor(4, 6);
  disp.print("Gyver");
  disp.update();

  delay(700);
  disp.clear();
  disp.drawBitmap(26, 0, wrench_43x36, 43, 36);
  disp.update();
}

void net() {
  const byte radius = 6;
  const byte amount = 3;
  static bool start = false;
  static int x[amount], y[amount];
  static int velX[amount], velY[amount];
  if (!start) {
    start = 1;
    for (byte i = 0; i < amount; i++) {
      x[i] = random(radius, (disp.W() - radius) * 10);
      y[i] = random(radius, (disp.H() - radius) * 10);
      velX[i] = random(10, 20);
      velY[i] = random(10, 20);
    }
  }
  disp.clear();
  for (byte i = 0; i < amount; i++) {
    x[i] += velX[i];
    y[i] += velY[i];
    if (x[i] >= (disp.W() - 1 - radius) * 10 || x[i] < radius * 10) velX[i] = -velX[i];
    if (y[i] >= (disp.H() - 1 - radius) * 10 || y[i] < radius * 10) velY[i] = -velY[i];
    disp.circle(x[i] / 10, y[i] / 10, radius);
  }

  for (int i = 0; i < amount; i++) {
    for (int j = 0; j < amount; j++) {
      disp.line(x[i] / 10, y[i] / 10, x[j] / 10, y[j] / 10);
    }
  }
  disp.update();
}

void bigBall() {
  disp.clear();
  byte radius = 6;
  static int x = (disp.W() / 2) * 10, y = (disp.H() / 2) * 10;
  static int velX = random(3, 6), velY = random(3, 6);
  x += velX;
  y += velY;
  if (x >= (disp.W() - radius) * 10 || x < radius * 10) velX = -velX;
  if (y >= (disp.H() - radius) * 10 || y < radius * 10) velY = -velY;

  disp.circle(x / 10, y / 10, radius, GFX_FILL);
  disp.update();
}
