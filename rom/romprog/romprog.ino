#define WRITE 0b01
#define READ  0b10
#define DATA  PORTF
#define DATA_IN  PINF
#define DATA_DIR DDRF
#define ADDR  PORTB
#define CTRL  PORTH

void write(byte addr, byte value) {
	DATA_DIR = 0xFF;
	ADDR = addr;
	DATA = value;
	delay(1);
	CTRL = WRITE;
	delay(1);
	CTRL = 0xFF;
	delay(1);
}

void progbyte(byte addr, byte value) {
	write(0b11010, 0xAA); // 0x5555
	write(0b01010, 0x55); // 0x2AAA
	write(0b11010, 0xA0); // 0x5555
	write(addr, value);
}

byte read(byte addr) {
	DATA_DIR = 0;
	ADDR = addr;
	CTRL = READ;
	delay(1);
	byte res  = DATA_IN;
	CTRL = 0xFF;
	return res;
}

void erase() {
	write(0b11010, 0xAA); // 0x5555
	write(0b01010, 0x55); // 0x2AAA
	write(0b11010, 0x80); // 0x5555
	write(0b11010, 0xAA); // 0x5555
	write(0b01010, 0x55); // 0x2AAA
	write(0b11010, 0x10); // 0x5555
	delay(200);
}

void program(byte *prog, byte length) {
	// We can only address 16 bytes, and bytes 12 & 13 are the RESET vector
	// ROM chip should be hardwired to address 0b101010101010XXXX
	if (length > 12) length = 12;
	erase();
	for (byte i = 0; i < length; i++) {
		progbyte(0b10000 + i, prog[i]);
	}
	progbyte(0b11100, 0);
	progbyte(0b11101, 0);
}

void setup() {
	Serial.begin(9600);
	DDRH = 0xFF;
	DDRB = 0xFF;
	delay(5000);

	for (byte i = 0; i < 16; i++)
		Serial.println(read(0b10000 + i));

	Serial.print("Programming... ");
	
	byte prog[] = {
		0xAD,
		0xEA,
		0x55,
		0x4C,
		0x00,
		0x00
	};
	program(prog, 6);
	delay(100);

	Serial.println("Done!");
	
	for (byte i = 0; i < 16; i++)
		Serial.println(read(0b10000 + i));
}

void loop() {}
