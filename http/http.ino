#include <PacketSerial.h>

#define revl(n) ((n) >> 24 | (((n) >> 16 & 255) << 8) | (((n) >> 8 & 255) << 16) | (((n) & 255) << 24))
#define revs(n) (((n) >> 8) | (((n) & 255) << 8))

#define MTU 576

PacketSerial_<SLIP, SLIP::END, MTU> myPacketSerial;

uint8_t lorem[] = "Lorem ipsum dolor sit amet?!";
uint8_t scratch[MTU];

struct IP_header {
	uint8_t version_ihl;
	uint8_t dscp_ecn;
	uint16_t length;
	uint16_t identification;
	uint16_t flags_offset;
	uint8_t ttl;
	uint8_t protocol;
	uint16_t checksum;
	uint32_t src;
	uint32_t dst;
};

struct ICMP_header {
	uint8_t type;
	uint8_t code;
	uint16_t checksum;
	uint32_t rest;
};

struct UDP_header {
	uint16_t src_port;
	uint16_t dst_port;
	uint16_t length;
	uint16_t checksum;
};

struct TCP_header {
	uint16_t src_port;
	uint16_t dst_port;
	uint32_t seqnum;
	uint32_t acknum;
	uint8_t offset_ns;
	uint8_t flags;
	uint16_t winsize;
	uint16_t checksum;
	uint16_t urgent;
};

void blink(uint8_t pin, uint8_t times, uint16_t duration) {
	for (uint8_t i = 0; i < times; i++) {
		digitalWrite(pin, HIGH);
		delay(duration);
		digitalWrite(pin, LOW);
		delay(duration);
	}
}

uint16_t IP_checksum(uint16_t *header, size_t size) {
	uint32_t checksum = size & 1 ? (short)(((uint8_t*)(header))[size - 1]) << 8 : 0;
	if (size & 1) size -= 1;
	for (uint8_t i = 0; i < size >> 1; i++) {
		checksum += header[i];
	}
	checksum = (checksum & 65535) + (checksum >> 16);
	checksum += checksum >> 16;
	return ~(uint16_t)checksum;
}

void setup() {
	myPacketSerial.begin(57600);
	myPacketSerial.setPacketHandler(&onPacketReceived);
	pinMode(13, OUTPUT);
	pinMode(8, OUTPUT);
	digitalWrite(8, LOW);
	digitalWrite(13, LOW);
}

void loop() {
	myPacketSerial.update();
}

void ICMP_echo(struct ICMP_header *icmp, size_t size) {
	if (icmp->type != 8) return; // Only process echo requests
	icmp->type = 0; // Echo reply
	icmp->checksum = 0;
	icmp->checksum = IP_checksum((uint16_t*)icmp, size);
}

void UDP_echo(struct UDP_header *udp) {
	udp->checksum = 0; // Optional, yay!
	udp->src_port ^= udp->dst_port;
	udp->dst_port ^= udp->src_port;
	udp->src_port ^= udp->dst_port;
}

void TCP_echo(struct IP_header *dgram) {
	struct TCP_header *tcp = (struct TCP_header*)(((uint8_t*)dgram) + 20);
	uint8_t fin = tcp->flags & 1, syn = tcp->flags >> 1 & 1, ack = tcp->flags >> 4 & 1, psh = tcp->flags >> 3 & 1;
	uint8_t offset = (tcp->offset_ns & 0b11110000) >> 2;
	tcp->src_port ^= tcp->dst_port;
	tcp->dst_port ^= tcp->src_port;
	tcp->src_port ^= tcp->dst_port;
	tcp->acknum = revl(revl(tcp->seqnum) + revs(dgram->length) - 20 - offset);
	if (tcp->acknum == tcp->seqnum)
		tcp->acknum = revl(revl(tcp->seqnum) + 1);
	tcp->flags = 0b11000 | (syn ? 2 : 0); // If not SYN, then set PSH (single packet echo)
	tcp->checksum = 0;
	uint32_t fullcheck = (dgram->src >> 16) + (dgram->src & 65535)
		+ (dgram->dst >> 16) + (dgram->dst & 65535) + 1536 + revs(revs(dgram->length) - 20);
	uint16_t i;
	for (i = 0; i < (revs(dgram->length) - 20) >> 1; i++)
		fullcheck += ((uint16_t*)tcp)[i];
	if (revs(dgram->length) & 1)
		fullcheck += ((uint8_t*)dgram)[revs(dgram->length) - 1];
	fullcheck = (fullcheck & 65535) + (fullcheck >> 16);
	fullcheck += fullcheck >> 16;
	tcp->checksum = ~(uint16_t)fullcheck;
}

