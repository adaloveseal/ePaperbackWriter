#include "eink.h"
#include "event.h"
char window[] = {
    0b10101010, 0b10101010,
    255, 255,
    255, 255,
    0, 0,
    0, 0,
    255, 255,
    255, 255,
    0, 0,
    0, 0,
    0b01010101, 0b01010101
  };
void setup() {
	Serial.begin(9600);
	delay(5000);
	eink_init(400, 300);
	
  
	eink_set_window(16, 25, window, 16, 10);
	//eink_set_window(64, 20, window, 16, 10);
	//eink_set_window(128, 40, window, 16, 10);
  Serial.println("Done");
}

void loop() {
  ev_run_interrupts();
  delay(1000);
  ev_run_interrupts();
  delay(1000);
  ev_run_interrupts();
  delay(1000);
  eink_set_window(64, 20, window, 16, 10);
}
