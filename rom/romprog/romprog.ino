/*
 * Implemented on Arduino Mega
 * Addresses A0 to A15 are mapped to analog pins A0 to A15 (PORTF and PORTK)
 * Data is PORTL
 * /WE is port 36, /OE is 37 (control is PORTC)
 */

#define WRITE 0b01
#define READ  0b10
#define DATA  PORTL
#define DATA_IN  PINL
#define DATA_DIR DDRL
#define ADDR_LOW PORTF
#define ADDR_HI  PORTK
#define CTRL  PORTC

void write(uint16_t addr, byte value) {
	DATA_DIR = 0xFF;
	ADDR_LOW = addr & 0xFF;
	ADDR_HI = addr >> 8;
	DATA = value;
	delay(1);
	CTRL = WRITE;
	delay(1);
	CTRL = 0xFF;
	delay(1);
}

void progbyte(uint16_t addr, byte value) {
	write(0x5555, 0xAA);
	write(0x2AAA, 0x55);
	write(0x5555, 0xA0);
	write(addr, value);
}

byte read(uint16_t addr) {
	DATA_DIR = 0;
	ADDR_LOW = addr & 0xFF;
	ADDR_HI = addr >> 8;
	CTRL = READ;
	delay(1);
	byte res  = DATA_IN;
	delay(1);
	CTRL = 0xFF;
	return res;
}

void erase() {
	write(0x5555, 0xAA);
	write(0x2AAA, 0x55);
	write(0x5555, 0x80);
	write(0x5555, 0xAA);
	write(0x2AAA, 0x55);
	write(0x5555, 0x10);
	delay(200);
}

void program(byte *prog, uint16_t length) {
	erase();
	for (uint16_t i = 0; i < length; i++) {
		progbyte(i, prog[i]);
	}
	Serial.println("Programming done.");
}

void setup() {
	Serial.begin(9600);
	DDRF = 0xFF;
	DDRK = 0xFF;
	DDRC = 0xFF;
	delay(1000);
	
	Serial.print("Programming... ");
	byte *prog = calloc(256, 1);
	/* 6522 is A0-A3 (zero page). Program reads:
	 * LDA #FF
	 * STA $03; DDRA = 0xFF
	 * LDA #00
	 * STA $02: DDRB = 0
	 * here:
	 * LDA $00; read PORTB
	 * BEQ PB0
	 * LDA #55
	 * BRA there
	 * PB0:
	 * LDA #AA
	 * there:
	 * STA $01; write to PORTA
	 * JMP here
	 * Noise with distinctive patterns
	 * JMP $88; infinite loop
	 */
	// LDA #FF
	prog[0x80] = 0xA9;
	prog[0x81] = 0xFF;
	// STA $03
	prog[0x82] = 0x85;
	prog[0x83] = 0x03;
	// LDA #00
	prog[0x84] = 0xA9;
	prog[0x85] = 0x00;
	// STA $02
	prog[0x86] = 0x85;
	prog[0x87] = 0x02;
	// LDA $00
	prog[0x88] = 0xA5;
	prog[0x89] = 0x00;
	// BEQ
	prog[0x8A] = 0xF0;
	prog[0x8B] = 0x04; // ?
	// LDA #55
	prog[0x8C] = 0xA9;
	prog[0x8D] = 0x55;
	// BRA
	prog[0x8E] = 0x4C;
	prog[0x8F] = 0x92; // ?
	// LDA #AA
	prog[0x90] = 0xA9; // This is read as the high bytes of the JMP @. Who cares. Save a byte.
	prog[0x91] = 0xAA;
	// STA $01
	prog[0x92] = 0x85;
	prog[0x93] = 0x01;
	// Back to the beginning
	prog[0x94] = 0x4C;
	prog[0x95] = 0x88;
	// Jump table
	prog[0xFA] = 0x80; // 6502 is little-endian
	prog[0xFC] = 0x80;
	prog[0xFE] = 0x80;
	prog[0xFF] = 67;
	program(prog, 256);
	delay(100);

	Serial.println("Done!");
	
	for (uint16_t i = 0; i < 256; i++)
		Serial.println(read(i));
}

void loop() {}
