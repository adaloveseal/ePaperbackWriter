#include "eink.h"

struct eink_display *display;

void setup() {
  Serial.begin(9600);
  while(!Serial)
    delay(50);
  delay(5000);
  display = eink_init(400, 300);
  eink_set_pixel(display, 8, 5, 1);
  eink_sync();
}

void loop() {
  // put your main code here, to run repeatedly:

}
