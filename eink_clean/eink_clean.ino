#include "eink.h"
#include "event.h"

uint8_t window[] = {
	0b10101010,
	255,
	255,
	0,
	0,
	255,
	255,
	0,
	0,
	0b01010101,
};

uint16_t cursor_x = 0, cursor_y = 0;

void put_symbol() {
	if (!ev_interrupt_available()) return;
	char symbol = Serial.read();
	Serial.print((char)symbol);
	eink_set_window(cursor_x, cursor_y, window, 8, 10);
	cursor_x += 8;
	if (cursor_x > 392) {
		cursor_x = 0;
		cursor_y += 12;
	}
}

uint8_t serial_available() {
	return Serial.available();
}

void setup() {
	Serial.begin(9600);
	delay(5000);
	eink_init(400, 300);
	ev_register_interrupt(serial_available, NULL, put_symbol, NULL, true);
	Serial.println("Done");
}

void loop() {
	ev_run_interrupts();
}
