#include <SPI.h>
#include "epd4in2.h"
#include "epdpaint.h"

#define COLORED     0
#define UNCOLORED   1

int i = 0, j = 0;
unsigned char image[1400];
Paint paint(image, 400, 28);    //width should be the multiple of 8 
Epd epd;

void setup() {
  Serial.begin(9600);
  epd.Init();
  epd.ClearFrame();
  paint.Clear(UNCOLORED);
  epd.DisplayFrame();
  epd.SetLut(true); // Switch to quick mode
}

void loop() {
  if (!Serial.available()) return;
  char c = Serial.read();
  if (j >= 391) {
    paint.DrawCharAt(j, 0, ' ', &Font24, COLORED);
    epd.SetPartialWindow(paint.GetImage(), 0, i, paint.GetWidth(), paint.GetHeight());
    epd.DisplayFrame();
    paint.Clear(UNCOLORED);
    i += 28;
    j = 0;
  }
  if (i >= 280) {
    epd.SetLut(false); // Slow mode, thorough cleanup
    paint.Clear(UNCOLORED);
    epd.ClearFrame();
    epd.DisplayFrame();
    epd.SetLut(true);
    i = 0;
    j = 0;
  }
  Serial.write(c);
  paint.DrawCharAt(j, 0, c, &Font24, COLORED);
  epd.SetPartialWindow(paint.GetImage(), 0, i, paint.GetWidth(), paint.GetHeight());
  epd.DisplayFrame();
  j += 17;
}
