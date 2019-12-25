#define DELAY 500
void setup() {
	Serial.begin(57600);
	delay(1000);
	DDRF = 0xFF; // A0-A7
	DDRK = 0xFF; // A8-A15
	DDRL = 0x00; // D0-D8
	DDRC = 0b00001111; // PROG, RESETB, RWB, PHI2
	PORTC= 0b00000011;
	Serial.println("RESETing 6522");
	PORTC = 0b00000010;
	delayMicroseconds(1);
	PORTC = 0b00000011;
	delayMicroseconds(1);
	PORTC = 0b00000110;
	delayMicroseconds(1);
	PORTC = 0b00000111;
	delayMicroseconds(1);
	PORTC = 0b00000010;
	delayMicroseconds(1);
	PORTC = 0b00000011;
	delayMicroseconds(1);
	PORTC = 0b00000010;
	delayMicroseconds(1);
	PORTC = 0b00000011;
	delayMicroseconds(1);
	PORTC = 0b00000110;
	delayMicroseconds(1);
	PORTC = 0b00000111;
	delayMicroseconds(1);
	PORTC = 0b00000110;
	delayMicroseconds(1);
	PORTC = 0b00000111;
	delayMicroseconds(1);
	Serial.print("Zeroing RAM...      ");
	PORTC = 0b00000101;
	DDRL  = 0xFF;
	PORTL = 0x00;
	for (uint8_t l = 0; l < 0x80; l++) {
		PORTK = l;
		for (uint8_t h = 0; h < 0xFF; h++) {
			PORTF = h;
			delayMicroseconds(1);
		}
	}
	Serial.println("done.");
	PORTC = 0b00000111;
	Serial.println("Setting IO ports as outputs");
	PORTK = 0xA0;
	PORTF = 0x03; // DDRA
	PORTL = 0xFF;
	PORTC = 0b00000100; // Write, PHI2 low
	delayMicroseconds(1);
	PORTC = 0b00000101; // Write, PHI2 high
	PORTF = 0x02; // DDRB
	PORTC = 0b00000100; // Write, PHI2 low
	delayMicroseconds(1);
	PORTC = 0b00000101; // Write, PHI2 high
	delayMicroseconds(1);
	Serial.println("Programming ROM...");
	PORTC = 0b00001111;
	delayMicroseconds(10);
	// Erase ROM
	/*
	writeByte(0x5555, 0xAA);
	writeByte(0x2AAA, 0x55);
	writeByte(0x5555, 0x80);
	writeByte(0x5555, 0xAA);
	writeByte(0x2AAA, 0x55);
	writeByte(0x5555, 0x10);
	delay(100);
	// Byte program
	Serial.println("Starting programming");
	for (uint16_t i = 0xC000; i > 0; i++)
		programByte(i, 0xEA);
	Serial.println("Done.");
	*/
	DDRL  = 0x00;
	PORTC = 0b00000111;
}

void programByte(uint16_t addr, uint8_t data) {
	writeByte(0x5555, 0xAA);
	writeByte(0x2AAA, 0x55);
	writeByte(0x5555, 0xA0);
	writeByte(addr, data);
}

void writeByte(uint16_t addr, uint8_t data) {
	/*
	char buffer[100];
	sprintf(buffer, "$%04X -- $%02X\n", addr, data);
	Serial.write(buffer);
	*/
	PORTK = addr >> 8;
	PORTF = addr & 0xFF;
	PORTL = data;
	DDRL  = 0xFF;
	PORTC = 0b00001101;
	delayMicroseconds(50);
	DDRL  = 0x00;
	PORTC = 0b00001111;
	delayMicroseconds(50);
}

void loop() {
	return;
	char buffer[10];
	Serial.print("Reading address $0000: ");
	DDRL  = 0x00;
	PORTC = 0b00000111;
	PORTF = 0x00;
	PORTK = 0x00;
	delay(DELAY);
	sprintf(buffer, "0x%X\n", PINL);
	Serial.write(buffer);
	Serial.println("Writing 0xEA to address $5555");
	DDRL  = 0xFF;
	PORTL = 0xEA;
	PORTF = 0x55;
	PORTK = 0x55;
	PORTC = 0b00000101;
	delay(DELAY);
	Serial.print("Reading from address $15EA: ");
	PORTC = 0b00000111;
	DDRL  = 0x00;
	PORTF = 0xEA;
	PORTK = 0x15;
	delay(DELAY);
	sprintf(buffer, "0x%X\n", PINL);
	Serial.write(buffer);
	Serial.print("Reading back from $5555: ");
	PORTF = 0x55;
	PORTK = 0x55;
	delay(DELAY);
	sprintf(buffer, "0x%X\n", PINL);
	Serial.write(buffer);
	Serial.print("Reading address $FFFF: ");
	PORTF = 0xFF;
	PORTK = 0xFF;
	delay(DELAY);
	sprintf(buffer, "0x%X\n", PINL);
	Serial.write(buffer);
	Serial.print("Reading address $A002 (DDRB) : ");
	PORTF = 0x02;
	PORTK = 0xA0;
	delay(DELAY);
	sprintf(buffer, "0x%X\n", PINL);
	Serial.write(buffer);
	Serial.print("Reading address $A003 (DDRA) : ");
	PORTC = 0b00000110; //PHI2 low
	PORTF = 0x03;
	PORTK = 0xA0;
	delayMicroseconds(1);
	PORTC = 0b00000111; //PHI2 high
	delay(DELAY);
	sprintf(buffer, "0x%X\n", PINL);
	Serial.write(buffer);
	Serial.println("Toggling LEDs");
	DDRL  = 0xFF;
	PORTC = 0b00000100; // Write, PHI2 low
	PORTK = 0xA0;
	PORTF = 0x01; // PORTA
	PORTL = 0xEA;
	delayMicroseconds(1);
	PORTC = 0b00000101; // Write, PHI2 hi
	delay(DELAY);
	PORTL = 0x55;
	PORTC = 0b00000100; // Write, PHI2 low
	delayMicroseconds(1);
	PORTC = 0b00000101; // Write, PHI2 hi
	delay(DELAY);
	DDRL  = 0x00;
}
