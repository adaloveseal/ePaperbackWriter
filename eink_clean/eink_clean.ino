#include "eink.h"
#include "event.h"

uint16_t cursor_x = 0, cursor_y = 0;

void put_symbol() {
	if (!ev_interrupt_available()) return;
	char symbol = Serial.read();
	Serial.print((char)symbol);
	eink_putchar(cursor_x, cursor_y, symbol);
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
