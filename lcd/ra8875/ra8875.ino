#include <Adafruit_RA8875.h>

#include <SPI.h>

#define CS_PIN   21
#define RST_PIN  20

Adafruit_RA8875 *lcd;

uint16_t rgb2b16(uint8_t r, uint8_t g, uint8_t b) {
  return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
}

uint16_t hex2b16(uint32_t hex) {
  return rgb2b16(hex >> 16, hex >> 8 & 255, hex & 255);
}

void setup() {
  Serial.begin(9600);
  uint16_t bg = hex2b16(0x483201), fg = hex2b16(0xf5b526);
  lcd = new Adafruit_RA8875(CS_PIN, RST_PIN);
  lcd->begin(RA8875_480x272);
  lcd->displayOn(true);
  lcd->GPIOX(true);
  lcd->PWM1config(true, RA8875_PWM_CLK_DIV1024);
  lcd->PWM1out(255);
  lcd->fillScreen(bg);
  lcd->textMode();
  lcd->cursorBlink(32);
  lcd->textSetCursor(10, 10);
  lcd->textColor(fg, bg);
}

char buffer[2] = "x";

void loop() {
  if (!Serial.available()) return;
  buffer[0] = Serial.read();
  lcd->textWrite(buffer);
  Serial.write(buffer);
}
