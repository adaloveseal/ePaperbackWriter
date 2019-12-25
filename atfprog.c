#define ATF_PE 2
#define ATF_PV 3
#define ATF_RS 8
#define ATF_CE 9
#define ATF_SC 10
#define ATF_SI 11
#define ATF_SB 13
#define ATF_SO 14
#define ATF_VV 24

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
}

void atf_end() {
	digitalWrite(ATF_PE, LOW);
	delay(5);
	digitalWrite(ATF_SB, LOW);
	digitalWrite(ATF_SC, LOW);
	delay(5);
	digitalWrite(ATF_VV, LOW);
}

bool atf_clock(bool data) {
	bool res = digitalRead(ATF_SO);
	digitalWrite(ATF_SC, LOW);
	delayMicroseconds(10);
	digitalWrite(ATF_SI, data);
	delayMicroseconds(10);
	digitalWrite(ATF_SC, HIGH);
}

void atf_read_row(uint8_t addr, uint8_t *buffer) {
	uint8_t i;
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
	uint8_t i;
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
	uint8_t i;
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

