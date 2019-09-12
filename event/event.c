#include <stdint.h>
#include <stdio.h>

typedef struct {
	uint8_t(*handler)(void*);
	uint8_t args[16];
} vector;

char testr[] = "Hop, ca marche, trop bien.";

struct __attribute__((__packed__)) test {
	uint8_t retval;
	char *string;
};

uint8_t print_stuff(struct test *argl) {
	for (uint8_t i = 0; i < 16; i++)
		printf("%x", ((uint8_t*)argl)[i]);
	printf("\n");
	printf("From print_stuff, string is at %p\n", argl->string);
	printf("retval is %u\n", argl->retval);
	puts(argl->string);
	return argl->retval;
}

uint8_t interrupt(vector v) {
	for (uint8_t i = 0; i < sizeof(v.args); i++)
		printf("%x", ((uint8_t*)&v.args)[i]);
	printf("\n");
	return v.handler(v.args);
}

int main(int argc, char **argv) {
	printf("Global string is at %p\n", testr);
	struct test machin = { .retval = 42, .string = testr };
	printf("machin has size %d, retval %d\n", sizeof(machin), machin.retval);
	printf("From main, string is at %p\n", machin.string);
	printf("%d\n", sizeof(vector));
	vector v;
	v.handler = print_stuff;
	for (uint8_t i = 0; i < sizeof(machin); i++)
		printf("%x", ((uint8_t*)&machin)[i]);
	printf("\n");
	*(struct test*)&v.args = machin;
	for (uint8_t i = 0; i < sizeof(machin); i++)
		printf("%x", ((uint8_t*)&v.args)[i]);
	printf("\n");
	uint8_t res = interrupt(v);
	printf("%d\n", res);
	return 0;
}
