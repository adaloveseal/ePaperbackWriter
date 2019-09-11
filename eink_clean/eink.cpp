#include <SPI.h>
#include <stdarg.h>
#include <stdlib.h>
#include "eink.h"
#include "event.h"

struct eink_display display;
uint8_t pending_updates = 0;

void eink_init(uint16_t cols, uint16_t rows) {
	pinMode(CS_PIN, OUTPUT);
	pinMode(RST_PIN, OUTPUT);
	pinMode(DC_PIN, OUTPUT);
	pinMode(BUSY_PIN, INPUT);
	digitalWrite(CS_PIN, LOW);
	Serial.println("Starting SPI");
	SPI.begin();
	SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
	digitalWrite(RST_PIN, LOW);
	delay(200);
	digitalWrite(RST_PIN, HIGH);
	delay(200);
	eink_exec(POWER_SETTING, 5, 0x03, 0x00, 0x2B, 0x2B, 0xFF);
	eink_exec(BOOSTER_SOFT_START, 3, 0x17, 0x17, 0x17);
	eink_exec(POWER_ON, 0);
	// Dirty polling
	while (digitalRead(BUSY_PIN) == LOW)
		delay(50);
	eink_exec(PANEL_SETTING, 2, 0xBF, 0x0B);
	eink_exec(PLL_CONTROL, 1, 0x3C);
	eink_set_luts(false);  // Start in slow mode to get a good erase
	display.cols = cols;
	display.rows = rows;
	display.text = NULL;
	eink_clear();
	eink_exec(DISPLAY_REFRESH, 0);
	// More dirty polling
	while (digitalRead(BUSY_PIN) == LOW)
		delay(50);
	eink_set_luts(true);  // Switch to fast for real operation
}

void eink_set_luts(uint8_t quick) {
	Serial.println("Setting LUTs");
	eink_execv(LUT_FOR_VCOM, 44, quick ? lut_vcom0_quick : lut_vcom0);
	eink_execv(LUT_WHITE_TO_WHITE, 42, quick ? lut_ww_quick : lut_ww);
	eink_execv(LUT_BLACK_TO_WHITE, 42, quick ? lut_bw_quick : lut_bw);
	eink_execv(LUT_WHITE_TO_BLACK, 42, quick ? lut_wb_quick : lut_wb);
	eink_execv(LUT_BLACK_TO_BLACK, 42, quick ? lut_bb_quick : lut_bb);
}

void eink_set_window(uint16_t x, uint16_t y, uint8_t *window, uint16_t cols, uint16_t rows) {
	if ((x + cols > display.cols) || (y + rows > display.rows)) {
		Serial.println("/!\\ Wrong window size or position");
		return;
	}
	// TODO: check async works
	pending_updates++;
	Serial.println(ev_register_interrupt(
		[] {
		  return digitalRead(BUSY_PIN);
		},
		[x, y, window, cols, rows] {
			eink_exec(PARTIAL_IN, 0);
			eink_exec(PARTIAL_WINDOW, 9, x >> 8, x & 0xF8,
				((x & 0xF8) + cols  - 1) >> 8, ((x & 0xF8) + cols  - 1) | 0x07,
				y >> 8, y & 0xFF, (y + rows - 1) >> 8, (y + rows - 1) & 0xFF, 0x01);
			eink_execv(DATA_START_TRANSMISSION_1, cols / 8 * rows, window);
			eink_execv(DATA_START_TRANSMISSION_2, cols / 8 * rows, window);
			eink_exec(PARTIAL_OUT, 0);
			if (!--pending_updates)
				eink_exec(DISPLAY_REFRESH, 0);
		}
	));
}

void eink_clear() {
	uint16_t i;
	eink_exec(RESOLUTION_SETTING, 4, 
		display.cols >> 8, display.cols & 0xFF,
		display.rows >> 8, display.rows & 0xFF);
	eink_exec(DATA_START_TRANSMISSION_1, 0);
	for (i = 0; i < display.cols / 8 * display.rows; i++)
		SPI.transfer(0xFF);
	eink_exec(DATA_START_TRANSMISSION_2, 0);
	for (i = 0; i < display.cols / 8 * display.rows; i++)
		SPI.transfer(0xFF);
}

void eink_exec(uint8_t opcode, uint8_t dcount, ...) {
	digitalWrite(DC_PIN, LOW);  // Command mode
	SPI.transfer(opcode);
	digitalWrite(DC_PIN, HIGH); // Now data
	if (dcount == 0) return;
	va_list argv;
	va_start(argv, dcount);
	for (uint8_t i = 0; i < dcount; i++) {
		SPI.transfer((uint8_t)va_arg(argv, uint16_t));
	}
	va_end(argv);
}

void eink_execv(uint8_t opcode, uint8_t dcount, uint8_t *data) {
	digitalWrite(DC_PIN, LOW);
	SPI.transfer(opcode);
	digitalWrite(DC_PIN, HIGH);
	for (uint8_t i = 0; i < dcount; i++) {
		SPI.transfer(data[i]);
	}
}
