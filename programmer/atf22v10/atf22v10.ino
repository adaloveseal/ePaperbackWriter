#define ATF_PE 21
#define ATF_PV 20
#define ATF_RS 2
#define ATF_CE 3
#define ATF_SC 4
#define ATF_SI 5
#define ATF_SB 6
#define ATF_SO 7
#define ATF_VV 8

#include "decoder.jed.hpp"

void atf_init() {
	pinMode(ATF_PE, OUTPUT);
	pinMode(ATF_PV, OUTPUT);
	pinMode(ATF_RS, OUTPUT);
	pinMode(ATF_CE, OUTPUT);
	pinMode(ATF_SC, OUTPUT);
	pinMode(ATF_SI, OUTPUT);
	pinMode(ATF_SB, OUTPUT);
	pinMode(ATF_SO, INPUT);
	pinMode(ATF_VV, OUTPUT);
	digitalWrite(ATF_PE, LOW);
	digitalWrite(ATF_PV, LOW);
	digitalWrite(ATF_RS, LOW);
	digitalWrite(ATF_CE, LOW);
	digitalWrite(ATF_SC, LOW);
	digitalWrite(ATF_SI, LOW);
	digitalWrite(ATF_SB, LOW);
	digitalWrite(ATF_VV, LOW);
}

void atf_begin() {
	digitalWrite(ATF_VV, HIGH);
	delay(5);
	digitalWrite(ATF_SB, HIGH);
	digitalWrite(ATF_SC, HIGH);
	delay(5);
	digitalWrite(ATF_PE, HIGH);
	delay(10);
}

void atf_end() {
	digitalWrite(ATF_PE, LOW);
	delay(5);
	digitalWrite(ATF_SB, LOW);
	digitalWrite(ATF_SC, LOW);
	delay(5);
	digitalWrite(ATF_VV, LOW);
	delay(10);
}

uint8_t atf_clock(uint8_t data) {
	uint8_t res = digitalRead(ATF_SO);
	delayMicroseconds(10);
	digitalWrite(ATF_SC, LOW);
	delayMicroseconds(10);
	digitalWrite(ATF_SI, data ? HIGH : LOW);
	delayMicroseconds(10);
	digitalWrite(ATF_SC, HIGH);
	delayMicroseconds(10);
	return res;
}

void atf_read_row(uint8_t addr, uint8_t *buffer) {
	int16_t i;
	digitalWrite(ATF_PV, LOW);
	digitalWrite(ATF_CE, LOW);
	digitalWrite(ATF_RS, LOW);
	for (i = 0; i < 132; i++)
		atf_clock(false);
	for (i = 5; i >= 0; i--)
		atf_clock((addr >> i) & 1);
	digitalWrite(ATF_SB, LOW);
	delayMicroseconds(10);
	digitalWrite(ATF_SB, HIGH);
	for (i = 0; i < 132; i++)
		buffer[i] = atf_clock(false);
}

void atf_read_olmc(uint8_t *buffer) {
	int16_t i;
	digitalWrite(ATF_PV, LOW);
	digitalWrite(ATF_CE, LOW);
	digitalWrite(ATF_RS, HIGH);
	for (i = 0; i < 20; i++)
		atf_clock(false);
	digitalWrite(ATF_SB, LOW);
	delayMicroseconds(10);
	digitalWrite(ATF_SB, HIGH);
	for (i = 0; i < 20; i++)
		buffer[i] = atf_clock(false);
}

void atf_erase() {
	digitalWrite(ATF_PV, HIGH);
	digitalWrite(ATF_CE, HIGH);
	digitalWrite(ATF_SB, LOW);
	delay(10);
	digitalWrite(ATF_SB, HIGH);
	delay(10);
}

void atf_write_row(uint8_t addr, uint8_t *buffer) {
	int16_t i;
	digitalWrite(ATF_PV, HIGH);
	digitalWrite(ATF_CE, LOW);
	digitalWrite(ATF_RS, LOW);
	for (i = 0; i < 132; i++)
		atf_clock(buffer[i]);
	for (i = 5; i >= 0; i--)
		atf_clock((addr >> i) & 1);
	digitalWrite(ATF_SB, LOW);
	delay(5);
	digitalWrite(ATF_SB, HIGH);
	delay(10);
}

void atf_write_olmc(uint8_t *buffer) {
	uint8_t i;
	digitalWrite(ATF_PV, HIGH);
	digitalWrite(ATF_CE, LOW);
	digitalWrite(ATF_RS, HIGH);
	for (i = 0; i < 20; i++)
		atf_clock(buffer[i]);
	digitalWrite(ATF_SB, LOW);
	delay(5);
	digitalWrite(ATF_SB, HIGH);
	delay(10);
}

void setup() {
	Serial.begin(9600);
	delay(1000);
	uint8_t *buffer = malloc(200);
	uint8_t zeros = calloc(132, 1);
	int16_t i, j;
	Serial.println("Start programming...");
	atf_init();
	atf_begin();
	/*
	atf_erase();
	for (i = 0; i < 44; i++)
		atf_write_row(i, atf_main[i]);
	// Disable power-down
	atf_write_row(59, zeros);
	atf_write_olmc(atf_olmc);
	*/
	Serial.println("Programming done. Readback:");
	for (i = 0; i < 44; i++) {
		if (i < 10) Serial.print("0");
		Serial.print(i);
		Serial.print(": ");
		atf_read_row(i, buffer);
		for (j = 0; j < 132; j++)
			Serial.write(buffer[i] ? '1' : '0');
		Serial.println("");
	}
	Serial.print("58: ");
	atf_read_row(58, buffer);
	for (j = 0; j < 132; j++)
		Serial.write(buffer[i] ? '1' : '0');
	Serial.println("");
	Serial.print("OLMC: ");
	atf_read_olmc(buffer);
	for (i = 0; i < 20; i++)
		Serial.write(buffer[i] ? '1' : '0');
	Serial.println("");
	atf_end();
	free(buffer);
}

void loop() {}
