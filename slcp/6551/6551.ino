#define DATA PORTF
#define ACIA PORTK
#define PHI2 0b1000
#define RXR  0b100
#define TXR  0b000
#define STR  0b101
#define CMDR 0b110
#define CMDW 0b010
#define CTRR 0b111
#define CTRW 0b011
#define IRQB 37

#define dm delayMicroseconds
#define tick() ({ delayMicroseconds(1); ACIA ^= PHI2; delayMicroseconds(1); ACIA ^= PHI2; })

void setup() {
	DDRF = 0xFF;
	DDRK = 0xFF;
	pinMode(IRQB, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(IRQB), newdata, LOW);
	delay(1000);
	Serial.begin(9600);
	Serial2.begin(115200);
	// Set baud rate to external clock (/16), 8 bits, 1 stop bit, no parity (14400, 8N1)
	ACIA = 1; // RESET
	tick();
	ACIA = CTRW;
	DATA = 0b00010000;
	tick();
	ACIA = CMDW;
	DATA = 0b11001001;
	tick();
	DDRF = 0x00;
}

void newdata() {
	ACIA = RXR;
	tick();
	Serial.write(PINF);
}

void loop() {
	/*
	Serial2.print("Mais quoi?");
	delay(1000);
	return;
	*/
	char buffer[30];
	delay(1000);
	// Try sending data
	char *test = "\nVoici qui est fort douteux.\n";
	
	DDRF = 0xFF;
	ACIA = TXR;
	for (uint8_t i = 0; i < strlen(test); i++) {
		DATA = test[i];
		tick();
		delayMicroseconds(100);
	}
	DDRF = 0x00;
	
	Serial2.print("Mais quoi?");
	
	ACIA = RXR;
	for (uint8_t i = 0; i < 20; i++) {
		delayMicroseconds(76);
		tick();
		buffer[i] = PINF;
	}
	buffer[20] = 0;
	Serial.write(buffer);
	
	if (Serial2.available()) {
		while (Serial2.available()) {
			Serial.write(Serial2.read());
		}
	} else {
		Serial.println("Nope...");
	}
}