struct IP_header *TCP_reply(struct IP_header *dgram) {
	static uint32_t seqn = 0;
	static uint32_t ackn = 0;
	static uint8_t isnew = 0;
	static uint8_t state = 0;
	uint8_t *raw = (uint8_t*)dgram;
	struct TCP_header *tcp = (struct TCP_header*)(raw + 20);
	uint8_t fin = tcp->flags & 1, syn = tcp->flags >> 1 & 1, ack = tcp->flags >> 4 & 1, psh = tcp->flags >> 3 & 1, rst = tcp->flags >> 2 & 1;
	uint8_t offset = (tcp->offset_ns & 0b11110000) >> 2;
	uint16_t tosend = psh ? 28 : 0;
	if (syn) {
		ackn = revl(revl(tcp->seqnum) + 1);
		seqn = tcp->seqnum + tcp->src_port + tcp->dst_port; // Pseudo-really-not-random
		isnew = 1;
		state = 1;
	} else if (rst) {
		seqn = 0;
		ackn = 0;
		state = 0;
		return NULL;
	} else if (isnew) {
		isnew = 0;
		seqn = revl(1);
	} else if (!state) {
		return NULL;
	}
	if ((!syn) && (revl(tcp->seqnum) < revl(ackn))) { // Retransmission, ignore
		return NULL;
	}
	tcp->src_port ^= tcp->dst_port;
	tcp->dst_port ^= tcp->src_port;
	tcp->src_port ^= tcp->dst_port;
	ackn = revl(revl(ackn) + revs(dgram->length) - 20 - offset);
	tcp->acknum = ackn;
	tcp->seqnum = seqn;
	seqn = revl(revl(seqn) + tosend);
	dgram->length = revs(((fin || syn || (!psh)) ? 0 : tosend) + 20 + offset);
	tcp->flags = 0b10000 | (syn ? 2 : 0) | fin; // If not SYN, then set PSH (single packet echo)
	tcp->checksum = 0;
	uint32_t fullcheck = (dgram->src >> 16) + (dgram->src & 65535)
		+ (dgram->dst >> 16) + (dgram->dst & 65535) + 1536 + revs(revs(dgram->length) - 20);
	uint16_t i;
	for (i = 0; i < offset >> 1; i++)
		fullcheck += ((uint16_t*)tcp)[i];
	//Done with header stuff, now to the data
	uint8_t carry;
	if (fin || syn || (!psh)) goto nope;
	for (i = 0; i < tosend; i++) {
		raw[i + offset + 20] = lorem[i];
		if (i & 1) {
			fullcheck += carry | raw[i + offset + 20] << 8;
		} else {
			carry = raw[i + offset + 20];
		}
	}
	if (revs(dgram->length) & 1)
		fullcheck += carry;
	nope:
	fullcheck = (fullcheck & 65535) + (fullcheck >> 16);
	fullcheck += fullcheck >> 16;
	tcp->checksum = ~(uint16_t)fullcheck;
	if (state == 2) {
		state = 0;
	}
	if (fin) {
		state = 2;
		ackn = revl(revl(ackn) + 1);
		seqn = revl(revl(seqn) + 1);
	}
	return dgram;
}

void onPacketReceived(const uint8_t* buffer, size_t size) {
	if (myPacketSerial.overflow()) {
		blink(13, 3, 500);
		return;
	}
	memcpy(scratch, buffer, size);
	struct IP_header *dgram = (struct IP_header*)scratch;
	uint8_t send = 1;
	if (revs(dgram->length) > MTU) {
		blink(13, 3, 500);
		return;
	}
	switch (dgram->protocol) {
		case 1: //ICMP
		ICMP_echo((struct ICMP_header*)(scratch + 20), revs(dgram->length) - 20);
		break;
		case 17: //UDP
		UDP_echo((struct UDP_header*)(scratch + 20));
		break;
		case 6: //TCP
		TCP_echo(dgram);
		//dgram = TCP_reply(dgram);
		break;
		default:
		send = 0;
	}
	if (!send) return;
	if (dgram == NULL) return;
	dgram->src ^= dgram->dst;
	dgram->dst ^= dgram->src;
	dgram->src ^= dgram->dst;
	dgram->checksum = 0;
	dgram->checksum = IP_checksum((uint16_t*)scratch, 20);
	myPacketSerial.send((uint8_t*)dgram, revs(dgram->length));
}
